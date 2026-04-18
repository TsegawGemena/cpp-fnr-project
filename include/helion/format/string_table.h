#pragma once
#include "helion/buffer.h"
#include "helion/span.h"
#include "helion/status.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace helion {
namespace format {

struct StringEntry {
  uint32_t id = 0;
  uint32_t offset = 0;
  uint32_t length = 0;
  std::string value;
};

class StringTable {
 public:
  Status Parse(ByteSpan data);
  Status Build(const std::vector<std::string>& strings);
  Status Serialize(GrowableBuffer* out) const;
  const std::string* Lookup(uint32_t id) const;
  const std::string* LookupByOffset(uint32_t offset) const;
  size_t Count() const { return entries_.size(); }
  const std::vector<StringEntry>& Entries() const { return entries_; }

 private:
  std::vector<StringEntry> entries_;
  std::unordered_map<uint32_t, size_t> id_map_;
};

}  // namespace format
}  // namespace helion

