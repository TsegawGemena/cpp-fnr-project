#include "helion/format/object_graph.h"
#include "helion/common/endian.h"
#include "helion/io/cursor.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace helion {
namespace format {

Status ObjectGraphParser::Parse(ByteSpan data, ObjectGraph* out) {
  if (!out) return Status::kRangeError;
  io::ReadCursor cur(data);
  uint32_t ver = 0, node_count = 0, edge_count = 0;
  Status st = cur.ReadU32LE(&ver);
  if (Failed(st)) return st;
  st = cur.ReadU32LE(&node_count);
  if (Failed(st)) return st;
  st = cur.ReadU32LE(&edge_count);
  if (Failed(st)) return st;
  out->version = ver;
  out->nodes.clear();
  out->edges.clear();
  out->nodes.reserve(node_count);
  for (uint32_t i = 0; i < node_count; ++i) {
    GraphNode n;
    uint16_t kind = 0;
    uint32_t plen = 0;
    st = cur.ReadU32LE(&n.id);
    if (Failed(st)) return st;
    st = cur.ReadU16LE(&kind);
    if (Failed(st)) return st;
    st = cur.ReadU32LE(&n.type_id);
    if (Failed(st)) return st;
    st = cur.ReadU32LE(&n.parent);
    if (Failed(st)) return st;
    st = cur.ReadU32LE(&plen);
    if (Failed(st)) return st;
    st = cur.ReadBytes(plen, &n.payload);
    if (Failed(st)) return st;
    uint32_t cc = 0;
    st = cur.ReadU32LE(&cc);
    if (Failed(st)) return st;
    n.kind = static_cast<NodeKind>(kind);
    n.children.reserve(cc);
    for (uint32_t c = 0; c < cc; ++c) {
      uint32_t child = 0;
      st = cur.ReadU32LE(&child);
      if (Failed(st)) return st;
      n.children.push_back(child);
    }
    out->nodes.push_back(n);
  }
  for (uint32_t i = 0; i < edge_count; ++i) {
    GraphEdge e;
    st = cur.ReadU32LE(&e.from);
    if (Failed(st)) return st;
    st = cur.ReadU32LE(&e.to);
    if (Failed(st)) return st;
    st = cur.ReadU16LE(&e.label);
    if (Failed(st)) return st;
    st = cur.ReadU16LE(&e.flags);
    if (Failed(st)) return st;
    out->edges.push_back(e);
  }
  return Status::kOk;
}

Status ObjectGraphParser::Serialize(const ObjectGraph& graph, GrowableBuffer* out) {
  if (!out) return Status::kRangeError;
  uint8_t hdr[12];
  endian::WriteU32LE(hdr, graph.version);
  endian::WriteU32LE(hdr + 4, static_cast<uint32_t>(graph.nodes.size()));
  endian::WriteU32LE(hdr + 8, static_cast<uint32_t>(graph.edges.size()));
  Status st = out->Append(ByteSpan(hdr, 12));
  if (Failed(st)) return st;
  for (const GraphNode& n : graph.nodes) {
    st = out->AppendByte(static_cast<uint8_t>(n.id));
    if (Failed(st)) return st;
    uint8_t rec[20];
    endian::WriteU32LE(rec, n.id);
    endian::WriteU16LE(rec + 4, static_cast<uint16_t>(n.kind));
    endian::WriteU32LE(rec + 6, n.type_id);
    endian::WriteU32LE(rec + 10, n.parent);
    endian::WriteU32LE(rec + 14, static_cast<uint32_t>(n.payload.size));
    st = out->Append(ByteSpan(rec, 18));
    if (Failed(st)) return st;
    st = out->Append(n.payload);
    if (Failed(st)) return st;
    endian::WriteU32LE(rec, static_cast<uint32_t>(n.children.size()));
    st = out->Append(ByteSpan(rec, 4));
    if (Failed(st)) return st;
    for (uint32_t c : n.children) {
      endian::WriteU32LE(rec, c);
      st = out->Append(ByteSpan(rec, 4));
      if (Failed(st)) return st;
    }
  }
  for (const GraphEdge& e : graph.edges) {
    uint8_t rec[12];
    endian::WriteU32LE(rec, e.from);
    endian::WriteU32LE(rec + 4, e.to);
    endian::WriteU16LE(rec + 8, e.label);
    endian::WriteU16LE(rec + 10, e.flags);
    st = out->Append(ByteSpan(rec, 12));
    if (Failed(st)) return st;
  }
  return Status::kOk;
}

const GraphNode* ObjectGraphParser::FindNode(const ObjectGraph& g, uint32_t id) const {
  for (const GraphNode& n : g.nodes)
    if (n.id == id) return &n;
  return nullptr;
}

std::vector<uint32_t> ObjectGraphParser::TopologicalOrder(const ObjectGraph& g) const {
  std::unordered_map<uint32_t, size_t> indeg;
  for (const GraphNode& n : g.nodes) indeg[n.id] = 0;
  for (const GraphEdge& e : g.edges) indeg[e.to]++;
  std::queue<uint32_t> q;
  for (const auto& kv : indeg)
    if (kv.second == 0) q.push(kv.first);
  std::vector<uint32_t> order;
  while (!q.empty()) {
    uint32_t u = q.front();
    q.pop();
    order.push_back(u);
    for (const GraphEdge& e : g.edges) {
      if (e.from != u) continue;
      if (--indeg[e.to] == 0) q.push(e.to);
    }
  }
  return order;
}

Status ObjectGraphParser::Validate(const ObjectGraph& g) const {
  std::unordered_set<uint32_t> ids;
  for (const GraphNode& n : g.nodes) {
    if (!ids.insert(n.id).second) return Status::kInvalidSection;
  }
  for (const GraphEdge& e : g.edges) {
    if (!ids.count(e.from) || !ids.count(e.to)) return Status::kInvalidSection;
  }
  return Status::kOk;
}

uint32_t ograph_t0(uint32_t x){return x^0xCAFEBABEu;}
bool ograph_b0(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t1(uint32_t x){return x^0xCAEFABAFu;}
bool ograph_b1(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t2(uint32_t x){return x^0xCADC989Cu;}
bool ograph_b2(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t3(uint32_t x){return x^0xCACD898Du;}
bool ograph_b3(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t4(uint32_t x){return x^0xCABAFEFAu;}
bool ograph_b4(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t5(uint32_t x){return x^0xCAABEFEBu;}
bool ograph_b5(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t6(uint32_t x){return x^0xCA98DCD8u;}
bool ograph_b6(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t7(uint32_t x){return x^0xCA89CDC9u;}
bool ograph_b7(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t8(uint32_t x){return x^0xCA763236u;}
bool ograph_b8(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t9(uint32_t x){return x^0xCA672327u;}
bool ograph_b9(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t10(uint32_t x){return x^0xCA541014u;}
bool ograph_b10(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t11(uint32_t x){return x^0xCA450105u;}
bool ograph_b11(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t12(uint32_t x){return x^0xCA327672u;}
bool ograph_b12(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t13(uint32_t x){return x^0xCA236763u;}
bool ograph_b13(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t14(uint32_t x){return x^0xCA105450u;}
bool ograph_b14(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t ograph_t15(uint32_t x){return x^0xCA014541u;}
bool ograph_b15(uint32_t a,uint32_t b){return a+b>=a;}
}  // namespace format




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_90_object_graph_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 90u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
