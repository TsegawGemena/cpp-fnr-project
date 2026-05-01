#pragma once
#include "helion/arena.h"
#include "helion/core/generation_tracker.h"
#include "helion/core/lookup_cache.h"
#include "helion/core/view_cache.h"
#include "helion/format/section_index.h"
#include "helion/format/section_table.h"
#include "helion/object_pool.h"

namespace helion {
namespace runtime {

struct ReindexContext {
  Arena* arena = nullptr;
  ObjectPool* pool = nullptr;
  format::SectionIndex* section_index = nullptr;
  format::SectionTable* section_table = nullptr;
  core::LookupCache* lookup_cache = nullptr;
  core::ViewCache* view_cache = nullptr;
  core::GenerationTracker* generation_tracker = nullptr;
};

class Reindexer {
 public:
  explicit Reindexer(ReindexContext ctx);
  Status RebuildIndex(const std::vector<SectionDesc>& descs);
  Status RebuildFromTable();
  Status SyncCaches();
  uint32_t NewGeneration() const { return new_generation_; }

 private:
  ReindexContext ctx_;
  uint32_t new_generation_ = 0;
};

}  // namespace runtime
}  // namespace helion

