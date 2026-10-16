#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Optional native build of the boot ROM (build pipeline steps 1 and 2), tested on Linux.
#
#   tools/build/build_rom.sh [--out DIR] [--stamp]
#
# Assembles BOOTROM_FIX and BOOTROM with native NESASM CE from scratch copies, so
# nothing under tutorial_project/ is modified. It writes to DIR (default build/rom):
#
#   bootrom_fixr.bin                  the permanent bank, cut at 0x7010 like m.BAT
#   rom.NES                           the 32,784-byte boot ROM with the vendor's iNES header
#   bootrom_fix.lst, rom.lst          assembler listings
#   rom.NES.*.nl                      FCEUX symbol files for the boot ROM
#   *.log                             assembler segment usage
#
# Gates: the rebuilt fix bank must equal tutorial_project/BOOTROM/bootrom_fixr.bin,
# rom.NES must end with that fix bank and, while dbdate.h holds the distributed stamp
# 2026-0205-1239, rom.NES must match the distributed MD5
# B6CD675342B6C8AD79E537E2C9860579. A changed ROM under that stamp is an error:
# consoles that already have the stamp would never reflash to it.
#
# A changed ROM needs a new stamp: edit dbdate.h (reproducible; commit it), or pass
# --stamp, which regenerates dbdate.h in the scratch copy with the current time, as
# BOOTROM/m.BAT does. Either way the MD5 gate is skipped, and the ROM needs a
# matching firmware build (see the version coupling in the build pipeline
# documentation).
#
# NESASM_BIN selects the assembler (default .cache/nesasm/nesasm; create it with
# tools/build/fetch_nesasm.sh). The Windows vendor build in tutorial_project/ is
# unchanged and remains the reference.
set -euo pipefail

repo=$(cd "$(dirname "$0")/../.." && pwd)
out=$repo/build/rom
stamp=0
while [[ $# -gt 0 ]]; do
    case $1 in
        --out) out=$2; shift 2 ;;
        --stamp) stamp=1; shift ;;
        -h|--help) sed -n '3,25p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "unknown argument: $1 (see --help)" >&2; exit 2 ;;
    esac
done

nesasm=${NESASM_BIN:-$repo/.cache/nesasm/nesasm}
if ! command -v "$nesasm" >/dev/null 2>&1; then
    echo "NESASM CE not found at $nesasm; run tools/build/fetch_nesasm.sh or set NESASM_BIN" >&2
    exit 2
fi
python=${PYTHON:-python3}
distributed_stamp=2026-0205-1239
tools=$repo/tools
reference_fix=$repo/tutorial_project/BOOTROM/bootrom_fixr.bin

mkdir -p "$out"
out=$(cd "$out" && pwd)
scratch=$(mktemp -d)
trap 'rm -rf -- "$scratch"' EXIT
cp -a "$repo/tutorial_project/BOOTROM_FIX" "$scratch/BOOTROM_FIX"
cp -a "$repo/tutorial_project/BOOTROM" "$scratch/BOOTROM"
rm -f "$scratch"/BOOTROM_FIX/*.nes "$scratch"/BOOTROM_FIX/*.NES "$scratch"/BOOTROM/*.nes "$scratch"/BOOTROM/*.NES

# Step 1: the permanent fix bank.
(cd "$scratch/BOOTROM_FIX" &&
    "$nesasm" -s -i -L "$out/bootrom_fix.lst" -o "$scratch/fix.raw.nes" PG_main.asm) > "$out/bootrom_fix.log" 2>&1
PYTHONPATH=$tools "$python" -m nes.bincut "$scratch/fix.raw.nes" 0x7010 0x1000 "$out/bootrom_fixr.bin" > /dev/null
if ! cmp -s "$out/bootrom_fixr.bin" "$reference_fix"; then
    echo "error: rebuilt fix bank differs from $reference_fix; the permanent bank must not change" >&2
    exit 1
fi
cp "$out/bootrom_fixr.bin" "$scratch/BOOTROM/bootrom_fixr.bin"

# Step 2: the erasable boot ROM bank.
if [[ $stamp == 1 ]]; then
    printf '\tDB "%s",0\r\n' "$(date '+%Y-%m%d-%H%M')" > "$scratch/BOOTROM/dbdate.h"
fi
rm -f "$out"/rom.raw.nes* "$out"/rom.NES*
(cd "$scratch/BOOTROM" &&
    "$nesasm" -s -i -L "$out/rom.lst" -f -o "$out/rom.raw.nes" PG_main.asm) > "$out/rom.log" 2>&1
PYTHONPATH=$tools "$python" -m nes.normalize_ines "$out/rom.raw.nes" "$out/rom.NES"
rm -f "$out/rom.raw.nes"
for symbols in "$out"/rom.raw.nes.*.nl; do
    mv "$symbols" "$out/rom.NES.${symbols#"$out"/rom.raw.nes.}"
done
PYTHONPATH=$tools "$python" -m nes.check_fixbank "$out/rom.NES" "$reference_fix"

actual=$("$python" -c 'import hashlib, sys; print(hashlib.md5(open(sys.argv[1], "rb").read()).hexdigest())' "$out/rom.NES")
rom_stamp=$(tr -d '\r' < "$scratch/BOOTROM/dbdate.h" | sed -n 's/.*"\(.*\)".*/\1/p')
if [[ $rom_stamp != "$distributed_stamp" ]]; then
    echo "new ROM version, stamp $rom_stamp: MD5 gate skipped ($actual)"
elif [[ $actual != b6cd675342b6c8ad79e537e2c9860579 ]]; then
    echo "error: rom.NES (MD5 $actual) differs from the distributed rom.NES but keeps its stamp $distributed_stamp," >&2
    echo "so consoles with that stamp would never reflash to it. Give it a new stamp: edit" >&2
    echo "tutorial_project/BOOTROM/dbdate.h, or pass --stamp." >&2
    exit 1
else
    echo "rom.NES reproduces the distributed image (MD5 $actual)"
fi
echo "outputs: $out"
