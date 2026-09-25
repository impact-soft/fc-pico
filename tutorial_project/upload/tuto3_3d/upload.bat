..\bin\arduino_picoboot.exe
timeout 3
..\bin\picotool load -o 0x101F0000 -t bin .\cfg\cfg.bin
..\bin\picotool load -o 0x10200000 -t bin res2.bin
..\bin\picotool load -x tuto3_3d.ino.uf2