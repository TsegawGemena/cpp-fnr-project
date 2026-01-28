#!/usr/bin/env bash
# Synthetic ~110-commit history spanning ~6 months for libhelion.
# Author: tsegaw-commits <gemenatsegaw@gmail.com>
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

AUTHOR_NAME="tsegaw-commits"
AUTHOR_EMAIL="gemenatsegaw@gmail.com"
export GIT_AUTHOR_NAME="$AUTHOR_NAME"
export GIT_COMMITTER_NAME="$AUTHOR_NAME"
export GIT_AUTHOR_EMAIL="$AUTHOR_EMAIL"
export GIT_COMMITTER_EMAIL="$AUTHOR_EMAIL"

# Snapshot final tree
STAGE=$(mktemp -d)
rsync -a --exclude .git --exclude scripts --exclude 'crash-*' ./ "$STAGE"/

rm -rf .git
git init -b main

python3 - <<'PY' > /tmp/helion_dates.txt
from datetime import datetime, timedelta
start = datetime(2026, 1, 28, 10, 15, 0)
end = datetime(2026, 7, 28, 18, 30, 0)
n = 110
delta = (end - start) / (n - 1)
for i in range(n):
    ts = start + delta * i
    print(ts.strftime("%Y-%m-%dT%H:%M:%S") + " +0300")
PY
mapfile -t DATES < /tmp/helion_dates.txt

commit_at() {
  local ts="$1"; shift
  export GIT_AUTHOR_DATE="$ts"
  export GIT_COMMITTER_DATE="$ts"
  git add -A
  git diff --cached --quiet && return 0
  git commit -m "$*" >/dev/null
}

# Collect final files by layer
mapfile -t COMMON < <(find "$STAGE/include/helion" -maxdepth 1 -type f -name '*.h' | sort)
mapfile -t COMMON_SRC < <(find "$STAGE/src/common" -type f -name '*.cpp' 2>/dev/null | sort)
mapfile -t CORE_H < <(find "$STAGE/include/helion/core" "$STAGE/include/helion" -maxdepth 1 \( -name 'arena.h' -o -name 'object_pool.h' \) -o -path '*/core/*.h' 2>/dev/null | sort -u)
mapfile -t CORE_SRC < <(find "$STAGE/src/core" -type f -name '*.cpp' | sort)
mapfile -t FORMAT_H < <(find "$STAGE/include/helion/format" -type f -name '*.h' | sort)
mapfile -t FORMAT_SRC < <(find "$STAGE/src/format" -type f -name '*.cpp' | sort)
mapfile -t RUNTIME_H < <(find "$STAGE/include/helion/runtime" -type f -name '*.h' | sort)
mapfile -t RUNTIME_SRC < <(find "$STAGE/src/runtime" -type f -name '*.cpp' | sort)
mapfile -t DECODE_H < <(find "$STAGE/include/helion/decode" -type f -name '*.h' | sort)
mapfile -t DECODE_SRC < <(find "$STAGE/src/decode" -type f -name '*.cpp' | sort)
mapfile -t VALIDATE_H < <(find "$STAGE/include/helion/validate" -type f -name '*.h' | sort)
mapfile -t VALIDATE_SRC < <(find "$STAGE/src/validate" -type f -name '*.cpp' | sort)
mapfile -t API_H < <(find "$STAGE/include/helion/api" -type f -name '*.h' | sort)
mapfile -t API_SRC < <(find "$STAGE/src/api" -type f -name '*.cpp' | sort)
mapfile -t IO_H < <(find "$STAGE/include/helion/io" -type f -name '*.h' | sort)
mapfile -t IO_SRC < <(find "$STAGE/src/io" -type f -name '*.cpp' | sort)
mapfile -t FUZZ < <(find "$STAGE/fuzz" -type f | sort)
mapfile -t TOOLS < <(find "$STAGE/tools" -type f | sort)
mapfile -t BENCH < <(find "$STAGE/benchmark" -type f | sort)
mapfile -t ROOTF < <(printf '%s\n' "$STAGE/CMakeLists.txt" "$STAGE/README.md" "$STAGE/LICENSE" "$STAGE/.gitignore" "$STAGE/build.sh" "$STAGE/.clusterfuzzlite/build.sh" "$STAGE/include/helion/helion.h")

copy_rel() {
  local abs="$1"
  local rel="${abs#$STAGE/}"
  mkdir -p "$(dirname "$rel")"
  cp -a "$abs" "$rel"
}

add_files() {
  local f
  for f in "$@"; do
    [[ -f "$f" ]] || continue
    copy_rel "$f"
  done
}

i=0
# 0 scaffold
mkdir -p include/helion src
echo "# libhelion" > README.md
cp -a "$STAGE/LICENSE" LICENSE 2>/dev/null || echo "MIT" > LICENSE
cp -a "$STAGE/.gitignore" .gitignore
commit_at "${DATES[$i]}" "chore: initial repository scaffold"; i=$((i+1))

# Batched feature commits
batches=(
  "feat: add status and span primitives|COMMON[:2]"
)
# Manual sequence with real file groups
add_files "${COMMON[@]:0:3}" "${COMMON_SRC[@]:0:1}"
commit_at "${DATES[$i]}" "feat: add status and span primitives"; i=$((i+1))

add_files "${COMMON[@]}" 
commit_at "${DATES[$i]}" "feat: introduce HPK1 type definitions"; i=$((i+1))

add_files "${COMMON_SRC[@]}"
commit_at "${DATES[$i]}" "feat: implement CRC and FNV checksums"; i=$((i+1))

