;/// @file PG_main.asm
;/// @brief Assembly root: iNES header, bank layout and the two entry points.
;/// @ingroup gamerom
;///
;/// The whole ROM is assembled from this one file. It declares an NROM cartridge
;/// -- 32 KB PRG in four `.BANK`s, 8 KB CHR, mapper 0, vertical mirroring -- and
;/// then pulls in every other source in the order the banks require.
;///
;/// It also holds the pair of entry points that make this ROM unusual. `INIT` and
;/// `NMI`, vectored at `$FFFA`, are what a real Famicom runs. `jvcFCP_GAME_INIT`
;/// and `jvcFCP_GAME_MAIN`, at the fixed addresses `$E000` and `$E004`, are what
;/// the cartridge calls instead when the ROM is being driven a frame at a time by
;/// the RP2350. @see @ref sample_game

	.list			; リスティングファイル出力
	.mlist			; リスティングファイル上でマクロを展開

	.INCLUDE	"defDebug.h"

        .inesprg 2		; プログラムバンク数
        .ineschr 1		; CHR バンク数
        .inesmir 1		; 0:V 垂直２画面（ 水平ミラー） 1:H 水平２画面（垂直ミラー）
        .inesmap 0		; mapper #0

	.INCLUDE	"defMacro.h"

	.INCLUDE	"defRAM.h"
	.INCLUDE	"defGame.h"
	.INCLUDE	"defMission.h"
	.INCLUDE	".\chr\pallet.h"


	.code

	;========================================
	; ゲームバンク0
	;========================================
	.BANK		0
	ORG  $8000

	.INCBIN		"mml\sound.bin"				;4090 bytes
	.INCLUDE	".\SysSound.asm"

	.BANK		1
     ORG      $A000





	.BANK		2
      ORG     $C000

	.INCLUDE	".\cfg\cfgGame.h"
	.INCLUDE	".\cfg\cfgEnemyNT.h"
	.INCLUDE	".\cfg\cfgStage.h"
	.INCLUDE	".\cfg\cfgMissonSP.h"		; サブルーチン
	.INCLUDE	".\cfg\cfgMissonHara.h"
	.INCLUDE	".\cfg\cfgMissonAnime.h"


	.INCLUDE	"AplTitle.asm"
	.INCLUDE	"AplMiHara.asm"	;
	.INCLUDE	"AplLicense.asm"
	.INCLUDE	"AplClear.asm"
	.INCLUDE	"AplOver.asm"

	.INCLUDE	"AplGameDisp.asm"
	.INCLUDE	"AplGame.asm"		;ゲーム本体
	.INCLUDE	"AplGameSub.asm"

	.INCLUDE	"AplEnemy.asm"
	.INCLUDE	"AplMissionFunc.asm"	;
	.INCLUDE	"AplMission.asm"	;


	.BANK		3
      ORG     $E000
;-----------------------------
;  $E000
;  ゲーム変数初期化　FC PICO用
; 
;-----------------------------
;/// @brief Cartridge entry point at `$E000`: start a stage.
;/// @ingroup gamerom
;///
;/// One of the two fixed addresses the RP2350 calls. `jsr` to the real routine
;/// and then `brk`, which is what returns control to the emulator.
;/// @see @ref sample_game
jvcFCP_GAME_INIT:
	jsr  FCP_GAME_INIT
	brk
;-----------------------------
;  $E004
;  ゲーム本体処理　FC PICO用
; 
;-----------------------------
;/// @brief Cartridge entry point at `$E004`: advance the stage by one frame.
;/// @ingroup gamerom
;/// @note The addresses matter, not the names: `ap_game.cpp` calls
;///       `emu.run(0xE000 + 4 * n)`, so these two must stay four bytes apart and
;///       at the top of bank 3.
;/// @see @ref sample_game
jvcFCP_GAME_MAIN:
	jsr  FCP_GAME_MAIN
	brk



