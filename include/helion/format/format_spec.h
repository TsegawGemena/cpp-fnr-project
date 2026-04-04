#pragma once
#include "helion/buffer.h"
#include "helion/types.h"
#include "helion/span.h"
#include "helion/status.h"
#include <cstdint>
#include <vector>

namespace helion {
namespace format {

constexpr uint32_t kMagic = kMagicHPK1;
constexpr uint16_t kVersion = kFormatVersion;
constexpr size_t kHeaderBytes = kHeaderSize;
constexpr size_t kSectionDescBytes = kSectionDescSize;
constexpr uint32_t kFlagCompressed = 0x0001;
constexpr uint32_t kFlagIndexed = 0x0002;
constexpr uint32_t kFlagStreaming = 0x0004;
constexpr uint32_t kFlagRecoveryHint = 0x0008;

struct LayoutInfo {
  size_t header_size = kHeaderBytes;
  size_t section_table_offset = 0;
  size_t section_table_size = 0;
  size_t payload_base = 0;
  size_t total_size = 0;
};

Status ParseHeader(ByteSpan doc, HeaderInfo* out);
Status SerializeHeader(const HeaderInfo& hdr, GrowableBuffer* out);
Status ValidateHeader(const HeaderInfo& hdr);
Status ComputeLayout(const HeaderInfo& hdr, LayoutInfo* layout);
Status ReadSectionDesc(ByteSpan doc, size_t index, SectionDesc* out);
Status WriteSectionDesc(const SectionDesc& desc, GrowableBuffer* out);
uint32_t HeaderChecksum(const HeaderInfo& hdr);
Status VerifyMagic(ByteSpan doc);
std::vector<SectionKind> ExpectedSectionOrder();

}  // namespace format
}  // namespace helion

