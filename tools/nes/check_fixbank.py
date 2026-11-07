#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-3-Clause
"""check_fixbank.py -- verify an assembled ROM's fix bank tail.

    check_fixbank.py <rom.nes> <bootrom_fixr.bin>

Parses the 16-byte iNES header of ``<rom.nes>``, checks its PRG-ROM is
exactly 32 KB (the erasable boot ROM bank plus the fix bank), and verifies
that the *last* 4096 bytes of that PRG-ROM equal ``<bootrom_fixr.bin>``
exactly: the permanent fix bank at $F000-$FFFF must be reproduced unmodified
(see @ref boot_reflash). Prints a diagnostic and exits 1 on any mismatch;
exits 0 with a one-line confirmation if the fix bank matches.
"""

from __future__ import annotations

import sys
from pathlib import Path

INES_MAGIC = b"NES\x1a"
INES_HEADER_SIZE = 16
PRG_BANK_UNIT = 16384
EXPECTED_PRG_SIZE = 32768
FIX_BANK_SIZE = 4096


def fix_bank_tail(nes_path: Path) -> bytes:
    """The last FIX_BANK_SIZE bytes of ``nes_path``'s PRG-ROM.

    Raises ``ValueError`` if the file is not a well-formed iNES image or
    its PRG-ROM is not exactly EXPECTED_PRG_SIZE bytes.
    """
    data = nes_path.read_bytes()
    if len(data) < INES_HEADER_SIZE or data[:4] != INES_MAGIC:
        raise ValueError(f"{nes_path}: not an iNES file (bad magic)")

    prg_units = data[4]
    prg_size = prg_units * PRG_BANK_UNIT
    if prg_size != EXPECTED_PRG_SIZE:
        raise ValueError(
            f"{nes_path}: PRG-ROM is {prg_size} bytes ({prg_units} x 16 KB "
            f"unit(s)), expected {EXPECTED_PRG_SIZE} (32 KB)"
        )

    prg = data[INES_HEADER_SIZE : INES_HEADER_SIZE + prg_size]
    if len(prg) != prg_size:
        raise ValueError(
            f"{nes_path}: header declares {prg_size} bytes of PRG-ROM but "
            f"only {len(prg)} follow the header"
        )
    return prg[-FIX_BANK_SIZE:]


def main(argv: list[str] | None = None) -> int:
    argv = sys.argv[1:] if argv is None else argv
    if len(argv) != 2:
        print("usage: check_fixbank.py <rom.nes> <bootrom_fixr.bin>", file=sys.stderr)
        return 1
    nes_arg, fixbank_arg = argv
    nes_path = Path(nes_arg)
    fixbank_path = Path(fixbank_arg)

    try:
        actual_tail = fix_bank_tail(nes_path)
    except (ValueError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    expected = fixbank_path.read_bytes()
    if len(expected) != FIX_BANK_SIZE:
        print(
            f"error: {fixbank_path}: {len(expected)} bytes, expected "
            f"{FIX_BANK_SIZE}",
            file=sys.stderr,
        )
        return 1

    if actual_tail != expected:
        diffs = sum(1 for a, b in zip(actual_tail, expected) if a != b)
        print(
            f"error: {nes_path}'s last {FIX_BANK_SIZE} PRG bytes do not "
            f"match {fixbank_path} ({diffs} differing byte(s))",
            file=sys.stderr,
        )
        return 1

    print(f"ok: {nes_path}'s fix bank matches {fixbank_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
