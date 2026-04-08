#pragma once
#include "helion/format/format_spec.h"
#include "helion/format/section_table.h"
#include "helion/span.h"
#include <vector>

namespace helion {
namespace format {

struct IntegrityRegion {
  uint32_t offset = 0;
  uint32_t length = 0;
  uint32_t expected = 0;
  uint32_t actual = 0;
  bool valid = false;
};

class IntegrityChecker {
 public:
  Status BuildRegions(ByteSpan doc, const SectionTable& table, std::vector<IntegrityRegion>* out);
  Status VerifyAll(ByteSpan doc, const std::vector<IntegrityRegion>& regions);
  Status VerifyHeader(ByteSpan doc);
  Status VerifySection(ByteSpan doc, const SectionDesc& desc);
  uint32_t ComputeDocumentDigest(ByteSpan doc);
  std::vector<IntegrityRegion> FailedRegions(const std::vector<IntegrityRegion>& regions) const;
};

}  // namespace format
}  // namespace helion

