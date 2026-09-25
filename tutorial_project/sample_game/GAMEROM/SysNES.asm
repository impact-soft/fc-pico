;/// @file SysNES.asm
;/// @brief Reset vector, the NMI handler and the random-number generator.
;/// @ingroup gamerom
;///
;/// The frame is driven from `NMI`, in the order the PPU demands: sprite DMA
;/// first, then the queued VRAM writes, then the scroll registers, and only then
;/// the APU. Everything before the scroll write has to fit inside vertical blank.
;///
;/// @note None of this runs under the cartridge. The RP2350 calls the game logic
;///       directly and does the drawing itself, so the NMI handler, the sprite DMA
;///       and the VRAM queue are all dead code in that mode.

;=====================================================


;/// @brief Reset vector. Runs only on a real console.
;/// @ingroup gamerom
INIT:
	sei
	cld
	ldx	#$ff
	txs

	jsr  SYS_INIT
	
	; PPU 制御フラグ 1 初期化
	lda	#FLG_PPU2000
	sta	<FLG_2000
	sta	 $2000				; このタイミングでNMI発生

;/// @brief Clear loop used during reset.
;/// @ingroup gamerom
LOPX:
	jsr WAIT_VSYNC

	; demo timer 処理
	lda  <DEMO_TIMER
	beq  .lpx00
	lda  <SYS_TIMER
	and  #$0f
	beq  .lpx00
	dec  <DEMO_TIMER
.lpx00
	jsr  KEY_RTN		;--- キー入力チェック -----
	jsr  MAKE_RND	; 乱数生成

	jsr  PLY_MAIN_S	; メイン処理呼び出し

	jsr PAL_FADE_SYSTEM

	jmp LOPX


;*****************************************
;乱数システム
;*****************************************
;/// @brief Advances the pseudo-random generator. Called once per frame.
;/// @ingroup gamerom
;/// @note Called from `FCP_GAME_MAIN` as well, so the sequence advances at the
;///       same rate in both modes.
MAKE_RND:
	lda  RND_WK0
	adc  #77
	sta  RND_WK0
	lda  RND_WK1
	sbc  #77
	sta  RND_WK1
	lda  RND_WK2
	adc  #77
	sta  RND_WK2
	lda  RND_WK3
	sbc  #77
	sta  RND_WK3
	rts


;===========================================================
;===========================================================
;===========================================================
;===========================================================
;===========================================================
;===========================================================

;***************************************
;***************************************
;***************************************
;***************************************
; NMI割り込み
;***************************************
;***************************************
;***************************************
;***************************************

;/// @brief Vertical-blank interrupt: the frame driver on a real console.
;/// @ingroup gamerom
;///
;/// Order is forced by the hardware -- sprite DMA, queued VRAM writes, scroll
;/// registers, then the APU. Everything up to the scroll write has to complete
;/// inside vertical blank or the picture breaks.
;/// @note Dead code under the cartridge, which drives the game from `$E004` and
;///       does its own drawing. @see @ref sample_game
NMI:
	bit	 $2002
	pha
	incw  <SYS_TIMER
	lda  <NMI_FLG	;NMI処理中か?
	beq  .nmi_ok
	pla
	rti

.nmi_ok
	inc  <NMI_FLG	;NMI処理中フラグオン
    txa
    pha
    tya
    pha

; --- NMI メイン処理 --------------

;****************************
; ＰＰＵ制御（※ＰＰＵは、垂直帰線期間中に処理を終わらせる）
;****************************
;--- スプライトDMA転送 ----- （※512 clock消費）
	lda  #high(OBJ_BUF)
	sta  $4014

	jsr  transPALLET


;--- ユーザーVRAM書き換え処理 -----
 .if 1
	lda <NMI_CALL_ADR+1
	beq  .no_usr_nmi
	lda <NMI_CALL_BNK
	jsr .usr_nmi_sub
.no_usr_nmi

 .endif

	lda	<FLG_2000
	sta	 $2000
	lda	<FLG_2001
	sta	 $2001


; ＩＲＱ割り込み値設定=============
;--- スクロールレジスタ設定 -----
	lda  <BG_SCR_X
	sta  $2005
	lda  <BG_SCR_Y
	cmp  #240 -1
	bcc  .y_set
	lda  #239
.y_set
	sta  $2005


;****************************
; ＡＰＵ制御（※ＰＰＵ後に処理する）
;****************************

	jsr  SOUND_SYSTEM


;****************************
; ＮＭＩの最後の最後
;****************************
	PLA
	TAY
	PLA
	TAX

	LDA	#0		;NMI処理中のフラグオフ
	STA	<NMI_FLG

	pla
	rti

.usr_nmi_sub
	jmp [NMI_CALL_ADR]



