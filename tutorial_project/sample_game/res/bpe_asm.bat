::/ @file bpe_asm.bat
::/ @brief Assembles one licence nametable and BPE-compresses it.
::/ @ingroup toolchain
::/
::/ The cartridge's copy of `GAMEROM/chr/bpe_asm.bat`; the output is packed into
::/ the resource archive rather than into the ROM.
@echo off
..\..\bin\nesasm -s %1.asm > NUL
..\..\bin\bin_catcut %1.nes tmp.nam -new -size 0x400 -r_offset 0x10 > NUL
del %1.nes /f
..\..\bin\bpe_fc -t -e tmp.nam %1.bpe
del tmp.nam /f
