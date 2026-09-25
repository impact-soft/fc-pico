;/// @file AplBgStar.asm
;/// @brief The scrolling starfield drawn into the background layer.
;/// @ingroup gamerom
;///
;/// Console-only. The cartridge draws its own starfield out of 3D points.
;=====================================
;
;	BG裏に流れる星システム 8x16スプライト専用
;
;
;=====================================
BG_STAR_MAX   equ 16			; BG星の表示数デフォルト		;///< Default number of background stars.


;/// @brief Seeds the background starfield.
;/// @ingroup gamerom
initGameBgStarObj:
	lda  #BG_STAR_MAX
	sta  <BG_STAR_DISP
	lda  #$81
	sta  <BG_STAR_MODE
	rts

;-----------------------------------
;  BG流星表示
; y reg = スプライトの開始位置
;-----------------------------------
;/// @brief Emits the starfield sprites, starting at the OAM index in Y.
;/// @ingroup gamerom
createBgStarObj:
	ldx  <FLM_TIMER
	stx  <TMP_SV0
	lda  <BG_STAR_MODE
	and  #$07
	tax
.loop
	dex
	beq  .exit
	asl  <TMP_SV0
	jmp  .loop

.exit
	bit  <BG_STAR_MODE
;	bpl  .scr_down
	bmi  .scr_down
	lda  <TMP_SV0
	eor  #$FF
	sta  <TMP_SV0

.scr_down
	ldx  <TMP_SV0
	lda  <BG_STAR_DISP
	bne  createBgStarObj2
	rts
;-----------------------------------
;  BG流星表示（拡張版）
; a reg = 表示個数（偶数単位
; x reg = スクロール値0-255
; y reg = スプライトの開始位置
;-----------------------------------
;/// @brief Starfield entry point that skips the setup.
;/// @ingroup gamerom
createBgStarObj2:
	stx  <TMP_SV0
	lsr  a
	sta  <TMP_LOOP_CNT
	sta  <TMP_SV1

	ldx  #0
.loop
	lda TBL_BG_STAR_Y,x
	clc
	adc <TMP_SV0
	sta	OBJ_BUF+0,y	; ypos

	lda TBL_BG_STAR_X,x
	sta	OBJ_BUF+3,y	; xpos

	lda TBL_BG_STAR_CHR,x
	sta	OBJ_BUF +1,y	; pat


;	bit7:垂直反転(１で反転)
;	bit6:水平反転(１で反転)
;	bit5:BGとの優先順位(0:手前、1:奥)
;	bit0-1:パレットの上位2bit	
	lda #$23
	sta	OBJ_BUF +2,y	; pal
	
	iny
	iny
	iny
	iny
	cpy #0
	beq .end
	inx
	dec <TMP_LOOP_CNT
    bne .loop

	lda <TMP_SV1
	sta <TMP_LOOP_CNT
	ldx #0
.loop2
	; 倍速
	lda TBL_BG_STAR_X,x
	clc
	adc <TMP_SV0
	clc
	adc <TMP_SV0
	sta	OBJ_BUF+0,y	; ypos

	lda  #$EB
	eor  TBL_BG_STAR_Y,x
	sta  OBJ_BUF+3,y	; xpos

	lda TBL_BG_STAR_CHR,x
	sta	OBJ_BUF +1,y	; pat


;	bit7:垂直反転(１で反転)
;	bit6:水平反転(１で反転)
;	bit5:BGとの優先順位(0:手前、1:奥)
;	bit0-1:パレットの上位2bit	
	lda #$80+$23
	sta	OBJ_BUF +2,y	; pal
	
	iny
	iny
	iny
	iny
	cpy #0
	beq .end
.next
	inx
	dec <TMP_LOOP_CNT
    bne .loop2

.end
	rts



;=====================================
;
;	BG裏に流れる星システム 8x16スプライト専用 データテーブル
;
;
;=====================================
BG_STAR_CHR   equ $31		; BG星のキャラ番号		;///< CHR tile used for a background star.


;/// @brief Starting X for each background star.
;/// @ingroup gamerom
TBL_BG_STAR_X:
	db $00,$20,$40,$60
	db $80,$A0,$C0,$E0

	db $10,$30,$50,$70
	db $90,$B0,$D0,$F0

	db $08,$28,$48,$68
	db $88,$A8,$C8,$E8

	db $18,$38,$58,$78
	db $98,$B8,$D8,$F8


;/// @brief Starting Y for each background star.
;/// @ingroup gamerom
TBL_BG_STAR_Y:
	db 32+2,76,240,50
	db 100,8,180,64

	db 215,16,140,90
	db 20,168,58,244

	db 132,226,60,180
	db 20,148,140,164

	db 12,176,80,230
	db 140,8,38,94



;/// @brief CHR tile for each background star.
;/// @ingroup gamerom
TBL_BG_STAR_CHR:
	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6
	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6

	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6
	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6

	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6
	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6

	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6
	db BG_STAR_CHR+ 0,BG_STAR_CHR+ 2,BG_STAR_CHR+ 4,BG_STAR_CHR+ 6



