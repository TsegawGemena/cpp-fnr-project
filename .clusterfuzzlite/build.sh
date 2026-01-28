#!/bin/bash
set -euo pipefail

: "${SRC:?SRC must be set}"
: "${WORK:?WORK must be set}"
: "${OUT:?OUT must be set}"

CXX="${CXX:-clang++}"
CXXFLAGS="${CXXFLAGS:--O1 -g -std=c++17}"
LIB_FUZZING_ENGINE="${LIB_FUZZING_ENGINE:--fsanitize=fuzzer,address}"

mkdir -p "${WORK}" "${OUT}"

OBJECTS=()
while IFS= read -r -d '' src_file; do
  rel="${src_file#${SRC}/}"
  obj="${WORK}/${rel%.cpp}.o"
  mkdir -p "$(dirname "${obj}")"
  ${CXX} ${CXXFLAGS} -std=c++17 -I"${SRC}/include" -c "${src_file}" -o "${obj}"
  OBJECTS+=("${obj}")
done < <(find "${SRC}/src" -name '*.cpp' -print0 | sort -z)

FUZZERS=(parser_fuzzer decoder_fuzzer state_machine_fuzzer)
for fuzz in "${FUZZERS[@]}"; do
  ${CXX} ${CXXFLAGS} -std=c++17 -I"${SRC}/include" \
    "${SRC}/fuzz/${fuzz}.cc" "${OBJECTS[@]}" \
    -o "${OUT}/${fuzz}" ${LIB_FUZZING_ENGINE}
done

for corpus in parser decoder state; do
  if [[ -d "${SRC}/fuzz/corpus/${corpus}" ]]; then
    mkdir -p "${OUT}/${corpus}_fuzzer_corpus"
    cp -r "${SRC}/fuzz/corpus/${corpus}/." "${OUT}/${corpus}_fuzzer_corpus/"
  fi
done

echo "Built fuzzers in ${OUT}"
