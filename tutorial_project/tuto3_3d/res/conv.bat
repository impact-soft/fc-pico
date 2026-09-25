::/ @file conv.bat
::/ @brief Builds every cartridge-side asset and packs the two archives.
::/ @ingroup toolchain
::/
::/ The full asset pipeline for the sample game, in order: convert the STL models
::/ to C arrays, compress the licence nametables, then pack two archives.
::/
::/ `binlink.lst` becomes `res.bin` with an id base of 0 -- boot ROM, game ROM,
::/ music, artwork and licence pages -- and is turned into `_resdata` for the
::/ firmware to link. `binlink2.lst` becomes `res2.bin` with an id base of 10000,
::/ which is the MP3 archive; it is not linked but flashed separately by
::/ `upload.bat`.
::/
::/ @note `binlink2.lst` uses the `:LABEL` form, which emits a bracketing constant
::/       rather than an entry: `MP3_RES_ID` and `MP3_RES_ID_MAX` mark the range
::/       the tracks occupy.
::/ @warning Reads `BOOTROM/rom.NES`, `GAMEROM/map0demo.NES` and `mml/sound.nsf`
::/          from disk. All three must be current.
::/ @see @ref sample_game, @ref generated_resources
..\..\bin\stl_converter fcpico

call stlmake.bat mdl_player
call stlmake.bat mdl_enemy
call stlmake.bat mdl_enemy2
call stlmake.bat mdl_enemy3
call stlmake.bat mdl_bullet


call bpe_asm NamLicense0
call bpe_asm NamLicense1

..\..\bin\binlink binlink.lst res_id.h res.bin  0
..\..\bin\binlink binlink2.lst res_id2.h res2.bin 10000
..\..\bin\bin2c res.bin resdata.c _resdata
