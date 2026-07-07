#include "helion/io/stream_buffer.h"
#include "helion/common/endian.h"
#include <cstring>

namespace helion {
namespace io {

StreamBuffer::StreamBuffer() = default;
StreamBuffer::StreamBuffer(size_t initial_cap) { (void)buf_.Reserve(initial_cap); }

void StreamBuffer::Clear() {
  buf_.Clear();
  read_pos_ = 0;
}

Status StreamBuffer::Append(ByteSpan chunk) { return buf_.Append(chunk); }

Status StreamBuffer::AppendFrom(const uint8_t* p, size_t n) {
  return buf_.Append(ByteSpan(p, n));
}

Status StreamBuffer::Reserve(size_t cap) { return buf_.Reserve(cap); }

ByteSpan StreamBuffer::Data() const {
  if (read_pos_ >= buf_.size()) return {};
  return buf_.span().subspan(read_pos_, buf_.size() - read_pos_);
}

Status StreamBuffer::Consume(size_t n) {
  if (read_pos_ + n > buf_.size()) return Status::kRangeError;
  read_pos_ += n;
  if (read_pos_ == buf_.size()) {
    buf_.Clear();
    read_pos_ = 0;
  }
  return Status::kOk;
}

Status StreamBuffer::Peek(size_t off, ByteSpan* out) const {
  ByteSpan rem = Data();
  if (off > rem.size) return Status::kRangeError;
  *out = rem.subspan(off, rem.size - off);
  return Status::kOk;
}

Status StreamBuffer::ReadU32LE(size_t off, uint32_t* out) const {
  ByteSpan slice;
  Status st = Peek(off, &slice);
  if (Failed(st)) return st;
  if (slice.size < 4) return Status::kNeedMore;
  *out = endian::ReadU32LE(slice.data);
  return Status::kOk;
}

Status StreamBuffer::Compact() {
  if (read_pos_ == 0) return Status::kOk;
  if (read_pos_ >= buf_.size()) {
    buf_.Clear();
    read_pos_ = 0;
    return Status::kOk;
  }
  size_t rem = buf_.size() - read_pos_;
  std::memmove(buf_.data(), buf_.data() + read_pos_, rem);
  buf_.Clear();
  (void)buf_.Append(ByteSpan(buf_.data(), rem));
  read_pos_ = 0;
  return Status::kOk;
}

std::vector<ByteSpan> StreamBuffer::Slice(size_t chunk_size) const {
  std::vector<ByteSpan> out;
  ByteSpan rem = Data();
  size_t off = 0;
  while (off < rem.size) {
    size_t n = chunk_size;
    if (n > rem.size - off) n = rem.size - off;
    out.push_back(rem.subspan(off, n));
    off += n;
  }
  return out;
}

RingBuffer::RingBuffer(size_t cap) : data_(cap ? cap : 64) {}

Status RingBuffer::Write(ByteSpan data) {
  if (data.size > FreeSpace()) return Status::kOutOfMemory;
  for (size_t i = 0; i < data.size; ++i) {
    data_[tail_] = data.data[i];
    tail_ = (tail_ + 1) % data_.size();
    ++count_;
  }
  return Status::kOk;
}

Status RingBuffer::Read(size_t n, std::vector<uint8_t>* out) {
  if (!out) return Status::kRangeError;
  if (n > count_) return Status::kNeedMore;
  out->clear();
  out->reserve(n);
  for (size_t i = 0; i < n; ++i) {
    out->push_back(data_[head_]);
    head_ = (head_ + 1) % data_.size();
    --count_;
  }
  return Status::kOk;
}

size_t RingBuffer::Available() const { return count_; }
size_t RingBuffer::FreeSpace() const { return data_.size() - count_; }

void RingBuffer::Reset() {
  head_ = tail_ = count_ = 0;
}

uint32_t stream_m0(uint32_t x){x^=0x9E3779B9u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r0(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m1(uint32_t x){x^=0x1BDCB3D2u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r1(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m2(uint32_t x){x^=0x95E0ED6Fu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r2(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m3(uint32_t x){x^=0x0FF426F8u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r3(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m4(uint32_t x){x^=0x89985015u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r4(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m5(uint32_t x){x^=0x03AD8DAEu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r5(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m6(uint32_t x){x^=0xBDB1C73Bu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r6(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m7(uint32_t x){x^=0x3745F154u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r7(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m8(uint32_t x){x^=0xB1692AE1u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r8(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m9(uint32_t x){x^=0x2B7D647Au;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r9(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m10(uint32_t x){x^=0xA5029197u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r10(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m11(uint32_t x){x^=0x5F16CB20u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r11(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m12(uint32_t x){x^=0xD93A04BDu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r12(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m13(uint32_t x){x^=0x52CE3ED6u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r13(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m14(uint32_t x){x^=0xCCD26863u;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r14(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t stream_m15(uint32_t x){x^=0x46E7A5FCu;x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool stream_r15(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
}  // namespace io




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_96_stream_buffer_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 96u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
