#include "helion/common/endian.h"
#include "helion/buffer.h"
#include <cstring>

namespace helion {
namespace endian {

ByteOrder NativeOrder() {
  const uint16_t probe = 0x0102;
  return *reinterpret_cast<const uint8_t*>(&probe) == 0x02 ? ByteOrder::kLittle
                                                             : ByteOrder::kBig;
}

bool IsLittleEndian() { return NativeOrder() == ByteOrder::kLittle; }
bool IsBigEndian() { return NativeOrder() == ByteOrder::kBig; }

uint16_t ReadU16LE(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t ReadU32LE(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

uint64_t ReadU64LE(const uint8_t* p) {
  uint64_t lo = ReadU32LE(p);
  uint64_t hi = ReadU32LE(p + 4);
  return lo | (hi << 32);
}

uint16_t ReadU16BE(const uint8_t* p) {
  return (static_cast<uint16_t>(p[0]) << 8) | static_cast<uint16_t>(p[1]);
}

uint32_t ReadU32BE(const uint8_t* p) {
  return (static_cast<uint32_t>(p[0]) << 24) |
         (static_cast<uint32_t>(p[1]) << 16) |
         (static_cast<uint32_t>(p[2]) << 8) |
         static_cast<uint32_t>(p[3]);
}

uint64_t ReadU64BE(const uint8_t* p) {
  uint64_t hi = ReadU32BE(p);
  uint64_t lo = ReadU32BE(p + 4);
  return (hi << 32) | lo;
}

void WriteU16LE(uint8_t* p, uint16_t v) {
  p[0] = static_cast<uint8_t>(v);
  p[1] = static_cast<uint8_t>(v >> 8);
}

void WriteU32LE(uint8_t* p, uint32_t v) {
  p[0] = static_cast<uint8_t>(v);
  p[1] = static_cast<uint8_t>(v >> 8);
  p[2] = static_cast<uint8_t>(v >> 16);
  p[3] = static_cast<uint8_t>(v >> 24);
}

void WriteU64LE(uint8_t* p, uint64_t v) {
  WriteU32LE(p, static_cast<uint32_t>(v));
  WriteU32LE(p + 4, static_cast<uint32_t>(v >> 32));
}

void WriteU16BE(uint8_t* p, uint16_t v) {
  p[0] = static_cast<uint8_t>(v >> 8);
  p[1] = static_cast<uint8_t>(v);
}

void WriteU32BE(uint8_t* p, uint32_t v) {
  p[0] = static_cast<uint8_t>(v >> 24);
  p[1] = static_cast<uint8_t>(v >> 16);
  p[2] = static_cast<uint8_t>(v >> 8);
  p[3] = static_cast<uint8_t>(v);
}

void WriteU64BE(uint8_t* p, uint64_t v) {
  WriteU32BE(p, static_cast<uint32_t>(v >> 32));
  WriteU32BE(p + 4, static_cast<uint32_t>(v));
}

uint16_t ReadU16(ByteSpan s, size_t off, ByteOrder order) {
  if (off + 2 > s.size) return 0;
  return order == ByteOrder::kBig ? ReadU16BE(s.data + off) : ReadU16LE(s.data + off);
}

uint32_t ReadU32(ByteSpan s, size_t off, ByteOrder order) {
  if (off + 4 > s.size) return 0;
  return order == ByteOrder::kBig ? ReadU32BE(s.data + off) : ReadU32LE(s.data + off);
}

Status WriteU32To(GrowableBuffer& buf, uint32_t v, ByteOrder order) {
  uint8_t tmp[4];
  if (order == ByteOrder::kBig)
    WriteU32BE(tmp, v);
  else
    WriteU32LE(tmp, v);
  return buf.Append(ByteSpan(tmp, 4));
}

uint32_t Bswap32(uint32_t v) {
  return ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) | ((v & 0x00FF0000u) >> 8) |
         ((v & 0xFF000000u) >> 24);
}

uint64_t Bswap64(uint64_t v) {
  uint32_t lo = static_cast<uint32_t>(v);
  uint32_t hi = static_cast<uint32_t>(v >> 32);
  return (static_cast<uint64_t>(Bswap32(lo)) << 32) | Bswap32(hi);
}

uint32_t endian_hash_0(uint32_t x, uint32_t y) {
  x ^= y + 0x9E3779B9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_0(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_0(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_0(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_0(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_1(uint32_t x, uint32_t y) {
  x ^= y + 0xCF4BB80Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_1(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_1(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_1(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_1(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_2(uint32_t x, uint32_t y) {
  x ^= y + 0x3CCEFAD7u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_2(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_2(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_2(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_2(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_3(uint32_t x, uint32_t y) {
  x ^= y + 0x6A413C9Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_3(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_3(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_3(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_3(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_4(uint32_t x, uint32_t y) {
  x ^= y + 0xDBC47F65u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_4(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_4(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_4(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_4(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_5(uint32_t x, uint32_t y) {
  x ^= y + 0x0958B12Au + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_5(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_5(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_5(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_5(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_6(uint32_t x, uint32_t y) {
  x ^= y + 0x76DBF3F3u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_6(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_6(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_6(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_6(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_7(uint32_t x, uint32_t y) {
  x ^= y + 0xA45E35B8u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_7(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_7(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_7(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_7(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_8(uint32_t x, uint32_t y) {
  x ^= y + 0x15D17401u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_8(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_8(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_8(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_8(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_9(uint32_t x, uint32_t y) {
  x ^= y + 0x4355B6D6u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_9(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_9(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_9(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_9(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_10(uint32_t x, uint32_t y) {
  x ^= y + 0xB0E8E89Fu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_10(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_10(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_10(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_10(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

uint32_t endian_hash_11(uint32_t x, uint32_t y) {
  x ^= y + 0x1E6B2B64u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool endian_validate_range_11(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint16_t endian_read_u16_11(const uint8_t* p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t endian_read_u32_11(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) |
         (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

size_t endian_align_11(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

}  // namespace endian
}  // namespace helion

namespace {
uint32_t helion_hist_mix_72_endian_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 72u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_7(uint32_t x) {
  x ^= 0xC0FFEEu + 7u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
