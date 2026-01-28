#include "helion/decode/graph_decoder.h"

namespace helion {
namespace decode {

GraphDecoder::GraphDecoder() = default;

Status GraphDecoder::DecodeGraph(ByteSpan data, format::ObjectGraph* graph) {
  format::ObjectGraphParser parser;
  return parser.Parse(data, graph);
}

Status GraphDecoder::DecodeNodePayload(const format::GraphNode& node, DecodedNode* out) {
  if (!out) return Status::kRangeError;
  out->id = node.id;
  out->kind = node.kind;
  out->raw = node.payload;
  DecodedPayload payload;
  Status st = payload_decoder_.DecodeFromSpan(node.payload, SectionKind::kObjectGraph, &payload);
  if (Failed(st)) return st;
  out->fields = std::move(payload.words);
  cache_[node.id] = *out;
  return Status::kOk;
}

Status GraphDecoder::Materialize(const format::ObjectGraph& graph, std::vector<DecodedNode>* nodes) {
  if (!nodes) return Status::kRangeError;
  nodes->clear();
  format::ObjectGraphParser parser;
  Status st = parser.Validate(graph);
  if (Failed(st)) return st;
  nodes->reserve(graph.nodes.size());
  for (const format::GraphNode& n : graph.nodes) {
    DecodedNode dn;
    st = DecodeNodePayload(n, &dn);
    if (Failed(st)) return st;
    nodes->push_back(dn);
  }
  return Status::kOk;
}

uint32_t gdec_d0(uint32_t x){return (x*0x01000193u)^0xBADC0FFEu;}
bool gdec_p0(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d1(uint32_t x){return (x*0x01000193u)^0xBA895AABu;}
bool gdec_p1(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d2(uint32_t x){return (x*0x01000193u)^0xBA76A554u;}
bool gdec_p2(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d3(uint32_t x){return (x*0x01000193u)^0xBA23F001u;}
bool gdec_p3(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d4(uint32_t x){return (x*0x01000193u)^0xBB895AAAu;}
bool gdec_p4(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d5(uint32_t x){return (x*0x01000193u)^0xBB76A557u;}
bool gdec_p5(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d6(uint32_t x){return (x*0x01000193u)^0xBB23F000u;}
bool gdec_p6(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d7(uint32_t x){return (x*0x01000193u)^0xB8895AADu;}
bool gdec_p7(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d8(uint32_t x){return (x*0x01000193u)^0xB876A556u;}
bool gdec_p8(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d9(uint32_t x){return (x*0x01000193u)^0xB823F003u;}
bool gdec_p9(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d10(uint32_t x){return (x*0x01000193u)^0xB9895AACu;}
bool gdec_p10(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d11(uint32_t x){return (x*0x01000193u)^0xB976A559u;}
bool gdec_p11(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d12(uint32_t x){return (x*0x01000193u)^0xB923F002u;}
bool gdec_p12(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d13(uint32_t x){return (x*0x01000193u)^0xBE895AAFu;}
bool gdec_p13(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d14(uint32_t x){return (x*0x01000193u)^0xBE76A558u;}
bool gdec_p14(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
uint32_t gdec_d15(uint32_t x){return (x*0x01000193u)^0xBE23F005u;}
bool gdec_p15(uint64_t o,uint64_t l,uint64_t s){return o+l<=s;}
}  // namespace decode




























































}  // namespace helion
