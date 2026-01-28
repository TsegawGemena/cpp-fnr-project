#include "helion/api/session.h"
#include "helion/decode/payload_decoder.h"

#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  helion::api::SessionConfig config;
  config.enable_recovery = true;
  helion::api::Session session(config);

  helion::ByteSpan doc(data, size);
  helion::Status st = session.StageDocument(doc);
  if (helion::Ok(st)) {
    if (size > 1) {
      const size_t partial_len = (data[0] % (size - 1)) + 1;
      (void)session.ProcessPartial(helion::ByteSpan(data, partial_len));
    } else {
      (void)session.ProcessPartial(helion::ByteSpan());
    }

    (void)session.Reindex();
    (void)session.ResumeAfterRecovery();

    for (uint16_t id = 1; id <= 8; ++id) {
      helion::decode::DecodedPayload payload;
      (void)session.DecodePayload(id, &payload);
    }

    if (session.ViewCache()) {
      for (uint16_t id : session.ViewCache()->StagedSectionIds()) {
        helion::decode::DecodedPayload payload;
        (void)session.DecodePayload(id, &payload);
      }
    }
  }

  return 0;
}
