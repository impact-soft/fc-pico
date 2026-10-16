# SPDX-License-Identifier: BSD-3-Clause
"""Portable Python replacements for the Windows-only NES ROM tools.

* ``bincut`` -- the cut mode of ``bin_catcut.exe``.
* ``normalize_ines`` -- converts NESASM CE's NES 2.0 header to the vendor's iNES header.
* ``check_fixbank`` -- verifies that a ROM's last 4 KB equal ``bootrom_fixr.bin``.
"""
