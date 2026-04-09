#pragma once
#include "helion/buffer.h"
#include "helion/span.h"
#include "helion/status.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {
namespace format {

struct MetadataField {
  uint16_t key_id = 0;
  uint16_t type_id = 0;
  ByteSpan value;
};

struct MetadataBlock {
  uint32_t version = 0;
  uint32_t flags = 0;
  std::vector<MetadataField> fields;
};

class MetadataParser {
 public:
  Status Parse(ByteSpan data, MetadataBlock* out);
  Status Serialize(const MetadataBlock& block, GrowableBuffer* out);
  const MetadataField* FindField(const MetadataBlock& block, uint16_t key_id) const;
  Status AddField(MetadataBlock* block, uint16_t key_id, uint16_t type_id, ByteSpan value);
  std::string FieldAsString(const MetadataField& field) const;
};

}  // namespace format
}  // namespace helion

