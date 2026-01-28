#include "helion/buffer.h"
#include "helion/format/document_writer.h"
#include "helion/format/metadata_block.h"
#include "helion/format/object_graph.h"
#include "helion/types.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace {

bool WriteFile(const std::string& path, helion::GrowableBuffer& buf) {
  std::ofstream out(path, std::ios::binary);
  if (!out) return false;
  out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
  return static_cast<bool>(out);
}

helion::GrowableBuffer MakePayloadWords(const std::vector<uint32_t>& words) {
  helion::GrowableBuffer buf;
  for (uint32_t w : words) {
    uint8_t b[4];
    b[0] = static_cast<uint8_t>(w);
    b[1] = static_cast<uint8_t>(w >> 8);
    b[2] = static_cast<uint8_t>(w >> 16);
    b[3] = static_cast<uint8_t>(w >> 24);
    (void)buf.Append(helion::ByteSpan(b, 4));
  }
  return buf;
}

bool WriteBaseline(const std::string& dir) {
  helion::format::DocumentWriter writer;
  writer.Reset();
  helion::format::WriterSectionSpec spec;
  spec.kind = helion::SectionKind::kPayload;
  spec.section_id = 5;
  spec.payload = MakePayloadWords({0x01020304, 0x05060708});
  (void)writer.AddSection(spec);
  helion::GrowableBuffer out;
  if (helion::Failed(writer.Finalize(&out))) return false;
  return WriteFile(dir + "/baseline.hpk", out);
}

bool WriteMultipass(const std::string& dir) {
  helion::format::DocumentWriter writer;
  writer.Reset();
  helion::format::MetadataBlock meta;
  meta.version = 1;
  meta.flags = 0;
  (void)writer.SetMetadata(meta);

  for (uint16_t id = 5; id <= 6; ++id) {
    helion::format::WriterSectionSpec spec;
    spec.kind = helion::SectionKind::kPayload;
    spec.section_id = id;
    spec.payload = MakePayloadWords({static_cast<uint32_t>(id), static_cast<uint32_t>(id * 2),
                                     static_cast<uint32_t>(id * 3), static_cast<uint32_t>(id * 4)});
    (void)writer.AddSection(spec);
  }

  helion::format::WriterSectionSpec hint;
  hint.kind = helion::SectionKind::kRecoveryHint;
  hint.section_id = 8;
  hint.payload = MakePayloadWords({0xDEC0BE01u, 0xDEC0BE02u});
  (void)writer.AddSection(hint);

  helion::GrowableBuffer out;
  if (helion::Failed(writer.Finalize(&out))) return false;
  return WriteFile(dir + "/multipass.hpk", out);
}

bool WriteDense(const std::string& dir) {
  helion::format::DocumentWriter writer;
  writer.Reset();
  for (uint16_t id = 1; id <= 8; ++id) {
    helion::format::WriterSectionSpec spec;
    spec.kind = static_cast<helion::SectionKind>(id);
    spec.section_id = id;
    std::vector<uint32_t> words;
    for (int j = 0; j < 16; ++j) words.push_back((id << 16) | static_cast<uint32_t>(j));
    spec.payload = MakePayloadWords(words);
    (void)writer.AddSection(spec);
  }
  helion::GrowableBuffer out;
  if (helion::Failed(writer.Finalize(&out))) return false;
  return WriteFile(dir + "/dense.hpk", out);
}

bool WriteRecover(const std::string& dir) {
  helion::format::DocumentWriter writer;
  writer.Reset();
  helion::format::WriterSectionSpec payload;
  payload.kind = helion::SectionKind::kPayload;
  payload.section_id = 5;
  payload.flags = 0x8;  // kFlagRecoveryHint
  payload.payload = MakePayloadWords({0xDEADBEEF, 0xCAFEBABE, 0x12345678});
  (void)writer.AddSection(payload);

  helion::format::WriterSectionSpec hint;
  hint.kind = helion::SectionKind::kRecoveryHint;
  hint.section_id = 7;
  hint.payload = MakePayloadWords({1, 2, 3, 4});
  (void)writer.AddSection(hint);

  helion::GrowableBuffer out;
  if (helion::Failed(writer.Finalize(&out))) return false;
  return WriteFile(dir + "/recover.hpk", out);
}

