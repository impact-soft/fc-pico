#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-3-Clause
"""bincut.py -- pure-Python replacement for bin_catcut.exe's cut mode.

    bincut.py <in> <offset> <size> <out>

Copies ``size`` bytes starting at ``offset`` from ``<in>`` into a new file
``<out>``. ``offset`` and ``size`` accept either decimal ("28688") or 0x-hex
("0x7010") -- parsed with ``int(x, 0)``, so a leading "0x"/"0X" selects hex
and anything else is decimal.

Matches ``BOOTROM_FIX/m.BAT``'s real invocation, which cuts the 4096-byte
fix bank out of the assembled ``PG_main.NES`` at offset 0x7010 (the 16-byte
iNES header plus $F000):

    bin_catcut PG_main.NES ..\\BOOTROM\\bootrom_fixr.bin -new -r_offset 0x7010 -size 0x1000

This tool only implements that cut/extract behaviour (bin_catcut's other
modes -- concatenation, in-place patching -- are not used anywhere in this
tree and are out of scope).
"""

from __future__ import annotations

import sys
from pathlib import Path


def parse_int(text: str) -> int:
    """Decimal or 0x-hex, e.g. "4096" or "0x1000"."""
    return int(text, 0)


def cut(in_path: Path, offset: int, size: int) -> bytes:
    data = in_path.read_bytes()
    if offset < 0 or size < 0:
        raise ValueError(f"offset ({offset}) and size ({size}) must be non-negative")
    if offset + size > len(data):
        raise ValueError(
            f"{in_path}: {len(data)} bytes, too short to cut {size} bytes "
            f"starting at offset {offset} (needs at least {offset + size})"
        )
    return data[offset : offset + size]


def main(argv: list[str] | None = None) -> int:
    argv = sys.argv[1:] if argv is None else argv
    if len(argv) != 4:
        print("usage: bincut.py <in> <offset> <size> <out>", file=sys.stderr)
        return 1
    in_path, offset_arg, size_arg, out_path = argv
    try:
        offset = parse_int(offset_arg)
        size = parse_int(size_arg)
        chunk = cut(Path(in_path), offset, size)
    except (ValueError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    Path(out_path).write_bytes(chunk)
    print(f"wrote {out_path} ({len(chunk)} bytes, cut from {in_path}+{offset:#x})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