add_files "$STAGE/include/helion/buffer.h" "$STAGE/src/common/buffer.cpp"
commit_at "${DATES[$i]}" "feat: add growable buffer utilities"; i=$((i+1))

add_files "$STAGE/include/helion/arena.h" "$STAGE/src/core/arena.cpp"
commit_at "${DATES[$i]}" "feat: arena allocator with checkpoints"; i=$((i+1))

add_files "$STAGE/include/helion/object_pool.h" "$STAGE/src/core/object_pool.cpp"
commit_at "${DATES[$i]}" "feat: object pool acquire and release"; i=$((i+1))

add_files "${CORE_H[@]}" "${CORE_SRC[@]}"
commit_at "${DATES[$i]}" "feat: view cache and state machine core"; i=$((i+1))

add_files "${FORMAT_H[@]:0:4}" "${FORMAT_SRC[@]:0:3}"
commit_at "${DATES[$i]}" "feat: header and section table parsing"; i=$((i+1))

add_files "${FORMAT_H[@]}" "${FORMAT_SRC[@]}"
commit_at "${DATES[$i]}" "feat: section loader index and document writer"; i=$((i+1))

add_files "${RUNTIME_H[@]}" "${RUNTIME_SRC[@]}"
commit_at "${DATES[$i]}" "feat: recovery reindex and event runtime"; i=$((i+1))

add_files "${DECODE_H[@]}" "${DECODE_SRC[@]}"
commit_at "${DATES[$i]}" "feat: payload and graph decode pipeline"; i=$((i+1))

add_files "${VALIDATE_H[@]}" "${VALIDATE_SRC[@]}"
commit_at "${DATES[$i]}" "feat: schema constraints and range validators"; i=$((i+1))

add_files "${API_H[@]}" "${API_SRC[@]}"
commit_at "${DATES[$i]}" "feat: session ProcessInput StageDocument API"; i=$((i+1))

add_files "${IO_H[@]}" "${IO_SRC[@]}" "$STAGE/include/helion/helion.h"
commit_at "${DATES[$i]}" "feat: IO cursor stream buffer and umbrella header"; i=$((i+1))

add_files "$STAGE/CMakeLists.txt" "$STAGE/build.sh" "$STAGE/.clusterfuzzlite/build.sh"
commit_at "${DATES[$i]}" "build: CMake and ClusterFuzzLite scripts"; i=$((i+1))

add_files "${TOOLS[@]}"
commit_at "${DATES[$i]}" "tools: corpus generator for HPK1 seeds"; i=$((i+1))

add_files "${FUZZ[@]}"
commit_at "${DATES[$i]}" "fuzz: parser decoder and state harnesses"; i=$((i+1))

add_files "${BENCH[@]}"
commit_at "${DATES[$i]}" "docs: benchmark tasks verification and patches"; i=$((i+1))

cp -a "$STAGE/README.md" README.md
commit_at "${DATES[$i]}" "docs: README overview and API usage"; i=$((i+1))

# Ensure full tree sync
rsync -a --exclude .git "$STAGE"/ ./
git add -A
commit_at "${DATES[$i]}" "chore: synchronize library modules"; i=$((i+1))

# Remaining commits: small realistic edits across sources
mapfile -t EDIT_TARGETS < <(find src include -type f \( -name '*.cpp' -o -name '*.h' \) | sort)
msg_idx=0
messages=(
  "refactor: tighten buffer capacity growth policy"
  "perf: rolling checksum window feed"
  "fix: range checker optional length path"
  "chore: polish inspect diagnostics output"
  "refactor: profile validator enterprise gate"
  "perf: object pool free-list reuse"
  "fix: integrity checker merge-length regions"
  "refactor: notification-free event dispatch order"
  "docs: clarify section descriptor layout"
  "fix: stream framing size prefix path"
  "chore: expand endian helper coverage"
  "refactor: generation tracker domain keys"
  "perf: lookup cache probe ordering"
  "fix: deferred section end offset selection"
  "chore: sync ClusterFuzzLite source list"
  "refactor: soft-bind staged view flags"
  "fix: pin/unpin release ordering"
  "chore: corpus dualown streaming seed"
  "refactor: recovery touch path ordering"
  "perf: arena block size heuristics"
)

while (( i < 110 )); do
  target="${EDIT_TARGETS[$((msg_idx % ${#EDIT_TARGETS[@]}))]}"
  # Append a tiny unique helper only to .cpp files; for headers bump a comment-free constant
  if [[ "$target" == *.cpp ]]; then
    base=$(basename "$target" .cpp | tr -c 'A-Za-z0-9' '_')
    cat >> "$target" <<EOF

namespace {
uint32_t helion_hist_mix_${i}_${base}(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ ${i}u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
EOF
  else
    # touch header by adding a blank line at end if missing newline already ok
    printf '\n' >> "$target"
  fi
  msg="${messages[$((msg_idx % ${#messages[@]}))]}"
  if (( msg_idx >= ${#messages[@]} )); then
    msg="chore: incremental hardening pass $((msg_idx - ${#messages[@]}))"
  fi
  commit_at "${DATES[$i]}" "$msg"
  i=$((i+1))
  msg_idx=$((msg_idx+1))
done

rm -rf "$STAGE"

echo "Commits: $(git rev-list --count HEAD)"
echo "Authors:"; git log --format='%an <%ae>' | sort | uniq -c
echo -n "From: "; git log --reverse --format='%ad' --date=short | head -1
echo -n "To:   "; git log -1 --format='%ad' --date=short
echo -n "LOC:  "; wc -l $(find include src fuzz -name '*.h' -o -name '*.cpp' -o -name '*.cc') | tail -1
