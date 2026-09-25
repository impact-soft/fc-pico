@echo off


call mcfg cfgStage
call mcfg cfgMissonHara

..\..\bin\binlink binlink.lst res_id_cfg.h cfg.bin  20000



type nesasm.log
find "error" nesasm.log

del /F *.nes 
