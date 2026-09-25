;/// @file AplMissionFunc.asm
;/// @brief The `MC_*` opcode implementations the mission interpreter dispatches to.
;/// @ingroup gamerom
;=====================================
;
;	ミッションから呼び出される拡張関数
;
;=====================================



;-------------------------------------------------------
;	ステージ開始ライフセット
;-------------------------------------------------------
;/// @brief Sets the life count for the start of a stage.
;/// @ingroup gamerom
PLY_LIFE_SET:
	lda  PLY_CONTINUE
	clc
	adc  #PLY_LIFE_INIT
	sta  PLY_LIFE
	rts



;/// @brief Stage palette set 1.
;/// @ingroup gamerom
STAGE_PAL1:
	PAL_STAGE_COM

;/// @brief Stage palette set 2.
;/// @ingroup gamerom
STAGE_PAL2:
	PAL_STAGE01
	PAL_STAGE02
	PAL_STAGE03
	PAL_STAGE04
	PAL_STAGE05
	PAL_STAGE06
	PAL_STAGE01
	PAL_STAGE01

;------------------------------------
; ゲーム画面共通パレットセット
;------------------------------------
;/// @brief Installs the palette shared by every play screen.
;/// @ingroup gamerom
setGameCommonPal:
	PAL_CHG
	ldy  #0
	lda  PLY_STAGE
	asl  a
	asl  a
	asl  a
	tax

.loop1
	lda  STAGE_PAL2-8,x
	sta  PAL_WRK,y
	inx
	iny
	cpy  #8
	bne  .loop1

.loop2
	lda  STAGE_PAL1-8,y
	sta  PAL_WRK,y
	iny
	cpy  #32
	bne  .loop2
	rts


	rts

