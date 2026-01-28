#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CXX="${CXX:-clang++}"
WORK="${WORK:-/tmp/helion-corpus-work}"
OUT="${OUT:-/tmp/helion-corpus-bin}"

mkdir -p "${WORK}" "${OUT}"
mkdir -p "${ROOT}/fuzz/corpus/parser" "${ROOT}/fuzz/corpus/decoder" "${ROOT}/fuzz/corpus/state"
mkdir -p "${ROOT}/fuzz/corpus/_gen"

OBJECTS=()
while IFS= read -r -d '' src_file; do
  rel="${src_file#${ROOT}/}"
  obj="${WORK}/${rel%.cpp}.o"
  mkdir -p "$(dirname "${obj}")"
  ${CXX} -std=c++17 -O1 -g -I"${ROOT}/include" -c "${src_file}" -o "${obj}"
  OBJECTS+=("${obj}")
done < <(find "${ROOT}/src" -name '*.cpp' -print0 | sort -z)

${CXX} -std=c++17 -O1 -g -I"${ROOT}/include" \
  "${ROOT}/tools/gen_corpus.cc" "${OBJECTS[@]}" \
  -o "${OUT}/gen_corpus"

"${OUT}/gen_corpus" "${ROOT}"
