;/// @file AplOver.asm
;/// @brief Game-over screen, on the console's own hardware.
;/// @ingroup gamerom

	;======== GAME OVER BG =============


;============================================
; ゲームオーバー
;============================================
;/// @brief Console-side game-over screen.
;/// @ingroup gamerom
OVER_STG:
;.ovrstg000:
	jsr .tbljump_sub

	; スプライト制御
	ldy #0*4	; スプライト開始位置

	LDA	<PUSH_CTR
	AND	#$10
	BEQ	.draw_p00

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
	lda  #32
	jsr  createBgStarObj2
	
;	jsr  SPT_BNK2_ANIME
	; 余ったスプライトを非表示にする
	jmp  clearObj
;	rts


.tblCurPosX:
	db 8*13-4, 8*13-4

.tblCurPosY:
	db 8*16-4, 8*18-4

.tbljump_sub
	LDA   <STG_COD_SUB
	TBL_JUMP
	JPTBL	overstg00	; 0
	JPTBL	overstg10	; 1
	JPTBL	overstg20	; 2
	JPTBL	overstg30	; 3

;---- 初期化 ------------------------
;/// @brief Game-over phase 0: initialisation.
;/// @ingroup gamerom
overstg00:

	jsr STOP_SE
	lda	#0
	sta	 $4015
	lda	#%00001111
	sta	 $4015

	inc  <NMI_FLG
	lda  #0
	sta  $2000
	sta  $2001
	sei
	jsr  initSelDisp



	SET_VRAM_ADD2	#$2000 + 32*6 + 11
	DRAW_STRING2 "GAME OVER"

	SET_VRAM_ADD2	#$2000 + 32*9 + 9
	jsr  STR_CLEAR_2
	jsr  DRAW_SCORE


	lda  #BGM_OVER
	jsr  PLAY_BGM


	jsr  CHK_HISCORE

    SET_NMI_CALL OVER_DRAW		; NMI描画処理登録
	inc  <STG_COD_SUB

	lda  <FLG_2000
	sta	 $2000				; このタイミングでNMI発生
	DISP_ON
	jsr SET_FADE_IN_B


	lda  #3*60/8
	sta  <GM_WAIT
	jmp  WAIT_FADE_END


;/// @brief Game-over phase 1.
;/// @ingroup gamerom
overstg10:
	jsr  SLOW_DEC_GM_WAIT
    bne  .end

;	DEBUG_HALT
	inc  <STG_COD_SUB
	lda  #18*60/8
	sta  <GM_WAIT
.end
	rts

;/// @brief Game-over phase 2.
;/// @ingroup gamerom
overstg20:
	inc  <PUSH_CTR

	CHK_BIT	<KEY_TRG, #KEY_UP|KEY_DOWN
	beq  .TIT_STG_12_00

	lda  #SE_CUR_SEL
	jsr  PLAY_SE
	inc  <GM_TMP0
	lda  <GM_TMP0
	and  #$01
	sta  <GM_TMP0

.TIT_STG_12_00
	jsr  SLOW_DEC_GM_WAIT
    beq  .overstg20_01		; 時間切れ

	CHK_BIT	<KEY_TRG, #KEY_A|KEY_B|KEY_RUN
    bne  .overstg20_00
	rts

.overstg20_01:
	lda  #1
	sta  <GM_TMP0

.overstg20_00:
	lda  #SE_CUR_ENT
	jsr  PLAY_SE
	lda  <GM_TMP0
	bne  .continue_no

	inc  <STG_COD_SUB
	lda  #3*60
	sta  <GM_WAIT
	rts

	;----------------------------------
	; コンティニューNO
	;----------------------------------
.continue_no:
	jsr SET_FADE_OUT_B
	jsr WAIT_FADE_END
	lda  #ST_TITLE
	jmp	SET_STG_COD		; =jsr+rts
	

;/// @brief Game-over phase 3.
;/// @ingroup gamerom
overstg30:
	lda  <PUSH_CTR
	clc
	adc  #8
	sta  <PUSH_CTR

	dec  <GM_WAIT
    beq  .overstg30_job

	lda  #2*60
	cmp  <GM_WAIT
	bne  .end

.end
	rts
.overstg30_job

	; コンティニュー
	jsr SET_FADE_OUT_B
	jsr WAIT_FADE_END

	lda  #0
	sta  GM_SCORE+0
	sta  GM_SCORE+1
	sta  GM_SCORE+2

	lda  PLY_CONTINUE
	cmp  #COTI_MAX_LIFE
	beq  .skip_conti_inc
	inc  PLY_CONTINUE
.skip_conti_inc

 .if 0
	ldx  #PLY_LIFE_INIT
	cpx  PLY_CONTINUE
	bcs  .nolife_boost
	ldx  PLY_CONTINUE
.nolife_boost
	stx  PLY_LIFE
 .endif

	lda  #ST_MAIN
	jmp	SET_STG_COD		; =jsr+rts





;=================================
; ゲームオーバー
;=================================



;=================================
;VBLANK 描画
;=================================
;/// @brief Vertical-blank draw for the game-over screen.
;/// @ingroup gamerom
OVER_DRAW:
	LDA   <STG_COD_SUB
	TBL_JUMP
	JPTBL	JMP_RTS		; 0
	JPTBL	.OVER_D2	; 1
	JPTBL	.OVER_D1	; 2
	JPTBL	.OVER_D2	; 3

.OVER_D1
;	SET_VRAM_ADD2	#$2000 + 32*18 + 8
;	jsr  .DRAW_PUSH_ANY_BUTTON

	SET_VRAM_ADD2	#$2000 + 32*14 + 12
	DRAW_STRING2 "CONTINUE"

	SET_VRAM_ADD2	#$2000 + 32*16 + 12
	DRAW_STRING2 "   YES  "
	SET_VRAM_ADD2	#$2000 + 32*18 + 12
	DRAW_STRING2 "   NO   "
	
.OVER_D2

	lda  TMP_DISP2
	beq  .clrd2_01

	SET_VRAM_ADD2	#$2000 + 32*9 + 6
	lda  <SYS_TIMER
	and  #$04
	beq  .clrd2_00
	
	lda  #'H'
	sta  $2007
	lda  #'I'
	sta  $2007
	rts
.clrd2_00:
	lda  #0
	sta  $2007
	sta  $2007
.clrd2_01:
	rts

