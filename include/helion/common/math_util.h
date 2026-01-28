#pragma once
#include "helion/span.h"
#include <cstdint>
#include <vector>

namespace helion {
namespace math {

uint32_t Murmur3_32(ByteSpan data, uint32_t seed);
uint64_t SplitMix64(uint64_t* state);
uint32_t HashCombine32(uint32_t a, uint32_t b);
uint64_t HashCombine64(uint64_t a, uint64_t b);
uint32_t CityHash32(ByteSpan data);
uint64_t CityHash64(ByteSpan data);
uint32_t JenkinsMix(uint32_t a, uint32_t b, uint32_t c);
uint64_t Wyhash64(ByteSpan data, uint64_t seed);
uint32_t XxHash32(ByteSpan data, uint32_t seed);
size_t NextPowerOfTwo(size_t v);
size_t AlignUp(size_t v, size_t a);
int CompareLex(ByteSpan a, ByteSpan b);
uint32_t RunningXor32(const std::vector<uint32_t>& vals);
double EntropyEstimate(ByteSpan data);
uint32_t PolynomialHash(ByteSpan data, uint32_t base);
bool IsPrime32(uint32_t n);
uint32_t NextPrime32(uint32_t n);

}  // namespace math
}  // namespace helion
