@page build_pipeline Build pipeline

Four build steps, in a required order, plus one coupling that will bite you.

## Order

@dot
digraph build {
  rankdir=TB;
  node [shape=box, fontname="Helvetica", fontsize=9];
  edge [fontname="Helvetica", fontsize=8];

  fixsrc [label="BOOTROM_FIX/*.asm"];
  fixnes [label="PG_main.nes"];
  fixbin [label="BOOTROM/bootrom_fixr.bin\n4096 bytes"];
  mainsrc[label="BOOTROM/*.asm\n+ dbdate.h (regenerated!)"];
  romnes [label="BOOTROM/rom.NES\n32784 bytes"];
  mmlsrc [label="mml/*.mml, *.mmh"];
  nsf    [label="mml/sound.nsf\n7332 bytes"];
  chr    [label="res/font.chr, OBJ.chr"];
  resc   [label="tuto1_hw/res/resdata.c\n_resdata[48340]"];
  uf2    [label="Arduino IDE ->\nUF2 firmware"];
  cart   [label="cartridge flash\n(anago)"];

  fixsrc -> fixnes [label="1. m.BAT (nesasm)"];
  fixnes -> fixbin [label="bin_catcut"];
  fixbin -> romnes [label=".incbin"];
  mainsrc-> romnes [label="2. m.BAT (nesasm)"];
  mmlsrc -> nsf    [label="m.bat (nsc)"];
  romnes -> resc   [label="3. conv.bat\nbinlink + Bin2C"];
  nsf    -> resc   [label="3. conv.bat"];
  chr    -> resc   [label="3. conv.bat"];
  resc -> uf2;
  romnes -> cart   [label="rom/rom.bat"];
}
@enddot

## Step by step

### 1. Fix bank

```
cd tutorial_project/BOOTROM_FIX
m.BAT
```

Assembles with `nesasm -s`, filters the log, and cuts 4096 bytes from offset `0x7010`
(16-byte iNES header + `0x7000`, i.e. CPU `$F000`) straight into
`../BOOTROM/bootrom_fixr.bin`.

### 2. Main bank

```
cd tutorial_project/BOOTROM
m.BAT
```

Regenerates `dbdate.h`, assembles, copies `PG_main.NES` to `rom.NES`. The fix bank is
pulled in with `.incbin`, so step 1 must have run first.

### 3. Pack the assets into the firmware

```
cd tutorial_project/tuto1_hw/res
conv.bat
```

`binlink` packs everything listed in `binlink.lst` -- `BOOTROM/rom.NES`,
`mml/sound.nsf`, `OBJ.chr` and `font.chr` -- into `res.bin` behind an index table,
and `Bin2C` converts that to `_resdata[]`. It reads whatever is on disk, so the ROM
(step 2) and the music (`mml/m.bat`) must be current. See @ref generated_resources.

