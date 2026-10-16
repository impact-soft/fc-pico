#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Fetch and build the pinned native NESASM CE used by tools/build/build_rom.sh.
#
#   tools/build/fetch_nesasm.sh            # builds into .cache/nesasm
#   NESASM_CACHE=/path tools/build/fetch_nesasm.sh
#
# Prints the assembler path on success. Needs git, make and a C compiler.
set -euo pipefail

repo=$(cd "$(dirname "$0")/../.." && pwd)
pinned=6fc41cda37b934aa29aa2639d0baa74424268e31
cache=${NESASM_CACHE:-$repo/.cache/nesasm}

if [[ ! -d $cache/.git ]]; then
    mkdir -p "$(dirname "$cache")"
    git clone --quiet https://github.com/ClusterM/nesasm.git "$cache"
fi
git -C "$cache" fetch --quiet origin "$pinned" 2>/dev/null || true
git -C "$cache" checkout --quiet --detach "$pinned"
[[ $(git -C "$cache" rev-parse HEAD) == "$pinned" ]] || { echo "NESASM CE is not at $pinned" >&2; exit 1; }
make -C "$cache/source" --quiet all >/dev/null
echo "$cache/nesasm"
