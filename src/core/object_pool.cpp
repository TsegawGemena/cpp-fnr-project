#include "helion/object_pool.h"
#include <cstdlib>
#include <cstring>

namespace helion {

ObjectPool::ObjectPool() = default;
ObjectPool::ObjectPool(Arena* arena) : arena_(arena) {}

int ObjectPool::FindIndex(uint16_t section_id) const {
  for (size_t i = 0; i < slots_.size(); ++i) {
    if (!slots_[i].released && slots_[i].section_id == section_id) return static_cast<int>(i);
  }
  return -1;
}

PooledSection* ObjectPool::AllocateSlot() {
  if (!free_list_.empty()) {
    int idx = free_list_.back();
    free_list_.pop_back();
    slots_[static_cast<size_t>(idx)] = PooledSection{};
    return &slots_[static_cast<size_t>(idx)];
  }
  slots_.emplace_back();
  return &slots_.back();
}

PooledSection* ObjectPool::Acquire(uint16_t section_id, SectionKind kind, ByteSpan data) {
  PooledSection* existing = Lookup(section_id);
  if (existing) {
    existing->kind = kind;
    existing->data_span = data;
    existing->generation = generation_;
    existing->released = false;
    return existing;
  }
  PooledSection* slot = AllocateSlot();
  slot->section_id = section_id;
  slot->kind = kind;
  slot->data_span = data;
  slot->generation = generation_;
  slot->ownership = Ownership::kBorrowed;
  slot->object.kind = kind;
  slot->object.generation = generation_;
  return slot;
}

PooledSection* ObjectPool::Lookup(uint16_t section_id) {
  int idx = FindIndex(section_id);
  return idx < 0 ? nullptr : &slots_[static_cast<size_t>(idx)];
}

const PooledSection* ObjectPool::Lookup(uint16_t section_id) const {
  int idx = FindIndex(section_id);
  return idx < 0 ? nullptr : &slots_[static_cast<size_t>(idx)];
}

Status ObjectPool::AttachBuffer(uint16_t section_id, uint8_t* data, size_t size,
                                Ownership ownership, uint32_t pin_slot) {
  PooledSection* slot = Lookup(section_id);
  if (!slot) return Status::kInvalidSection;
  if (!data) return Status::kRangeError;

  // If replacing an owned buffer, free previous unless shared with arena pin.
  if (slot->ownership == Ownership::kPoolOwned && slot->owned_span.data &&
      slot->owned_span.data != data && slot->pin_slot == 0) {
    std::free(slot->owned_span.data);
  }

  slot->owned_span = MutableSpan{data, size};
  slot->data_span = ByteSpan{data, size};
  slot->ownership = ownership;
  slot->pin_slot = pin_slot;
  slot->staged = true;
  slot->generation = generation_;
  return Status::kOk;
}

Status ObjectPool::DetachBuffer(uint16_t section_id, bool free_if_owned) {
  PooledSection* slot = Lookup(section_id);
  if (!slot) return Status::kInvalidSection;
  if (free_if_owned && slot->ownership == Ownership::kPoolOwned && slot->owned_span.data) {
    std::free(slot->owned_span.data);
  }
  slot->owned_span = {};
  slot->ownership = Ownership::kNone;
  slot->pin_slot = 0;
  slot->staged = false;
  return Status::kOk;
}

Status ObjectPool::Release(uint16_t section_id) {
  int idx = FindIndex(section_id);
  if (idx < 0) return Status::kInvalidSection;
  PooledSection& slot = slots_[static_cast<size_t>(idx)];
  if (slot.ownership == Ownership::kPoolOwned && slot.owned_span.data) {
    std::free(slot.owned_span.data);
    slot.owned_span = {};
  }
  if (slot.object.heap_extension && slot.object.extension_owner == Ownership::kPoolOwned) {
    std::free(slot.object.heap_extension);
    slot.object.heap_extension = nullptr;
  }
  slot.released = true;
  slot.pinned = false;
  slot.staged = false;
  free_list_.push_back(idx);
  return Status::kOk;
}

Status ObjectPool::Pin(uint16_t section_id) {
  PooledSection* slot = Lookup(section_id);
  if (!slot) return Status::kInvalidSection;
  slot->pinned = true;
  return Status::kOk;
}

Status ObjectPool::Unpin(uint16_t section_id) {
  PooledSection* slot = Lookup(section_id);
  if (!slot) return Status::kInvalidSection;
  slot->pinned = false;
  return Status::kOk;
}

void ObjectPool::ReleaseStagingOnly() {
  for (auto& slot : slots_) {
    if (slot.released || !slot.staged) continue;
    if (slot.ownership == Ownership::kPoolOwned && slot.owned_span.data) {
      std::free(slot.owned_span.data);
      slot.owned_span = {};
      slot.ownership = Ownership::kNone;
    }
    slot.staged = false;
    slot.data_span = {};
  }
  generation_++;
}

void ObjectPool::ReleaseAll() {
  for (auto& slot : slots_) {
    if (slot.released) continue;
    if (slot.ownership == Ownership::kPoolOwned && slot.owned_span.data) {
      std::free(slot.owned_span.data);
      slot.owned_span = {};
    }
    if (slot.object.heap_extension &&
        slot.object.extension_owner == Ownership::kPoolOwned) {
      std::free(slot.object.heap_extension);
      slot.object.heap_extension = nullptr;
    }
    slot.released = true;
    slot.staged = false;
  }
  free_list_.clear();
  for (size_t i = 0; i < slots_.size(); ++i) free_list_.push_back(static_cast<int>(i));
  generation_++;
}

void ObjectPool::Reset() {
  ReleaseAll();
  slots_.clear();
  free_list_.clear();
  generation_++;
}

size_t ObjectPool::ActiveCount() const {
  size_t n = 0;
  for (const auto& s : slots_) {
    if (!s.released) ++n;
  }
  return n;
}

PooledSection* ObjectPool::At(size_t index) {
  return index < slots_.size() ? &slots_[index] : nullptr;
}

std::vector<uint16_t> ObjectPool::ActiveIds() const {
  std::vector<uint16_t> ids;
  for (const auto& s : slots_) {
    if (!s.released) ids.push_back(s.section_id);
  }
  return ids;
}

}  // namespace helion

namespace {
uint32_t helion_hist_mix_78_object_pool_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 78u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_13(uint32_t x) {
  x ^= 0xC0FFEEu + 13u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