@note `BOOTROM/update.bat` used to be the step that carried the ROM into the
firmware, as `res/rom.c`. It no longer does: it still produces `rom.c` and then
deletes it again, and only its `copy rom.nes ..\ROM\` line still has an effect. The
`copy` into `..\fc_pico_v100\res\`, a path from the full product tree that does not
exist here, has been removed upstream.

### 4. Firmware

Open `tutorial_project/tuto1_hw/tuto1_hw.ino` in the Arduino IDE and build. See
@ref dev_setup. Deploy by holding **BOOT** while connecting USB-C and copying the UF2
onto the drive that appears.

## The version coupling

@warning `BOOTROM/m.BAT` writes a fresh timestamp into `dbdate.h` on every run:

```bat
echo 	DB "%yyyy%-%mm%%dd%-%hh%%mn%",0 > dbdate.h
```

That string is assembled at `$EFF0`, packed into the firmware via `resdata.c`, and
compared at boot by `CHK_ROMVER`. **Rebuilding the ROM without also rebuilding and
reflashing the firmware makes every boot trigger a self-reflash.**

Rebuild the ROM, and you must run step 3, step 4, and redeploy. See @ref boot_reflash.

To make a build reproducible, pin `dbdate.h` and skip the `echo` line.

## Toolchain

Everything is checked in under `bin/` -- no installation needed.

| Tool | Location | Purpose |
|---|---|---|
| `nesasm.exe` | `BOOTROM*/bin/` | 6502 assembler, v2.51+autozp |
| `nesasm_logfilter.exe` | `BOOTROM*/bin/` | Condenses the assembler log |
| `bin_catcut.exe` | `bin/`, `BOOTROM*/bin/` | Cut/concatenate binaries by offset and size |
| `binlink.exe` | `bin/` | Packs files into one indexed archive, emits the `res_id.h` constants |
| `Bin2C.exe` | `bin/`, `BOOTROM/bin/` | Binary to C array, 4-byte aligned |
| `nsc.exe` | `mml/bin/` | NSD.Lib MML compiler, v1.28 |
| `nsd.bin` | `mml/bin/` | The 6502 sound driver linked into the NSF |
| `VirtuaNES.exe` | `mml/bin/` | NSF audition |
| `anago.exe` | `rom/` | Cartridge flasher |
| `picotool.exe` | `bin/`, `upload/` | Writes a UF2 or a raw image to the RP2350 over USB |

Present but **never invoked** by any script under `tuto1_hw/`: `bpe_fc.exe`,
`inesAlterer.exe`, `nes_mirror.exe`, `spchr_cnv.exe`, `stl_maker.exe`,
`stl_converter.exe`. `nes_mirror` is referenced indirectly by the unused
`ROM_MIRROR` macro in `macro.h`. `bpe_fc` and `spchr_cnv` are driven by the sample
game's own scripts, which are outside this reference.

`BOOTROM/g.BAT` launches FCEUX from three directories up; the emulator is not bundled.

## Verifying a build

The distributed artifacts have known hashes. After a rebuild with `dbdate.h` pinned:

| Artifact | Size | MD5 |
|---|---|---|
| `BOOTROM/bootrom_fixr.bin` | 4096 | `1A9A2AC85F1E7A15AF9DF47013432BF9` |
| `BOOTROM/rom.NES` | 32784 | `B6CD675342B6C8AD79E537E2C9860579` |
| `mml/sound.nsf` | 7332 | `30ED8536E395865680D0457493240EB8` |

`BOOTROM/PG_main.nes`, `BOOTROM/rom.NES` and `rom/rom.nes` are the same file.

Bank usage from the assembler log: main bank 2 uses 1855/6337 bytes and bank 3 uses
5610/2582; banks 0 and 1 are empty. The fix bank uses 3051/5141.

## Optional native build on Linux

Steps 1 and 2 can also be built without Windows, using
[NESASM CE](https://github.com/ClusterM/nesasm) at commit
`6fc41cda37b934aa29aa2639d0baa74424268e31`, which reproduces the vendor PRG byte for
byte. The Windows tools above stay the reference; this path changes nothing under
`tutorial_project/`.

```sh
tools/build/fetch_nesasm.sh    # clones and builds the pinned assembler into .cache/nesasm
tools/build/build_rom.sh       # writes build/rom/
```

`build_rom.sh` assembles scratch copies of `BOOTROM_FIX` and `BOOTROM`, then:

- cuts the fix bank at `0x7010` exactly like `BOOTROM_FIX/m.BAT`, and fails unless it
  equals `BOOTROM/bootrom_fixr.bin` -- the permanent bank must never change;
- normalizes the iNES header: NESASM CE writes a NES 2.0 header (byte 7 = `0x08`,
  byte 11 = `0x07`) where nesasm 2.51 writes zeros. `tools/nes/normalize_ines.py`
  clears exactly those two bytes and never touches PRG;
- checks that `rom.NES` ends with that fix bank, and, while `dbdate.h` holds the
  distributed stamp `2026-0205-1239`, that it matches the distributed MD5
  `B6CD675342B6C8AD79E537E2C9860579`. A changed ROM under that stamp fails: consoles
  that already have the stamp would never reflash to it;
- keeps the listings (`*.lst`), segment usage (`*.log`) and FCEUX symbol files
  (`rom.NES.*.nl`) for debugging and NMI timing work.

A changed ROM therefore needs a new stamp. Edit `dbdate.h` and commit it for a ROM
you can rebuild exactly, or pass `--stamp`, which regenerates `dbdate.h` in the
scratch copy with the current time like `BOOTROM/m.BAT`. A new stamp skips the MD5
gate; the version coupling above still applies to such a ROM.
`--out DIR` changes the output directory, and `NESASM_BIN` selects another
assembler. The Python helpers in `tools/nes/` (`bincut`, `normalize_ines`,
`check_fixbank`) are tested with `python3 -m pytest tests`.
