::/ @file conv.bat
::/ @brief Converts the sprite sheet and builds the licence nametables.
::/ @ingroup toolchain
::/
::/ `spchr_cnv` rearranges `OBJ.chr` into the layout the game's sprite code
::/ expects; the two `bpe_asm` calls produce the compressed licence pages.
@echo off

..\bin\spchr_cnv OBJ.chr OBJ_SP.chr
rem ..\bin\bin_catcut OBJ_SP.chr .\div\OBJ_SP_0.chr -new -r_offset 0x000 -size 0xC00 > NUL


call bpe_asm NamLicense0
call bpe_asm NamLicense1