bool WriteDualOwn(const std::string& dir) {
  helion::format::DocumentWriter writer;
  writer.Reset();
  for (uint16_t id = 5; id <= 6; ++id) {
    helion::format::WriterSectionSpec spec;
    spec.kind = helion::SectionKind::kPayload;
    spec.section_id = id;
    spec.flags = 0x4;  // kFlagStreaming — dual ownership staging
    spec.payload = MakePayloadWords({0xA11C001Du, static_cast<uint32_t>(id), 0xBEEFu});
    (void)writer.AddSection(spec);
  }
  helion::GrowableBuffer out;
  if (helion::Failed(writer.Finalize(&out))) return false;
  return WriteFile(dir + "/dualown.hpk", out);
}

bool WriteGraph(const std::string& dir) {
  helion::format::DocumentWriter writer;
  writer.Reset();
  helion::format::ObjectGraph graph;
  graph.version = 1;
  helion::format::GraphNode root;
  root.id = 1;
  root.kind = helion::format::NodeKind::kObject;
  root.type_id = 42;
  root.children = {2, 3};
  graph.nodes.push_back(root);

  helion::format::GraphNode child;
  child.id = 2;
  child.kind = helion::format::NodeKind::kScalar;
  child.parent = 1;
  graph.nodes.push_back(child);

  child.id = 3;
  child.kind = helion::format::NodeKind::kArray;
  graph.nodes.push_back(child);

  helion::format::GraphEdge edge;
  edge.from = 1;
  edge.to = 2;
  edge.label = 1;
  graph.edges.push_back(edge);

  (void)writer.SetGraph(graph);

  helion::format::WriterSectionSpec payload;
  payload.kind = helion::SectionKind::kPayload;
  payload.section_id = 5;
  payload.payload = MakePayloadWords({0x60000001u, 0x60000002u});
  (void)writer.AddSection(payload);

  helion::GrowableBuffer out;
  if (helion::Failed(writer.Finalize(&out))) return false;
  return WriteFile(dir + "/graph.hpk", out);
}

bool CopyCorpusSet(const std::string& src_dir, const std::string& dst_dir, bool include_dual) {
  const char* names[] = {"baseline.hpk", "multipass.hpk", "dense.hpk", "recover.hpk", "graph.hpk"};
  for (const char* name : names) {
    std::ifstream in(src_dir + "/" + name, std::ios::binary);
    std::ofstream out(dst_dir + "/" + name, std::ios::binary);
    if (!in || !out) return false;
    out << in.rdbuf();
  }
  if (include_dual) {
    std::ifstream in(src_dir + "/dualown.hpk", std::ios::binary);
    std::ofstream out(dst_dir + "/dualown.hpk", std::ios::binary);
    if (!in || !out) return false;
    out << in.rdbuf();
  }
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  const std::string root = (argc > 1) ? argv[1] : ".";
  const std::string tmp = root + "/fuzz/corpus/_gen";

  if (!WriteBaseline(tmp) || !WriteMultipass(tmp) || !WriteDense(tmp) ||
      !WriteRecover(tmp) || !WriteGraph(tmp) || !WriteDualOwn(tmp)) {
    std::fprintf(stderr, "gen_corpus: failed to write seed files\n");
    return 1;
  }

  if (!CopyCorpusSet(tmp, root + "/fuzz/corpus/parser", false) ||
      !CopyCorpusSet(tmp, root + "/fuzz/corpus/decoder", false) ||
      !CopyCorpusSet(tmp, root + "/fuzz/corpus/state", true)) {
    std::fprintf(stderr, "gen_corpus: failed to copy corpus sets\n");
    return 1;
  }

  std::printf("Generated HPK1 corpus seeds in fuzz/corpus/{parser,decoder,state}\n");
  return 0;
}
