::/ @file m.bat
::/ @brief Compiles the game ROM's music with nsc.
::/ @ingroup toolchain
::/
::/ Produces the `sound.bin` driver image that PG_main.asm pulls in with
::/ `.INCBIN` at the base of bank 0. Distinct from the cartridge's own music,
::/ which is an NSF. @see @ref audio_page
.\bin\nsc -N sound.mml
.\bin\nsf2rom sound.nsf sound.bin