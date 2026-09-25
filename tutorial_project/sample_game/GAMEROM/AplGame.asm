;/// @file AplGame.asm
;/// @brief The play state: the per-frame step machine for a stage.
;/// @ingroup gamerom
;///
;/// `APL_GAME` is entry 5 in the `STG_COD` jump table, and `PLY_STG_0` through
;/// `PLY_STG_4` are the phases it walks -- stage intro, play, and the exits into
;/// the clear and game-over states.
;///
;/// @note Under the cartridge only the innermost part of this runs. `FCP_GAME_MAIN`
;///       calls `updateMission` and `moveGameObj` directly and never enters this
;///       state machine, so the intro and exit phases are skipped and the C++ side
;///       decides when a run has ended.


;=====================================
;プレイ画面
;=====================================
;/// @brief Entry 5 of the `STG_COD` table: the play state on a real console.
;/// @ingroup gamerom
;/// @note Not reached under the cartridge. @see @ref sample_game
APL_GAME:
	lda  #0
	sta  <ENEMY_FLFG
	sta  <ENEMY_NT_FLFG
	
	jsr PLY_STG_MAIN
	
	jsr updateMission
	jsr moveGameObj

	jsr  updateGameDisp

	lda  <STG_COD	;
	cmp  #ST_MAIN
	beq  .skip
	lda  <FLG_2000
	sta	 $2000				; このタイミングでNMI発生
.skip
	rts


;/// @brief Dispatches on #STG_COD_SUB to the current play phase.
;/// @ingroup gamerom
PLY_STG_MAIN:
	LDA	<STG_COD_SUB
	TBL_JUMP
	JPTBL	PLY_STG_0	; 0
	JPTBL	PLY_STG_1	; 1
	JPTBL	PLY_STG_2	; 2	オーバー演出
	JPTBL	PLY_STG_3	; 3	クリアー演出
	JPTBL	PLY_STG_4	; 4	PAUSE

;****** INIT ************
;/// @brief Play phase 0: stage initialisation.
;/// @ingroup gamerom
PLY_STG_0:
	DISP_OFF
;	INC	<NMI_FLG


	ldx  #low( CLEAR_300W_TOP )
	lda  #0
.loop
	sta  $300,x
	inx
	bne  .loop

	lda  #GAME_DEMO_TM
	sta  <DEMO_TIMER



	jsr  PLY_LIFE_SET		; ライフ初期化

	jsr  initGameDisp

	jsr  initGameBgStarObj

	
	jsr initMission


	lda  #PLY_AN_WAIT
	jsr  SET_PLY_ANM

    SET_NMI_CALL PLY_DRAW_S		; NMI描画処理登録
	
	inc  <STG_COD_SUB
	DISP_ON
	jsr SET_FADE_IN_B

	RTS

;****** MAIN ***********
;/// @brief Play phase 1: the stage proper.
;/// @ingroup gamerom
PLY_STG_1:

.plydm_10:

	lda  PLY_ANM_NO
	cmp  #PLY_AN_DEAD
	bne  .plydm_20
	
	jsr  PLY_MOVE

	lda  #180
	sta  <GM_WAIT
	
	lda  #SE_PLY_DEAD
	jsr  PLAY_SE
	lda #2
	sta <STG_COD_SUB
	rts
	
.plydm_20:
	lda MISSON_TYPE
	cmp #$FF
	bne .plydm_11

	; クリアー画面へ
;	lda  #BGM_GAME_CLEAR
;	jsr  PLAY_SE
	jsr  STOP_BGM
	lda #3
	sta <STG_COD_SUB
	rts
.plydm_11:
	
	rts



;****** OVER WAIT ***********
;/// @brief Play phase 2: waiting out the game-over sequence.
;/// @ingroup gamerom
PLY_STG_2:
	DEC	<GM_WAIT
	LDA	<GM_WAIT
	CMP	#30
	beq .ps03		

	lda  <FLM_TIMER
	and  #$02
	sta PLY_DISP_FG

	jsr  setPlayerDeadEffect
	jsr  PLY_MOVE
	RTS

