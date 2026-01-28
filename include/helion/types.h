#pragma once
#include "helion/span.h"
#include "helion/status.h"

#include <cstdint>
#include <vector>

namespace helion {

constexpr uint32_t kMagicHPK1 = 0x314B5048u;  // 'HPK1' LE
constexpr uint16_t kFormatVersion = 0x0001;
constexpr size_t kHeaderSize = 64;
constexpr size_t kSectionDescSize = 32;
constexpr size_t kMaxSections = 256;
constexpr size_t kMaxPayloadBytes = 16u * 1024u * 1024u;

enum class SectionKind : uint16_t {
  kReserved = 0,
  kMetadata = 1,
  kObjectGraph = 2,
  kStringTable = 3,
  kIndex = 4,
  kPayload = 5,
  kChecksum = 6,
  kRecoveryHint = 7,
  kExtension = 8,
};

enum class Ownership : uint8_t {
  kNone = 0,
  kBorrowed = 1,
  kArenaPinned = 2,
  kPoolOwned = 3,
  kExternalHeap = 4,
};

enum class ProcessState : uint8_t {
  kIdle = 0,
  kHeaderParsed = 1,
  kSectionsIndexed = 2,
  kStaged = 3,
  kPartial = 4,
  kRecovering = 5,
  kReindexed = 6,
  kDecoding = 7,
  kFinalized = 8,
  kFailed = 9,
};

enum class InterpretMode : uint8_t {
  kRaw = 0,
  kGraph = 1,
  kStreaming = 2,
  kRecovered = 3,
};

struct HeaderInfo {
  uint32_t magic = 0;
  uint16_t version = 0;
  uint16_t flags = 0;
  uint32_t section_count = 0;
  uint32_t index_offset = 0;
  uint32_t payload_offset = 0;
  uint32_t total_size = 0;
  uint32_t checksum = 0;
  uint32_t feature_bits = 0;
  uint64_t document_id = 0;
  uint32_t generation = 0;
};

struct SectionDesc {
  uint16_t section_id = 0;
  SectionKind kind = SectionKind::kReserved;
  uint16_t flags = 0;
  uint32_t offset = 0;
  uint32_t length = 0;
  uint32_t checksum = 0;
  uint32_t parent_id = 0;
  uint32_t aux = 0;
};

struct TypedObject {
  SectionKind kind = SectionKind::kReserved;
  uint32_t generation = 0;
  uint32_t word_count = 0;
  void* heap_extension = nullptr;
  size_t extension_bytes = 0;
  Ownership extension_owner = Ownership::kNone;
};

struct SessionStats {
  size_t bytes_consumed = 0;
  size_t sections_parsed = 0;
  size_t decode_ops = 0;
  size_t recovery_ops = 0;
  size_t reindex_ops = 0;
  size_t cache_hits = 0;
  size_t cache_misses = 0;
};

}  // namespace helion
