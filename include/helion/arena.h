#pragma once
#include "helion/span.h"
#include "helion/types.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace helion {

struct PinSlot {
  uint32_t slot_id = 0;
  uint8_t* data = nullptr;
  size_t size = 0;
  size_t capacity = 0;
  uint32_t generation = 0;
  Ownership ownership = Ownership::kNone;
  bool pinned = false;
  bool external = false;
};

class Arena {
 public:
  Arena();
  explicit Arena(size_t block_size);
  ~Arena();

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  void* Allocate(size_t size, size_t alignment = 8);
  template <typename T>
  T* AllocateArray(size_t count) {
    return static_cast<T*>(Allocate(sizeof(T) * count, alignof(T)));
  }

  MutableSpan AllocateSpan(size_t size);
  uint8_t* Duplicate(ByteSpan src);

  // Pin an externally allocated buffer into the arena tracker.
  // When ownership == kExternalHeap / kPoolOwned, Release()/Reset() may free it.
  Status PinExternal(uint8_t* data, size_t size, Ownership ownership, uint32_t* out_slot);
  Status Unpin(uint32_t slot_id);
  PinSlot* GetPin(uint32_t slot_id);
  const PinSlot* GetPin(uint32_t slot_id) const;

  void MarkCheckpoint();
  void RewindToCheckpoint();
  void Reset();
  void Release();

  size_t BytesUsed() const { return used_; }
  size_t Generation() const { return generation_; }
  size_t PinCount() const { return pins_.size(); }

 private:
  struct Block {
    uint8_t* data = nullptr;
    size_t capacity = 0;
    size_t used = 0;
  };

  Status EnsureBlock(size_t needed);
  static uint8_t* AlignPtr(uint8_t* p, size_t alignment);

  size_t block_size_ = 4096;
  size_t used_ = 0;
  size_t total_allocated_ = 0;
  size_t generation_ = 1;
  std::vector<Block> blocks_;
  size_t current_block_ = 0;
  bool checkpoint_valid_ = false;
  size_t checkpoint_block_ = 0;
  size_t checkpoint_used_ = 0;
  size_t checkpoint_generation_ = 0;
  std::vector<PinSlot> pins_;
  uint32_t next_pin_id_ = 1;
};

class ScratchScope {
 public:
  explicit ScratchScope(Arena* arena);
  ~ScratchScope();
 private:
  Arena* arena_;
};

}  // namespace helion


