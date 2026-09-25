;/// @file AplClear.asm
;/// @brief Stage-clear screen, on the console's own hardware.
;/// @ingroup gamerom



;============================================
; クリア
;============================================



;=========================================
;/// @brief Console-side stage-clear screen.
;/// @ingroup gamerom
CLEAR_STG:
	jsr .tbljump_sub

	; スプライト制御
	ldy #0*4	; スプライト開始位置
	; BG流星表示
	lda  <FLM_TIMER
	asl  a
	eor  #$FF
	tax
	lda  #64
	jsr  createBgStarObj2

	
;	jsr  SPT_BNK2_ANIME
	jmp  clearObj		; 余ったスプライトを非表示にする


.tbljump_sub
	LDA	<STG_COD_SUB
	TBL_JUMP
	JPTBL	.clearstg00	; 0
	JPTBL	.clearstg10	; 1
	JPTBL	.clearstg20	; 2
	JPTBL	.clearstg30	; 3

;---- 初期化 ------------------------
.clearstg00:
	inc  <NMI_FLG
	lda  #0
	sta  $2001
	sei

	jsr initSelDisp



	;---- STAGE ? CLEAR ----
	SET_VRAM_ADD2	#$2000 + 32*10 + 7
	DRAW_STRING2	"  STAGE "

	lda	 PLY_STAGE
	jsr	 DRAW_HEX_BYTE2
	DRAW_STRING2	" CLEAR"

	lda  PLY_CONTINUE
	bne  .cotinue_skip

	; ノーコンティニューボーナス
	SET_VRAM_ADD2	#$2000 + 32*12 + 8
	lda  #$7F	; 自機アイコン
    sta  $2007

	lda  PLY_LIFE
	jsr  convBCD
	sta  <GM_TMP0
	jsr  DRAW_HEX_BYTE

	DRAW_STRING2	" BONUS "


;	LDA    #ST_OVER
;	jmp	SET_STG_COD		; =jsr+rts


	lda  <GM_TMP0
	cmp  #$0F
	bcs  .dsp10ov
	ldy  #0
    sty  $2007
	jsr	 DRAW_HEX_BYTE2
	jmp  .dsp000
.dsp10ov
	jsr  DRAW_HEX_BYTE
.dsp000
	DRAW_STRING2	"000"

	lda  <GM_TMP0
	jsr  SCR_ADD_100




.cotinue_skip
	SET_VRAM_ADD2	#$2000 + 32*15 + 9
	jsr  STR_CLEAR_2
	jsr  DRAW_SCORE



	jsr  CHK_HISCORE

	jsr  STOP_SE

	lda  #BGM_CLEAR
	jsr  PLAY_BGM

	
	lda  #0
	sta <BG_SCR_Y
	sta <BG_SCR_X


    SET_NMI_CALL CLEAR_DRAW		; NMI描画処理登録

	INC	<STG_COD_SUB

	DISP_ON

	jsr SET_FADE_IN_B

	LDA	#5*60/8
	STA	<GM_WAIT
	RTS

.clearstg10:
	jsr  SLOW_DEC_GM_WAIT
	BNE	.end

	lda  #18*60/8
	sta  <GM_WAIT
	inc  <STG_COD_SUB
.end
	RTS

.clearstg20:
	jsr  SLOW_DEC_GM_WAIT
	beq  .clearstg00_2

	CHK_BIT	<KEY_TRG, #KEY_A|KEY_B|KEY_RUN
    beq    .end

.clearstg00_2:
	jsr  STOP_BGM

	lda  #SE_CUR_ENT
	jsr  PLAY_SE
	
	lda  #2*60/8
	sta  <GM_WAIT
	inc  <STG_COD_SUB
	rts

.clearstg30:
	jsr  SLOW_DEC_GM_WAIT
	bne  .end

	
	lda  #STAGE_MAX
	cmp  PLY_STAGE
	bne  .next
	; ソフトリセット
	jmp  INIT

.next
	; 次のステージへ
	INC	PLY_STAGE
	LDA    #ST_MAIN
.set_stg_cod:
	pha
	jsr SET_FADE_OUT_B
	jsr WAIT_FADE_END
	pla
	jmp	SET_STG_COD		; =jsr+rts





;=================================
;VBLANK 描画
;=================================
;/// @brief Vertical-blank draw for the stage-clear screen.
;/// @ingroup gamerom
CLEAR_DRAW:
	LDA   <STG_COD_SUB
	TBL_JUMP
	JPTBL	JMP_RTS		; 0
	JPTBL	.CLEAR_D2	; 1
	JPTBL	.CLEAR_D1	; 2
	JPTBL	.CLEAR_D3	; 3

.CLEAR_D3
	SET_VRAM_ADD2	#$2000 + 32*18 + 8
	jsr DRAW_PUSH_ANY_BUTTON_C
	jmp  .CLEAR_D2

.CLEAR_D1
	SET_VRAM_ADD2	#$2000 + 32*18 + 8
	jsr  DRAW_PUSH_ANY_BUTTON

.CLEAR_D2
	LDA	<TMP_DISP2
	BEQ	.clrd2_01

;	SET_VRAM_ADD2	#$2000 + 32*24 + 7
	SET_VRAM_ADD2	#$2000 + 32*15 + 6
	ldx	#0
	ldy	#0
	LDA	<FLM_TIMER
	AND	#$04
	BEQ	.clrd2_00

	ldx	#'H'
	ldy	#'I'
.clrd2_00:
	stx	 $2007
	sty	 $2007
.clrd2_01:

	RTS



