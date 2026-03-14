#pragma once
#include <cstdint>
#include <cstddef>

namespace helion {
namespace bitops {

int PopCount32(uint32_t x);
int PopCount64(uint64_t x);
int LeadingZeros32(uint32_t x);
int TrailingZeros32(uint32_t x);
uint32_t ReverseBits32(uint32_t x);
uint64_t ReverseBits64(uint64_t x);
uint32_t RotateLeft32(uint32_t x, unsigned n);
uint32_t RotateRight32(uint32_t x, unsigned n);
uint32_t ExtractBits32(uint32_t word, unsigned lo, unsigned hi);
uint64_t ExtractBits64(uint64_t word, unsigned lo, unsigned hi);
void SetBitRange(uint32_t* word, unsigned lo, unsigned hi, uint32_t val);
bool TestBit(uint32_t word, unsigned bit);
uint32_t InterleaveBits16(uint16_t a, uint16_t b);
uint16_t DeinterleaveLow(uint32_t x);
uint16_t DeinterleaveHigh(uint32_t x);
uint32_t GrayEncode32(uint32_t x);
uint32_t GrayDecode32(uint32_t x);
size_t BitWidth64(uint64_t x);
uint32_t SelectNthSetBit(uint32_t mask, unsigned n);

}  // namespace bitops
}  // namespace helion

