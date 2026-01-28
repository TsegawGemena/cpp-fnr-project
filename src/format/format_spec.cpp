#include "helion/format/format_spec.h"
#include "helion/checksum.h"
#include "helion/common/endian.h"

namespace helion {
namespace format {

Status VerifyMagic(ByteSpan doc) {
  if (doc.size < 4) return Status::kNeedMore;
  uint32_t magic = endian::ReadU32LE(doc.data);
  return magic == kMagic ? Status::kOk : Status::kInvalidHeader;
}

Status ParseHeader(ByteSpan doc, HeaderInfo* out) {
  if (!out) return Status::kRangeError;
  if (doc.size < kHeaderBytes) return Status::kNeedMore;
  const uint8_t* p = doc.data;
  out->magic = endian::ReadU32LE(p);
  out->version = endian::ReadU16LE(p + 4);
  out->flags = endian::ReadU16LE(p + 6);
  out->section_count = endian::ReadU32LE(p + 8);
  out->index_offset = endian::ReadU32LE(p + 12);
  out->payload_offset = endian::ReadU32LE(p + 16);
  out->total_size = endian::ReadU32LE(p + 20);
  out->checksum = endian::ReadU32LE(p + 24);
  out->feature_bits = endian::ReadU32LE(p + 28);
  out->document_id = endian::ReadU64LE(p + 32);
  out->generation = endian::ReadU32LE(p + 40);
  if (out->magic != kMagic) return Status::kInvalidHeader;
  if (out->version != kVersion) return Status::kUnsupported;
  if (out->section_count > kMaxSections) return Status::kInvalidHeader;
  return Status::kOk;
}

uint32_t HeaderChecksum(const HeaderInfo& hdr) {
  uint8_t tmp[kHeaderBytes] = {};
  endian::WriteU32LE(tmp, hdr.magic);
  endian::WriteU16LE(tmp + 4, hdr.version);
  endian::WriteU16LE(tmp + 6, hdr.flags);
  endian::WriteU32LE(tmp + 8, hdr.section_count);
  endian::WriteU32LE(tmp + 12, hdr.index_offset);
  endian::WriteU32LE(tmp + 16, hdr.payload_offset);
  endian::WriteU32LE(tmp + 20, hdr.total_size);
  endian::WriteU32LE(tmp + 28, hdr.feature_bits);
  endian::WriteU64LE(tmp + 32, hdr.document_id);
  endian::WriteU32LE(tmp + 40, hdr.generation);
  return Crc32(ByteSpan(tmp, kHeaderBytes - 8));
}

Status SerializeHeader(const HeaderInfo& hdr, GrowableBuffer* out) {
  if (!out) return Status::kRangeError;
  uint8_t tmp[kHeaderBytes] = {};
  HeaderInfo copy = hdr;
  copy.checksum = HeaderChecksum(copy);
  endian::WriteU32LE(tmp, copy.magic);
  endian::WriteU16LE(tmp + 4, copy.version);
  endian::WriteU16LE(tmp + 6, copy.flags);
  endian::WriteU32LE(tmp + 8, copy.section_count);
  endian::WriteU32LE(tmp + 12, copy.index_offset);
  endian::WriteU32LE(tmp + 16, copy.payload_offset);
  endian::WriteU32LE(tmp + 20, copy.total_size);
  endian::WriteU32LE(tmp + 24, copy.checksum);
  endian::WriteU32LE(tmp + 28, copy.feature_bits);
  endian::WriteU64LE(tmp + 32, copy.document_id);
  endian::WriteU32LE(tmp + 40, copy.generation);
  return out->Append(ByteSpan(tmp, kHeaderBytes));
}

Status ValidateHeader(const HeaderInfo& hdr) {
  if (hdr.magic != kMagic) return Status::kInvalidHeader;
  if (hdr.version != kVersion) return Status::kUnsupported;
  if (hdr.section_count > kMaxSections) return Status::kInvalidSection;
  if (hdr.total_size > kMaxPayloadBytes) return Status::kRangeError;
  if (hdr.checksum != HeaderChecksum(hdr)) return Status::kChecksumMismatch;
  return Status::kOk;
}

Status ComputeLayout(const HeaderInfo& hdr, LayoutInfo* layout) {
  if (!layout) return Status::kRangeError;
  layout->header_size = kHeaderBytes;
  layout->section_table_offset = kHeaderBytes;
  layout->section_table_size = static_cast<size_t>(hdr.section_count) * kSectionDescBytes;
  layout->payload_base = hdr.payload_offset ? hdr.payload_offset
                                            : layout->section_table_offset + layout->section_table_size;
  layout->total_size = hdr.total_size;
  return Status::kOk;
}

Status ReadSectionDesc(ByteSpan doc, size_t index, SectionDesc* out) {
  if (!out) return Status::kRangeError;
  size_t off = kHeaderBytes + index * kSectionDescBytes;
  if (off + kSectionDescBytes > doc.size) return Status::kNeedMore;
  const uint8_t* p = doc.data + off;
  out->section_id = endian::ReadU16LE(p);
  out->kind = static_cast<SectionKind>(endian::ReadU16LE(p + 2));
  out->flags = endian::ReadU16LE(p + 4);
  out->offset = endian::ReadU32LE(p + 8);
  out->length = endian::ReadU32LE(p + 12);
  out->checksum = endian::ReadU32LE(p + 16);
  out->parent_id = endian::ReadU32LE(p + 20);
  out->aux = endian::ReadU32LE(p + 24);
  return Status::kOk;
}

Status WriteSectionDesc(const SectionDesc& desc, GrowableBuffer* out) {
  if (!out) return Status::kRangeError;
  uint8_t tmp[kSectionDescBytes] = {};
  endian::WriteU16LE(tmp, desc.section_id);
  endian::WriteU16LE(tmp + 2, static_cast<uint16_t>(desc.kind));
  endian::WriteU16LE(tmp + 4, desc.flags);
  endian::WriteU32LE(tmp + 8, desc.offset);
  endian::WriteU32LE(tmp + 12, desc.length);
  endian::WriteU32LE(tmp + 16, desc.checksum);
  endian::WriteU32LE(tmp + 20, desc.parent_id);
  endian::WriteU32LE(tmp + 24, desc.aux);
  return out->Append(ByteSpan(tmp, kSectionDescBytes));
}

std::vector<SectionKind> ExpectedSectionOrder() {
  return {SectionKind::kMetadata, SectionKind::kStringTable, SectionKind::kObjectGraph,
          SectionKind::kIndex, SectionKind::kPayload, SectionKind::kChecksum};
}

uint32_t fmt_x0(uint32_t a,uint32_t b){a^=b+0x9E3779B9u+(a<<6)+(a>>2);return a;}
bool fmt_chk0(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x1(uint32_t a,uint32_t b){a^=b+0xCF4BB80Eu+(a<<6)+(a>>2);return a;}
bool fmt_chk1(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x2(uint32_t a,uint32_t b){a^=b+0x3CCEFAD7u+(a<<6)+(a>>2);return a;}
bool fmt_chk2(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x3(uint32_t a,uint32_t b){a^=b+0x6A413C9Cu+(a<<6)+(a>>2);return a;}
bool fmt_chk3(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x4(uint32_t a,uint32_t b){a^=b+0xDBC47F65u+(a<<6)+(a>>2);return a;}
bool fmt_chk4(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x5(uint32_t a,uint32_t b){a^=b+0x0958B12Au+(a<<6)+(a>>2);return a;}
bool fmt_chk5(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x6(uint32_t a,uint32_t b){a^=b+0x76DBF3F3u+(a<<6)+(a>>2);return a;}
bool fmt_chk6(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x7(uint32_t a,uint32_t b){a^=b+0xA45E35B8u+(a<<6)+(a>>2);return a;}
bool fmt_chk7(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x8(uint32_t a,uint32_t b){a^=b+0x15D17401u+(a<<6)+(a>>2);return a;}
bool fmt_chk8(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x9(uint32_t a,uint32_t b){a^=b+0x4355B6D6u+(a<<6)+(a>>2);return a;}
bool fmt_chk9(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x10(uint32_t a,uint32_t b){a^=b+0xB0E8E89Fu+(a<<6)+(a>>2);return a;}
bool fmt_chk10(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x11(uint32_t a,uint32_t b){a^=b+0x1E6B2B64u+(a<<6)+(a>>2);return a;}
bool fmt_chk11(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x12(uint32_t a,uint32_t b){a^=b+0x4FEE6D2Du+(a<<6)+(a>>2);return a;}
bool fmt_chk12(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x13(uint32_t a,uint32_t b){a^=b+0xBD62AFF2u+(a<<6)+(a>>2);return a;}
bool fmt_chk13(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x14(uint32_t a,uint32_t b){a^=b+0xEAE5E1BBu+(a<<6)+(a>>2);return a;}
bool fmt_chk14(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
uint32_t fmt_x15(uint32_t a,uint32_t b){a^=b+0x58782000u+(a<<6)+(a>>2);return a;}
bool fmt_chk15(uint64_t o,uint64_t l,uint64_t s){return l==0?o<=s:(o<=s&&o+l<=s);}
}  // namespace format




























































}  // namespace helion
