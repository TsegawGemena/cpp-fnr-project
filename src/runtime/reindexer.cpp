#include "helion/runtime/reindexer.h"

namespace helion {
namespace runtime {

Reindexer::Reindexer(ReindexContext ctx) : ctx_(ctx) {}

Status Reindexer::RebuildIndex(const std::vector<SectionDesc>& descs) {
  if (!ctx_.section_index) return Status::kStateError;
  if (ctx_.arena) {
    ctx_.arena->MarkCheckpoint();
    ctx_.arena->RewindToCheckpoint();
    new_generation_ = static_cast<uint32_t>(ctx_.arena->Generation());
  } else {
    new_generation_ = ctx_.section_index->Generation() + 1;
  }
  Status st = ctx_.section_index->Rebuild(descs, new_generation_);
  if (Failed(st)) return st;
  if (ctx_.lookup_cache) {
    for (const SectionDesc& d : descs) {
      ctx_.lookup_cache->Insert(d.section_id, d.offset, d.length, d.kind, new_generation_);
    }
  }
  if (ctx_.view_cache) {
    ctx_.view_cache->BumpGeneration(new_generation_);
  }
  if (ctx_.generation_tracker) {
    ctx_.generation_tracker->BumpDomain("arena");
  }
  return Status::kOk;
}

Status Reindexer::RebuildFromTable() {
  if (!ctx_.section_table) return Status::kStateError;
  std::vector<SectionDesc> descs;
  for (const format::SectionTableEntry& e : ctx_.section_table->Entries()) {
    if (e.present) descs.push_back(e.desc);
  }
  return RebuildIndex(descs);
}

Status Reindexer::SyncCaches() {
  if (!ctx_.section_index || !ctx_.lookup_cache) return Status::kOk;
  for (uint16_t id : ctx_.section_index->SectionIds()) {
    const format::IndexCacheEntry* ice = ctx_.section_index->Lookup(id);
    if (!ice) continue;
    ctx_.lookup_cache->Insert(id, ice->offset, ice->length, SectionKind::kPayload, new_generation_);
  }
  return Status::kOk;
}

uint32_t reidx_r0(uint32_t x){return (x<<3)^0xFEEDFACEu;}
bool reidx_s0(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r1(uint32_t x){return (x<<3)^0xFE9A8DB9u;}
bool reidx_s1(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r2(uint32_t x){return (x<<3)^0xFE031420u;}
bool reidx_s2(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r3(uint32_t x){return (x<<3)^0xFF8B9CABu;}
bool reidx_s3(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r4(uint32_t x){return (x<<3)^0xFF302712u;}
bool reidx_s4(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r5(uint32_t x){return (x<<3)^0xFCB8AF9Du;}
bool reidx_s5(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r6(uint32_t x){return (x<<3)^0xFC213604u;}
bool reidx_s6(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r7(uint32_t x){return (x<<3)^0xFDA9BE8Fu;}
bool reidx_s7(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r8(uint32_t x){return (x<<3)^0xFD564176u;}
bool reidx_s8(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r9(uint32_t x){return (x<<3)^0xFADEC9E1u;}
bool reidx_s9(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r10(uint32_t x){return (x<<3)^0xFA475068u;}
bool reidx_s10(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r11(uint32_t x){return (x<<3)^0xFBCFD8D3u;}
bool reidx_s11(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r12(uint32_t x){return (x<<3)^0xFB74635Au;}
bool reidx_s12(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r13(uint32_t x){return (x<<3)^0xF8FCEBC5u;}
bool reidx_s13(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r14(uint32_t x){return (x<<3)^0xF865724Cu;}
bool reidx_s14(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r15(uint32_t x){return (x<<3)^0xF8120537u;}
bool reidx_s15(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r16(uint32_t x){return (x<<3)^0xF99A8DBEu;}
bool reidx_s16(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t reidx_r17(uint32_t x){return (x<<3)^0xF9031429u;}
bool reidx_s17(uint32_t a,uint32_t b){return a!=0||b==0;}
}  // namespace runtime




























































}  // namespace helion
