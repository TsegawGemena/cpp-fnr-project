#include "helion/checksum.h"

namespace helion {
namespace {

uint32_t* CrcTable() {
  static uint32_t table[256];
  static bool init = false;
  if (!init) {
    for (uint32_t i = 0; i < 256; ++i) {
      uint32_t c = i;
      for (int j = 0; j < 8; ++j) {
        c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
      }
      table[i] = c;
    }
    init = true;
  }
  return table;
}

}  // namespace

uint32_t Crc32Update(uint32_t crc, ByteSpan data) {
  const uint32_t* table = CrcTable();
  crc = ~crc;
  for (size_t i = 0; i < data.size; ++i) {
    crc = table[(crc ^ data.data[i]) & 0xFFu] ^ (crc >> 8);
  }
  return ~crc;
}

uint32_t Crc32(ByteSpan data) { return Crc32Update(0, data); }

uint32_t Fnv1a32(ByteSpan data) {
  uint32_t h = 2166136261u;
  for (size_t i = 0; i < data.size; ++i) {
    h ^= data.data[i];
    h *= 16777619u;
  }
  return h;
}

uint64_t Fnv1a64(ByteSpan data) {
  uint64_t h = 14695981039346656037ull;
  for (size_t i = 0; i < data.size; ++i) {
    h ^= data.data[i];
    h *= 1099511628211ull;
  }
  return h;
}

uint32_t RollingChecksum(ByteSpan data, size_t window) {
  if (data.empty()) return 0;
  if (window == 0 || window > data.size) window = data.size;
  uint32_t a = 1, b = 0;
  for (size_t i = 0; i < window; ++i) {
    a = (a + data.data[i]) % 65521u;
    b = (b + a) % 65521u;
  }
  uint32_t best = (b << 16) | a;
  for (size_t i = window; i < data.size; ++i) {
    a = (a + data.data[i] - data.data[i - window]) % 65521u;
    b = (b + a) % 65521u;
    uint32_t cur = (b << 16) | a;
    best ^= cur;
  }
  return best;
}

bool VerifyRegionChecksum(ByteSpan doc, uint32_t offset, uint32_t length, uint32_t expect) {
  if (static_cast<uint64_t>(offset) + length > doc.size) return false;
  return Crc32(doc.subspan(offset, length)) == expect;
}

std::vector<uint32_t> BuildChecksumChain(ByteSpan doc, const std::vector<uint32_t>& cuts) {
  std::vector<uint32_t> out;
  uint32_t prev = 0;
  size_t cursor = 0;
  for (uint32_t cut : cuts) {
    if (cut > doc.size) cut = static_cast<uint32_t>(doc.size);
    if (cut < cursor) continue;
    uint32_t region = Crc32(doc.subspan(cursor, cut - cursor));
    prev = Crc32Update(prev, ByteSpan(reinterpret_cast<const uint8_t*>(&region), 4));
    out.push_back(prev);
    cursor = cut;
  }
  if (cursor < doc.size) {
    uint32_t region = Crc32(doc.subspan(cursor, doc.size - cursor));
    prev = Crc32Update(prev, ByteSpan(reinterpret_cast<const uint8_t*>(&region), 4));
    out.push_back(prev);
  }
  return out;
}

}  // namespace helion
