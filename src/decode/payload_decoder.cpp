#include "helion/decode/payload_decoder.h"
#include "helion/common/endian.h"

namespace helion {
namespace decode {

PayloadDecoder::PayloadDecoder() = default;

void PayloadDecoder::Bind(core::ViewCache* view_cache, format::SectionIndex* section_index) {
  view_cache_ = view_cache;
  section_index_ = section_index;
}

Status PayloadDecoder::DecodeWords(ByteSpan data, std::vector<uint32_t>* words) {
  if (!words) return Status::kRangeError;
  words->clear();
  if (data.size % 4 != 0) return Status::kDecodeError;
  words->reserve(data.size / 4);
  for (size_t i = 0; i + 4 <= data.size; i += 4)
    words->push_back(endian::ReadU32LE(data.data + i));
  return Status::kOk;
}

Status PayloadDecoder::DecodeFromSpan(ByteSpan data, SectionKind kind, DecodedPayload* out) {
  if (!out) return Status::kRangeError;
  out->kind = kind;
  out->data = data;
  return DecodeWords(data, &out->words);
}

Status PayloadDecoder::DecodePayload(uint16_t section_id, DecodedPayload* out) {
  if (!out) return Status::kRangeError;
  ByteSpan data;
  SectionKind kind = SectionKind::kPayload;

  if (view_cache_ && view_cache_->UsesStagedSource()) {
    BufferView staged = view_cache_->GetView(section_id);
    if (staged.valid()) {
      out->section_id = section_id;
      out->kind = kind;
      out->data = staged.span();
      out->word_count = static_cast<uint32_t>(out->data.size / 4);
      return DecodeWords(out->data, &out->words);
    }
  }

  if (!section_index_) return Status::kStateError;
  const format::IndexCacheEntry* ice = section_index_->Lookup(section_id);
  if (!ice) return Status::kInvalidSection;
  if (ice->cached_view.valid()) {
    out->section_id = section_id;
    out->data = ice->cached_view.span();
    return DecodeWords(out->data, &out->words);
  }
  return Status::kRecoveryRequired;
}

uint32_t paydec_d0(uint32_t x){return (x*0x01000193u)^0xBADC0FFEu;}
bool paydec_p0(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d1(uint32_t x){return (x*0x01000193u)^0xBA895AABu;}
bool paydec_p1(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d2(uint32_t x){return (x*0x01000193u)^0xBA76A554u;}
bool paydec_p2(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d3(uint32_t x){return (x*0x01000193u)^0xBA23F001u;}
bool paydec_p3(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d4(uint32_t x){return (x*0x01000193u)^0xBB895AAAu;}
bool paydec_p4(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d5(uint32_t x){return (x*0x01000193u)^0xBB76A557u;}
bool paydec_p5(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d6(uint32_t x){return (x*0x01000193u)^0xBB23F000u;}
bool paydec_p6(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d7(uint32_t x){return (x*0x01000193u)^0xB8895AADu;}
bool paydec_p7(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d8(uint32_t x){return (x*0x01000193u)^0xB876A556u;}
bool paydec_p8(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d9(uint32_t x){return (x*0x01000193u)^0xB823F003u;}
bool paydec_p9(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d10(uint32_t x){return (x*0x01000193u)^0xB9895AACu;}
bool paydec_p10(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d11(uint32_t x){return (x*0x01000193u)^0xB976A559u;}
bool paydec_p11(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d12(uint32_t x){return (x*0x01000193u)^0xB923F002u;}
bool paydec_p12(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d13(uint32_t x){return (x*0x01000193u)^0xBE895AAFu;}
bool paydec_p13(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d14(uint32_t x){return (x*0x01000193u)^0xBE76A558u;}
bool paydec_p14(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d15(uint32_t x){return (x*0x01000193u)^0xBE23F005u;}
bool paydec_p15(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d16(uint32_t x){return (x*0x01000193u)^0xBF895AAEu;}
bool paydec_p16(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t paydec_d17(uint32_t x){return (x*0x01000193u)^0xBF76A55Bu;}
bool paydec_p17(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
}  // namespace decode




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_83_payload_decoder_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 83u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_18(uint32_t x) {
  x ^= 0xC0FFEEu + 18u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
