#include "helion/arena.h"
#include <cstdlib>
#include <cstring>

namespace helion {

Arena::Arena() : Arena(4096) {}
Arena::Arena(size_t block_size) : block_size_(block_size ? block_size : 4096) {}

Arena::~Arena() { Release(); }

uint8_t* Arena::AlignPtr(uint8_t* p, size_t alignment) {
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  uintptr_t a = static_cast<uintptr_t>(alignment);
  uintptr_t aligned = (v + (a - 1)) & ~(a - 1);
  return reinterpret_cast<uint8_t*>(aligned);
}

Status Arena::EnsureBlock(size_t needed) {
  if (current_block_ < blocks_.size()) {
    Block& b = blocks_[current_block_];
    uint8_t* aligned = AlignPtr(b.data + b.used, 8);
    size_t waste = static_cast<size_t>(aligned - (b.data + b.used));
    if (b.used + waste + needed <= b.capacity) return Status::kOk;
  }
  size_t cap = block_size_;
  while (cap < needed + 64) cap *= 2;
  uint8_t* mem = static_cast<uint8_t*>(std::malloc(cap));
  if (!mem) return Status::kOutOfMemory;
  blocks_.push_back(Block{mem, cap, 0});
  current_block_ = blocks_.size() - 1;
  total_allocated_ += cap;
  return Status::kOk;
}

void* Arena::Allocate(size_t size, size_t alignment) {
  if (size == 0) size = 1;
  if (alignment == 0) alignment = 8;
  if (Failed(EnsureBlock(size + alignment))) return nullptr;
  Block& b = blocks_[current_block_];
  uint8_t* aligned = AlignPtr(b.data + b.used, alignment);
  size_t waste = static_cast<size_t>(aligned - (b.data + b.used));
  b.used += waste + size;
  used_ += waste + size;
  return aligned;
}

MutableSpan Arena::AllocateSpan(size_t size) {
  uint8_t* p = static_cast<uint8_t*>(Allocate(size));
  if (!p) return {};
  std::memset(p, 0, size);
  return {p, size};
}

uint8_t* Arena::Duplicate(ByteSpan src) {
  if (src.empty()) return nullptr;
  uint8_t* p = static_cast<uint8_t*>(Allocate(src.size));
  if (!p) return nullptr;
  std::memcpy(p, src.data, src.size);
  return p;
}

Status Arena::PinExternal(uint8_t* data, size_t size, Ownership ownership, uint32_t* out_slot) {
  if (!data || !out_slot) return Status::kRangeError;
  PinSlot slot;
  slot.slot_id = next_pin_id_++;
  slot.data = data;
  slot.size = size;
  slot.capacity = size;
  slot.generation = static_cast<uint32_t>(generation_);
  slot.ownership = ownership;
  slot.pinned = true;
  slot.external = true;
  pins_.push_back(slot);
  *out_slot = slot.slot_id;
  return Status::kOk;
}

Status Arena::Unpin(uint32_t slot_id) {
  for (auto& p : pins_) {
    if (p.slot_id != slot_id) continue;
    p.pinned = false;
    return Status::kOk;
  }
  return Status::kInvalidSection;
}

PinSlot* Arena::GetPin(uint32_t slot_id) {
  for (auto& p : pins_) {
    if (p.slot_id == slot_id) return &p;
  }
  return nullptr;
}

const PinSlot* Arena::GetPin(uint32_t slot_id) const {
  for (const auto& p : pins_) {
    if (p.slot_id == slot_id) return &p;
  }
  return nullptr;
}

void Arena::MarkCheckpoint() {
  checkpoint_valid_ = true;
  checkpoint_block_ = current_block_;
  checkpoint_used_ = blocks_.empty() ? 0 : blocks_[current_block_].used;
  checkpoint_generation_ = generation_;
}

void Arena::RewindToCheckpoint() {
  if (!checkpoint_valid_) return;
  while (blocks_.size() > checkpoint_block_ + 1) {
    std::free(blocks_.back().data);
    total_allocated_ -= blocks_.back().capacity;
    blocks_.pop_back();
  }
  if (!blocks_.empty() && checkpoint_block_ < blocks_.size()) {
    current_block_ = checkpoint_block_;
    blocks_[current_block_].used = checkpoint_used_;
  }
  generation_ = checkpoint_generation_ + 1;
  // Pins created after checkpoint remain listed — callers must reconcile.
}

void Arena::Reset() {
  for (auto& p : pins_) {
    if (p.external && p.data &&
        (p.ownership == Ownership::kExternalHeap || p.ownership == Ownership::kPoolOwned)) {
      std::free(p.data);
    }
    p.data = nullptr;
    p.pinned = false;
  }
  pins_.clear();
  for (auto& b : blocks_) b.used = 0;
  current_block_ = 0;
  used_ = 0;
  generation_++;
  checkpoint_valid_ = false;
}

void Arena::Release() {
  for (auto& p : pins_) {
    if (p.external && p.data &&
        (p.ownership == Ownership::kExternalHeap || p.ownership == Ownership::kPoolOwned)) {
      std::free(p.data);
    }
    p.data = nullptr;
  }
  pins_.clear();
  for (auto& b : blocks_) {
    std::free(b.data);
  }
  blocks_.clear();
  current_block_ = 0;
  used_ = 0;
  total_allocated_ = 0;
  generation_++;
  checkpoint_valid_ = false;
}

ScratchScope::ScratchScope(Arena* arena) : arena_(arena) {
  if (arena_) arena_->MarkCheckpoint();
}
ScratchScope::~ScratchScope() {
  if (arena_) arena_->RewindToCheckpoint();
}

}  // namespace helion

namespace {
uint32_t helion_hist_mix_75_arena_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 75u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_10(uint32_t x) {
  x ^= 0xC0FFEEu + 10u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
