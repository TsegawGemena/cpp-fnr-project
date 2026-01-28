#pragma once
#include "helion/decode/payload_decoder.h"
#include "helion/format/object_graph.h"
#include <unordered_map>

namespace helion {
namespace decode {

struct DecodedNode {
  uint32_t id = 0;
  format::NodeKind kind = format::NodeKind::kObject;
  std::vector<uint32_t> fields;
  ByteSpan raw;
};

class GraphDecoder {
 public:
  GraphDecoder();
  Status DecodeGraph(ByteSpan data, format::ObjectGraph* graph);
  Status DecodeNodePayload(const format::GraphNode& node, DecodedNode* out);
  Status Materialize(const format::ObjectGraph& graph, std::vector<DecodedNode>* nodes);

 private:
  PayloadDecoder payload_decoder_;
  std::unordered_map<uint32_t, DecodedNode> cache_;
};

}  // namespace decode
}  // namespace helion
