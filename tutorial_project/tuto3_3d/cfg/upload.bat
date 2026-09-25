::/ @file upload.bat
::/ @brief Flashes the MP3 archive to the board and reboots it.
::/ @ingroup toolchain
::/
::/ Writes `cfg.bin` to `0x101f0000`, which is #RES_DATA_ADR, then reboots. The
::/ firmware UF2 is uploaded separately.
::/
::/ @warning Nothing checks that the firmware image has not grown past that
::/          address. @see @ref generated_resources
..\..\bin\arduino_picoboot.exe
timeout 3
..\..\bin\picotool load -o 0x101F0000 -t bin cfg.bin
..\..\bin\picotool reboot
