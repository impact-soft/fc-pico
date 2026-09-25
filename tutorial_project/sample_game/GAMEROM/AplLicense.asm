;/// @file AplLicense.asm
;/// @brief Licence screen, on the console's own hardware.
;/// @ingroup gamerom
;///
;/// The cartridge shows the same text from BPE-compressed nametables instead.
;/// @see @ref sample_game


;/// @brief Palette data for the licence screen.
;/// @ingroup gamerom
PAL_LICENSE:
	PAL_STAFF

;/// @brief Licence page 1, BPE-compressed.
;/// @ingroup gamerom
BPE_LICENSE0:
	.incbin		".\CHR\NamLicense0.bpe"
;/// @brief Licence page 2, BPE-compressed.
;/// @ingroup gamerom
BPE_LICENSE1:
	.incbin		".\CHR\NamLicense1.bpe"


LICENSE_DISP_WAIT	EQU  60*10/8		;///< Frames the licence page is held before it may be dismissed.
LICENSE_DISP_SCR	EQU  60* 6/8		;///< Scroll position of the licence text.


;============================================
; NSDLIB ライセンス画面
;============================================

;=========================================
;/// @brief Console-side licence screen, showing the NSD.Lib notice.
;/// @ingroup gamerom
APL_LICENSE:
	ldy  #0
	jsr  clearObj2


	LDA	<STG_COD_SUB
	TBL_JUMP
	JPTBL	.stg00	; 0
	JPTBL	.stg10	; 1
	JPTBL	.stg20	; 2
	JPTBL	.stg30	; 3
	JPTBL	.stg40	; 4

;---- 初期化 ------------------------
.stg00:
	jsr  STOP_BGM

	JSR     BAK_CLR_RTN

	SET_DATA_SRC BPE_LICENSE0
	SET_DATA_DST  $2000 + 32*0 + 0
	jsr  bpe_dec_vram

	SET_DATA_SRC BPE_LICENSE1
	SET_DATA_DST  $2C00 + 32*0 + 0
	jsr  bpe_dec_vram

	SET_DATA_SRC  PAL_LICENSE
	jsr  setPalData


	lda  #0
	sta  <BG_SCR_Y
	sta  <BG_SCR_X

	JSR	SPT_CLR_RTN

	jsr SET_FADE_IN_B
	DISP_ON
	jmp  .stg21

;---- メイン ------------------------
.stg10:
.stg30:
	CHK_BIT <KEY_TRG, #KEY_ABRS
	beq  .wait
	inc  <STG_COD_SUB
	rts
.wait
	jmp  ST_GM_WAIT2

.stg20
	lda  <BG_SCR_X
	clc
	adc  #5
	sta  <BG_SCR_X
	cmp  #255
	beq  .stg21
	rts

.stg21
	LDA  #LICENSE_DISP_WAIT
	STA  <GM_WAIT
	inc  <STG_COD_SUB
	rts



.st10_UP:
	sty  <GM_WAIT
	jsr  .st10_UP_sub
.st10_UP_sub:
	lda  <BG_SCR_Y
	beq  .end_scr
	dec  <BG_SCR_Y
.end_scr
	rts


.stg40
	jsr SET_FADE_OUT_B
	jsr WAIT_FADE_END

	lda  #0
	sta  <BG_SCR_X

	LDA  #ST_TITLE
	jmp  SET_STG_COD		; =jsr+rts



