# SPDX-License-Identifier: BSD-3-Clause
"""Tests for the stamp rule in tools/build/build_rom.sh: a changed boot ROM must carry
a new dbdate.h stamp, because consoles only reflash when the stamp changes.
"""

from __future__ import annotations

import os
import shutil
import subprocess
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[2]
NESASM = Path(os.environ.get("NESASM_BIN", REPO_ROOT / ".cache/nesasm/nesasm"))

pytestmark = pytest.mark.skipif(
    not NESASM.exists() or not (REPO_ROOT / "tutorial_project/BOOTROM").is_dir(),
    reason="needs NESASM CE (tools/build/fetch_nesasm.sh) and tutorial_project",
)

GREEN_ROW = "\tDB\t$0F,$1A,$14,$30"


@pytest.fixture
def tree(tmp_path):
    """A copy of the files build_rom.sh reads, with one boot ROM palette byte changed."""
    root = tmp_path / "repo"
    shutil.copytree(REPO_ROOT / "tools/build", root / "tools/build")
    shutil.copytree(REPO_ROOT / "tools/nes", root / "tools/nes")
    for name in ("BOOTROM", "BOOTROM_FIX"):
        shutil.copytree(REPO_ROOT / "tutorial_project" / name, root / "tutorial_project" / name)
    source = root / "tutorial_project/BOOTROM/AplGame.asm"
    text = source.read_text(encoding="utf-8")
    assert text.count(GREEN_ROW) == 1
    source.write_text(text.replace(GREEN_ROW, "\tDB\t$0F,$1A,$14,$31"), encoding="utf-8")
    return root


def build(root: Path, *args: str) -> subprocess.CompletedProcess:
    return subprocess.run([str(root / "tools/build/build_rom.sh"), "--out", str(root / "out"), *args],
                          env=dict(os.environ, NESASM_BIN=str(NESASM)), capture_output=True, text=True)


def test_changed_rom_under_the_distributed_stamp_is_rejected(tree):
    result = build(tree)
    assert result.returncode == 1
    assert "keeps its stamp 2026-0205-1239" in result.stderr


def test_changed_rom_with_an_edited_stamp_builds(tree):
    (tree / "tutorial_project/BOOTROM/dbdate.h").write_bytes(b'\tDB "2026-1020-0900",0 \r\n')
    result = build(tree)
    assert result.returncode == 0, result.stderr
    assert "new ROM version, stamp 2026-1020-0900" in result.stdout
    rom = (tree / "out/rom.NES").read_bytes()
    assert rom[16 + 0x6FF0:16 + 0x6FFE] == b"2026-1020-0900"


def test_stamp_option_builds_a_changed_rom(tree):
    result = build(tree, "--stamp")
    assert result.returncode == 0, result.stderr
    assert "new ROM version, stamp " in result.stdout
