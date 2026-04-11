#pragma once
#include "helion/buffer.h"
#include "helion/span.h"
#include "helion/status.h"
#include <cstdint>
#include <vector>

namespace helion {
namespace format {

enum class NodeKind : uint16_t { kObject = 1, kArray = 2, kScalar = 3, kRef = 4, kEdge = 5 };

struct GraphNode {
  uint32_t id = 0;
  NodeKind kind = NodeKind::kObject;
  uint32_t type_id = 0;
  uint32_t parent = 0;
  std::vector<uint32_t> children;
  ByteSpan payload;
};

struct GraphEdge {
  uint32_t from = 0;
  uint32_t to = 0;
  uint16_t label = 0;
  uint16_t flags = 0;
};

struct ObjectGraph {
  uint32_t version = 0;
  std::vector<GraphNode> nodes;
  std::vector<GraphEdge> edges;
};

class ObjectGraphParser {
 public:
  Status Parse(ByteSpan data, ObjectGraph* out);
  Status Serialize(const ObjectGraph& graph, GrowableBuffer* out);
  const GraphNode* FindNode(const ObjectGraph& g, uint32_t id) const;
  std::vector<uint32_t> TopologicalOrder(const ObjectGraph& g) const;
  Status Validate(const ObjectGraph& g) const;
};

}  // namespace format
}  // namespace helion

