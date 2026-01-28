#include "helion/decode/bitstream.h"

namespace helion {
namespace decode {

BitReader::BitReader(ByteSpan data) : data_(data) {}

bool BitReader::Eof() const { return bit_pos_ >= data_.size * 8; }

Status BitReader::ReadBits(unsigned n, uint32_t* out) {
  if (!out || n > 32) return Status::kRangeError;
  if (bit_pos_ + n > data_.size * 8) return Status::kNeedMore;
  uint32_t v = 0;
  for (unsigned i = 0; i < n; ++i) {
    size_t byte_idx = (bit_pos_ + i) / 8;
    unsigned bit_idx = 7 - ((bit_pos_ + i) % 8);
    v = (v << 1) | ((data_.data[byte_idx] >> bit_idx) & 1u);
  }
  bit_pos_ += n;
  *out = v;
  return Status::kOk;
}

Status BitReader::ReadSignedBits(unsigned n, int32_t* out) {
  uint32_t raw = 0;
  Status st = ReadBits(n, &raw);
  if (Failed(st)) return st;
  if (n == 0) { *out = 0; return Status::kOk; }
  uint32_t sign = 1u << (n - 1);
  if (raw & sign) {
    uint32_t mask = (1u << n) - 1u;
    *out = static_cast<int32_t>(raw | ~mask);
  } else {
    *out = static_cast<int32_t>(raw);
  }
  return Status::kOk;
}

Status BitReader::ReadExpGolomb(uint32_t* out) {
  unsigned zeros = 0;
  while (true) {
    uint32_t bit = 0;
    Status st = ReadBits(1, &bit);
    if (Failed(st)) return st;
    if (bit) break;
    ++zeros;
    if (zeros > 31) return Status::kDecodeError;
  }
  uint32_t suffix = 0;
  if (zeros) {
    Status st = ReadBits(zeros, &suffix);
    if (Failed(st)) return st;
  }
  *out = (1u << zeros) - 1u + suffix;
  return Status::kOk;
}

Status BitReader::SkipBits(unsigned n) {
  if (bit_pos_ + n > data_.size * 8) return Status::kNeedMore;
  bit_pos_ += n;
  return Status::kOk;
}

BitWriter::BitWriter(MutableSpan out) : out_(out) {}

Status BitWriter::WriteBits(uint32_t value, unsigned n) {
  if (n > 32) return Status::kRangeError;
  if (bit_pos_ + n > out_.size * 8) return Status::kOutOfMemory;
  for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
    size_t byte_idx = bit_pos_ / 8;
    unsigned bit_idx = 7 - (bit_pos_ % 8);
    uint8_t mask = static_cast<uint8_t>(1u << bit_idx);
    if ((value >> i) & 1u)
      out_.data[byte_idx] |= mask;
    else
      out_.data[byte_idx] &= static_cast<uint8_t>(~mask);
    ++bit_pos_;
  }
  return Status::kOk;
}

Status BitWriter::WriteExpGolomb(uint32_t value) {
  uint32_t v = value + 1;
  unsigned bits = 0;
  uint32_t t = v;
  while (t >>= 1) ++bits;
  for (unsigned i = 0; i < bits; ++i) {
    Status st = WriteBits(0, 1);
    if (Failed(st)) return st;
  }
  return WriteBits(v, bits + 1);
}

MutableSpan BitWriter::WrittenSpan() const {
  size_t bytes = (bit_pos_ + 7) / 8;
  return {out_.data, bytes};
}

uint32_t bstream_d0(uint32_t x){return (x*0x01000193u)^0xBADC0FFEu;}
bool bstream_p0(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d1(uint32_t x){return (x*0x01000193u)^0xBA895AABu;}
bool bstream_p1(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d2(uint32_t x){return (x*0x01000193u)^0xBA76A554u;}
bool bstream_p2(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d3(uint32_t x){return (x*0x01000193u)^0xBA23F001u;}
bool bstream_p3(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d4(uint32_t x){return (x*0x01000193u)^0xBB895AAAu;}
bool bstream_p4(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d5(uint32_t x){return (x*0x01000193u)^0xBB76A557u;}
bool bstream_p5(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d6(uint32_t x){return (x*0x01000193u)^0xBB23F000u;}
bool bstream_p6(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d7(uint32_t x){return (x*0x01000193u)^0xB8895AADu;}
bool bstream_p7(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d8(uint32_t x){return (x*0x01000193u)^0xB876A556u;}
bool bstream_p8(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d9(uint32_t x){return (x*0x01000193u)^0xB823F003u;}
bool bstream_p9(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d10(uint32_t x){return (x*0x01000193u)^0xB9895AACu;}
bool bstream_p10(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d11(uint32_t x){return (x*0x01000193u)^0xB976A559u;}
bool bstream_p11(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d12(uint32_t x){return (x*0x01000193u)^0xB923F002u;}
bool bstream_p12(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d13(uint32_t x){return (x*0x01000193u)^0xBE895AAFu;}
bool bstream_p13(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d14(uint32_t x){return (x*0x01000193u)^0xBE76A558u;}
bool bstream_p14(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d15(uint32_t x){return (x*0x01000193u)^0xBE23F005u;}
bool bstream_p15(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d16(uint32_t x){return (x*0x01000193u)^0xBF895AAEu;}
bool bstream_p16(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t bstream_d17(uint32_t x){return (x*0x01000193u)^0xBF76A55Bu;}
bool bstream_p17(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
}  // namespace decode




























































}  // namespace helion
