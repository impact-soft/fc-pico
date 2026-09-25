::/ @file bpe_asm.bat
::/ @brief Assembles one nametable source and BPE-compresses it.
::/ @ingroup toolchain
::/
::/ Assembles `%1.asm`, cuts the 1 KB nametable out of the `.nes` it produces,
::/ compresses it with `bpe_fc -t -e`, and deletes the intermediates.
@echo off
..\bin\nesasm -s %1.asm > NUL
..\bin\bin_catcut %1.nes tmp.nam -new -size 0x400 -r_offset 0x10 > NUL
del %1.nes /f
..\bin\bpe_fc -t -e tmp.nam %1.bpe
del tmp.nam /f
