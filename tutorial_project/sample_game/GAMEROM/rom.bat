::/ @file rom.bat
::/ @brief Flashes the game ROM onto a real cartridge with anago.
::/ @ingroup toolchain
::/
::/ The ROM is a plain NROM image, so it runs on an unmodified Famicom as well as
::/ under the RP2350. @see @ref sample_game
cls
anago Fff nrom_wx.af map0demo.nes AM29F040B AM29F040B
