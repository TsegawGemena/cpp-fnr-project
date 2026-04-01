#pragma once
#include "helion/span.h"
#include "helion/status.h"
#include <vector>

namespace helion {
namespace decode {

enum class TransformKind : uint8_t { kNone, kDelta, kRle, kZigZag, kTranspose };

class TransformPipeline {
 public:
  void Add(TransformKind kind);
  Status ApplyForward(ByteSpan input, std::vector<uint8_t>* output);
  Status ApplyInverse(ByteSpan input, std::vector<uint8_t>* output);
  void Clear();

 private:
  Status DeltaEncode(ByteSpan in, std::vector<uint8_t>* out);
  Status DeltaDecode(ByteSpan in, std::vector<uint8_t>* out);
  Status RleEncode(ByteSpan in, std::vector<uint8_t>* out);
  Status RleDecode(ByteSpan in, std::vector<uint8_t>* out);
  Status ZigZagEncode(ByteSpan in, std::vector<uint8_t>* out);
  Status ZigZagDecode(ByteSpan in, std::vector<uint8_t>* out);
  std::vector<TransformKind> stages_;
};

}  // namespace decode
}  // namespace helion