.ps03
	inc PLY_DISP_FG


	lda  #ST_OVER
	jmp  exitAplGame
;	jsr  exitAplGame
;	DEBUG_HALT
;	rts

;****** CLEAR WAIT ***********
;/// @brief Play phase 3: waiting out the stage-clear sequence.
;/// @ingroup gamerom
PLY_STG_3:
	DEC	<GM_WAIT
	LDA	<GM_WAIT
	CMP	#180-25-32
	beq .ps03		
	jsr  PLY_MOVE
	RTS
.ps03
	; 次のステージへ
	lda  #ST_CLEAR
	jmp  exitAplGame


;****** PAUSE ***********
;/// @brief Play phase 4: paused.
;/// @ingroup gamerom
PLY_STG_4:
	CHK_BIT <KEY_TRG, #KEY_RUN
        BEQ     .plyst4_00
	LDA	#1
	STA	<STG_COD_SUB
	RTS

.plyst4_00:
	RTS


;=====================================
;ゲーム終了処理（ラスターシステムの影響で特定の手順を踏まないと画面化ける）
;  Areg -> ジャンプ先STEP番号
;=====================================
;/// @brief Leaves the play state for the screen named in the accumulator.
;/// @ingroup gamerom
exitAplGame:
	pha
	jsr  exitAplGameSub

	pla
    jmp  SET_STG_COD

;/// @brief Exit entry point that skips the setup.
;/// @ingroup gamerom
exitAplGame2:
	pha
	jsr  exitAplGameSub
	pla
    jmp  SET_STG_COD2


;/// @brief Common tail of the play-state exits.
;/// @ingroup gamerom
exitAplGameSub:
	jsr  SET_FADE_OUT_B
	jsr  WAIT_FADE_END

	LDA	#0
	sta <NMI_CALL_BNK

	jsr WAIT_VSYNC

	inc	<NMI_FLG	;ハング防止


	jmp  WAIT_VSYNC



;=================================
; メイン描画処理
;=================================
;/// @brief Console-side draw for the play state.
;/// @ingroup gamerom
PLY_DRAW_S:
	lda  PALFADE_VAL
	bne  .skip_pal_trans

	ldx  DAM_BG_FLASH
	beq  .pa_90
	dec  DAM_BG_FLASH
	php

	SET_VRAM_ADD2 #$3F00
	
	lda  .tblDAM_BG_FLASH-1,x
	sta  $2007
	plp
	bne  .pa_90

	jsr  drawPlyLife

.pa_90

	; パレットアニメーション
;	ldx  #$00
	ldx  #$0D
	lda  <SYS_TIMER
	lsr  a
	bcc  .pa_00
	ldx  #$19
.pa_00
	and  #$03
	tay
	lda  .tblPalAmm,y
	sta  PAL_WRK,x

	lda  #$3F
	sta  $2006	; hi
	stx  $2006	; low
	lda  PAL_WRK,x
	sta  $2007

.skip_pal_trans
 
	;=スコア表示=====
	LDA     SCR_CHG_SW
	BEQ     .pds00
	LDA     #0
	STA     SCR_CHG_SW

	SET_VRAM_ADD2 #$2000 + 32*28 + 8
	lda  GM_SCORE+3
	jsr  DRAW_HEX_BYTE
	lda  GM_SCORE+2
	jsr  DRAW_HEX_BYTE
	lda  GM_SCORE+1
	jsr  DRAW_HEX_BYTE
	lda  GM_SCORE+0
	jsr  DRAW_HEX_BYTE
	lda  #'0'
	sta  $2007

.pds00:

	rts

.tblPalAmm:
	db  $0F,$05,$15,$25

.tblDAM_BG_FLASH:
	db  $0F,$06,$06,$06


