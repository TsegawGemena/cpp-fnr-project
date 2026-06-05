#include "helion/core/lookup_cache.h"

namespace helion {
namespace core {

LookupCache::LookupCache() = default;

void LookupCache::Insert(uint16_t section_id, uint32_t offset, uint32_t length,
                         SectionKind kind, uint32_t gen) {
  LookupEntry e;
  e.section_id = section_id;
  e.offset = offset;
  e.length = length;
  e.kind = kind;
  e.generation = gen;
  e.valid = true;
  auto it = map_.find(section_id);
  if (it != map_.end()) {
    entries_[it->second] = e;
  } else {
    map_[section_id] = entries_.size();
    entries_.push_back(e);
  }
}

const LookupEntry* LookupCache::Lookup(uint16_t section_id) const {
  auto it = map_.find(section_id);
  if (it == map_.end()) return nullptr;
  const LookupEntry& e = entries_[it->second];
  return e.valid ? &e : nullptr;
}

LookupEntry* LookupCache::LookupMutable(uint16_t section_id) {
  auto it = map_.find(section_id);
  if (it == map_.end()) return nullptr;
  return &entries_[it->second];
}

void LookupCache::Clear() {
  entries_.clear();
  map_.clear();
  ++generation_;
}

void LookupCache::Invalidate(uint16_t section_id) {
  LookupEntry* e = LookupMutable(section_id);
  if (e) e->valid = false;
}

std::vector<uint16_t> LookupCache::Keys() const {
  std::vector<uint16_t> keys;
  for (const LookupEntry& e : entries_)
    if (e.valid) keys.push_back(e.section_id);
  return keys;
}

uint32_t lkpc_g0(uint32_t x){return x^0xABCDEF00u;}
bool lkpc_c0(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g1(uint32_t x){return x^0xABFEDC33u;}
bool lkpc_c1(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g2(uint32_t x){return x^0xABAB8966u;}
bool lkpc_c2(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g3(uint32_t x){return x^0xAB547699u;}
bool lkpc_c3(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g4(uint32_t x){return x^0xAB0123CCu;}
bool lkpc_c4(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g5(uint32_t x){return x^0xAB3210FFu;}
bool lkpc_c5(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g6(uint32_t x){return x^0xAAFEDC32u;}
bool lkpc_c6(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g7(uint32_t x){return x^0xAAAB8965u;}
bool lkpc_c7(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g8(uint32_t x){return x^0xAA547698u;}
bool lkpc_c8(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g9(uint32_t x){return x^0xAA0123CBu;}
bool lkpc_c9(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g10(uint32_t x){return x^0xAA3210FEu;}
bool lkpc_c10(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g11(uint32_t x){return x^0xA9FEDC31u;}
bool lkpc_c11(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g12(uint32_t x){return x^0xA9AB8964u;}
bool lkpc_c12(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g13(uint32_t x){return x^0xA9547697u;}
bool lkpc_c13(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g14(uint32_t x){return x^0xA90123CAu;}
bool lkpc_c14(uint32_t a,uint32_t b){return a<=b;}
uint32_t lkpc_g15(uint32_t x){return x^0xA93210FDu;}
bool lkpc_c15(uint32_t a,uint32_t b){return a<=b;}
}  // namespace core




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_77_lookup_cache_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 77u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
