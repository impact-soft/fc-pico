..\..\bin\nesasm -s %1.h > nesasm.log
..\..\bin\bin_catcut %1.NES %1.bin -new -r_offset 0x0010 -size 0xF000
del /F %1.lst

