# libhelion
C++17 library for parsing, staging, and decoding **HPK1** binary documents.
## Features
- HPK1 header and section-table parsing with CRC32 integrity checks
- Multipass session API: stage documents, recover arena state, reindex, and decode payloads
- Section loader with arena/object-pool integration for staged views
- Payload and object-graph decoders
- Optional schema validation and profile constraints
- Streaming partial input via `ProcessPartial` / `Finalize`
## Build
### CMake
```bash
mkdir -p build && cd build
cmake .. -DHELION_BUILD_FUZZERS=ON
cmake --build .
```
Library target: `helion`. Fuzz targets (when enabled): `parser_fuzzer`, `decoder_fuzzer`, `state_machine_fuzzer`.
### ClusterFuzzLite / OSS-Fuzz style
```bash
export SRC=/path/to/cpp-fnr-project
export WORK=/tmp/helion-work
export OUT=/tmp/helion-out
export CXX=clang++
export CXXFLAGS="-O1 -g -fsanitize=address,fuzzer-no-link"
export LIB_FUZZING_ENGINE="-fsanitize=fuzzer,address"
bash "$SRC/.clusterfuzzlite/build.sh"
```
Or use the convenience wrapper:
```bash
bash build.sh
```
## Corpus seeds
Generate valid HPK1 seed files for fuzzing:
```bash
bash tools/make_corpus.sh
```
Seeds are written to `fuzz/corpus/parser/`, `fuzz/corpus/decoder/`, and `fuzz/corpus/state/`.
## HPK1 layout
| Offset | Field | Size |
|--------|-------|------|
| 0 | magic (`HPK1` = `0x314B5048`) | 4 |
| 4 | version | 2 |
| 6 | flags | 2 |
| 8 | section_count | 4 |
| 12 | index_offset | 4 |
| 16 | payload_offset | 4 |
| 20 | total_size | 4 |
| 24 | checksum (CRC32 of bytes 0–55) | 4 |
| 28 | feature_bits | 4 |
| 32 | document_id | 8 |
| 40 | generation | 4 |
| 44–63 | reserved | 20 |
Each **section descriptor** is 32 bytes: `section_id`, `kind`, `flags`, padding, `offset`, `length`, `checksum`, `parent_id`, `aux`.
Document layout: `[64-byte header][section table][payload blobs]`.
**Section kinds:** Metadata (1), ObjectGraph (2), StringTable (3), Index (4), Payload (5), Checksum (6), RecoveryHint (7), Extension (8).
## API overview
```cpp
#include "helion/api/session.h"
helion::api::Session session;
session.ProcessInput(document_bytes);           // parse header + index sections
session.StageDocument(document_bytes);          // multipass staging
session.Reindex();                              // recovery + rebuild index
session.DecodePayload(section_id, &payload);  // decode staged payload
helion::api::DocumentParser parser;
helion::ParseResult result;
parser.ParseDocument(document_bytes, &result);
helion::format::DocumentWriter writer;
writer.AddSection(spec);
writer.Finalize(&output_buffer);
```
Primary entry points live under `include/helion/`:
- `helion/api/session.h` — multipass session lifecycle
- `helion/api/parser.h` — one-shot document parsing
- `helion/format/document_writer.h` — HPK1 serialization
- `helion/format/format_spec.h` — header layout helpers
- `helion/decode/payload_decoder.h` — payload word decoding
## License
Apache-2.0 — see [LICENSE](LICENSE).
