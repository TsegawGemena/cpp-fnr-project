#pragma once
#include "helion/arena.h"
#include "helion/core/lookup_cache.h"
#include "helion/core/view_cache.h"
#include "helion/format/section_index.h"
#include "helion/object_pool.h"

namespace helion {
namespace runtime {

struct RecoveryContext {
  Arena* arena = nullptr;
  ObjectPool* pool = nullptr;
  core::ViewCache* view_cache = nullptr;
  core::LookupCache* lookup_cache = nullptr;
  format::SectionIndex* section_index = nullptr;
};

class RecoveryManager {
 public:
  explicit RecoveryManager(RecoveryContext ctx);
  Status ReleaseStaging();
  Status ReleaseStagingOnly();
  Status ReleasePools();
  Status RewindArena();
  Status RunRecovery();
  Status ResumeAfterRecovery();
  bool Recovered() const { return recovered_; }
  uint32_t RecoveryGeneration() const { return recovery_gen_; }

 private:
  RecoveryContext ctx_;
  bool recovered_ = false;
  uint32_t recovery_gen_ = 0;
};

}  // namespace runtime
}  // namespace helion

