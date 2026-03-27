#pragma once
#include "helion/span.h"
#include "helion/status.h"
#include <cstdint>

namespace helion {
namespace decode {

class BitReader {
 public:
  explicit BitReader(ByteSpan data);
  Status ReadBits(unsigned n, uint32_t* out);
  Status ReadSignedBits(unsigned n, int32_t* out);
  Status ReadExpGolomb(uint32_t* out);
  Status SkipBits(unsigned n);
  size_t BitPosition() const { return bit_pos_; }
  bool Eof() const;

 private:
  ByteSpan data_;
  size_t bit_pos_ = 0;
};

class BitWriter {
 public:
  explicit BitWriter(MutableSpan out);
  Status WriteBits(uint32_t value, unsigned n);
  Status WriteExpGolomb(uint32_t value);
  size_t BitPosition() const { return bit_pos_; }
  MutableSpan WrittenSpan() const;

 private:
  MutableSpan out_;
  size_t bit_pos_ = 0;
};

}  // namespace decode
}  // namespace helion

