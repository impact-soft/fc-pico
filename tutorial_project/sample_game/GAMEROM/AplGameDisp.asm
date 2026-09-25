;/// @file AplGameDisp.asm
;/// @brief Turns the object tables into sprites and BG updates.
;/// @ingroup gamerom
;///
;/// The half of the game that the cartridge replaces wholesale. Here each live
;/// object becomes OAM entries and nametable writes; under the cartridge the same
;/// tables are read by `ap_game::conv3DObje()` and become 3D objects instead.
;/// @see @ref sample_game




;=====================================
;
;	各種ワークに従ってゲーム画面を更新する
;
;
;=====================================

;/// @brief Rebuilds the sprite list and background updates from the object tables.
;/// The routine the cartridge replaces. `ap_game::conv3DObje()` reads the same
;/// tables and produces 3D objects instead. @see @ref sample_game
;/// @ingroup gamerom
updateGameDisp:
	;--------------------------------
	; スプライト構築
	;    y reg にスプライト開始位置をセットして使った枚数x4 y reg を加算する
	;--------------------------------
	ldy  #0*4	; スプライト開始位置
	sty  <TMP_COUNT

	lda  MISSON_TYPE
	cmp  #MT_EVNT
	beq  .skip_00

	jsr  createPlayerObj

	; 敵ノーマル弾表示
	jsr createEnemyNTObj


	lda  <TMP_COUNT
	cmp  #64
	bcs  .end

;.skip_test2

	lda  <TMP_COUNT
	cmp  #64
	bcs  .end

	; 自機弾表示
	jsr  createPlyShotAObj

	lda  <TMP_COUNT
	cmp  #64
	bcs  .end


	; 爆発演出表示
	jsr  createBakuEfcObj

	lda  <TMP_COUNT
	cmp  #64
	bcs  .end

.skip_00
	; BG流星表示
	jsr  createBgStarObj

	; 余ったスプライトを非表示にする
	jsr  clearObj

.end
	rts

;---------------------------------
;		LIFE描画
;---------------------------------
;/// @brief Draws the remaining lives.
;/// @ingroup gamerom
drawPlyLife:
	SET_VRAM_ADD2 #$2000 + 32*28 + 19

	lda  PLY_LIFE
	jsr  convBCD
	jmp  DRAW_HEX_BYTE


;-----------------------------------
; 自機キャラスプライトを構築
; y reg = スプライトの開始位置
;-----------------------------------
;/// @brief Emits the player's sprites, starting at the OAM index in Y.
;/// @ingroup gamerom
createPlayerObj:
	lda  PLY_DISP_FG
	bne  .end

	lda  PLY_MUTEKI_TM
	and  #$02
	bne  .end

	
.loop
	lda  POS_PLY_X
;	clc
;	adc  #4
	sta  OBJ_BUF +4 +3,y	; x
	clc
	adc  #-8
	sta  OBJ_BUF +3,y	; x

	lda  POS_PLY_Y
	clc
	adc  #-8
	sta  OBJ_BUF,y		; y
	sta  OBJ_BUF+4,y		; y

	lda  #$41
	sta OBJ_BUF +1,y	; pat
	lda  #$43
	sta OBJ_BUF +4 +1,y	; pat

	lda #$03
	sta OBJ_BUF +2,y	; pal
	lda #$03
	sta OBJ_BUF +4 +2,y	; pal

	tya
	clc
	adc  #8
	tay
	inc  <TMP_COUNT
	inc  <TMP_COUNT

.end
	rts


;-----------------------------------
; 爆発演出スプライトを構築
; y reg = スプライトの開始位置
;-----------------------------------

;/// @brief Emits the explosion sprites.
;/// @ingroup gamerom
createBakuEfcObj:
	ldx #0
.cb_loop
	lda BAKU_EFC_CNT,x
	beq .cb_next
	inc BAKU_EFC_CNT,x

	cmp #16
	beq .cb_fin
	cmp #24
	beq .cb_fin
	and #$FE
	asl a
	adc #BAKU_EFC_CHR -4
	
	sta OBJ_BUF +1,y
	clc
	adc #2
	sta OBJ_BUF +4 +1,y

	lda #$03			;パレット
	sta OBJ_BUF +2,y
	sta OBJ_BUF +4 +2,y

	lda BAKU_EFC_Y,x
	clc
	adc  #-8
	sta OBJ_BUF,y
	sta OBJ_BUF +4,y

	lda BAKU_EFC_X,x

	sta OBJ_BUF +4 +3,y
	clc
	adc #-8
	sta OBJ_BUF +3,y

	inc  <TMP_COUNT
	inc  <TMP_COUNT
	tya
	clc
	adc #8
	tay
	bne .cb_next
.cb_fin
	lda #0
	sta BAKU_EFC_CNT,x

.cb_next
	inx
	inx
	inx
	cpx #3*BAKU_EFC_SUU
	bne .cb_loop
	rts


