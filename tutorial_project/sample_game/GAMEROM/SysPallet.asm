;/// @file SysPallet.asm
;/// @brief Palette sets and the fade-in and fade-out routines.
;/// @ingroup gamerom
;///
;/// Fades run by stepping every entry towards or away from black through a
;/// brightness table, a frame at a time. The result is queued for the NMI handler
;/// like any other VRAM write.
;========================================
; Pallet System
;========================================
DEF_FADE_SPD	equ 4	; デフォルトフェード速度		;///< Default fade speed, in frames per step.

;*****************************************
;パレットフェードシステム
;*****************************************
;/// @brief Advances a running fade by one step, if one is due.
;/// @warning The mask it applies, #PALFADE_MASK, is never written -- the only
;///          `sta` to it in this file is commented out -- and its address falls
;///          inside the enemy table. @see @ref sample_game
;/// @ingroup gamerom
PAL_FADE_SYSTEM:
	jsr PAL_FADE_SYSTEM2	; フェード値制御
	jmp PAL_SET_RTN2		; パレット転送
	
;/// @brief Fade entry point that skips the due-time check.
;/// @ingroup gamerom
PAL_FADE_SYSTEM2:
	lda	PALFADE_TIME
	beq	.ret		; フェードタイムが０なら何もしないでリターン
	dec	PALFADE_CNT
	bne	.ret		; カウントダウン中ならリターン
	sta	PALFADE_CNT

	PAL_CHG
	lda	PALFADE_ADD
	clc
	adc	PALFADE_VAL
	sta	PALFADE_VAL
	beq	.end_fade
	cmp #$50
	beq	.end_fade
	cmp #-$50
	beq	.end_fade
.ret
	rts

.end_fade
	lda	#0
	sta	PALFADE_TIME
;	sta	PALFADE_MASK
	rts


;*****************************************
;黒フェードイン
;*****************************************
;/// @brief Starts a fade in from black.
;/// @ingroup gamerom
SET_FADE_IN_B:
	lda	#DEF_FADE_SPD
;/// @brief Fade in from black, at a caller-supplied speed.
;/// @ingroup gamerom
SET_FADE_IN_B2:
	ldy	#-$40
	ldx	#$10
	bne fade_set_end
;*****************************************
;黒フェードアウト
;*****************************************
;/// @brief Starts a fade out to black.
;/// @ingroup gamerom
SET_FADE_OUT_B:
	lda	#DEF_FADE_SPD
;/// @brief Fade out to black, at a caller-supplied speed.
;/// @ingroup gamerom
SET_FADE_OUT_B2:
	ldy	#0
	ldx	#-$10
	bne fade_set_end
;*****************************************
;白フェードイン
;*****************************************
;/// @brief Starts a fade in from white.
;/// @ingroup gamerom
SET_FADE_IN_W:
	lda	#DEF_FADE_SPD
;/// @brief Fade in from white, at a caller-supplied speed.
;/// @ingroup gamerom
SET_FADE_IN_W2:
	ldy	#$40
	ldx	#-$10
	bne fade_set_end
;*****************************************
;白フェードアウト
;*****************************************
;/// @brief Starts a fade out to white.
;/// @ingroup gamerom
SET_FADE_OUT_W:
	lda	#DEF_FADE_SPD
;/// @brief Fade out to white, at a caller-supplied speed.
;/// @ingroup gamerom
SET_FADE_OUT_W2:
	ldy	#0
	ldx	#$10
;/// @brief Common tail of the fade setup routines.
;/// @ingroup gamerom
fade_set_end:
	sta PALFADE_TIME
	sta	PALFADE_CNT
	stx	PALFADE_ADD
	sty	PALFADE_VAL
	PAL_CHG
	rts

;*****************************************
;フェード終了待ち
;*****************************************
;/// @brief Blocks until the running fade finishes.
;/// @ingroup gamerom
WAIT_FADE_END:
	lda  <FLG_2000
	sta	 $2000				; このタイミングでNMI発生
.loop
	jsr  WAIT_VSYNC
	jsr  PAL_FADE_SYSTEM
	lda	 PALFADE_TIME
	bne  .loop
	rts


;*****************************************
;パレットＰＰＵ転送システム
;*****************************************
;/// @brief Copies #PAL_WRK to the PPU. Must run inside vertical blank.
;/// @ingroup gamerom
transPALLET:
	lda  <PAL_CHG_FG
	beq  .end

	ldx  #0
	stx  <PAL_CHG_FG
	lda  #$3F
	sta  $2006	; hi
	stx  $2006	; low
	
	lda  PALFADE_VAL
	BNE  .fadepal00
	; ダイレクト転送
.loop
	lda  PAL_WRK,x
	sta  $2007
	inx
	cpx  #32
	bne  .loop
	rts

	; フェード中転送
.fadepal00
	lda  PAL_WRK2,x
	sta  $2007
	inx
	cpx  #32
	bne  .fadepal00
.end
	rts

;*****************************************
;パレット制御システム
;*****************************************
;/// @brief Installs a palette set and requests the transfer.
;/// @ingroup gamerom
PAL_SET_RTN2:
	LDA	<PAL_CHG_FG
	BNE	.pal00
	RTS
.pal00
	lda   #0
	tax
	tay
	LDA     PALFADE_VAL
	BNE     .fadepal00

	rts

;--- パレット転送処理 -----
.fadepal00
	BMI		.fadepal01

.palwcre020		; 加算転送 （白フェード用）
	lda  tblFadeMask,y
	and  PALFADE_MASK
	jsr  sub_palwcre
	iny
	cpy   #8
	BNE     .palwcre020
	rts



.fadepal01	; 減算転送 （黒フェード用）
	lda  tblFadeMask,y
	and  PALFADE_MASK
	jsr  sub_palbcre
	iny
	cpy   #8
	BNE     .fadepal01
	rts


;/// @brief One bit per palette entry, used to decide which entries a fade step touches.
;/// @ingroup gamerom
tblFadeMask:
	db  $01,$02,$04,$08,$10,$20,$40,$80

;--------------------
; 白フェードサブ
;--------------------
sub_palwcre
	bne  sub_paldirect
	tya
	pha
	ldy  #4

.loop
	lda	PAL_WRK,x
	cmp	#$0F		; $0Fは特殊扱い
	bne	.palwcre030
	lda	#$F0
.palwcre030
	; 白以上なら白にする
	clc
	adc	PALFADE_VAL
	cmp	#$40
	bcc	.palwcre040
	lda	#$30
.palwcre040
	sta  PAL_WRK2,x
	inx
	dey
	bne  .loop

	pla
	tay
	rts

;--------------------
; 黒フェードサブ
;--------------------
sub_palbcre
	bne  sub_paldirect
	tya
	pha
	ldy  #4

.loop
    LDA     PAL_WRK,X
	cmp	#$0F		; $0Fは特殊扱いで何もしない
	beq	.palbcre040
.palbcre030
	; 黒以下なら黒にする
	clc
	adc	PALFADE_VAL
	bpl	.palbcre040
	lda	#$0F
.palbcre040
	sta  PAL_WRK2,x
	inx
	dey
	bne  .loop

	pla
	tay
	rts

;--------------------
; ダイレクト転送
;--------------------
sub_paldirect
	tya
	pha
	ldy  #4

.loop
	lda	PAL_WRK,x
	sta  PAL_WRK2,x
	inx
	dey
	bne  .loop

	pla
	tay
	rts

