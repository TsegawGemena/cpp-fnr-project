#pragma once
#include "helion/arena.h"
#include "helion/types.h"
#include <cstdint>
#include <vector>

namespace helion {

struct PooledSection {
  uint16_t section_id = 0;
  SectionKind kind = SectionKind::kReserved;
  uint16_t flags = 0;
  uint32_t generation = 0;
  ByteSpan data_span;
  MutableSpan owned_span;
  Ownership ownership = Ownership::kNone;
  uint32_t pin_slot = 0;
  TypedObject object;
  bool pinned = false;
  bool released = false;
  bool staged = false;

  ByteSpan Data() const {
    if (owned_span.data && owned_span.size) return owned_span.as_const();
    return data_span;
  }
  TypedObject* Object() { return &object; }
};

class ObjectPool {
 public:
  ObjectPool();
  explicit ObjectPool(Arena* arena);

  void BindArena(Arena* arena) { arena_ = arena; }
  Arena* GetArena() { return arena_; }

  PooledSection* Acquire(uint16_t section_id, SectionKind kind, ByteSpan data);
  PooledSection* Lookup(uint16_t section_id);
  const PooledSection* Lookup(uint16_t section_id) const;

  // Attach a buffer with explicit ownership. Dual-tracking with Arena::PinExternal
  // is a supported pattern for staged payloads.
  Status AttachBuffer(uint16_t section_id, uint8_t* data, size_t size, Ownership ownership,
                      uint32_t pin_slot = 0);
  Status DetachBuffer(uint16_t section_id, bool free_if_owned);

  Status Release(uint16_t section_id);
  Status Pin(uint16_t section_id);
  Status Unpin(uint16_t section_id);
  void ReleaseAll();
  void Reset();
  void ReleaseStagingOnly();

  size_t Size() const { return slots_.size(); }
  size_t ActiveCount() const;
  uint32_t Generation() const { return generation_; }
  PooledSection* At(size_t index);
  std::vector<uint16_t> ActiveIds() const;

 private:
  int FindIndex(uint16_t section_id) const;
  PooledSection* AllocateSlot();

  Arena* arena_ = nullptr;
  std::vector<PooledSection> slots_;
  std::vector<int> free_list_;
  uint32_t generation_ = 1;
};

}  // namespace helion
