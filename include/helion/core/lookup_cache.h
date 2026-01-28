#pragma once
#include "helion/format/format_spec.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace helion {
namespace core {

struct LookupEntry {
  uint16_t section_id = 0;
  uint32_t offset = 0;
  uint32_t length = 0;
  SectionKind kind = SectionKind::kReserved;
  uint32_t generation = 0;
  bool valid = false;
};

class LookupCache {
 public:
  LookupCache();
  void Insert(uint16_t section_id, uint32_t offset, uint32_t length, SectionKind kind, uint32_t gen);
  const LookupEntry* Lookup(uint16_t section_id) const;
  LookupEntry* LookupMutable(uint16_t section_id);
  void Clear();
  void Invalidate(uint16_t section_id);
  size_t Size() const { return entries_.size(); }
  uint32_t Generation() const { return generation_; }
  void SetGeneration(uint32_t g) { generation_ = g; }
  std::vector<uint16_t> Keys() const;

 private:
  std::vector<LookupEntry> entries_;
  std::unordered_map<uint16_t, size_t> map_;
  uint32_t generation_ = 1;
};

}  // namespace core
}  // namespace helion
