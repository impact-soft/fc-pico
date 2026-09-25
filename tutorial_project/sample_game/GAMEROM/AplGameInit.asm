;/// @file AplGameInit.asm
;/// @brief Per-stage setup: clears the object tables and seeds the mission.
;/// @ingroup gamerom



;=====================================
;
;	ゲーム初期化
;
;
;=====================================

;/// @brief Prepares the display for a stage: nametables, palette and sprites.
;/// @ingroup gamerom
initGameDisp:

	;--------------------------
	; BG関連初期化
	;--------------------------

	lda #0
	JSR CLEAR_BG

	lda #0
	jsr CLEAR_BG_2C

	jsr setGameCommonPal
	jsr PAL_FADE_SYSTEM

	; アトリビュート書き換え
	SET_VRAM_ADD2	#$23C0

	lda #%00000000
	ldy #64 -8
.loop_at
	sta	$2007
	dey
	bne .loop_at

	lda #%11111111
	ldy #8
.loop_at2
	sta	$2007
	dey
	bne .loop_at2

	SET_VRAM_ADD2 #$2000 + 32*28 + 0
	DRAW_STRING2 " SCORE 000000000  "

	lda  #$7F	; 自機アイコン
    sta  $2007
	DRAW_STRING2 "    STAGE "
	

	; 	ステージ番号描画
	SET_VRAM_ADD2 #$2000 + 32*28 + 28
	
	LDA	PLY_STAGE
	jsr  convBCD
	JSR	DRAW_HEX_BYTE

	; 	残機数描画
	jsr  drawPlyLife


;	メモリークリアー
	ldx  #0
	txa			; =lda #0
.CLR_LOP:
	sta  $0400,x
	sta  $0500,x
	sta  $0600,x
	sta  $0700,x
	inx
	bne  .CLR_LOP

	jsr  SPT_CLR_RTN


	lda  #1
	sta  SCR_CHG_SW


	lda #POS_PLY_X_INIT
	sta POS_PLY_X
	lda #POS_PLY_Y_INIT
	sta POS_PLY_Y

	rts




