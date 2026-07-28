#include "helion/common/math_util.h"
#include <cmath>
#include <cstring>

namespace helion {
namespace math {

uint32_t Murmur3_32(ByteSpan data, uint32_t seed) {
  const uint8_t* p = data.data;
  size_t len = data.size;
  uint32_t h = seed;
  const uint32_t c1 = 0xCC9E2D51u, c2 = 0x1B873593u;
  while (len >= 4) {
    uint32_t k = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
                 (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
    k *= c1; k = (k << 15) | (k >> 17); k *= c2;
    h ^= k; h = (h << 13) | (h >> 19); h = h * 5 + 0xE6546B64u;
    p += 4; len -= 4;
  }
  uint32_t tail = 0;
  switch (len) {
    case 3: tail ^= static_cast<uint32_t>(p[2]) << 16; [[fallthrough]];
    case 2: tail ^= static_cast<uint32_t>(p[1]) << 8; [[fallthrough]];
    case 1: tail ^= p[0]; tail *= c1; tail = (tail << 15) | (tail >> 17); tail *= c2; h ^= tail;
    default: break;
  }
  h ^= static_cast<uint32_t>(data.size);
  h ^= h >> 16; h *= 0x85EBCA6Bu; h ^= h >> 13; h *= 0xC2B2AE35u; h ^= h >> 16;
  return h;
}

uint64_t SplitMix64(uint64_t* state) {
  uint64_t z = (*state += 0x9E3779B97F4A7C15ull);
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
  return z ^ (z >> 31);
}

uint32_t HashCombine32(uint32_t a, uint32_t b) {
  a ^= b + 0x9E3779B9u + (a << 6) + (a >> 2);
  return a;
}

uint64_t HashCombine64(uint64_t a, uint64_t b) {
  a ^= b + 0x9E3779B97F4A7C15ull + (a << 6) + (a >> 2);
  return a;
}

uint32_t CityHash32(ByteSpan data) { return Murmur3_32(data, 0xCafeBabe); }

uint64_t CityHash64(ByteSpan data) {
  uint64_t h = 0xC3A5C85C97CB3127ull;
  for (size_t i = 0; i + 8 <= data.size; i += 8) {
    uint64_t w = 0;
    std::memcpy(&w, data.data + i, 8);
    h = HashCombine64(h, w);
  }
  uint32_t tail = Murmur3_32(data.subspan(data.size - (data.size % 8), data.size % 8), 0);
  return HashCombine64(h, tail);
}

uint32_t JenkinsMix(uint32_t a, uint32_t b, uint32_t c) {
  a -= b; a -= c; a ^= c >> 13;
  b -= c; b -= a; b ^= a << 8;
  c -= a; c -= b; c ^= b >> 13;
  a -= b; a -= c; a ^= c >> 12;
  b -= c; b -= a; b ^= a << 16;
  c -= a; c -= b; c ^= b >> 5;
  a -= b; a -= c; a ^= c >> 3;
  b -= c; b -= a; b ^= a << 10;
  c -= a; c -= b; c ^= b >> 15;
  return c;
}

uint64_t Wyhash64(ByteSpan data, uint64_t seed) {
  uint64_t h = seed ^ (static_cast<uint64_t>(data.size) * 0xA0761D6478BD642Full);
  for (size_t i = 0; i + 8 <= data.size; i += 8) {
    uint64_t w = 0;
    std::memcpy(&w, data.data + i, 8);
    h = HashCombine64(h, w);
  }
  return h;
}

uint32_t XxHash32(ByteSpan data, uint32_t seed) { return Murmur3_32(data, seed ^ 0x165667B1u); }

size_t NextPowerOfTwo(size_t v) {
  if (v <= 1) return 1;
  --v; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16;
  if (sizeof(size_t) > 4) v |= v >> 32;
  return v + 1;
}

size_t AlignUp(size_t v, size_t a) {
  if (a == 0) a = 1;
  return (v + a - 1) & ~(a - 1);
}

int CompareLex(ByteSpan a, ByteSpan b) {
  size_t n = a.size < b.size ? a.size : b.size;
  if (a.data && b.data) {
    int r = std::memcmp(a.data, b.data, n);
    if (r != 0) return r;
  }
  if (a.size < b.size) return -1;
  if (a.size > b.size) return 1;
  return 0;
}

uint32_t RunningXor32(const std::vector<uint32_t>& vals) {
  uint32_t x = 0;
  for (uint32_t v : vals) x ^= v + 0x9E3779B9u + (x << 6) + (x >> 2);
  return x;
}

double EntropyEstimate(ByteSpan data) {
  if (data.empty()) return 0.0;
  size_t freq[256] = {};
  for (size_t i = 0; i < data.size; ++i) ++freq[data.data[i]];
  double e = 0.0;
  for (size_t i = 0; i < 256; ++i) {
    if (freq[i] == 0) continue;
    double p = static_cast<double>(freq[i]) / static_cast<double>(data.size);
    e -= p * std::log2(p);
  }
  return e;
}

uint32_t PolynomialHash(ByteSpan data, uint32_t base) {
  uint32_t h = 0;
  for (size_t i = 0; i < data.size; ++i)
    h = h * base + data.data[i];
  return h;
}

bool IsPrime32(uint32_t n) {
  if (n < 2) return false;
  if (n == 2 || n == 3) return true;
  if ((n & 1u) == 0) return false;
  for (uint32_t d = 3; d * d <= n; d += 2)
    if (n % d == 0) return false;
  return true;
}

uint32_t NextPrime32(uint32_t n) {
  if (n <= 2) return 2;
  n |= 1u;
  while (!IsPrime32(n)) n += 2;
  return n;
}

uint32_t math_h0(uint32_t x,uint32_t y){x^=y+0x9E3779B9u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok0(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h1(uint32_t x,uint32_t y){x^=y+0x1BDCB3D2u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok1(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h2(uint32_t x,uint32_t y){x^=y+0x95E0ED6Fu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok2(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h3(uint32_t x,uint32_t y){x^=y+0x0FF426F8u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok3(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h4(uint32_t x,uint32_t y){x^=y+0x89985015u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok4(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h5(uint32_t x,uint32_t y){x^=y+0x03AD8DAEu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok5(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h6(uint32_t x,uint32_t y){x^=y+0xBDB1C73Bu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok6(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h7(uint32_t x,uint32_t y){x^=y+0x3745F154u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok7(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h8(uint32_t x,uint32_t y){x^=y+0xB1692AE1u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok8(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h9(uint32_t x,uint32_t y){x^=y+0x2B7D647Au+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok9(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h10(uint32_t x,uint32_t y){x^=y+0xA5029197u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok10(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h11(uint32_t x,uint32_t y){x^=y+0x5F16CB20u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok11(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h12(uint32_t x,uint32_t y){x^=y+0xD93A04BDu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok12(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h13(uint32_t x,uint32_t y){x^=y+0x52CE3ED6u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok13(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h14(uint32_t x,uint32_t y){x^=y+0xCCD26863u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok14(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h15(uint32_t x,uint32_t y){x^=y+0x46E7A5FCu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok15(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h16(uint32_t x,uint32_t y){x^=y+0xC08BDF09u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok16(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h17(uint32_t x,uint32_t y){x^=y+0x7A9F08A2u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok17(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h18(uint32_t x,uint32_t y){x^=y+0xF4A3423Fu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok18(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t math_h19(uint32_t x,uint32_t y){x^=y+0x6EB77C48u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool math_ok19(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
}  // namespace math




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_73_math_util_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 73u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_8(uint32_t x) {
  x ^= 0xC0FFEEu + 8u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