;-----------------------------------
; 敵ノーマル弾表示
; y reg = スプライトの開始位置
;-----------------------------------
;/// @brief Emits the enemy and enemy-shot sprites.
;/// @ingroup gamerom
createEnemyNTObj:
	ldx  #0
	stx  <CACHE_GET_NENMY_NT_FG
.loop
	lda ENEMY_NT_KIND,x
	beq .next

	lda ENEMY_NT_Y+1,x
	cmp  #ENEMY_LINE_SUU
	bcs  .next

	lda  ENEMY_NT_X+1,x
	cmp  #8
	bcc  .next
	cmp  #248
	bcs  .next

	sec
	sbc #4
	sta OBJ_BUF +3,y
	
	lda ENEMY_NT_Y+1,x
	clc
	adc  #-8
	sta OBJ_BUF,y

	lda  ENEMY_NT_KIND,x
	php
	jsr  getEnemyNTcfg2
	plp
	bpl  .nt00
	; 16x16表示
	jsr  .disp16

	inc  <TMP_COUNT
	iny
	iny
	iny
	iny
	beq .end
	bne .nt01

.nt00
	lda  <TMP_WRK0
	and  #$FC
	bne  .nt02

	lda  ENEMY_NT_MP,x
	jsr  dirbullet
	jmp .nt01
.nt02
	lsr  a
	adc  #$51
	sta OBJ_BUF +1,y

	lda  <TMP_WRK0
	and  #$03
	sta  <TMP_SVX
	lda  ENEMY_NT_MP,x
	and  #%0011_0000
	asl  a
	asl  a
	ora  <TMP_SVX
	sta  OBJ_BUF +2,y

.nt01
	inc  <TMP_COUNT
	iny
	iny
	iny
	iny
	beq .end
.next
	txa
	clc
	adc  #ENEMY_NT_SIZE
	tax
	cpx #ENEMY_NT_SIZE*ENEMY_NT_SUU
	bne .loop
.end
	rts


.disp16
	stx  <TMP_SVX
	lda  <TMP_WRK0
	and  #%11_1100
	lsr  a
	tax

	lda OBJ_BUF,y
	sta OBJ_BUF+4,y

	lda OBJ_BUF +3,y
	sec
	sbc #4
	sta OBJ_BUF +3,y
	clc
	adc #8
	sta OBJ_BUF+4 +3,y
	
	lda  <FLM_TIMER
	and  .tbl_disp16chr+1,x
	clc
	adc  .tbl_disp16chr+0,x
	sta OBJ_BUF +1,y
	adc  #2
	sta OBJ_BUF +4 +1,y

	lda  <TMP_WRK0
	and  #$03
	sta OBJ_BUF +2,y
	sta OBJ_BUF +4 +2,y
	ldx  <TMP_SVX
	rts

.tbl_disp16chr
	db  $81,%111100	; 0
	db  $71,%1100	; 1
	db  $39,%0000	; 2
	db  $3D,%0000	; 3
	db  $59,%0000	; 4
	db  $5D,%0000	; 5

;/// @brief Picks the shot tile that matches a direction.
;/// @ingroup gamerom
dirbullet:
	stx  <TMP_SVX
	clc
	adc  #2
	lsr  a
	lsr  a
	and  #$0F
	tax
	lda  .tbl_dirbillet_chr,x
	sta  OBJ_BUF +1,y

	lda  <TMP_WRK0
	and  #$03
	ora  .tbl_dirbillet_flip,x
	sta OBJ_BUF +2,y

	ldx  <TMP_SVX
	rts

.tbl_dirbillet_chr
	db  $49,$4B,$4D,$4F
	db  $51,$4F,$4D,$4B
	db  $49,$4B,$4D,$4F
	db  $51,$4F,$4D,$4B

.tbl_dirbillet_flip
	db  $00,$00,$00,$00
	db  $40,$40,$40,$40
	db  $C0,$C0,$C0,$C0
	db  $80,$80,$80,$80



;-----------------------------------
; 自機の通常弾表示
; y reg = スプライトの開始位置
;-----------------------------------
;/// @brief Emits the player-shot sprites.
;/// @ingroup gamerom
createPlyShotAObj:

	lda  <SYS_TIMER
	and  #$01
	bne  .revece

	ldx #0
.loop
	jsr .set_sub
	inx
	inx
	cpx #2*PSHOT_A_SUU
	bne .loop
.end
	rts


.revece
	ldx  #2*(PSHOT_A_SUU -1)
.loop2
	jsr .set_sub
	dex
	dex
	cpx #-2
	bne .loop2
	rts


.set_sub
	lda PSHOT_A_Y,x
	bne .ss00
	rts
.ss00
	clc
	adc  #-8
	sta OBJ_BUF,y

	lda PSHOT_A_X,x
	sec
	sbc #4
	sta OBJ_BUF +3,y

	lda #$03
	sta  <TMP_WRK0

	lda  PSHOT_DIR,x
	jsr  dirbullet

	inc  <TMP_COUNT
	iny
	iny
	iny
	iny
	rts
