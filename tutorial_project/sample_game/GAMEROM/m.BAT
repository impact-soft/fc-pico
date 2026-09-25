::/ @file m.BAT
::/ @brief Assembles the game ROM.
::/ @ingroup toolchain
::/
::/ Writes `strver.h`, assembles `PG_main.asm` with nesasm, copies the result to
::/ `map0demo.NES` and also cuts a headerless `gamerom.bin`. The `find "error"`
::/ at the end is the only check; nesasm's exit status is not consulted, so a
::/ failed build has to be spotted in the log.
::/
::/ @note `map0demo.NES` is what `res/binlink.lst` packs into the cartridge's
::/       resource archive, so `res/conv.bat` has to run after this.
::/ @see @ref sample_game, @ref build_pipeline
@echo off

set title=map0demo
set vr=V1.00


echo 	DRAW_STRING2 " 2026 %vr%" > strver.h

del /F *.nes 

.\bin\nesasm -s PG_main.asm > nesasm.log

copy PG_main.NES %title%.NES > NUL

.\bin\bin_catcut %title%.NES gamerom.bin -new -r_offset 0x0010 -size 0x8000


type nesasm.log
find "error" nesasm.log
