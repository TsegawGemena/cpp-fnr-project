#!/bin/bash
set -euo pipefail

SRC="${SRC:-$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)}"
WORK="${WORK:-/tmp/helion-work}"
OUT="${OUT:-/tmp/helion-out}"

export SRC WORK OUT
exec bash "${SRC}/.clusterfuzzlite/build.sh"
