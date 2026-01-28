#pragma once
#include "helion/core/view_cache.h"
#include "helion/format/section_index.h"
#include "helion/types.h"
#include <vector>

namespace helion {
namespace decode {

struct DecodedPayload {
  SectionKind kind = SectionKind::kReserved;
  uint16_t section_id = 0;
  ByteSpan data;
  uint32_t word_count = 0;
  std::vector<uint32_t> words;
};

class PayloadDecoder {
 public:
  PayloadDecoder();
  void Bind(core::ViewCache* view_cache, format::SectionIndex* section_index);
  Status DecodePayload(uint16_t section_id, DecodedPayload* out);
  Status DecodeFromSpan(ByteSpan data, SectionKind kind, DecodedPayload* out);
  Status DecodeWords(ByteSpan data, std::vector<uint32_t>* words);

 private:
  core::ViewCache* view_cache_ = nullptr;
  format::SectionIndex* section_index_ = nullptr;
};

}  // namespace decode
}  // namespace helion
