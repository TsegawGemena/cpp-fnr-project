#pragma once
#include "helion/span.h"
#include <cstdint>
#include <vector>

namespace helion {

uint32_t Crc32(ByteSpan data);
uint32_t Crc32Update(uint32_t crc, ByteSpan data);
uint32_t Fnv1a32(ByteSpan data);
uint64_t Fnv1a64(ByteSpan data);
uint32_t RollingChecksum(ByteSpan data, size_t window);
bool VerifyRegionChecksum(ByteSpan doc, uint32_t offset, uint32_t length, uint32_t expect);
std::vector<uint32_t> BuildChecksumChain(ByteSpan doc, const std::vector<uint32_t>& cuts);

}  // namespace helion

