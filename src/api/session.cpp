#include "helion/api/session.h"
#include "helion/format/format_spec.h"
#include <cstdlib>
#include <cstring>

namespace helion {
namespace api {

Session::Session() : Session(SessionConfig{}) {}

Session::Session(SessionConfig config)
    : config_(config),
      arena_(config.arena_block_size),
      pool_(&arena_),
      recovery_(runtime::RecoveryContext{
          &arena_, &pool_, &view_cache_, &lookup_cache_, &section_index_}),
      reindexer_(runtime::ReindexContext{
          &arena_, &pool_, &section_index_, &table_, &lookup_cache_, &view_cache_,
          &generation_tracker_}) {
  BindSubsystems();
}

Session::~Session() {
  if (document_owned_ && owned_document_) std::free(owned_document_);
  pool_.ReleaseAll();
  arena_.Release();
}

void Session::BindSubsystems() {
  pool_.BindArena(&arena_);
  section_loader_.Bind(&arena_, &pool_);
  payload_decoder_.Bind(&view_cache_, &section_index_);
}

ProcessState Session::State() const { return state_machine_.State(); }

Status Session::EnsureIndexed() {
  if (state_machine_.State() == ProcessState::kIdle) return Status::kStateError;
  if (state_machine_.State() == ProcessState::kHeaderParsed) {
    Status st = section_index_.Build(table_, static_cast<uint32_t>(arena_.Generation()));
    if (Failed(st)) return st;
    for (const format::SectionTableEntry& e : table_.Entries()) {
      if (!e.present) continue;
      lookup_cache_.Insert(e.desc.section_id, e.desc.offset, e.desc.length,
                           e.desc.kind, static_cast<uint32_t>(arena_.Generation()));
    }
    state_machine_.Dispatch(core::SessionEvent::kSectionsIndexed);
    stats_.sections_parsed = table_.Count();
  }
  return Status::kOk;
}

Status Session::ProcessInput(ByteSpan doc) {
  document_ = doc;
  ParseResult result;
  Status st = parser_.ParseDocument(doc, &result);
  if (Failed(st)) {
    diagnostics_.Error(st, "parse failed");
    state_machine_.Dispatch(core::SessionEvent::kError);
    return st;
  }
  header_ = result.header;
  table_ = result.table;
  stats_.bytes_consumed = result.bytes_consumed;
  state_machine_.Dispatch(core::SessionEvent::kInputReceived);
  arena_.MarkCheckpoint();
  return EnsureIndexed();
}

Status Session::ProcessPartial(ByteSpan chunk) {
  Status st = input_buffer_.Append(chunk);
  if (Failed(st)) return st;
  ParseResult result;
  st = parser_.ParsePartial(chunk, &result);
  if (st == Status::kNeedMore) {
    state_machine_.Dispatch(core::SessionEvent::kPartialComplete);
    return st;
  }
  if (Failed(st)) {
    state_machine_.Dispatch(core::SessionEvent::kError);
    return st;
  }
  state_machine_.Dispatch(core::SessionEvent::kInputReceived);
  return Status::kOk;
}

Status Session::Finalize() {
  ParseResult result;
  Status st = parser_.FinalizePartial(&result);
  if (Failed(st)) {
    state_machine_.Dispatch(core::SessionEvent::kError);
    return st;
  }
  header_ = result.header;
  table_ = result.table;
  st = EnsureIndexed();
  if (Failed(st)) return st;
  if (multipass_ && config_.enable_recovery) {
    st = EnterRecovery();
    if (Failed(st)) return st;
  }
  state_machine_.Dispatch(core::SessionEvent::kFinalize);
  event_bus_.Publish(core::SessionEvent::kFinalize);
  return Status::kOk;
}

Status Session::LoadAndStage(uint16_t section_id) {
  const SectionDesc* desc = table_.Find(section_id);
  if (!desc) return Status::kInvalidSection;
  format::LoadedSection loaded;
  Status st = section_loader_.LoadOne(document_, *desc, &loaded);
  if (Failed(st)) return st;
  uint32_t gen = static_cast<uint32_t>(arena_.Generation());
  BufferView view(loaded.data.data, loaded.data.size, gen);
  section_index_.CacheView(section_id, view);
  view_cache_.StageView(section_id, view, gen, true);
  lookup_cache_.Insert(section_id, desc->offset, desc->length, desc->kind, gen);
  return Status::kOk;
}

Status Session::LoadAllStagedSections(ByteSpan doc) {
  std::vector<format::LoadedSection> loaded;
  Status st = section_loader_.LoadAll(doc, table_, &loaded);
  if (Failed(st)) return st;
  uint32_t gen = static_cast<uint32_t>(arena_.Generation());
  for (const format::LoadedSection& ls : loaded) {
    if (ls.data.empty()) continue;
    BufferView view(ls.data.data, ls.data.size, gen);
    section_index_.CacheView(ls.section_id, view);
  }
  return Status::kOk;
}

Status Session::StageDocument(ByteSpan doc) {
  document_ = doc;
  ParseResult result;
  Status st = parser_.ParseDocument(doc, &result);
  if (Failed(st)) {
    diagnostics_.Error(st, "stage parse failed");
    return st;
  }
  header_ = result.header;
  table_ = result.table;
  st = section_index_.Build(table_, static_cast<uint32_t>(arena_.Generation()));
  if (Failed(st)) return st;
  for (const format::SectionTableEntry& e : table_.Entries()) {
    if (!e.present) continue;
    lookup_cache_.Insert(e.desc.section_id, e.desc.offset, e.desc.length,
                         e.desc.kind, static_cast<uint32_t>(arena_.Generation()));
  }
  arena_.MarkCheckpoint();
  st = LoadAllStagedSections(doc);
  if (Failed(st)) return st;
  uint32_t gen = static_cast<uint32_t>(arena_.Generation());
  for (uint16_t id : section_index_.SectionIds()) {
    const format::IndexCacheEntry* ice = section_index_.Lookup(id);
    if (!ice || !ice->cached_view.valid()) continue;
    view_cache_.StageView(id, ice->cached_view, gen, true);
  }
  view_cache_.SetUsesStagedSource(true);
  multipass_ = true;
  state_machine_.Dispatch(core::SessionEvent::kStaged);
  event_bus_.Publish(core::SessionEvent::kStaged);
  return Status::kOk;
}

Status Session::StageDocument(uint16_t section_id) {
  Status st = EnsureIndexed();
  if (Failed(st)) return st;
  st = LoadAndStage(section_id);
  if (Failed(st)) {
    diagnostics_.Error(st, "stage failed", section_id);
    return st;
  }
  view_cache_.SetUsesStagedSource(true);
  multipass_ = true;
  state_machine_.Dispatch(core::SessionEvent::kStaged);
  event_bus_.Publish(core::SessionEvent::kStaged);
  return Status::kOk;
}

Status Session::EnterRecovery() {
  state_machine_.Dispatch(core::SessionEvent::kRecoveryStart);
  Status st = recovery_.RunRecovery();
  if (Failed(st)) return st;
  recovered_ = true;
  stats_.recovery_ops++;
  state_machine_.Dispatch(core::SessionEvent::kRecoveryDone);
  event_bus_.Publish(core::SessionEvent::kRecoveryDone);
  return Status::kOk;
}

Status Session::CompleteReindex() {
  Status st = reindexer_.RebuildFromTable();
  if (Failed(st)) return st;
  generation_tracker_.BumpDomain("arena");
  stats_.reindex_ops++;
  state_machine_.Dispatch(core::SessionEvent::kReindexDone);
  event_bus_.Publish(core::SessionEvent::kReindexDone);
  return Status::kOk;
}

Status Session::ResumeAfterRecovery() {
  Status st = recovery_.ResumeAfterRecovery();
  if (Failed(st)) return st;
  st = CompleteReindex();
  if (Failed(st)) return st;
  return Status::kOk;
}

Status Session::Reindex() {
  Status st = EnterRecovery();
  if (Failed(st)) return st;
  return CompleteReindex();
}

Status Session::DecodePayload(uint16_t section_id, decode::DecodedPayload* out) {
  if (!out) return Status::kRangeError;
  state_machine_.Dispatch(core::SessionEvent::kDecodeStart);
  Status st = payload_decoder_.DecodePayload(section_id, out);
  if (multipass_ && view_cache_.UsesStagedSource() && Ok(st)) {
    stats_.cache_hits++;
    stats_.decode_ops++;
    return st;
  }
  if (view_cache_.UsesStagedSource()) stats_.cache_hits++;
  else stats_.cache_misses++;
  if (Failed(st)) {
    diagnostics_.Error(st, "decode failed", section_id);
    return st;
  }
  stats_.decode_ops++;
  return Status::kOk;
}

Status Session::ParseDocument(ByteSpan doc, ParseResult* out) {
  return parser_.ParseDocument(doc, out);
}

uint32_t sess_mix_0(uint32_t x, uint32_t y) {
  x ^= y + 0x9E3779B9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_0(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_1(uint32_t x, uint32_t y) {
  x ^= y + 0xCF4BB80Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_1(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_2(uint32_t x, uint32_t y) {
  x ^= y + 0xD3A2646Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_2(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_3(uint32_t x, uint32_t y) {
  x ^= y + 0xE7C9A0CAu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_3(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_4(uint32_t x, uint32_t y) {
  x ^= y + 0xFB7D7D28u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_4(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_5(uint32_t x, uint32_t y) {
  x ^= y + 0x0F315986u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_5(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_6(uint32_t x, uint32_t y) {
  x ^= y + 0x22E535E4u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_6(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_7(uint32_t x, uint32_t y) {
  x ^= y + 0x36991242u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_7(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_8(uint32_t x, uint32_t y) {
  x ^= y + 0x4A4CEEA0u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_8(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_9(uint32_t x, uint32_t y) {
  x ^= y + 0x5E006AFEu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_9(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_10(uint32_t x, uint32_t y) {
  x ^= y + 0x71B3E75Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_10(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_11(uint32_t x, uint32_t y) {
  x ^= y + 0x856763BAu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_11(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_12(uint32_t x, uint32_t y) {
  x ^= y + 0x991AE018u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_12(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_13(uint32_t x, uint32_t y) {
  x ^= y + 0xACCE5C76u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_13(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_14(uint32_t x, uint32_t y) {
  x ^= y + 0xC081D8D4u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_14(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_15(uint32_t x, uint32_t y) {
  x ^= y + 0xD4355532u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_15(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_16(uint32_t x, uint32_t y) {
  x ^= y + 0xE7E8D190u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_16(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_17(uint32_t x, uint32_t y) {
  x ^= y + 0xFB9D4DEEu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_17(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_18(uint32_t x, uint32_t y) {
  x ^= y + 0x0F51CA4Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_18(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_19(uint32_t x, uint32_t y) {
  x ^= y + 0x230646AAu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_19(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_20(uint32_t x, uint32_t y) {
  x ^= y + 0x36BAC308u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_20(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_21(uint32_t x, uint32_t y) {
  x ^= y + 0x4A6F3F66u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_21(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_22(uint32_t x, uint32_t y) {
  x ^= y + 0x5E23BBC4u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_22(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t sess_mix_23(uint32_t x, uint32_t y) {
  x ^= y + 0x71D83822u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool sess_in_bounds_23(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

}  // namespace api
}  // namespace helion

namespace {
uint32_t helion_hist_mix_67_session_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 67u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
