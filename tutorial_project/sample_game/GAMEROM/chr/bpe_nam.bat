::/ @file bpe_nam.bat
::/ @brief BPE-compresses an existing nametable dump.
::/ @ingroup toolchain
@echo off
..\bin\bpe_fc -e %1.nam %1.bpe
