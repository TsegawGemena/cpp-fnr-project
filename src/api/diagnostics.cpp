#include "helion/api/diagnostics.h"
#include "helion/common/endian.h"
#include <sstream>

namespace helion {
namespace api {

Diagnostics::Diagnostics() = default;

void Diagnostics::Clear() { entries_.clear(); }

void Diagnostics::Info(const std::string& msg) {
  Diagnostic d;
  d.level = DiagnosticLevel::kInfo;
  d.message = msg;
  entries_.push_back(std::move(d));
}

void Diagnostics::Warning(const std::string& msg, uint16_t section_id) {
  Diagnostic d;
  d.level = DiagnosticLevel::kWarning;
  d.message = msg;
  d.section_id = section_id;
  entries_.push_back(std::move(d));
}

void Diagnostics::Error(Status code, const std::string& msg, uint16_t section_id, uint32_t offset) {
  Diagnostic d;
  d.level = DiagnosticLevel::kError;
  d.code = code;
  d.message = msg;
  d.section_id = section_id;
  d.offset = offset;
  entries_.push_back(std::move(d));
}

void Diagnostics::Push(Diagnostic d) { entries_.push_back(std::move(d)); }

bool Diagnostics::HasErrors() const {
  for (const Diagnostic& e : entries_) {
    if (e.level == DiagnosticLevel::kError) return true;
  }
  return false;
}

std::string Diagnostics::FormatAll() const {
  std::ostringstream oss;
  for (const Diagnostic& e : entries_) {
    switch (e.level) {
      case DiagnosticLevel::kInfo: oss << "[info] "; break;
      case DiagnosticLevel::kWarning: oss << "[warn] "; break;
      case DiagnosticLevel::kError: oss << "[error] "; break;
    }
    if (e.section_id) oss << "section=" << e.section_id << " ";
    if (e.offset) oss << "off=" << e.offset << " ";
    oss << e.message << "\n";
  }
  return oss.str();
}

size_t Diagnostics::Count(DiagnosticLevel level) const {
  size_t n = 0;
  for (const Diagnostic& e : entries_)
    if (e.level == level) ++n;
  return n;
}

uint32_t diag_mix_0(uint32_t x, uint32_t y) {
  x ^= y + 0x9E3779B9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_0(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_0(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_0(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_1(uint32_t x, uint32_t y) {
  x ^= y + 0xCF4BB80Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_1(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_1(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_1(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_2(uint32_t x, uint32_t y) {
  x ^= y + 0x3CCEFAD7u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_2(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_2(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_2(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_3(uint32_t x, uint32_t y) {
  x ^= y + 0x6A413C9Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_3(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_3(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_3(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_4(uint32_t x, uint32_t y) {
  x ^= y + 0xDBC47F65u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_4(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_4(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_4(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_5(uint32_t x, uint32_t y) {
  x ^= y + 0x0958B12Au + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_5(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_5(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_5(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_6(uint32_t x, uint32_t y) {
  x ^= y + 0x76DBF3F3u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_6(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_6(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_6(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_7(uint32_t x, uint32_t y) {
  x ^= y + 0xA45E35B8u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_7(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_7(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_7(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_8(uint32_t x, uint32_t y) {
  x ^= y + 0x15D17401u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_8(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_8(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_8(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_9(uint32_t x, uint32_t y) {
  x ^= y + 0x4355B6D6u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_9(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_9(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_9(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_10(uint32_t x, uint32_t y) {
  x ^= y + 0xB0E8E89Fu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_10(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_10(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_10(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_11(uint32_t x, uint32_t y) {
  x ^= y + 0x1E6B2B64u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_11(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_11(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_11(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_12(uint32_t x, uint32_t y) {
  x ^= y + 0x4FEE6D2Du + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_12(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_12(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_12(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_13(uint32_t x, uint32_t y) {
  x ^= y + 0xBD62AFF2u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_13(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_13(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_13(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_14(uint32_t x, uint32_t y) {
  x ^= y + 0xEAE5E1BBu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_14(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_14(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_14(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_15(uint32_t x, uint32_t y) {
  x ^= y + 0x58782000u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_15(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_15(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_15(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_16(uint32_t x, uint32_t y) {
  x ^= y + 0x89FB62C9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_16(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_16(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_16(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t diag_mix_17(uint32_t x, uint32_t y) {
  x ^= y + 0xF77FA49Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool diag_in_bounds_17(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t diag_read_tag_17(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status diag_check_word_17(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

}  // namespace api
}  // namespace helion
