#include "helion/buffer.h"
#include <cstdlib>
#include <cstring>

namespace helion {

GrowableBuffer::GrowableBuffer(size_t cap) { (void)Reserve(cap); }

GrowableBuffer::GrowableBuffer(GrowableBuffer&& other) noexcept
    : data_(other.data_),
      size_(other.size_),
      capacity_(other.capacity_),
      owns_(other.owns_) {
  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;
  other.owns_ = true;
}

GrowableBuffer& GrowableBuffer::operator=(GrowableBuffer&& other) noexcept {
  if (this == &other) return *this;
  if (owns_ && data_) std::free(data_);
  data_ = other.data_;
  size_ = other.size_;
  capacity_ = other.capacity_;
  owns_ = other.owns_;
  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;
  other.owns_ = true;
  return *this;
}

GrowableBuffer::~GrowableBuffer() {
  if (owns_ && data_) std::free(data_);
}

Status GrowableBuffer::Reserve(size_t n) {
  if (n <= capacity_) return Status::kOk;
  size_t nc = capacity_ ? capacity_ : 64;
  while (nc < n) nc *= 2;
  uint8_t* p = static_cast<uint8_t*>(std::realloc(owns_ ? data_ : nullptr, nc));
  if (!p) return Status::kOutOfMemory;
  if (!owns_ && data_ && size_) std::memcpy(p, data_, size_);
  data_ = p;
  capacity_ = nc;
  owns_ = true;
  return Status::kOk;
}

Status GrowableBuffer::Append(ByteSpan bytes) {
  if (bytes.empty()) return Status::kOk;
  Status st = Reserve(size_ + bytes.size);
  if (Failed(st)) return st;
  std::memcpy(data_ + size_, bytes.data, bytes.size);
  size_ += bytes.size;
  return Status::kOk;
}

Status GrowableBuffer::AppendByte(uint8_t b) {
  Status st = Reserve(size_ + 1);
  if (Failed(st)) return st;
  data_[size_++] = b;
  return Status::kOk;
}

void GrowableBuffer::Clear() { size_ = 0; }

void GrowableBuffer::ShrinkToFit() {
  if (!owns_ || !data_ || size_ == capacity_) return;
  uint8_t* p = static_cast<uint8_t*>(std::realloc(data_, size_ ? size_ : 1));
  if (!p) return;
  data_ = p;
  capacity_ = size_ ? size_ : 1;
}

void GrowableBuffer::Adopt(uint8_t* p, size_t n, size_t cap, bool own) {
  if (owns_ && data_) std::free(data_);
  data_ = p;
  size_ = n;
  capacity_ = cap;
  owns_ = own;
}

uint8_t* GrowableBuffer::Release() {
  uint8_t* p = data_;
  data_ = nullptr;
  size_ = 0;
  capacity_ = 0;
  owns_ = true;
  return p;
}

}  // namespace helion
