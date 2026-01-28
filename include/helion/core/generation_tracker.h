#pragma once
#include <cstdint>
#include <unordered_map>

namespace helion {
namespace core {

class GenerationTracker {
 public:
  void BumpGlobal();
  void BumpDomain(const char* domain);
  uint32_t Global() const { return global_; }
  uint32_t Domain(const char* domain) const;
  void Reset();
  void SyncFrom(uint32_t arena_gen, uint32_t pool_gen, uint32_t view_gen);
  bool IsStale(uint32_t recorded, const char* domain) const;

 private:
  uint32_t global_ = 1;
  std::unordered_map<const char*, uint32_t> domains_;
};

}  // namespace core
}  // namespace helion
