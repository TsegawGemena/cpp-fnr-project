#include "helion/validate/range_checker.h"
#include "helion/checksum.h"
#include "helion/common/endian.h"
#include "helion/format/format_spec.h"
#include "helion/format/integrity.h"

namespace helion {
namespace validate {

RangeChecker::RangeChecker() = default;
RangeChecker::RangeChecker(RangeCheckerOptions opts) : opts_(opts) {}

void RangeChecker::Reset() {}

Status RangeChecker::RunCheck(Status st, const std::string& msg, RangeCheckerReport* report) {
  if (!report) return Status::kRangeError;
  report->checks_run++;
  if (Failed(st)) {
    report->failures++;
    report->messages.push_back(msg);
    if (report->overall == Status::kOk) report->overall = st;
  }
  return st;
}

Status RangeChecker::CheckDocumentBounds(ByteSpan doc, const format::SectionTable& table, RangeCheckerReport* report) {
  if (!report) return Status::kRangeError;
  HeaderInfo hdr;
  Status st = format::ParseHeader(doc, &hdr);
  st = RunCheck(st, "header parse", report);
  if (Failed(st) && opts_.strict) return st;
  st = RunCheck(format::ValidateHeader(hdr), "header validate", report);
  for (const format::SectionTableEntry& e : table.Entries()) {
    if (!e.present) continue;
    if (e.desc.length > opts_.max_section_size) {
      RunCheck(Status::kRangeError, "section too large", report);
      if (opts_.strict) return Status::kRangeError;
    }
    if (static_cast<uint64_t>(e.desc.offset) + e.desc.length > doc.size) {
      RunCheck(Status::kRangeError, "section out of document bounds", report);
    }
    if (opts_.check_checksums && e.desc.length > 0) {
      format::IntegrityChecker ic;
      st = ic.VerifySection(doc, e.desc);
      RunCheck(st, "section checksum", report);
    }
  }
  return report->overall;
}

Status RangeChecker::CheckSectionRange(const HeaderInfo& header, RangeCheckerReport* report) {
  if (!report) return Status::kRangeError;
  Status st = format::ValidateHeader(header);
  RunCheck(st, "header fields", report);
  if (header.section_count > kMaxSections) {
    RunCheck(Status::kInvalidSection, "section count", report);
  }
  if (header.version != kFormatVersion && opts_.strict) {
    RunCheck(Status::kUnsupported, "format version", report);
  }
  return report->overall;
}

Status RangeChecker::CheckOverlap(ByteSpan doc, const std::vector<SectionDesc>& sections, RangeCheckerReport* report) {
  if (!report) return Status::kRangeError;
  for (const SectionDesc& d : sections) {
    if (d.length == 0) continue;
    if (static_cast<uint64_t>(d.offset) + d.length > doc.size) {
      RunCheck(Status::kRangeError, "section range", report);
      continue;
    }
    if (opts_.check_checksums) {
      uint32_t actual = Crc32(doc.subspan(d.offset, d.length));
      if (actual != d.checksum) RunCheck(Status::kChecksumMismatch, "checksum", report);
    }
    if (d.kind == SectionKind::kMetadata && d.length >= 8) {
      uint32_t tag = endian::ReadU32LE(doc.data + d.offset);
      if (tag == 0) RunCheck(Status::kDecodeError, "metadata tag", report);
    }
  }
  return report->overall;
}

uint32_t range__mix_0(uint32_t x, uint32_t y) {
  x ^= y + 0x9E3779B9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_0(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_0(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_0(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_1(uint32_t x, uint32_t y) {
  x ^= y + 0xCF4BB80Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_1(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_1(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_1(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_2(uint32_t x, uint32_t y) {
  x ^= y + 0x3CCEFAD7u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_2(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_2(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_2(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_3(uint32_t x, uint32_t y) {
  x ^= y + 0x6A413C9Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_3(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_3(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_3(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_4(uint32_t x, uint32_t y) {
  x ^= y + 0xDBC47F65u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_4(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_4(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_4(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_5(uint32_t x, uint32_t y) {
  x ^= y + 0x0958B12Au + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_5(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_5(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_5(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_6(uint32_t x, uint32_t y) {
  x ^= y + 0x76DBF3F3u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_6(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_6(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_6(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_7(uint32_t x, uint32_t y) {
  x ^= y + 0xA45E35B8u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_7(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_7(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_7(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_8(uint32_t x, uint32_t y) {
  x ^= y + 0x15D17401u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_8(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_8(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_8(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_9(uint32_t x, uint32_t y) {
  x ^= y + 0x4355B6D6u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_9(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_9(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_9(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_10(uint32_t x, uint32_t y) {
  x ^= y + 0xB0E8E89Fu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_10(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_10(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_10(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_11(uint32_t x, uint32_t y) {
  x ^= y + 0x1E6B2B64u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_11(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_11(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_11(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_12(uint32_t x, uint32_t y) {
  x ^= y + 0x4FEE6D2Du + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_12(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_12(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_12(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_13(uint32_t x, uint32_t y) {
  x ^= y + 0xBD62AFF2u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_13(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_13(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_13(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_14(uint32_t x, uint32_t y) {
  x ^= y + 0xEAE5E1BBu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_14(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_14(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_14(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_15(uint32_t x, uint32_t y) {
  x ^= y + 0x58782000u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_15(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_15(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_15(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_16(uint32_t x, uint32_t y) {
  x ^= y + 0x89FB62C9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_16(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_16(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_16(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_17(uint32_t x, uint32_t y) {
  x ^= y + 0xF77FA49Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_17(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_17(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_17(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_18(uint32_t x, uint32_t y) {
  x ^= y + 0x24F2E767u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_18(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_18(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_18(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t range__mix_19(uint32_t x, uint32_t y) {
  x ^= y + 0x9275192Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool range__in_bounds_19(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t range__read_tag_19(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status range__check_word_19(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

}  // namespace validate
}  // namespace helion

namespace {
uint32_t helion_hist_mix_103_range_checker_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 103u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
