#pragma once
#include "helion/buffer.h"
#include "helion/format/format_spec.h"
#include "helion/format/metadata_block.h"
#include "helion/format/object_graph.h"
#include "helion/format/string_table.h"
#include <vector>

namespace helion {
namespace format {

struct WriterSectionSpec {
  SectionKind kind = SectionKind::kReserved;
  uint16_t section_id = 0;
  GrowableBuffer payload;
  uint16_t flags = 0;
  uint32_t parent_id = 0;
};

class DocumentWriter {
 public:
  DocumentWriter();
  void Reset();
  Status AddSection(const WriterSectionSpec& spec);
  Status SetMetadata(const MetadataBlock& meta);
  Status SetGraph(const ObjectGraph& graph);
  Status SetStrings(const StringTable& strings);
  Status Finalize(GrowableBuffer* out);
  HeaderInfo Header() const { return header_; }

 private:
  Status LayoutSections();
  Status WriteHeader(GrowableBuffer* out);
  Status WriteSectionTable(GrowableBuffer* out);

  HeaderInfo header_;
  std::vector<WriterSectionSpec> sections_;
  MetadataBlock metadata_;
  ObjectGraph graph_;
  StringTable strings_;
  bool has_meta_ = false;
  bool has_graph_ = false;
  bool has_strings_ = false;
};

}  // namespace format
}  // namespace helion
