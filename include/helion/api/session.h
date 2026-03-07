#pragma once
#include "helion/api/diagnostics.h"
#include "helion/api/parser.h"
#include "helion/arena.h"
#include "helion/core/generation_tracker.h"
#include "helion/core/lookup_cache.h"
#include "helion/core/state_machine.h"
#include "helion/core/view_cache.h"
#include "helion/decode/payload_decoder.h"
#include "helion/format/section_index.h"
#include "helion/format/section_loader.h"
#include "helion/format/section_table.h"
#include "helion/io/stream_buffer.h"
#include "helion/object_pool.h"
#include "helion/runtime/event_bus.h"
#include "helion/runtime/recovery.h"
#include "helion/runtime/reindexer.h"
#include "helion/types.h"
#include <vector>

namespace helion {
namespace api {

struct SessionConfig {
  size_t arena_block_size = 4096;
  InterpretMode mode = InterpretMode::kRaw;
  bool enable_recovery = true;
};

class Session {
 public:
  Session();
  explicit Session(SessionConfig config);
  ~Session();

  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;

  Status ProcessInput(ByteSpan doc);
  Status ProcessPartial(ByteSpan chunk);
  Status Finalize();
  Status StageDocument(ByteSpan doc);
  Status StageDocument(uint16_t section_id);
  Status ResumeAfterRecovery();
  Status Reindex();
  Status DecodePayload(uint16_t section_id, decode::DecodedPayload* out);
  Status ParseDocument(ByteSpan doc, ParseResult* out);

  ProcessState State() const;
  SessionStats Stats() const { return stats_; }
  Diagnostics* GetDiagnostics() { return &diagnostics_; }
  const HeaderInfo& Header() const { return header_; }
  format::SectionTable* Table() { return &table_; }
  core::ViewCache* ViewCache() { return &view_cache_; }

 private:
  Status EnsureIndexed();
  Status LoadAndStage(uint16_t section_id);
  Status LoadAllStagedSections(ByteSpan doc);
  Status EnterRecovery();
  Status CompleteReindex();
  void BindSubsystems();

  SessionConfig config_;
  Arena arena_;
  ObjectPool pool_;
  format::SectionTable table_;
  format::SectionIndex section_index_;
  format::SectionLoader section_loader_;
  core::ViewCache view_cache_;
  core::LookupCache lookup_cache_;
  core::GenerationTracker generation_tracker_;
  core::StateMachine state_machine_;
  decode::PayloadDecoder payload_decoder_;
  runtime::RecoveryManager recovery_;
  runtime::Reindexer reindexer_;
  runtime::EventBus event_bus_;
  DocumentParser parser_;
  Diagnostics diagnostics_;
  io::StreamBuffer input_buffer_;
  HeaderInfo header_;
  ByteSpan document_;
  SessionStats stats_;
  bool document_owned_ = false;
  uint8_t* owned_document_ = nullptr;
  size_t owned_document_size_ = 0;
  bool multipass_ = false;
  bool recovered_ = false;
};

}  // namespace api
}  // namespace helion

