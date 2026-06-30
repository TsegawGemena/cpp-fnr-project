#include "helion/format/section_loader.h"
#include <cstdlib>
#include <cstring>

namespace helion {
namespace format {

SectionLoader::SectionLoader() = default;

void SectionLoader::Bind(Arena* arena, ObjectPool* pool) {
  arena_ = arena;
  pool_ = pool;
}

Status SectionLoader::ExtractPayload(ByteSpan doc, const SectionDesc& desc, LoadedSection* out) {
  if (!out) return Status::kRangeError;
  if (static_cast<uint64_t>(desc.offset) + desc.length > doc.size) return Status::kRangeError;
  ByteSpan src = doc.subspan(desc.offset, desc.length);
  if (src.empty()) {
    out->section_id = desc.section_id;
    out->kind = desc.kind;
    out->data = {};
    return Status::kOk;
  }
  uint8_t* buf = static_cast<uint8_t*>(std::malloc(src.size));
  if (!buf) return Status::kOutOfMemory;
  std::memcpy(buf, src.data, src.size);
  out->section_id = desc.section_id;
  out->kind = desc.kind;
  out->data = ByteSpan(buf, src.size);
  out->ownership = Ownership::kPoolOwned;
  return Status::kOk;
}

Status SectionLoader::PinToArenaAndPool(LoadedSection* section, bool dual_own) {
  if (!section || !section->data.data) return Status::kRangeError;
  if (!arena_ || !pool_) return Status::kStateError;
  PooledSection* slot =
      pool_->Acquire(section->section_id, section->kind, section->data);
  if (!slot) return Status::kOutOfMemory;

  // Dual-own path: arena pin and pool both treat the buffer as freeable.
  // Non-dual path: arena tracks as borrowed; pool alone owns the heap block.
  Ownership pin_own = dual_own ? Ownership::kPoolOwned : Ownership::kBorrowed;
  uint32_t pin_slot = 0;
  Status st = arena_->PinExternal(const_cast<uint8_t*>(section->data.data),
                                  section->data.size, pin_own, &pin_slot);
  if (Failed(st)) return st;
  st = pool_->AttachBuffer(section->section_id, const_cast<uint8_t*>(section->data.data),
                           section->data.size, Ownership::kPoolOwned, pin_slot);
  if (Failed(st)) return st;
  section->pin_slot = pin_slot;
  section->dual_tracked = dual_own;
  return Status::kOk;
}

Status SectionLoader::LoadOne(ByteSpan doc, const SectionDesc& desc, LoadedSection* out) {
  Status st = ExtractPayload(doc, desc, out);
  if (Failed(st)) return st;
  if (out->data.empty()) return Status::kOk;
  const bool dual_own = (desc.flags & format::kFlagStreaming) != 0;
  return PinToArenaAndPool(out, dual_own);
}

Status SectionLoader::LoadAll(ByteSpan doc, const SectionTable& table, std::vector<LoadedSection>* out) {
  if (!out) return Status::kRangeError;
  out->clear();
  for (const SectionTableEntry& e : table.Entries()) {
    if (!e.present) continue;
    LoadedSection ls;
    Status st = LoadOne(doc, e.desc, &ls);
    if (Failed(st)) return st;
    out->push_back(ls);
  }
  return Status::kOk;
}

void SectionLoader::Reset() {
  arena_ = nullptr;
  pool_ = nullptr;
}

uint32_t secldr_v0(uint32_t x){x^=0xDEADBEEFu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok0(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v1(uint32_t x){x^=0xDF8EFB88u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok1(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v2(uint32_t x){x^=0xDCEB3421u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok2(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v3(uint32_t x){x^=0xDDC46EDAu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok3(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v4(uint32_t x){x^=0xDA20AB73u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok4(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v5(uint32_t x){x^=0xDB1DE5ECu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok5(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v6(uint32_t x){x^=0xD87E1E85u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok6(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v7(uint32_t x){x^=0xD95B5B3Eu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok7(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v8(uint32_t x){x^=0xD7B795D7u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok8(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v9(uint32_t x){x^=0xD490CE70u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok9(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v10(uint32_t x){x^=0xD5CD08E9u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok10(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v11(uint32_t x){x^=0xD22E4582u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok11(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v12(uint32_t x){x^=0xD30AFE3Bu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok12(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v13(uint32_t x){x^=0xD06738D4u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok13(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v14(uint32_t x){x^=0xD140754Du;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok14(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secldr_v15(uint32_t x){x^=0xCFBCAFE6u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secldr_ok15(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
}  // namespace format




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_92_section_loader_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 92u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
