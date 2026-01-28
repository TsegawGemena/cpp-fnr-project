#pragma once
#include "helion/span.h"
#include "helion/status.h"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace helion {

class GrowableBuffer {
 public:
  GrowableBuffer() = default;
  explicit GrowableBuffer(size_t cap);
  GrowableBuffer(GrowableBuffer&& other) noexcept;
  GrowableBuffer& operator=(GrowableBuffer&& other) noexcept;
  ~GrowableBuffer();

  GrowableBuffer(const GrowableBuffer&) = delete;
  GrowableBuffer& operator=(const GrowableBuffer&) = delete;

  Status Reserve(size_t n);
  Status Append(ByteSpan bytes);
  Status AppendByte(uint8_t b);
  void Clear();
  void ShrinkToFit();

  uint8_t* data() { return data_; }
  const uint8_t* data() const { return data_; }
  size_t size() const { return size_; }
  size_t capacity() const { return capacity_; }
  ByteSpan span() const { return {data_, size_}; }
  MutableSpan mutable_span() { return {data_, size_}; }
  bool owns() const { return owns_; }

  // Adopt external heap memory. Caller transfers ownership when own=true.
  void Adopt(uint8_t* p, size_t n, size_t cap, bool own);
  uint8_t* Release();

 private:
  uint8_t* data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;
  bool owns_ = true;
};

class BufferView {
 public:
  BufferView() = default;
  BufferView(const uint8_t* p, size_t n, uint32_t gen = 0)
      : data_(p), size_(n), generation_(gen) {}

  const uint8_t* data() const { return data_; }
  size_t size() const { return size_; }
  uint32_t generation() const { return generation_; }
  bool valid() const { return data_ != nullptr; }
  ByteSpan span() const { return {data_, size_}; }
  void Invalidate() { data_ = nullptr; size_ = 0; generation_ = 0; }
  void Reset(const uint8_t* p, size_t n, uint32_t gen) {
    data_ = p;
    size_ = n;
    generation_ = gen;
  }

 private:
  const uint8_t* data_ = nullptr;
  size_t size_ = 0;
  uint32_t generation_ = 0;
};

}  // namespace helion
