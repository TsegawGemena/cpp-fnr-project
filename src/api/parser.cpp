#include "helion/api/parser.h"
#include "helion/common/endian.h"
#include "helion/format/format_spec.h"
#include "helion/format/integrity.h"

namespace helion {
namespace api {

DocumentParser::DocumentParser() = default;

void DocumentParser::Reset() {
  header_parser_.Reset();
  diagnostics_.Clear();
  partial_mode_ = false;
}

Status DocumentParser::ValidateTable(const format::SectionTable& table) {
  if (table.Count() > kMaxSections) {
    diagnostics_.Error(Status::kInvalidSection, "section count exceeds maximum");
    return Status::kInvalidSection;
  }
  for (const format::SectionTableEntry& e : table.Entries()) {
    if (!e.present) continue;
    if (e.desc.length > kMaxPayloadBytes) {
      diagnostics_.Warning("section payload exceeds recommended size", e.desc.section_id);
    }
    if (e.desc.kind == SectionKind::kReserved && e.desc.length > 0) {
      diagnostics_.Warning("reserved section carries payload", e.desc.section_id);
    }
  }
  return Status::kOk;
}

Status DocumentParser::IndexSections(const format::ParseContext& ctx, format::SectionTable* table) {
  if (!table) return Status::kRangeError;
  Status st = table->Build(ctx.sections);
  if (Failed(st)) {
    diagnostics_.Error(st, "failed to build section table");
    return st;
  }
  return Status::kOk;
}

Status DocumentParser::ParseDocument(ByteSpan doc, ParseResult* out) {
  if (!out) return Status::kRangeError;
  Reset();
  format::ParseContext ctx;
  Status st = header_parser_.ParseAll(doc, &ctx);
  if (Failed(st)) {
    diagnostics_.Error(st, "header parse failed");
    return st;
  }
  out->context = ctx;
  out->header = ctx.header;
  out->bytes_consumed = ctx.bytes_consumed;
  st = IndexSections(ctx, &out->table);
  if (Failed(st)) return st;
  st = ValidateTable(out->table);
  if (Failed(st)) return st;
  format::IntegrityChecker checker;
  st = checker.VerifyHeader(doc);
  if (Failed(st)) diagnostics_.Warning("header checksum mismatch");
  return Status::kOk;
}

Status DocumentParser::ParsePartial(ByteSpan chunk, ParseResult* out) {
  if (!out) return Status::kRangeError;
  partial_mode_ = true;
  Status st = header_parser_.Feed(chunk);
  if (Failed(st) && st != Status::kNeedMore) {
    diagnostics_.Error(st, "partial feed failed");
    return st;
  }
  return st;
}

Status DocumentParser::FinalizePartial(ParseResult* out) {
  if (!out || !partial_mode_) return Status::kStateError;
  format::ParseContext ctx;
  Status st = header_parser_.Finalize(&ctx);
  if (Failed(st)) {
    diagnostics_.Error(st, "partial finalize failed");
    return st;
  }
  out->context = ctx;
  out->header = ctx.header;
  out->bytes_consumed = ctx.bytes_consumed;
  st = IndexSections(ctx, &out->table);
  if (Failed(st)) return st;
  return ValidateTable(out->table);
}

uint32_t docpar_mix_0(uint32_t x, uint32_t y) {
  x ^= y + 0x9E3779B9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_0(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_0(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_0(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_1(uint32_t x, uint32_t y) {
  x ^= y + 0xCF4BB80Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_1(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_1(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_1(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_2(uint32_t x, uint32_t y) {
  x ^= y + 0x3CCEFAD7u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_2(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_2(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_2(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_3(uint32_t x, uint32_t y) {
  x ^= y + 0x6A413C9Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_3(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_3(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_3(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_4(uint32_t x, uint32_t y) {
  x ^= y + 0xDBC47F65u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_4(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_4(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_4(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_5(uint32_t x, uint32_t y) {
  x ^= y + 0x0958B12Au + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_5(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_5(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_5(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_6(uint32_t x, uint32_t y) {
  x ^= y + 0x76DBF3F3u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_6(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_6(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_6(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_7(uint32_t x, uint32_t y) {
  x ^= y + 0xA45E35B8u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_7(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_7(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_7(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_8(uint32_t x, uint32_t y) {
  x ^= y + 0x15D17401u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_8(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_8(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_8(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_9(uint32_t x, uint32_t y) {
  x ^= y + 0x4355B6D6u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_9(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_9(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_9(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_10(uint32_t x, uint32_t y) {
  x ^= y + 0xB0E8E89Fu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_10(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_10(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_10(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_11(uint32_t x, uint32_t y) {
  x ^= y + 0x1E6B2B64u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_11(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_11(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_11(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_12(uint32_t x, uint32_t y) {
  x ^= y + 0x4FEE6D2Du + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_12(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_12(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_12(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_13(uint32_t x, uint32_t y) {
  x ^= y + 0xBD62AFF2u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_13(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_13(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_13(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_14(uint32_t x, uint32_t y) {
  x ^= y + 0xEAE5E1BBu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_14(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_14(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_14(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_15(uint32_t x, uint32_t y) {
  x ^= y + 0x58782000u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_15(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_15(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_15(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_16(uint32_t x, uint32_t y) {
  x ^= y + 0x89FB62C9u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_16(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_16(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_16(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_17(uint32_t x, uint32_t y) {
  x ^= y + 0xF77FA49Eu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_17(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_17(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_17(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_18(uint32_t x, uint32_t y) {
  x ^= y + 0x24F2E767u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_18(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_18(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_18(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_19(uint32_t x, uint32_t y) {
  x ^= y + 0x9275192Cu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_19(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_19(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_19(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_20(uint32_t x, uint32_t y) {
  x ^= y + 0xC3885BF5u + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_20(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_20(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_20(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

uint32_t docpar_mix_21(uint32_t x, uint32_t y) {
  x ^= y + 0x310C9DBAu + (x << 6) + (x >> 2);
  x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
  x = (x ^ (x >> 13)) * 0xC2B2AE35u;
  return x ^ (x >> 16);
}

bool docpar_in_bounds_21(uint64_t off, uint64_t len, uint64_t cap) {
  if (len == 0) return off <= cap;
  if (off > cap) return false;
  return off + len <= cap;
}

uint32_t docpar_read_tag_21(ByteSpan data, size_t off) {
  if (!data.data || off + 4 > data.size) return 0;
  return endian::ReadU32LE(data.data + off);
}

Status docpar_check_word_21(uint32_t w) {
  if ((w & 0xFF000000u) != 0) return Status::kDecodeError;
  return Status::kOk;
}

}  // namespace api
}  // namespace helion
