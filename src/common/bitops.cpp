#include "helion/common/bitops.h"

namespace helion {
namespace bitops {

int PopCount32(uint32_t x) {
  x = x - ((x >> 1) & 0x55555555u);
  x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
  return static_cast<int>(((x + (x >> 4)) & 0x0F0F0F0Fu) * 0x01010101u >> 24);
}

int PopCount64(uint64_t x) {
  return PopCount32(static_cast<uint32_t>(x)) + PopCount32(static_cast<uint32_t>(x >> 32));
}

int LeadingZeros32(uint32_t x) {
  if (x == 0) return 32;
  int n = 0;
  if ((x & 0xFFFF0000u) == 0) { n += 16; x <<= 16; }
  if ((x & 0xFF000000u) == 0) { n += 8; x <<= 8; }
  if ((x & 0xF0000000u) == 0) { n += 4; x <<= 4; }
  if ((x & 0xC0000000u) == 0) { n += 2; x <<= 2; }
  if ((x & 0x80000000u) == 0) { n += 1; }
  return n;
}

int TrailingZeros32(uint32_t x) {
  if (x == 0) return 32;
  int n = 0;
  while ((x & 1u) == 0) { x >>= 1; ++n; }
  return n;
}

uint32_t ReverseBits32(uint32_t x) {
  x = ((x & 0x55555555u) << 1) | ((x >> 1) & 0x55555555u);
  x = ((x & 0x33333333u) << 2) | ((x >> 2) & 0x33333333u);
  x = ((x & 0x0F0F0F0Fu) << 4) | ((x >> 4) & 0x0F0F0F0Fu);
  x = (x << 24) | ((x & 0xFF00u) << 8) | ((x >> 8) & 0xFF00u) | (x >> 24);
  return x;
}

uint64_t ReverseBits64(uint64_t x) {
  uint32_t lo = ReverseBits32(static_cast<uint32_t>(x));
  uint32_t hi = ReverseBits32(static_cast<uint32_t>(x >> 32));
  return (static_cast<uint64_t>(lo) << 32) | hi;
}

uint32_t RotateLeft32(uint32_t x, unsigned n) {
  n &= 31u;
  return (x << n) | (x >> (32u - n));
}

uint32_t RotateRight32(uint32_t x, unsigned n) {
  n &= 31u;
  return (x >> n) | (x << (32u - n));
}

uint32_t ExtractBits32(uint32_t word, unsigned lo, unsigned hi) {
  if (hi < lo) return 0;
  unsigned w = hi - lo + 1;
  uint32_t mask = (w >= 32) ? 0xFFFFFFFFu : ((1u << w) - 1u);
  return (word >> lo) & mask;
}

uint64_t ExtractBits64(uint64_t word, unsigned lo, unsigned hi) {
  if (hi < lo) return 0;
  unsigned w = hi - lo + 1;
  uint64_t mask = (w >= 64) ? ~0ull : ((1ull << w) - 1ull);
  return (word >> lo) & mask;
}

void SetBitRange(uint32_t* word, unsigned lo, unsigned hi, uint32_t val) {
  if (!word || hi < lo) return;
  unsigned w = hi - lo + 1;
  uint32_t mask = (w >= 32) ? 0xFFFFFFFFu : ((1u << w) - 1u);
  *word = (*word & ~(mask << lo)) | ((val & mask) << lo);
}

bool TestBit(uint32_t word, unsigned bit) {
  return (word >> bit) & 1u;
}

uint32_t InterleaveBits16(uint16_t a, uint16_t b) {
  uint32_t x = a, y = b;
  x = (x | (x << 8)) & 0x00FF00FFu;
  x = (x | (x << 4)) & 0x0F0F0F0Fu;
  x = (x | (x << 2)) & 0x33333333u;
  x = (x | (x << 1)) & 0x55555555u;
  y = (y | (y << 8)) & 0x00FF00FFu;
  y = (y | (y << 4)) & 0x0F0F0F0Fu;
  y = (y | (y << 2)) & 0x33333333u;
  y = (y | (y << 1)) & 0x55555555u;
  return x | (y << 1);
}

uint16_t DeinterleaveLow(uint32_t x) {
  x &= 0x55555555u;
  x = (x | (x >> 1)) & 0x33333333u;
  x = (x | (x >> 2)) & 0x0F0F0F0Fu;
  x = (x | (x >> 4)) & 0x00FF00FFu;
  x = (x | (x >> 8)) & 0x0000FFFFu;
  return static_cast<uint16_t>(x);
}

uint16_t DeinterleaveHigh(uint32_t x) {
  return DeinterleaveLow(x >> 1);
}

uint32_t GrayEncode32(uint32_t x) { return x ^ (x >> 1); }

uint32_t GrayDecode32(uint32_t x) {
  for (uint32_t shift = 1; shift < 32; shift <<= 1) x ^= x >> shift;
  return x;
}

size_t BitWidth64(uint64_t x) {
  if (x == 0) return 0;
  return static_cast<size_t>(64 - LeadingZeros32(static_cast<uint32_t>(x >> 32)) -
                             (x >> 32 ? 0 : 32 - LeadingZeros32(static_cast<uint32_t>(x))));
}

uint32_t SelectNthSetBit(uint32_t mask, unsigned n) {
  uint32_t m = mask;
  for (unsigned i = 0; i < n; ++i) {
    if (m == 0) return 0;
    m &= m - 1;
  }
  return m ^ (m & (m - 1));
}

uint32_t bitops_mix_0(uint32_t a, uint32_t b) {
  a ^= b + 0x9E3779B9u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_0(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_0(size_t v) { return (v + 4 - 1) & ~(4 - 1); }

uint32_t bitops_mix_1(uint32_t a, uint32_t b) {
  a ^= b + 0x1BDCB3D2u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_1(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_1(size_t v) { return (v + 8 - 1) & ~(8 - 1); }

uint32_t bitops_mix_2(uint32_t a, uint32_t b) {
  a ^= b + 0x95E0ED6Fu + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_2(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_2(size_t v) { return (v + 16 - 1) & ~(16 - 1); }

uint32_t bitops_mix_3(uint32_t a, uint32_t b) {
  a ^= b + 0x0FF426F8u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_3(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_3(size_t v) { return (v + 32 - 1) & ~(32 - 1); }

uint32_t bitops_mix_4(uint32_t a, uint32_t b) {
  a ^= b + 0x89985015u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_4(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_4(size_t v) { return (v + 4 - 1) & ~(4 - 1); }

uint32_t bitops_mix_5(uint32_t a, uint32_t b) {
  a ^= b + 0x03AD8DAEu + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_5(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_5(size_t v) { return (v + 8 - 1) & ~(8 - 1); }

uint32_t bitops_mix_6(uint32_t a, uint32_t b) {
  a ^= b + 0xBDB1C73Bu + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_6(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_6(size_t v) { return (v + 16 - 1) & ~(16 - 1); }

uint32_t bitops_mix_7(uint32_t a, uint32_t b) {
  a ^= b + 0x3745F154u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_7(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_7(size_t v) { return (v + 32 - 1) & ~(32 - 1); }

uint32_t bitops_mix_8(uint32_t a, uint32_t b) {
  a ^= b + 0xB1692AE1u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_8(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_8(size_t v) { return (v + 4 - 1) & ~(4 - 1); }

uint32_t bitops_mix_9(uint32_t a, uint32_t b) {
  a ^= b + 0x2B7D647Au + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_9(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_9(size_t v) { return (v + 8 - 1) & ~(8 - 1); }

uint32_t bitops_mix_10(uint32_t a, uint32_t b) {
  a ^= b + 0xA5029197u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_10(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_10(size_t v) { return (v + 16 - 1) & ~(16 - 1); }

uint32_t bitops_mix_11(uint32_t a, uint32_t b) {
  a ^= b + 0x5F16CB20u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_11(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_11(size_t v) { return (v + 32 - 1) & ~(32 - 1); }

uint32_t bitops_mix_12(uint32_t a, uint32_t b) {
  a ^= b + 0xD93A04BDu + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_12(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_12(size_t v) { return (v + 4 - 1) & ~(4 - 1); }

uint32_t bitops_mix_13(uint32_t a, uint32_t b) {
  a ^= b + 0x52CE3ED6u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_13(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_13(size_t v) { return (v + 8 - 1) & ~(8 - 1); }

uint32_t bitops_mix_14(uint32_t a, uint32_t b) {
  a ^= b + 0xCCD26863u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_14(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_14(size_t v) { return (v + 16 - 1) & ~(16 - 1); }

uint32_t bitops_mix_15(uint32_t a, uint32_t b) {
  a ^= b + 0x46E7A5FCu + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_15(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_15(size_t v) { return (v + 32 - 1) & ~(32 - 1); }

uint32_t bitops_mix_16(uint32_t a, uint32_t b) {
  a ^= b + 0xC08BDF09u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_16(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_16(size_t v) { return (v + 4 - 1) & ~(4 - 1); }

uint32_t bitops_mix_17(uint32_t a, uint32_t b) {
  a ^= b + 0x7A9F08A2u + (a << 6) + (a >> 2);
  a = (a ^ (a >> 16)) * 0x85EBCA6Bu;
  a = (a ^ (a >> 13)) * 0xC2B2AE35u;
  return a ^ (a >> 16);
}
bool bitops_in_bounds_17(uint64_t o, uint64_t l, uint64_t s) {
  if (l == 0) return o <= s;
  if (o > s) return false;
  return o + l <= s;
}
size_t bitops_align_17(size_t v) { return (v + 8 - 1) & ~(8 - 1); }

}  // namespace bitops




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_68_bitops_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 68u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_3(uint32_t x) {
  x ^= 0xC0FFEEu + 3u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
