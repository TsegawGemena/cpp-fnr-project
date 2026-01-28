#include "helion/api/session.h"
#include "helion/core/state_machine.h"
#include "helion/decode/payload_decoder.h"

#include <cstddef>
#include <cstdint>

namespace {

enum Op : uint8_t {
  kProcessInput = 0,
  kStageDocument = 1,
  kProcessPartial = 2,
  kFinalize = 3,
  kReindex = 4,
  kResumeAfterRecovery = 5,
  kDecodePayload = 6,
  kDispatchEvent = 7,
};

helion::ByteSpan Slice(const uint8_t* data, size_t size, uint8_t a, uint8_t b) {
  if (size == 0) return {};
  const size_t start = a % size;
  const size_t end = start + ((b % static_cast<uint8_t>(size - start)) + 1);
  return helion::ByteSpan(data + start, end - start);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  helion::api::SessionConfig config;
  config.enable_recovery = true;
  helion::api::Session session(config);

  helion::ByteSpan doc(data, size);
  size_t i = 0;
  const size_t max_ops = size > 0 ? size : 1;

  for (size_t n = 0; n < max_ops && size > 0; ++n) {
    const uint8_t ctrl = data[i % size];
    const uint8_t arg0 = size > 1 ? data[(i + 1) % size] : 0;
    const uint8_t arg1 = size > 2 ? data[(i + 2) % size] : 0;
    i += 3;

    switch (ctrl % 8) {
      case kProcessInput:
        (void)session.ProcessInput(doc);
        break;
      case kStageDocument:
        (void)session.StageDocument(doc);
        break;
      case kProcessPartial:
        (void)session.ProcessPartial(Slice(data, size, arg0, arg1));
        break;
      case kFinalize:
        (void)session.Finalize();
        break;
      case kReindex:
        (void)session.Reindex();
        break;
      case kResumeAfterRecovery:
        (void)session.ResumeAfterRecovery();
        break;
      case kDecodePayload: {
        const uint16_t section_id = static_cast<uint16_t>((arg0 % 8) + 1);
        helion::decode::DecodedPayload payload;
        (void)session.DecodePayload(section_id, &payload);
        break;
      }
      case kDispatchEvent: {
        const auto event = static_cast<helion::core::SessionEvent>(arg0 % 12);
        (void)session.State();
        if (session.State() == helion::ProcessState::kIdle &&
            event == helion::core::SessionEvent::kInputReceived) {
          (void)session.ProcessInput(doc);
        }
        break;
      }
      default:
        break;
    }
  }

  return 0;
}
