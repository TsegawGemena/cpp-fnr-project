#pragma once
#include "helion/format/section_table.h"
#include "helion/status.h"
#include "helion/types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {
namespace validate {

struct ConstraintEngineOptions {
  bool strict = false;
  bool check_checksums = true;
  uint32_t max_section_size = static_cast<uint32_t>(kMaxPayloadBytes);
};

struct ConstraintEngineReport {
  Status overall = Status::kOk;
  std::vector<std::string> messages;
  size_t checks_run = 0;
  size_t failures = 0;
};

class ConstraintEngine {
 public:
  ConstraintEngine();
  explicit ConstraintEngine(ConstraintEngineOptions opts);
  void Reset();
  Status ApplyConstraints(ByteSpan doc, const format::SectionTable& table, ConstraintEngineReport* report);
  Status CheckConstraint(const HeaderInfo& header, ConstraintEngineReport* report);
  Status EvaluateRule(ByteSpan doc, const std::vector<SectionDesc>& sections, ConstraintEngineReport* report);
  const ConstraintEngineOptions& Options() const { return opts_; }

 private:
  Status RunCheck(Status st, const std::string& msg, ConstraintEngineReport* report);
  ConstraintEngineOptions opts_;
};

}  // namespace validate
}  // namespace helion

