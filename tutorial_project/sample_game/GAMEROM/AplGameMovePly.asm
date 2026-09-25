;/// @file AplGameMovePly.asm
;/// @brief Player movement and the response to the controller bytes.
;/// @ingroup gamerom
;=======================================================================
;=======================================================================
;=======================================================================
;
;  各種ゲームOBJ移動処理
;

;-----------------------------------
; 自機ジャンプ中移動
;-----------------------------------
movePlyMoveObj
	jsr  PLY_KEY_CHK

	jmp  PLY_MOVE



;=====================================
;プレイヤーキーチェック
;=====================================
;/// @brief Turns the controller bytes into player movement and shots.
;/// Reads #KEY_NEW and #KEY_TRG, which under the cartridge were written by the
;/// C++ side rather than read from the port. @see @ref sample_game
;/// @ingroup gamerom
PLY_KEY_CHK:


;/// @brief Continuation of the play phase after the input check.
;/// @ingroup gamerom
PLY_STG_1A:
;	lda	 PALFADE_TIME
;	bne  .end

	lda  PLY_ANM_NO
	cmp  #PLY_AN_DEAD
	beq  .end

	lda  MISSON_TYPE
	cmp  #MT_EVNT
	beq  .end
	bne  .plydm_10

.end
	rts

.plydm_10
.plystg23:

;--- PLAY MAIN ----
	jsr PLY_SHOT_A

;/// @brief Main input handler during play.
;/// @ingroup gamerom
P1_KEYJOB:
	ldx #0
	jsr  shotHorming

.plystg24:
	jsr  KEY_DIR
	bmi  .end

	asl  a
	tax
	lda  PLY_OBJ_WX
	clc
	adc  .tblAddX+0,x
	sta  PLY_OBJ_WX
	lda  POS_PLY_X
	adc  .tblAddX+1,x

	; X limitter
	ldy  #PLY_LIM_XL
	cmp  #PLY_LIM_XL
	bcc  .xlim_end

	ldy  #PLY_LIM_XH
	cmp  #PLY_LIM_XH
	bcs  .xlim_end
	tay
.xlim_end
	sty  POS_PLY_X
	
	lda  PLY_OBJ_WY
	clc
	adc  .tblAddY+0,x
	sta  PLY_OBJ_WY
	lda  POS_PLY_Y
	adc  .tblAddY+1,x

	; Y limitter
	ldy  #PLY_LIM_YL
	cmp  #PLY_LIM_YL
	bcc  .ylim_end

	ldy  #PLY_LIM_YH
	cmp  #PLY_LIM_YH
	bcs  .ylim_end
	tay
.ylim_end
	sty  POS_PLY_Y

.end
	rts


.tblAddY
	dw  -MV_PLY_BASE0
	dw  -MV_PLY_BASE0_IR2
.tblAddX
	dw  0
	dw  MV_PLY_BASE0_IR2
	dw  MV_PLY_BASE0
	dw  MV_PLY_BASE0_IR2
	dw  0
	dw  -MV_PLY_BASE0_IR2
	dw  -MV_PLY_BASE0
	dw  -MV_PLY_BASE0_IR2


	CHK_BIT <KEY_NEW, #KEY_RIGHT
    beq  .p21_0
	;==========================
	; 右キー
	;==========================
	lda POS_PLY_X
	cmp #PLY_LIM_XH
	bcs .end
	inc POS_PLY_X
	inc POS_PLY_X

.p21_0
	CHK_BIT <KEY_NEW, #KEY_LEFT
    beq  .p21_1
	;==========================
	; 左キー
	;==========================
	lda POS_PLY_X
	cmp #PLY_LIM_XL
	bcc .end
	dec POS_PLY_X
	dec POS_PLY_X
	rts

.p21_1
	CHK_BIT <KEY_NEW, #KEY_UP
    beq  .p21_2
	;==========================
	; 上キー
	;==========================
	jmp  P1_UP

.p21_2
	CHK_BIT <KEY_NEW, #KEY_DOWN
    beq  .end

	;==========================
	; 下キー
	;==========================
;/// @brief Handles the down key.
;/// @ingroup gamerom
P1_DOWN:

	lda POS_PLY_Y
	cmp #PLY_LIM_YH
	bcs .end
	inc POS_PLY_Y
	inc POS_PLY_Y
.end
	rts


;/// @brief Handles the up key.
;/// @ingroup gamerom
P1_UP:
	lda POS_PLY_Y
	cmp #PLY_LIM_YL
	bcc .end
	dec POS_PLY_Y
	dec POS_PLY_Y
.end
	rts




