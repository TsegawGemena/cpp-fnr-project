#include "helion/status.h"

namespace helion {

const char* StatusName(Status s) {
  switch (s) {
    case Status::kOk: return "ok";
    case Status::kNeedMore: return "need_more";
    case Status::kInvalidHeader: return "invalid_header";
    case Status::kInvalidSection: return "invalid_section";
    case Status::kChecksumMismatch: return "checksum_mismatch";
    case Status::kStateError: return "state_error";
    case Status::kOutOfMemory: return "out_of_memory";
    case Status::kRangeError: return "range_error";
    case Status::kDecodeError: return "decode_error";
    case Status::kRecoveryRequired: return "recovery_required";
    case Status::kUnsupported: return "unsupported";
    case Status::kAborted: return "aborted";
  }
  return "unknown";
}

std::string StatusMessage(Status s) {
  return std::string("helion::Status::") + StatusName(s);
}

}  // namespace helion
