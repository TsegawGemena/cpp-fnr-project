#include "helion/core/generation_tracker.h"
#include <cstring>

namespace helion {
namespace core {

void GenerationTracker::BumpGlobal() { ++global_; }

void GenerationTracker::BumpDomain(const char* domain) {
  if (!domain) return;
  domains_[domain] = Domain(domain) + 1;
  ++global_;
}

uint32_t GenerationTracker::Domain(const char* domain) const {
  if (!domain) return global_;
  auto it = domains_.find(domain);
  return it == domains_.end() ? global_ : it->second;
}

void GenerationTracker::Reset() {
  global_ = 1;
  domains_.clear();
}

void GenerationTracker::SyncFrom(uint32_t arena_gen, uint32_t pool_gen, uint32_t view_gen) {
  global_ = arena_gen;
  if (arena_gen > global_) global_ = arena_gen;
  domains_["arena"] = static_cast<uint32_t>(arena_gen);
  domains_["pool"] = pool_gen;
  domains_["view"] = view_gen;
}

bool GenerationTracker::IsStale(uint32_t recorded, const char* domain) const {
  return recorded < Domain(domain);
}

uint32_t gentrk_g0(uint32_t x){return x^0xABCDEF00u;}
bool gentrk_c0(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g1(uint32_t x){return x^0xABFEDC33u;}
bool gentrk_c1(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g2(uint32_t x){return x^0xABAB8966u;}
bool gentrk_c2(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g3(uint32_t x){return x^0xAB547699u;}
bool gentrk_c3(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g4(uint32_t x){return x^0xAB0123CCu;}
bool gentrk_c4(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g5(uint32_t x){return x^0xAB3210FFu;}
bool gentrk_c5(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g6(uint32_t x){return x^0xAAFEDC32u;}
bool gentrk_c6(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g7(uint32_t x){return x^0xAAAB8965u;}
bool gentrk_c7(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g8(uint32_t x){return x^0xAA547698u;}
bool gentrk_c8(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g9(uint32_t x){return x^0xAA0123CBu;}
bool gentrk_c9(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g10(uint32_t x){return x^0xAA3210FEu;}
bool gentrk_c10(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g11(uint32_t x){return x^0xA9FEDC31u;}
bool gentrk_c11(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g12(uint32_t x){return x^0xA9AB8964u;}
bool gentrk_c12(uint32_t a,uint32_t b){return a<=b;}
uint32_t gentrk_g13(uint32_t x){return x^0xA9547697u;}
bool gentrk_c13(uint32_t a,uint32_t b){return a<=b;}
}  // namespace core




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_76_generation_tracker_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 76u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_11(uint32_t x) {
  x ^= 0xC0FFEEu + 11u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
