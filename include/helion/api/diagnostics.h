#pragma once
#include "helion/status.h"
#include "helion/types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {
namespace api {

enum class DiagnosticLevel : uint8_t {
  kInfo = 0,
  kWarning = 1,
  kError = 2,
};

struct Diagnostic {
  DiagnosticLevel level = DiagnosticLevel::kInfo;
  Status code = Status::kOk;
  std::string message;
  uint16_t section_id = 0;
  uint32_t offset = 0;
};

class Diagnostics {
 public:
  Diagnostics();
  void Clear();
  void Info(const std::string& msg);
  void Warning(const std::string& msg, uint16_t section_id = 0);
  void Error(Status code, const std::string& msg, uint16_t section_id = 0, uint32_t offset = 0);
  void Push(Diagnostic d);
  const std::vector<Diagnostic>& Entries() const { return entries_; }
  bool HasErrors() const;
  std::string FormatAll() const;
  size_t Count(DiagnosticLevel level) const;

 private:
  std::vector<Diagnostic> entries_;
};

}  // namespace api
}  // namespace helion
