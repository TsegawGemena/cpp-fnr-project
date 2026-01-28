#include "helion/format/header_parser.h"
#include "helion/checksum.h"

namespace helion {
namespace format {

HeaderParser::HeaderParser() { Reset(); }

void HeaderParser::Reset() {
  buffer_.Clear();
  ctx_ = ParseContext{};
  complete_ = false;
}

Status HeaderParser::Feed(ByteSpan chunk) {
  Status st = buffer_.Append(chunk);
  if (Failed(st)) return st;
  if (!complete_) {
    st = TryParseHeader();
    if (Failed(st) && st != Status::kNeedMore) return st;
    if (Ok(st)) {
      st = ParseSectionTable();
      if (Failed(st) && st != Status::kNeedMore) return st;
    }
  }
  return Status::kOk;
}

Status HeaderParser::TryParseHeader() {
  ByteSpan data = buffer_.Data();
  if (data.size < kHeaderBytes) return Status::kNeedMore;
  Status st = ParseHeader(data, &ctx_.header);
  if (Failed(st)) return st;
  st = ValidateHeader(ctx_.header);
  if (Failed(st)) return st;
  return Status::kOk;
}

Status HeaderParser::ParseSectionTable() {
  ByteSpan data = buffer_.Data();
  size_t need = kHeaderBytes + static_cast<size_t>(ctx_.header.section_count) * kSectionDescBytes;
  if (data.size < need) {
    ctx_.partial = true;
    return Status::kNeedMore;
  }
  ctx_.sections.clear();
  ctx_.sections.reserve(ctx_.header.section_count);
  for (size_t i = 0; i < ctx_.header.section_count; ++i) {
    SectionDesc desc;
    Status st = ReadSectionDesc(data, i, &desc);
    if (Failed(st)) return st;
    ctx_.sections.push_back(desc);
  }
  ctx_.bytes_consumed = need;
  complete_ = true;
  return Status::kOk;
}

Status HeaderParser::Finalize(ParseContext* ctx) {
  if (!ctx) return Status::kRangeError;
  if (!complete_ && ctx_.partial) return Status::kNeedMore;
  if (!complete_) return Status::kStateError;
  *ctx = ctx_;
  return Status::kOk;
}

Status HeaderParser::ParseAll(ByteSpan doc, ParseContext* ctx) {
  Reset();
  Status st = Feed(doc);
  if (Failed(st)) return st;
  st = TryParseHeader();
  if (Failed(st)) return st;
  st = ParseSectionTable();
  if (Failed(st)) return st;
  return Finalize(ctx);
}

uint32_t hdr_x0(uint32_t a,uint32_t b){a^=b+0x9E3779B9u+(a<<6)+(a>>2);return a;}
bool hdr_chk0(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x1(uint32_t a,uint32_t b){a^=b+0xCF4BB80Eu+(a<<6)+(a>>2);return a;}
bool hdr_chk1(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x2(uint32_t a,uint32_t b){a^=b+0x3CCEFAD7u+(a<<6)+(a>>2);return a;}
bool hdr_chk2(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x3(uint32_t a,uint32_t b){a^=b+0x6A413C9Cu+(a<<6)+(a>>2);return a;}
bool hdr_chk3(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x4(uint32_t a,uint32_t b){a^=b+0xDBC47F65u+(a<<6)+(a>>2);return a;}
bool hdr_chk4(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x5(uint32_t a,uint32_t b){a^=b+0x0958B12Au+(a<<6)+(a>>2);return a;}
bool hdr_chk5(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x6(uint32_t a,uint32_t b){a^=b+0x76DBF3F3u+(a<<6)+(a>>2);return a;}
bool hdr_chk6(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x7(uint32_t a,uint32_t b){a^=b+0xA45E35B8u+(a<<6)+(a>>2);return a;}
bool hdr_chk7(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x8(uint32_t a,uint32_t b){a^=b+0x15D17401u+(a<<6)+(a>>2);return a;}
bool hdr_chk8(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x9(uint32_t a,uint32_t b){a^=b+0x4355B6D6u+(a<<6)+(a>>2);return a;}
bool hdr_chk9(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x10(uint32_t a,uint32_t b){a^=b+0xB0E8E89Fu+(a<<6)+(a>>2);return a;}
bool hdr_chk10(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x11(uint32_t a,uint32_t b){a^=b+0x1E6B2B64u+(a<<6)+(a>>2);return a;}
bool hdr_chk11(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x12(uint32_t a,uint32_t b){a^=b+0x4FEE6D2Du+(a<<6)+(a>>2);return a;}
bool hdr_chk12(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t hdr_x13(uint32_t a,uint32_t b){a^=b+0xBD62AFF2u+(a<<6)+(a>>2);return a;}
bool hdr_chk13(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
}  // namespace format




























































}  // namespace helion
