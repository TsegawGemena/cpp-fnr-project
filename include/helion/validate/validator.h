#pragma once
#include "helion/format/section_table.h"
#include "helion/status.h"
#include "helion/types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {
namespace validate {

struct DocumentValidatorOptions {
  bool strict = false;
  bool check_checksums = true;
  uint32_t max_section_size = static_cast<uint32_t>(kMaxPayloadBytes);
};

struct DocumentValidatorReport {
  Status overall = Status::kOk;
  std::vector<std::string> messages;
  size_t checks_run = 0;
  size_t failures = 0;
};

class DocumentValidator {
 public:
  DocumentValidator();
  explicit DocumentValidator(DocumentValidatorOptions opts);
  void Reset();
  Status ValidateDocument(ByteSpan doc, const format::SectionTable& table, DocumentValidatorReport* report);
  Status ValidateHeader(const HeaderInfo& header, DocumentValidatorReport* report);
  Status ValidateSections(ByteSpan doc, const std::vector<SectionDesc>& sections, DocumentValidatorReport* report);
  const DocumentValidatorOptions& Options() const { return opts_; }

 private:
  Status RunCheck(Status st, const std::string& msg, DocumentValidatorReport* report);
  DocumentValidatorOptions opts_;
};

}  // namespace validate
}  // namespace helion

