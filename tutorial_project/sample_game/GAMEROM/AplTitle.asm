;/// @file AplTitle.asm
;/// @brief Title screen, on the console's own hardware.
;/// @ingroup gamerom
;///
;/// Reached through the `STG_COD` jump table in PG_main.asm. Unused when the ROM
;/// runs under the cartridge, which draws its own title screen in 3D and never
;/// advances `STG_COD` past the play state.

; INSERT COIN(S) 点滅周期 (フレーム単位)
; この値の半分の時間で INSERT COIN(S) を表示、もう半分で非表示となります。
INSCOIN_PERIOD = 120		;///< INSERT COIN blink period in frames; the text shows for half of it.



	;======== TITLE BG =============
;/// @brief Palette data for the title screen.
;/// @ingroup gamerom
PAL_TITLE_ADR:
	PAL_TITLE


;===================================
;タイトル画面
;===================================
;/// @brief Console-side title screen.
;/// @ingroup gamerom
TIT_STG:
	
	jsr .tbljump_sub

	; スプライト制御
	ldy #0*4	; スプライト開始位置

	inc  <PUSH_CTR
	lda  <PUSH_CTR
	and  #$10
	bne  .draw_p00

	ldx  <GM_TMP0
	lda  .tblCurPosX,x
	sta  OBJ_BUF +3,y	; x
	clc
	adc  #8
	sta  OBJ_BUF +4 +3,y	; x

	lda  .tblCurPosY,x
	sta  OBJ_BUF,y		; y
	sta  OBJ_BUF +4,y		; y

	lda  #$45
	sta OBJ_BUF +1,y	; pat
	lda  #$47
	sta OBJ_BUF +4 +1,y	; pat

	lda #$03
	sta OBJ_BUF +2,y	; pal
	lda #$03
	sta OBJ_BUF +4 +2,y	; pal

	tya
	clc
	adc  #8
	tay

.draw_p00
	; BG流星表示
	lda  <FLM_TIMER
;	asl  a
;	eor  #$FF
	tax
	lda  #30
	jsr createBgStarObj2
	
	; 余ったスプライトを非表示にする
	jsr clearObj

	; 何かしらキー入力があったらタイマーリセット
	lda  KEY_TRG
	beq  .end

	LDA  #TITLE_DEMO_TM
	STA  <DEMO_TIMER
	
.end
	rts

.tblCurPosX:
	db 8*9-4, 8*11-4

.tblCurPosY:
	db 8*19-6, 8*21-6
	
.tbljump_sub
	LDA     <STG_COD_SUB
	TBL_JUMP
	JPTBL	TIT_STG_0	; 0 初期化
	JPTBL	TIT_STG_2	; 3 キー待ち
	JPTBL	TIT_STG_3	; 4 ジングル終了待ち
	JPTBL	TIT_STG_4	; 5 ゲームメインへ


;---- 初期化 ------------------------
;/// @brief Title phase 0: initialisation.
;/// @ingroup gamerom
TIT_STG_0:
	inc  <NMI_FLG

	jsr  STOP_BGM


	lda  #0
	jsr  CLEAR_BG
	lda  #0
	jsr  CLEAR_BG_2C


	SET_DATA_SRC  PAL_TITLE_ADR
	jsr  setPalData

	lda  #0
	sta  <BG_SCR_Y
	sta  <BG_SCR_X + 0
	sta  <BG_SCR_X + 1
	sta  <BG_SCR_X + 2
	sta  <BG_SCR_X + 3


;	SET_DATA_SRC BPE_TITLE
;	SET_DATA_DST  $2000 + 32*0 + 0
;	jsr  bpe_dec_vram

	SET_VRAM_ADD2	#$2000 + 32*10 + 6
	DRAW_STRING2 "MAPPER 0 SAMPLE GAME"

	SET_VRAM_ADD2	#$2000 + 32*12 + 11
	DRAW_STRING2 "BG SWITCH H"

	SET_VRAM_ADD2	#$2FC0 + $10*0

	lda  #%0101_0101
	ldy  #$40
.loop_atr
	sta  $2007
	dey
	bne  .loop_atr

	
	SET_VRAM_ADD2	#$2000 + 32*19 + 11
	DRAW_STRING2 "GAME START"
	SET_VRAM_ADD2	#$2000 + 32*21 + 12
	DRAW_STRING2 " NSDLib"
