#pragma once
#include "helion/buffer.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace helion {
namespace core {

struct StagedViewEntry {
  uint16_t section_id = 0;
  BufferView view;
  uint32_t source_generation = 0;
  uint32_t arena_generation = 0;
  bool staged = false;
  bool from_pool = false;
};

class ViewCache {
 public:
  ViewCache();
  void StageView(uint16_t section_id, BufferView view, uint32_t arena_gen, bool from_pool);
  bool UsesStagedSource() const { return uses_staged_; }
  void SetUsesStagedSource(bool v) { uses_staged_ = v; }
  BufferView GetView(uint16_t section_id) const;
  const StagedViewEntry* Find(uint16_t section_id) const;
  StagedViewEntry* FindMutable(uint16_t section_id);
  void Invalidate(uint16_t section_id);
  void InvalidateAll();
  void BumpGeneration(uint32_t new_gen);
  uint32_t Generation() const { return generation_; }
  size_t Size() const { return entries_.size(); }
  std::vector<uint16_t> StagedSectionIds() const;

 private:
  std::vector<StagedViewEntry> entries_;
  std::unordered_map<uint16_t, size_t> index_;
  uint32_t generation_ = 1;
  bool uses_staged_ = false;
};

}  // namespace core
}  // namespace helion

