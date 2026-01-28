#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace helion {

struct ByteSpan {
  const uint8_t* data = nullptr;
  size_t size = 0;

  ByteSpan() = default;
  ByteSpan(const uint8_t* p, size_t n) : data(p), size(n) {}
  ByteSpan(const void* p, size_t n)
      : data(static_cast<const uint8_t*>(p)), size(n) {}

  bool empty() const { return size == 0 || data == nullptr; }
  const uint8_t* begin() const { return data; }
  const uint8_t* end() const { return data ? data + size : nullptr; }
  uint8_t operator[](size_t i) const { return data[i]; }

  ByteSpan subspan(size_t off, size_t len) const {
    if (!data || off > size) return {};
    if (len > size - off) len = size - off;
    return {data + off, len};
  }

  ByteSpan drop_prefix(size_t n) const {
    if (n >= size) return {};
    return {data + n, size - n};
  }
};

struct MutableSpan {
  uint8_t* data = nullptr;
  size_t size = 0;

  MutableSpan() = default;
  MutableSpan(uint8_t* p, size_t n) : data(p), size(n) {}
  ByteSpan as_const() const { return {data, size}; }
  bool empty() const { return size == 0 || data == nullptr; }
};

}  // namespace helion