;	DRAW_STRING2 " OPTION"


	SET_VRAM_ADD2	#$2000 + 32*24 + 18
	DRAW_STRING2 "~IMPACT SOFT"

	SET_VRAM_ADD2	#$2000 + 32*26 + 19
	.INCLUDE	".\strver.h"
;	SET_VRAM_ADD2	#$2000 + 32*28 + 7
;	.INCLUDE	".\strdate.h"
;@	BEEP $104,%11110011


	LDA  #0
	STA  <DEBUG_KEY_CNT

	sta	 PUSH_CTR

	LDA	#TITLE_DEMO_TM
	STA	<DEMO_TIMER

	LDA	PLY_STAGE
	BNE	.TIT_S00

	INC	PLY_STAGE
.TIT_S00:


	lda #FLG_PPU2000
	sta <FLG_2000
	lda #FLG_PPU2001
	sta <FLG_2001

	jsr  SPT_CLR_RTN

	dec  <NMI_FLG

	lda  <FLG_2000
	sta  $2000

	jsr SET_FADE_IN_B
	DISP_ON



	lda  #15
	sta  <MASTER_VOL

	inc  <STG_COD_SUB

;	lda  #SE_TITLE
;	jsr  PLAY_SE


	rts


;******* メイン **********************
;/// @brief Title phase 2: the menu.
;/// @ingroup gamerom
TIT_STG_2:
	CHK_BIT	<KEY_TRG, #KEY_UP|KEY_DOWN
	beq  .TIT_STG_12_00

	lda  #SE_CUR_SEL
	jsr  PLAY_SE
	inc  <GM_TMP0
	lda  <GM_TMP0
	and  #$01
	sta  <GM_TMP0

	lda  #0
	sta  <PUSH_CTR


.TIT_STG_12_00
	CHK_BIT	<KEY_TRG, #KEY_A|KEY_B|KEY_RUN
	beq  .end


	lda  <GM_TMP0
	beq  .next_stg

	;--- MENU -> NSE.LIB ----

	lda  #SE_CUR_ENT
	jsr  PLAY_SE
;	lda  #2
;	jsr  PLAY_BGM
;	rts

 	jsr SET_FADE_OUT_B
	jsr WAIT_FADE_END


;	lda	 #ST_OPTION       ;オプション画面へ
	lda	 #ST_LICENSE       ;ライセンス画面へ
;	lda	 #ST_OVER	      ; ゲームオーバー画面へ
;	lda	 #ST_CLEAR	      ; ゲームクリアー画面へ
	jmp  SET_STG_COD

.next_stg

	jsr  STOP_BGM

	lda  #SE_CUR_ENT
	jsr  PLAY_SE


	lda  #60
	sta  <GM_WAIT

	inc  <STG_COD_SUB
.end
	rts


;--- 効果音再生終了待ち ------------
;/// @brief Title phase 3: waiting for the confirmation sound to finish.
;/// @ingroup gamerom
TIT_STG_3:

	ldx  #0
	lda  <SYS_TIMER
	and  #2
	beq  .ts3_00
	ldx  #INSCOIN_PERIOD/2

.ts3_00
	stx  <PUSH_CTR


	lda  <GM_WAIT
	cmp  #80
	bne  .ts3_01
;	lda  #BGM_TITLE
;	JSR     PLAY_BGM
.ts3_01

	jsr  ST_GM_WAIT
	rts

;--- プレイ画面へ ------------
;/// @brief Title phase 4: entering the play screen.
;/// @ingroup gamerom
TIT_STG_4:

 	jsr SET_FADE_OUT_B
	jsr WAIT_FADE_END

	DISP_OFF

	lda  #0
	sta  <DEMO_FG
	jsr  GAME_INIT

	lda  #ST_MAIN        ;プレイ画面へ
	jsr  SET_STG_COD
	rts



;/// @brief Resets the game state for a new run.
;/// @ingroup gamerom
GAME_INIT:
	lda  #1
	sta  PLY_STAGE
	lda  #0
	sta  DEBUG_FLG

;/// @brief Game reset entry point that skips the setup.
;/// @ingroup gamerom
GAME_INIT2:
	; スコア初期化
	lda  #0
	sta  GM_SCORE
	sta  GM_SCORE+1
	sta  GM_SCORE+2
	sta  GM_SCORE+3
	sta  PLY_CONTINUE

	inc  SCR_CHG_SW
	rts


