#pragma once
#include "helion/format/section_table.h"
#include "helion/status.h"
#include "helion/types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {
namespace validate {

struct RangeCheckerOptions {
  bool strict = false;
  bool check_checksums = true;
  uint32_t max_section_size = static_cast<uint32_t>(kMaxPayloadBytes);
};

struct RangeCheckerReport {
  Status overall = Status::kOk;
  std::vector<std::string> messages;
  size_t checks_run = 0;
  size_t failures = 0;
};

class RangeChecker {
 public:
  RangeChecker();
  explicit RangeChecker(RangeCheckerOptions opts);
  void Reset();
  Status CheckDocumentBounds(ByteSpan doc, const format::SectionTable& table, RangeCheckerReport* report);
  Status CheckSectionRange(const HeaderInfo& header, RangeCheckerReport* report);
  Status CheckOverlap(ByteSpan doc, const std::vector<SectionDesc>& sections, RangeCheckerReport* report);
  const RangeCheckerOptions& Options() const { return opts_; }

 private:
  Status RunCheck(Status st, const std::string& msg, RangeCheckerReport* report);
  RangeCheckerOptions opts_;
};

}  // namespace validate
}  // namespace helion
