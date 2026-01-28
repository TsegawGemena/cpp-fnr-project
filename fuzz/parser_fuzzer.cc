#include "helion/api/session.h"
#include "helion/api/parser.h"

#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  helion::api::SessionConfig config;
  config.enable_recovery = true;
  helion::api::Session session(config);

  helion::ByteSpan doc(data, size);
  (void)session.ProcessInput(doc);

  if (size > 0) {
    const size_t split = size / 2;
    helion::api::Session partial_session(config);
    (void)partial_session.ProcessPartial(helion::ByteSpan(data, split));
    (void)partial_session.ProcessPartial(helion::ByteSpan(data + split, size - split));
    (void)partial_session.Finalize();
  }

  helion::api::ParseResult result;
  (void)session.ParseDocument(doc, &result);

  return 0;
}
