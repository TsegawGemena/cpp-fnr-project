#include "helion/io/cursor.h"
#include "helion/common/endian.h"
#include <cstring>

namespace helion {
namespace io {

ReadCursor::ReadCursor(ByteSpan span) : span_(span) {}
void ReadCursor::Reset(ByteSpan span) { span_ = span; pos_ = 0; }
ByteSpan ReadCursor::Remaining() const {
  if (pos_ >= span_.size) return {};
  return span_.subspan(pos_, span_.size - pos_);
}

Status ReadCursor::Require(size_t n) {
  if (pos_ + n > span_.size) return Status::kNeedMore;
  return Status::kOk;
}

Status ReadCursor::ReadU8(uint8_t* out) {
  Status st = Require(1);
  if (Failed(st)) return st;
  *out = span_.data[pos_++];
  return Status::kOk;
}

Status ReadCursor::ReadU16LE(uint16_t* out) {
  Status st = Require(2);
  if (Failed(st)) return st;
  *out = endian::ReadU16LE(span_.data + pos_);
  pos_ += 2;
  return Status::kOk;
}

Status ReadCursor::ReadU32LE(uint32_t* out) {
  Status st = Require(4);
  if (Failed(st)) return st;
  *out = endian::ReadU32LE(span_.data + pos_);
  pos_ += 4;
  return Status::kOk;
}

Status ReadCursor::ReadU64LE(uint64_t* out) {
  Status st = Require(8);
  if (Failed(st)) return st;
  *out = endian::ReadU64LE(span_.data + pos_);
  pos_ += 8;
  return Status::kOk;
}

Status ReadCursor::ReadBytes(size_t n, ByteSpan* out) {
  Status st = Require(n);
  if (Failed(st)) return st;
  *out = span_.subspan(pos_, n);
  pos_ += n;
  return Status::kOk;
}

Status ReadCursor::Skip(size_t n) {
  Status st = Require(n);
  if (Failed(st)) return st;
  pos_ += n;
  return Status::kOk;
}

Status ReadCursor::Seek(size_t pos) {
  if (pos > span_.size) return Status::kRangeError;
  pos_ = pos;
  return Status::kOk;
}

Status ReadCursor::PeekU32LE(uint32_t* out) {
  Status st = Require(4);
  if (Failed(st)) return st;
  *out = endian::ReadU32LE(span_.data + pos_);
  return Status::kOk;
}

Status ReadCursor::ReadVarUInt(uint64_t* out) {
  uint64_t result = 0;
  unsigned shift = 0;
  while (true) {
    uint8_t b = 0;
    Status st = ReadU8(&b);
    if (Failed(st)) return st;
    result |= static_cast<uint64_t>(b & 0x7Fu) << shift;
    if ((b & 0x80u) == 0) break;
    shift += 7;
    if (shift >= 64) return Status::kDecodeError;
  }
  *out = result;
  return Status::kOk;
}

Status ReadCursor::ReadLengthPrefixed(ByteSpan* out, size_t max_len) {
  uint64_t len = 0;
  Status st = ReadVarUInt(&len);
  if (Failed(st)) return st;
  if (len > max_len) return Status::kRangeError;
  return ReadBytes(static_cast<size_t>(len), out);
}

WriteCursor::WriteCursor(MutableSpan span) : span_(span) {}
void WriteCursor::Reset(MutableSpan span) { span_ = span; pos_ = 0; }

Status WriteCursor::Require(size_t n) {
  if (pos_ + n > span_.size) return Status::kOutOfMemory;
  return Status::kOk;
}

Status WriteCursor::WriteU8(uint8_t v) {
  Status st = Require(1);
  if (Failed(st)) return st;
  span_.data[pos_++] = v;
  return Status::kOk;
}

Status WriteCursor::WriteU16LE(uint16_t v) {
  Status st = Require(2);
  if (Failed(st)) return st;
  endian::WriteU16LE(span_.data + pos_, v);
  pos_ += 2;
  return Status::kOk;
}

Status WriteCursor::WriteU32LE(uint32_t v) {
  Status st = Require(4);
  if (Failed(st)) return st;
  endian::WriteU32LE(span_.data + pos_, v);
  pos_ += 4;
  return Status::kOk;
}

Status WriteCursor::WriteU64LE(uint64_t v) {
  Status st = Require(8);
  if (Failed(st)) return st;
  endian::WriteU64LE(span_.data + pos_, v);
  pos_ += 8;
  return Status::kOk;
}

Status WriteCursor::WriteBytes(ByteSpan data) {
  Status st = Require(data.size);
  if (Failed(st)) return st;
  if (data.data && data.size)
    std::memcpy(span_.data + pos_, data.data, data.size);
  pos_ += data.size;
  return Status::kOk;
}

Status WriteCursor::WriteVarUInt(uint64_t v) {
  while (v >= 0x80) {
    Status st = WriteU8(static_cast<uint8_t>((v & 0x7F) | 0x80));
    if (Failed(st)) return st;
    v >>= 7;
  }
  return WriteU8(static_cast<uint8_t>(v));
}

Status WriteCursor::WriteLengthPrefixed(ByteSpan data) {
  Status st = WriteVarUInt(data.size);
  if (Failed(st)) return st;
  return WriteBytes(data);
}

MutableSpan WriteCursor::WrittenSpan() const {
  return {span_.data, pos_};
}

uint32_t cursor_m0(uint32_t x){x^=0x9E3779B9u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r0(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m1(uint32_t x){x^=0x1BDCB3D2u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r1(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m2(uint32_t x){x^=0x95E0ED6Fu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r2(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m3(uint32_t x){x^=0x0FF426F8u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r3(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m4(uint32_t x){x^=0x89985015u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r4(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m5(uint32_t x){x^=0x03AD8DAEu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r5(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m6(uint32_t x){x^=0xBDB1C73Bu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r6(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m7(uint32_t x){x^=0x3745F154u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r7(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m8(uint32_t x){x^=0xB1692AE1u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r8(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m9(uint32_t x){x^=0x2B7D647Au;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r9(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m10(uint32_t x){x^=0xA5029197u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r10(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m11(uint32_t x){x^=0x5F16CB20u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r11(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m12(uint32_t x){x^=0xD93A04BDu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r12(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m13(uint32_t x){x^=0x52CE3ED6u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r13(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m14(uint32_t x){x^=0xCCD26863u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r14(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m15(uint32_t x){x^=0x46E7A5FCu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r15(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m16(uint32_t x){x^=0xC08BDF09u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r16(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t cursor_m17(uint32_t x){x^=0x7A9F08A2u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool cursor_r17(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
}  // namespace io




























































}  // namespace helion
