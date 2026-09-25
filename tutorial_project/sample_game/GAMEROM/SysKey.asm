;/// @file SysKey.asm
;/// @brief Controller reading and the two-frame debounce.
;/// @ingroup gamerom
;///
;/// Produces the held-key and newly-pressed-key bytes the rest of the game reads.
;/// A key must appear in two consecutive reads before it counts, which is what
;/// makes the boot-time reset gesture reliable.
;///
;/// @note Under the cartridge these bytes are not read from the port at all: the
;///       RP2350 writes them straight into `KEY_NEW` and `KEY_TRG`.
;///       @see @ref sample_game
;========================================
; Key System
;========================================

SYSKEY_OLD  EQU  0		;///< Previous read, used by the two-frame debounce.


;*****************************************
;キーの方向を取得
;*****************************************
;/// @brief Converts the held direction keys into one of the `KDIR_*` values.
;/// @ingroup gamerom
KEY_DIR:
	lda  <KEY_NEW
	and  #$0F
	tax
	lda  .tbl,x
	rts


.tbl
	db  KDIR_N	; 0000 入力無し
	db  KDIR_R	; 0001 KEY_RIGHT
	db  KDIR_L	; 0010 KEY_LEFT
	db  KDIR_N	; 0011 KEY_RIGHT + KEY_LEFT
	db  KDIR_D	; 0100 KEY_DOWN
	db  KDIR_DR	; 0101 KEY_DOWN + KEY_RIGHT
	db  KDIR_DL	; 0110 KEY_DOWN + KEY_LEFT
	db  KDIR_D	; 0111 KEY_DOWN + KEY_RIGHT + KEY_LEFT
	db  KDIR_U	; 1000 KEY_UP
	db  KDIR_UR	; 1001 KEY_UP + KEY_RIGHT
	db  KDIR_UL	; 1010 KEY_UP + KEY_LEFT
	db  KDIR_U	; 1011 KEY_UP + KEY_RIGHT + KEY_LEFT
	db  KDIR_N	; 1100 KEY_UP + KEY_DOWN
	db  KDIR_R	; 1101 KEY_UP + KEY_DOWN + KEY_RIGHT
	db  KDIR_L	; 1110 KEY_UP + KEY_DOWN + KEY_LEFT
	db  KDIR_N	; 1111 KEY_UP + KEY_DOWN + KEY_RIGHT + KEY_LEFT

;****************
; KEY RTN       *
;****************
;
;/// @brief Reads the controller and updates the held and newly-pressed bytes.
;/// @ingroup gamerom
;///
;/// A key is reported only once it has appeared in two consecutive reads, which
;/// is what makes the boot-time WRAM reset gesture dependable.
;/// @note Not called under the cartridge: the RP2350 writes `KEY_NEW` and
;///       `KEY_TRG` itself. @see @ref sample_game
KEY_RTN:
	;----------------------------------------------------------------------
	; 4 回読込み版
	;----------------------------------------------------------------------
	lda	<KEY_NEW
	sta	<KEY_OLD

	 jsr	.read			; 6+153
	 sta	<KEY_CH1		; 3
	 jsr	.read			; 6+153
	 pha				; 3
	  jsr	.read			; 6+153
	  sta	<KEY_CH3		; 3
	  jsr	.read			; 6+153  (645)
	 pla				; (2nd read)
	cmp	<KEY_CH1
	beq	.store			; XX--
	cmp	<KEY_NEW		; (4th read)
	beq	.store			; -X-X
	lda	<KEY_CH3		; X-X-
					; -XX-
					; --XX
.store:
	sta	<KEY_NEW

;;	lda	<KEY_NEW
	eor	<KEY_OLD
	pha
	 and	<KEY_NEW
	 sta	<KEY_TRG
	pla
	and	<KEY_OLD
	sta	<KEY_REL

;;	RTS

	lda	<KEY_TRG
	and	#(KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT)
	beq	.main			; 新たに押されたキーがない?

	tay
	lda  ro_keytable, y		; 同時押し対策 (下上右左の順に優先)
	sta	<REP_KEY		; 新たに押されたキーをリピート用に設定
	lda	#REP_WAIT
	sta	<REP_CNT		; 初回ウェイト
	.if	1
	 .if	 REP_WAIT
	  bne	.press			; =bra  押し始めは押下あり
	 .else
	  beq	.press			; =bra  押し始めは押下あり
	 .endif
	.else
	 lda	#0			; 押し始めは押下なし (trigger に任せる)
	 beq	.set			; =bra
	.endif
.main:
	lda	<KEY_NEW
	and	<REP_KEY
	beq	.set			; リピート用キーが押されていない (a=0)?

	dec	<REP_CNT		; ウェイトのカウントダウン
	beq	.press			; 初回ウェイト終了か?
	lda	<REP_CNT
	eor	#-REP_INTERVAL
	cmp	#1			; c = 0:一致 / 1:不一致
	lda	#0			; カウンタ初期値 または 押下キーなし
	bcs	.set			; 2 回目以降のウェイト終了でないか?

	sta	<REP_CNT		; カウンタを戻す。
.press:
	lda	<REP_KEY
.set:
	sta	<REP_NEW		; リピートにより ON/OFF される押下状態
	rts

	;----------------------------------------------------------------------
	; 4 回読込み版 - 1 回分サブ
	;----------------------------------------------------------------------
	; こちらの版は 2 フレーム間での比較を行わないため、
	; 反応が良くなって既存のゲームバランスに影響が出てしまった。
	; そのため、やむなく使用しないこととなった。
.read:
	lda	#1			; 2
	sta	<KEY_NEW		; 3
	sta	 $4016			; 4
	lsr	 a			; 2  =lda #0
	sta	 $4016			; 4
.read_loop:
	lda	 $4016			; 4x
	and	#(%11<<0)		; 2x
	cmp	#(%01<<0)		; 2x
	rol	<KEY_NEW		; 5x
	bcc	.read_loop		; 3x
					;-1
	lda	<KEY_NEW		; 3
	nop				; 2  調整用
	rts				; 6  (153)

	;----------------------------------------------------------------------
;;	Align	16
;/// @brief Direction lookup table: key bits to `KDIR_*`.
;/// @ingroup gamerom
ro_keytable:		;2143
	.db	%0000	;----
	.db	%0001	;---R
	.db	%0010	;--L-
	.db	%0001	;--lR
	.db	%0100	;-D--
	.db	%0100	;-D-r
	.db	%0100	;-Dl-
	.db	%0100	;-Dlr
	.db	%1000	;U---
	.db	%1000	;U--r
	.db	%1000	;U-l-
	.db	%1000	;U-lr
	.db	%0100	;uD--
	.db	%0100	;uD-r
	.db	%0100	;uDl-
	.db	%0100	;uDlr
	;--^--^--

