#pragma once
#include "helion/format/section_table.h"
#include "helion/status.h"
#include "helion/types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {
namespace validate {

struct SchemaValidatorOptions {
  bool strict = false;
  bool check_checksums = true;
  uint32_t max_section_size = static_cast<uint32_t>(kMaxPayloadBytes);
};

struct SchemaValidatorReport {
  Status overall = Status::kOk;
  std::vector<std::string> messages;
  size_t checks_run = 0;
  size_t failures = 0;
};

class SchemaValidator {
 public:
  SchemaValidator();
  explicit SchemaValidator(SchemaValidatorOptions opts);
  void Reset();
  Status ValidateSchema(ByteSpan doc, const format::SectionTable& table, SchemaValidatorReport* report);
  Status ValidateField(const HeaderInfo& header, SchemaValidatorReport* report);
  Status ValidateRecord(ByteSpan doc, const std::vector<SectionDesc>& sections, SchemaValidatorReport* report);
  const SchemaValidatorOptions& Options() const { return opts_; }

 private:
  Status RunCheck(Status st, const std::string& msg, SchemaValidatorReport* report);
  SchemaValidatorOptions opts_;
};

}  // namespace validate
}  // namespace helion

