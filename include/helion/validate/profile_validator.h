#pragma once
#include "helion/format/section_table.h"
#include "helion/status.h"
#include "helion/types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {
namespace validate {

struct ProfileValidatorOptions {
  bool strict = false;
  bool check_checksums = true;
  uint32_t max_section_size = static_cast<uint32_t>(kMaxPayloadBytes);
};

struct ProfileValidatorReport {
  Status overall = Status::kOk;
  std::vector<std::string> messages;
  size_t checks_run = 0;
  size_t failures = 0;
};

class ProfileValidator {
 public:
  ProfileValidator();
  explicit ProfileValidator(ProfileValidatorOptions opts);
  void Reset();
  Status ValidateProfile(ByteSpan doc, const format::SectionTable& table, ProfileValidatorReport* report);
  Status ValidateFeatureBits(const HeaderInfo& header, ProfileValidatorReport* report);
  Status ValidateVersion(ByteSpan doc, const std::vector<SectionDesc>& sections, ProfileValidatorReport* report);
  const ProfileValidatorOptions& Options() const { return opts_; }

 private:
  Status RunCheck(Status st, const std::string& msg, ProfileValidatorReport* report);
  ProfileValidatorOptions opts_;
};

}  // namespace validate
}  // namespace helion

