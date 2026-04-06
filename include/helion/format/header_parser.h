#pragma once
#include "helion/format/format_spec.h"
#include "helion/io/cursor.h"
#include "helion/io/stream_buffer.h"
#include <vector>

namespace helion {
namespace format {

struct ParseContext {
  HeaderInfo header;
  std::vector<SectionDesc> sections;
  size_t bytes_consumed = 0;
  bool partial = false;
};

class HeaderParser {
 public:
  HeaderParser();
  void Reset();
  Status Feed(ByteSpan chunk);
  Status Finalize(ParseContext* ctx);
  bool Complete() const { return complete_; }
  Status ParseAll(ByteSpan doc, ParseContext* ctx);

 private:
  Status TryParseHeader();
  Status ParseSectionTable();
  io::StreamBuffer buffer_;
  ParseContext ctx_;
  bool complete_ = false;
};

}  // namespace format
}  // namespace helion

