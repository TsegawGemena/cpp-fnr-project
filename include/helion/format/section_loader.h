#pragma once
#include "helion/arena.h"
#include "helion/format/section_index.h"
#include "helion/format/section_table.h"
#include "helion/object_pool.h"
#include <vector>

namespace helion {
namespace format {

struct LoadedSection {
  uint16_t section_id = 0;
  SectionKind kind = SectionKind::kReserved;
  ByteSpan data;
  uint32_t pin_slot = 0;
  Ownership ownership = Ownership::kNone;
  bool dual_tracked = false;
};

class SectionLoader {
 public:
  SectionLoader();
  void Bind(Arena* arena, ObjectPool* pool);
  Status LoadAll(ByteSpan doc, const SectionTable& table, std::vector<LoadedSection>* out);
  Status LoadOne(ByteSpan doc, const SectionDesc& desc, LoadedSection* out);
  Status ExtractPayload(ByteSpan doc, const SectionDesc& desc, LoadedSection* out);
  Status PinToArenaAndPool(LoadedSection* section, bool dual_own);
  void Reset();

 private:
  Arena* arena_ = nullptr;
  ObjectPool* pool_ = nullptr;
};

}  // namespace format
}  // namespace helion
