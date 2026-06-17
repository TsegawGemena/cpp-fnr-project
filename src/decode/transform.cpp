#include "helion/decode/transform.h"

namespace helion {
namespace decode {

void TransformPipeline::Add(TransformKind kind) { stages_.push_back(kind); }
void TransformPipeline::Clear() { stages_.clear(); }

Status TransformPipeline::DeltaEncode(ByteSpan in, std::vector<uint8_t>* out) {
  if (!out) return Status::kRangeError;
  out->clear();
  out->reserve(in.size);
  uint8_t prev = 0;
  for (size_t i = 0; i < in.size; ++i) {
    uint8_t cur = in.data[i];
    out->push_back(static_cast<uint8_t>(cur - prev));
    prev = cur;
  }
  return Status::kOk;
}

Status TransformPipeline::DeltaDecode(ByteSpan in, std::vector<uint8_t>* out) {
  if (!out) return Status::kRangeError;
  out->clear();
  out->reserve(in.size);
  uint8_t prev = 0;
  for (size_t i = 0; i < in.size; ++i) {
    uint8_t cur = static_cast<uint8_t>(prev + in.data[i]);
    out->push_back(cur);
    prev = cur;
  }
  return Status::kOk;
}

Status TransformPipeline::RleEncode(ByteSpan in, std::vector<uint8_t>* out) {
  if (!out) return Status::kRangeError;
  out->clear();
  size_t i = 0;
  while (i < in.size) {
    uint8_t v = in.data[i];
    size_t run = 1;
    while (i + run < in.size && in.data[i + run] == v && run < 255) ++run;
    out->push_back(static_cast<uint8_t>(run));
    out->push_back(v);
    i += run;
  }
  return Status::kOk;
}

Status TransformPipeline::RleDecode(ByteSpan in, std::vector<uint8_t>* out) {
  if (!out) return Status::kRangeError;
  out->clear();
  for (size_t i = 0; i + 1 < in.size; i += 2) {
    uint8_t run = in.data[i];
    uint8_t v = in.data[i + 1];
    out->insert(out->end(), run, v);
  }
  return Status::kOk;
}

Status TransformPipeline::ZigZagEncode(ByteSpan in, std::vector<uint8_t>* out) {
  if (!out) return Status::kRangeError;
  out->assign(in.data, in.data + in.size);
  for (size_t i = 0; i < out->size(); ++i) {
    int8_t s = static_cast<int8_t>((*out)[i]);
    (*out)[i] = static_cast<uint8_t>((s << 1) ^ (s >> 7));
  }
  return Status::kOk;
}

Status TransformPipeline::ZigZagDecode(ByteSpan in, std::vector<uint8_t>* out) {
  if (!out) return Status::kRangeError;
  out->assign(in.data, in.data + in.size);
  for (size_t i = 0; i < out->size(); ++i) {
    uint8_t u = (*out)[i];
    (*out)[i] = static_cast<uint8_t>((u >> 1) ^ (-(u & 1)));
  }
  return Status::kOk;
}

Status TransformPipeline::ApplyForward(ByteSpan input, std::vector<uint8_t>* output) {
  std::vector<uint8_t> cur(input.data, input.data + input.size);
  for (TransformKind k : stages_) {
    std::vector<uint8_t> next;
    Status st = Status::kOk;
    ByteSpan span(cur.data(), cur.size());
    switch (k) {
      case TransformKind::kDelta: st = DeltaEncode(span, &next); break;
      case TransformKind::kRle: st = RleEncode(span, &next); break;
      case TransformKind::kZigZag: st = ZigZagEncode(span, &next); break;
      default: next = cur; break;
    }
    if (Failed(st)) return st;
    cur.swap(next);
  }
  *output = std::move(cur);
  return Status::kOk;
}

Status TransformPipeline::ApplyInverse(ByteSpan input, std::vector<uint8_t>* output) {
  std::vector<uint8_t> cur(input.data, input.data + input.size);
  for (auto it = stages_.rbegin(); it != stages_.rend(); ++it) {
    std::vector<uint8_t> next;
    Status st = Status::kOk;
    ByteSpan span(cur.data(), cur.size());
    switch (*it) {
      case TransformKind::kDelta: st = DeltaDecode(span, &next); break;
      case TransformKind::kRle: st = RleDecode(span, &next); break;
      case TransformKind::kZigZag: st = ZigZagDecode(span, &next); break;
      default: next = cur; break;
    }
    if (Failed(st)) return st;
    cur.swap(next);
  }
  *output = std::move(cur);
  return Status::kOk;
}

uint32_t xform_d0(uint32_t x){return (x*0x01000193u)^0xBADC0FFEu;}
bool xform_p0(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d1(uint32_t x){return (x*0x01000193u)^0xBA895AABu;}
bool xform_p1(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d2(uint32_t x){return (x*0x01000193u)^0xBA76A554u;}
bool xform_p2(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d3(uint32_t x){return (x*0x01000193u)^0xBA23F001u;}
bool xform_p3(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d4(uint32_t x){return (x*0x01000193u)^0xBB895AAAu;}
bool xform_p4(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d5(uint32_t x){return (x*0x01000193u)^0xBB76A557u;}
bool xform_p5(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d6(uint32_t x){return (x*0x01000193u)^0xBB23F000u;}
bool xform_p6(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d7(uint32_t x){return (x*0x01000193u)^0xB8895AADu;}
bool xform_p7(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d8(uint32_t x){return (x*0x01000193u)^0xB876A556u;}
bool xform_p8(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d9(uint32_t x){return (x*0x01000193u)^0xB823F003u;}
bool xform_p9(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d10(uint32_t x){return (x*0x01000193u)^0xB9895AACu;}
bool xform_p10(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d11(uint32_t x){return (x*0x01000193u)^0xB976A559u;}
bool xform_p11(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d12(uint32_t x){return (x*0x01000193u)^0xB923F002u;}
bool xform_p12(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d13(uint32_t x){return (x*0x01000193u)^0xBE895AAFu;}
bool xform_p13(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d14(uint32_t x){return (x*0x01000193u)^0xBE76A558u;}
bool xform_p14(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t xform_d15(uint32_t x){return (x*0x01000193u)^0xBE23F005u;}
bool xform_p15(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
}  // namespace decode




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_84_transform_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 84u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
