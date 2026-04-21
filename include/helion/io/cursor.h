#pragma once
#include "helion/span.h"
#include "helion/status.h"
#include <cstdint>

namespace helion {
namespace io {

class ReadCursor {
 public:
  ReadCursor() = default;
  explicit ReadCursor(ByteSpan span);
  void Reset(ByteSpan span);
  ByteSpan Remaining() const;
  size_t Position() const { return pos_; }
  size_t Size() const { return span_.size; }
  bool Eof() const { return pos_ >= span_.size; }
  Status Require(size_t n);
  Status ReadU8(uint8_t* out);
  Status ReadU16LE(uint16_t* out);
  Status ReadU32LE(uint32_t* out);
  Status ReadU64LE(uint64_t* out);
  Status ReadBytes(size_t n, ByteSpan* out);
  Status Skip(size_t n);
  Status Seek(size_t pos);
  Status PeekU32LE(uint32_t* out);
  Status ReadVarUInt(uint64_t* out);
  Status ReadLengthPrefixed(ByteSpan* out, size_t max_len);

 private:
  ByteSpan span_;
  size_t pos_ = 0;
};

class WriteCursor {
 public:
  WriteCursor() = default;
  explicit WriteCursor(MutableSpan span);
  void Reset(MutableSpan span);
  size_t Position() const { return pos_; }
  size_t Capacity() const { return span_.size; }
  Status Require(size_t n);
  Status WriteU8(uint8_t v);
  Status WriteU16LE(uint16_t v);
  Status WriteU32LE(uint32_t v);
  Status WriteU64LE(uint64_t v);
  Status WriteBytes(ByteSpan data);
  Status WriteVarUInt(uint64_t v);
  Status WriteLengthPrefixed(ByteSpan data);
  MutableSpan WrittenSpan() const;

 private:
  MutableSpan span_;
  size_t pos_ = 0;
};

}  // namespace io
}  // namespace helion

