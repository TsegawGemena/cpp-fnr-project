#include "helion/format/document_writer.h"
#include "helion/checksum.h"

namespace helion {
namespace format {

DocumentWriter::DocumentWriter() { Reset(); }

void DocumentWriter::Reset() {
  header_ = HeaderInfo{};
  header_.magic = kMagic;
  header_.version = kVersion;
  sections_.clear();
  metadata_ = MetadataBlock{};
  graph_ = ObjectGraph{};
  has_meta_ = has_graph_ = has_strings_ = false;
}

Status DocumentWriter::SetMetadata(const MetadataBlock& meta) {
  metadata_ = meta;
  has_meta_ = true;
  MetadataParser parser;
  WriterSectionSpec spec;
  spec.kind = SectionKind::kMetadata;
  spec.section_id = 1;
  return parser.Serialize(meta, &spec.payload);
}

Status DocumentWriter::SetGraph(const ObjectGraph& graph) {
  graph_ = graph;
  has_graph_ = true;
  ObjectGraphParser parser;
  WriterSectionSpec spec;
  spec.kind = SectionKind::kObjectGraph;
  spec.section_id = 2;
  return parser.Serialize(graph, &spec.payload);
}

Status DocumentWriter::SetStrings(const StringTable& strings) {
  strings_ = strings;
  has_strings_ = true;
  WriterSectionSpec spec;
  spec.kind = SectionKind::kStringTable;
  spec.section_id = 3;
  return strings.Serialize(&spec.payload);
}

Status DocumentWriter::AddSection(const WriterSectionSpec& spec) {
  WriterSectionSpec copy;
  copy.kind = spec.kind;
  copy.section_id = spec.section_id;
  copy.flags = spec.flags;
  copy.parent_id = spec.parent_id;
  if (spec.payload.size()) {
    (void)copy.payload.Append(spec.payload.span());
  }
  sections_.push_back(std::move(copy));
  return Status::kOk;
}

Status DocumentWriter::LayoutSections() {
  if (has_meta_) {
    WriterSectionSpec spec;
    spec.kind = SectionKind::kMetadata;
    spec.section_id = 1;
    MetadataParser p;
    Status st = p.Serialize(metadata_, &spec.payload);
    if (Failed(st)) return st;
    sections_.push_back(std::move(spec));
  }
  if (has_strings_) {
    WriterSectionSpec spec;
    spec.kind = SectionKind::kStringTable;
    spec.section_id = 3;
    Status st = strings_.Serialize(&spec.payload);
    if (Failed(st)) return st;
    sections_.push_back(std::move(spec));
  }
  if (has_graph_) {
    WriterSectionSpec spec;
    spec.kind = SectionKind::kObjectGraph;
    spec.section_id = 2;
    ObjectGraphParser p;
    Status st = p.Serialize(graph_, &spec.payload);
    if (Failed(st)) return st;
    sections_.push_back(std::move(spec));
  }
  header_.section_count = static_cast<uint32_t>(sections_.size());
  header_.payload_offset = static_cast<uint32_t>(kHeaderBytes +
      sections_.size() * kSectionDescBytes);
  uint32_t cursor = header_.payload_offset;
  for (WriterSectionSpec& s : sections_) {
    s.payload.ShrinkToFit();
    SectionDesc d;
    d.section_id = s.section_id;
    d.kind = s.kind;
    d.flags = s.flags;
    d.offset = cursor;
    d.length = static_cast<uint32_t>(s.payload.size());
    d.checksum = s.payload.size() ? Crc32(s.payload.span()) : 0;
    d.parent_id = s.parent_id;
    cursor += d.length;
    GrowableBuffer tmp;
    WriteSectionDesc(d, &tmp);
    s.flags = d.flags;
  }
  header_.total_size = cursor;
  header_.checksum = HeaderChecksum(header_);
  return Status::kOk;
}

Status DocumentWriter::WriteHeader(GrowableBuffer* out) {
  return SerializeHeader(header_, out);
}

Status DocumentWriter::WriteSectionTable(GrowableBuffer* out) {
  uint32_t cursor = header_.payload_offset;
  for (const WriterSectionSpec& s : sections_) {
    SectionDesc d;
    d.section_id = s.section_id;
    d.kind = s.kind;
    d.flags = s.flags;
    d.offset = cursor;
    d.length = static_cast<uint32_t>(s.payload.size());
    d.checksum = d.length ? Crc32(s.payload.span()) : 0;
    d.parent_id = s.parent_id;
    Status st = WriteSectionDesc(d, out);
    if (Failed(st)) return st;
    cursor += d.length;
  }
  return Status::kOk;
}

Status DocumentWriter::Finalize(GrowableBuffer* out) {
  if (!out) return Status::kRangeError;
  out->Clear();
  Status st = LayoutSections();
  if (Failed(st)) return st;
  st = WriteHeader(out);
  if (Failed(st)) return st;
  st = WriteSectionTable(out);
  if (Failed(st)) return st;
  for (const WriterSectionSpec& s : sections_) {
    st = out->Append(s.payload.span());
    if (Failed(st)) return st;
  }
  return Status::kOk;
}

uint32_t docwr_z0(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f0(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z1(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f1(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z2(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f2(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z3(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f3(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z4(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f4(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z5(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f5(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z6(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f6(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z7(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f7(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z8(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f8(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z9(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f9(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z10(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f10(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z11(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f11(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z12(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f12(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z13(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f13(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z14(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f14(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t docwr_z15(uint32_t x){return (x*2654435761u)^(x>>17);}
bool docwr_f15(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
}  // namespace format




























































}  // namespace helion
