# libhelion benchmark verification

## Build fuzzers

```bash
export SRC=$PWD
export WORK=/tmp/helion-work
export OUT=/tmp/helion-out
export CXX=clang++
export CXXFLAGS="-O1 -g -fsanitize=address,fuzzer-no-link -fno-omit-frame-pointer"
export LIB_FUZZING_ENGINE="-fsanitize=fuzzer,address"
mkdir -p "$WORK" "$OUT"
bash .clusterfuzzlite/build.sh
```

Or: `bash build.sh`

## Reproduce crashes (buggy tree)

Bug A (UAF) — multipass decoder path (corpus only):

```bash
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 \
  $OUT/decoder_fuzzer fuzz/corpus/decoder -runs=0
```

Bug B (double-free) — streaming dual-own path:

```bash
# StageDocument + Reindex + Session teardown on dualown.hpk
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 \
  $OUT/state_machine_fuzzer fuzz/corpus/state/dualown.hpk -runs=0
```

Bug C (stale index) — recovery leaves SectionIndex cached views live; reached via
the same multipass decoder path when staged views are skipped and index views remain.

## Apply a fix and confirm

Each patch applies independently to the buggy tree:

```bash
patch -p1 < benchmark/patches/bug1-fix.patch
# rebuild fuzzers, then:
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 \
  $OUT/decoder_fuzzer fuzz/corpus/decoder -runs=0
# expect exit 0 / Done without heap-use-after-free
```

```bash
# fresh buggy tree first
patch -p1 < benchmark/patches/bug2-fix.patch
# rebuild, then dualown teardown should exit 0 without double-free
```

```bash
patch -p1 < benchmark/patches/bug3-fix.patch
```

## Zip-ready buggy snapshot

```bash
git archive --format=zip --prefix=libhelion/ HEAD -o /tmp/libhelion-buggy.zip
# Upload zip first, then the matching bugN-fix.patch
```
