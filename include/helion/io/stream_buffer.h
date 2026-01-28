#pragma once
#include "helion/buffer.h"
#include "helion/span.h"
#include "helion/status.h"
#include <cstdint>
#include <vector>

namespace helion {
namespace io {

class StreamBuffer {
 public:
  StreamBuffer();
  explicit StreamBuffer(size_t initial_cap);
  void Clear();
  Status Append(ByteSpan chunk);
  Status AppendFrom(const uint8_t* p, size_t n);
  Status Reserve(size_t cap);
  ByteSpan Data() const;
  size_t Size() const { return buf_.size(); }
  size_t Capacity() const { return buf_.capacity(); }
  Status Consume(size_t n);
  Status Peek(size_t off, ByteSpan* out) const;
  Status ReadU32LE(size_t off, uint32_t* out) const;
  Status Compact();
  GrowableBuffer& Raw() { return buf_; }
  const GrowableBuffer& Raw() const { return buf_; }
  std::vector<ByteSpan> Slice(size_t chunk_size) const;

 private:
  GrowableBuffer buf_;
  size_t read_pos_ = 0;
};

class RingBuffer {
 public:
  explicit RingBuffer(size_t cap);
  Status Write(ByteSpan data);
  Status Read(size_t n, std::vector<uint8_t>* out);
  size_t Available() const;
  size_t FreeSpace() const;
  void Reset();

 private:
  std::vector<uint8_t> data_;
  size_t head_ = 0;
  size_t tail_ = 0;
  size_t count_ = 0;
};

}  // namespace io
}  // namespace helion
