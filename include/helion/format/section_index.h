#pragma once
#include "helion/buffer.h"
#include "helion/format/format_spec.h"
#include "helion/format/section_table.h"
#include <unordered_map>
#include <vector>

namespace helion {
namespace format {

struct IndexCacheEntry {
  uint16_t section_id = 0;
  uint32_t offset = 0;
  uint32_t length = 0;
  uint32_t generation = 0;
  const SectionDesc* descriptor = nullptr;
  BufferView cached_view;
  bool live = false;
};

class SectionIndex {
 public:
  SectionIndex();
  Status Build(const SectionTable& table, uint32_t generation);
  Status Rebuild(const std::vector<SectionDesc>& descs, uint32_t generation);
  const IndexCacheEntry* Lookup(uint16_t section_id) const;
  IndexCacheEntry* LookupMutable(uint16_t section_id);
  Status CacheView(uint16_t section_id, BufferView view);
  void InvalidateView(uint16_t section_id);
  void ClearViews();
  size_t Size() const { return entries_.size(); }
  uint32_t Generation() const { return generation_; }
  std::vector<uint16_t> SectionIds() const;
  const SectionDesc* Descriptor(uint16_t section_id) const;

 private:
  std::vector<IndexCacheEntry> entries_;
  std::unordered_map<uint16_t, size_t> map_;
  uint32_t generation_ = 0;
};

}  // namespace format
}  // namespace helion
