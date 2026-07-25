#pragma once
#include "helion/api/diagnostics.h"
#include "helion/format/header_parser.h"
#include "helion/format/section_table.h"
#include "helion/types.h"

namespace helion {
namespace api {

struct ParseResult {
  HeaderInfo header;
  format::SectionTable table;
  format::ParseContext context;
  size_t bytes_consumed = 0;
};

class DocumentParser {
 public:
  DocumentParser();
  void Reset();
  Status ParseDocument(ByteSpan doc, ParseResult* out);
  Status ParsePartial(ByteSpan chunk, ParseResult* out);
  Status FinalizePartial(ParseResult* out);
  Diagnostics* GetDiagnostics() { return &diagnostics_; }

 private:
  Status ValidateTable(const format::SectionTable& table);
  Status IndexSections(const format::ParseContext& ctx, format::SectionTable* table);

  format::HeaderParser header_parser_;
  Diagnostics diagnostics_;
  bool partial_mode_ = false;
};

}  // namespace api
}  // namespace helion


