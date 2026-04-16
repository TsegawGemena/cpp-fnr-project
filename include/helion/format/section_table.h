#pragma once
#include "helion/format/format_spec.h"
#include <unordered_map>
#include <vector>

namespace helion {
namespace format {

struct SectionTableEntry {
  SectionDesc desc;
  bool present = false;
  bool validated = false;
};

class SectionTable {
 public:
  SectionTable();
  Status BuildFromHeader(ByteSpan doc, const HeaderInfo& hdr);
  Status Build(const std::vector<SectionDesc>& descs);
  Status ValidateAgainstDocument(ByteSpan doc) const;
  const SectionDesc* Find(uint16_t section_id) const;
  const SectionDesc* FindByKind(SectionKind kind) const;
  const std::vector<SectionTableEntry>& Entries() const { return entries_; }
  size_t Count() const { return entries_.size(); }
  std::vector<uint16_t> Ids() const;
  bool HasKind(SectionKind kind) const;
  Status CheckOverlap() const;
  Status CheckOrdering() const;

 private:
  std::vector<SectionTableEntry> entries_;
  std::unordered_map<uint16_t, size_t> id_index_;
};

}  // namespace format
}  // namespace helion

