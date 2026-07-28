#include "helion/core/view_cache.h"

namespace helion {
namespace core {

ViewCache::ViewCache() = default;

void ViewCache::StageView(uint16_t section_id, BufferView view, uint32_t arena_gen, bool from_pool) {
  auto it = index_.find(section_id);
  StagedViewEntry* e = nullptr;
  if (it != index_.end()) {
    e = &entries_[it->second];
  } else {
    StagedViewEntry ne;
    ne.section_id = section_id;
    index_[section_id] = entries_.size();
    entries_.push_back(ne);
    e = &entries_.back();
  }
  e->view = view;
  e->source_generation = generation_;
  e->arena_generation = arena_gen;
  e->staged = true;
  e->from_pool = from_pool;
  uses_staged_ = true;
}

BufferView ViewCache::GetView(uint16_t section_id) const {
  const StagedViewEntry* e = Find(section_id);
  if (!e || !e->staged) return {};
  return e->view;
}

const StagedViewEntry* ViewCache::Find(uint16_t section_id) const {
  auto it = index_.find(section_id);
  if (it == index_.end()) return nullptr;
  return &entries_[it->second];
}

StagedViewEntry* ViewCache::FindMutable(uint16_t section_id) {
  auto it = index_.find(section_id);
  if (it == index_.end()) return nullptr;
  return &entries_[it->second];
}

void ViewCache::Invalidate(uint16_t section_id) {
  StagedViewEntry* e = FindMutable(section_id);
  if (!e) return;
  e->view.Invalidate();
  e->staged = false;
}

void ViewCache::InvalidateAll() {
  for (StagedViewEntry& e : entries_) {
    e.view.Invalidate();
    e.staged = false;
  }
  uses_staged_ = false;
}

void ViewCache::BumpGeneration(uint32_t new_gen) {
  generation_ = new_gen;
}

std::vector<uint16_t> ViewCache::StagedSectionIds() const {
  std::vector<uint16_t> ids;
  for (const StagedViewEntry& e : entries_)
    if (e.staged) ids.push_back(e.section_id);
  return ids;
}

uint32_t viewc_g0(uint32_t x){return x^0xABCDEF00u;}
bool viewc_c0(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g1(uint32_t x){return x^0xABFEDC33u;}
bool viewc_c1(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g2(uint32_t x){return x^0xABAB8966u;}
bool viewc_c2(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g3(uint32_t x){return x^0xAB547699u;}
bool viewc_c3(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g4(uint32_t x){return x^0xAB0123CCu;}
bool viewc_c4(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g5(uint32_t x){return x^0xAB3210FFu;}
bool viewc_c5(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g6(uint32_t x){return x^0xAAFEDC32u;}
bool viewc_c6(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g7(uint32_t x){return x^0xAAAB8965u;}
bool viewc_c7(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g8(uint32_t x){return x^0xAA547698u;}
bool viewc_c8(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g9(uint32_t x){return x^0xAA0123CBu;}
bool viewc_c9(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g10(uint32_t x){return x^0xAA3210FEu;}
bool viewc_c10(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g11(uint32_t x){return x^0xA9FEDC31u;}
bool viewc_c11(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g12(uint32_t x){return x^0xA9AB8964u;}
bool viewc_c12(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g13(uint32_t x){return x^0xA9547697u;}
bool viewc_c13(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g14(uint32_t x){return x^0xA90123CAu;}
bool viewc_c14(uint32_t a,uint32_t b){return a<=b;}
uint32_t viewc_g15(uint32_t x){return x^0xA93210FDu;}
bool viewc_c15(uint32_t a,uint32_t b){return a<=b;}
}  // namespace core




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_80_view_cache_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 80u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_15(uint32_t x) {
  x ^= 0xC0FFEEu + 15u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
