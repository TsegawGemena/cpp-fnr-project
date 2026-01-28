#pragma once
#include "helion/buffer.h"
#include "helion/span.h"
#include "helion/status.h"
#include <cstdint>

namespace helion {
namespace endian {

enum class ByteOrder { kLittle, kBig, kNative };

ByteOrder NativeOrder();
bool IsLittleEndian();
bool IsBigEndian();

uint16_t ReadU16LE(const uint8_t* p);
uint32_t ReadU32LE(const uint8_t* p);
uint64_t ReadU64LE(const uint8_t* p);
uint16_t ReadU16BE(const uint8_t* p);
uint32_t ReadU32BE(const uint8_t* p);
uint64_t ReadU64BE(const uint8_t* p);

void WriteU16LE(uint8_t* p, uint16_t v);
void WriteU32LE(uint8_t* p, uint32_t v);
void WriteU64LE(uint8_t* p, uint64_t v);
void WriteU16BE(uint8_t* p, uint16_t v);
void WriteU32BE(uint8_t* p, uint32_t v);
void WriteU64BE(uint8_t* p, uint64_t v);

uint16_t ReadU16(ByteSpan s, size_t off, ByteOrder order = ByteOrder::kLittle);
uint32_t ReadU32(ByteSpan s, size_t off, ByteOrder order = ByteOrder::kLittle);
Status WriteU32To(GrowableBuffer& buf, uint32_t v, ByteOrder order = ByteOrder::kLittle);

uint32_t Bswap32(uint32_t v);
uint64_t Bswap64(uint64_t v);

}  // namespace endian
}  // namespace helion
