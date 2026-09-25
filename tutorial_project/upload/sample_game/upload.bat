..\bin\arduino_picoboot.exe
timeout 3
..\bin\picotool load -o 0x10200000 -t bin res2.bin
..\bin\picotool load -x sample_game.ino.uf2