#pragma once
#include <cstdint>
#include <string>

namespace helion {

enum class Status : int32_t {
  kOk = 0,
  kNeedMore = 1,
  kInvalidHeader = -1,
  kInvalidSection = -2,
  kChecksumMismatch = -3,
  kStateError = -4,
  kOutOfMemory = -5,
  kRangeError = -6,
  kDecodeError = -7,
  kRecoveryRequired = -8,
  kUnsupported = -9,
  kAborted = -10,
};

inline bool Ok(Status s) { return s == Status::kOk; }
inline bool Failed(Status s) { return static_cast<int32_t>(s) < 0; }
const char* StatusName(Status s);
std::string StatusMessage(Status s);

}  // namespace helion