;/// @brief Resets the game variables for a new stage.
;/// @ingroup gamerom
;///
;/// Clears the `$300` page of work RAM, sets the starting position and lives, and
;/// seeds the mission engine. It sets `STG_COD` to the play state as well, though
;/// nothing under the cartridge reads it -- the console-side jump table does.
FCP_GAME_INIT:
	lda  #0
	sta  <DEMO_FG

	lda  #ST_MAIN        ;プレイ画面へ
	STA	<STG_COD

	LDA	#0
	sta <NMI_CALL_ADR+1
	sta <NMI_CALL_BNK
	STA	<STG_COD_SUB
	STA	<KEY_NEW
	STA	<KEY_TRG

	ldx  #low( CLEAR_300W_TOP )
	lda  #0
.loop
	sta  $300,x
	inx
	bne  .loop

	jsr  PLY_LIFE_SET
	lda #POS_PLY_X_INIT
	sta POS_PLY_X
	lda #POS_PLY_Y_INIT
	sta POS_PLY_Y

	lda  #PLY_AN_WAIT
	jsr  SET_PLY_ANM

	jsr initMission
	rts


;/// @brief One frame of the game, without any drawing.
;/// @ingroup gamerom
;///
;/// The whole of what the cartridge runs: advance the mission script, move every
;/// object, run the death animation if the player is in it, then step the timers.
;/// The object tables are left in RAM for the RP2350 to read.
FCP_GAME_MAIN:
	jsr updateMission
	jsr moveGameObj

	lda  PLY_ANM_NO
	cmp  #PLY_AN_DEAD
	bne  .plydm_20

	jsr  setPlayerDeadEffect


.plydm_20
	jsr  MAKE_RND
	incw  <SYS_TIMER
	inc <FLM_TIMER
	rts

	.INCLUDE	"AplGameMove.asm"
	.INCLUDE	"AplGameMovePly.asm"
	.INCLUDE	"AplGameInit.asm"
	.INCLUDE	"AplBgStar.asm"
	.INCLUDE	"AplSelSub.asm"


;/// @brief Alternate name for the console-side main loop; falls straight into #PLY_MAIN.
;/// @ingroup gamerom
PLY_MAIN_S:
;/// @brief Console-side main loop: dispatch on `STG_COD`.
;/// @ingroup gamerom
;///
;/// The other half of the ROM's dual personality. On a real Famicom this jump
;/// table walks the whole game -- title, play, clear, game over, licence -- and
;/// entry 5 is the same play state the cartridge reaches directly.
;/// @note Never entered when the ROM runs under the cartridge.
PLY_MAIN:
	inc  <FLM_TIMER
	lda  <STG_COD	;
	TBL_JUMP
	JPTBL	JMP_NEXT_STG	; 0
	JPTBL	JMP_NEXT_STG2	; 1
	JPTBL	TIT_STG			; 2
	JPTBL	JMP_NEXT_STG	; 3
	JPTBL	JMP_NEXT_STG	; 4
	JPTBL	APL_GAME		; 5
	JPTBL	CLEAR_STG		; 6
	JPTBL	OVER_STG		; 7
	JPTBL	APL_LICENSE		; 8


;/// @brief Falls into #JMP_NEXT_STG. Fills a slot in the jump table.
;/// @ingroup gamerom
JMP_NEXT_STG2:

;/// @brief Advances #STG_COD to the next console-side screen.
;/// @ingroup gamerom
JMP_NEXT_STG:
	INC	<STG_COD
;/// @brief Does nothing. The jump-table entry for a screen with no handler.
;/// @ingroup gamerom
JMP_RTS:
	RTS



	.INCLUDE	".\SysKey.asm"
	.INCLUDE	".\SysPallet.asm"
	.INCLUDE	".\SysData.asm"

	.INCLUDE	".\SysSub.asm"
	.include	".\SysNES.asm"
	.include	".\SysINIT.asm"



; HIRQ割り込みエントリ
;/// @brief IRQ vector. An immediate `rti`; this cartridge has no IRQ source.
;/// @ingroup gamerom
IRQ_ENTRY:
	rti


	ORG	$FFFA
	DW	NMI
	DW	INIT
	DW	IRQ_ENTRY


;========================================
; CHR-ROM 0x0000 - 0x1FFF
;========================================
	.BANK		4
       ORG     $0000
	.INCBIN		"chr\font.chr"
	.INCBIN		"chr\OBJ_SP.chr"

