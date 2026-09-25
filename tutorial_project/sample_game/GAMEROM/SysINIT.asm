;/// @file SysINIT.asm
;/// @brief Cold-start: RAM clear, PPU warm-up and the WRAM reset gesture.
;/// @ingroup gamerom
;///
;/// Runs only on a real console. Clears memory, waits out the PPU's power-on
;/// settling time, and blanks the palette so the reset garbage never reaches the
;/// screen.
;///
;/// The stack page is cleared with care: the high score lives there and is meant
;/// to survive a reset, so the routine steps around it. Holding a key combination
;/// at boot forces the whole of WRAM to be wiped anyway, which is why the key
;/// port is read twice here -- the debouncer needs the same keys on two
;/// consecutive frames before it will report them.
;=====================================================
;
;	起動時初期化処理
;
;=====================================================




;---------------------------------------------------
MAGIC_LEN	EQU	6		;///< Length of the #MAGIC signature, in bytes.

;/// @brief The #MAGIC signature, in ROM, compared against the copy in the stack page.
;/// @ingroup gamerom
ro_magic:
	.db	"MAP0DM"


;/// @brief Clears memory, waits out the PPU and decides whether to wipe WRAM.
;/// Steps around the high-score bytes in the stack page so they survive a reset,
;/// and reads the controller twice so the two-frame debounce can report the
;/// WRAM-wipe key combination on the very first frame.
;/// @ingroup gamerom
SYS_INIT:
	sei
	ldx  #0		; =ldx #0
	stx  $2000
	stx  $2001
	stx  $4015
	stx	 $4010  ;apu__dmc_control
	lda	#(%1<<6)
	sta	 $4017  ;apu__frame



	lda	 $2002  ;ppu__status		; 1st-write
	lda	#$10
	tax
.toggle_ppu_a12:
	sta	 $2006  ;ppu__address
	sta	 $2006  ;ppu__address
	eor	#$10
	dex
	bne	.toggle_ppu_a12
	stx	 $2001  ;ppu__control2

	bit	 $2002  ;ppu__status
.v:	bit	 $2002  ;ppu__status
	bpl	.v


;=======================
; メモリークリア       *
;=======================
	txa				; =lda #0
.CLR_LOP:
	sta  <$00 ,x

; この領域に置いたハイスコアをリセット時にも保持させるため、
;   sta  $0100,X
	sta  $0200,X
	sta  $0300,X
	sta  $0400,X
	sta  $0500,X
	sta  $0600,X
	sta  $0700,X
	inx
	bne  .CLR_LOP

;===============================
;	グラフィック関連初期化
;===============================
.v2:
	bit	 $2002  ;ppu__status
	bpl	.v2

;===============================
;	パレットクリア
;===============================
; 全て白にしてリセット時のゴミを隠す
	SET_VRAM_ADD2 #$3F00
	ldy  #32
	lda  #$30			; 白
	jsr  SYS_VRAM_WLP

	SET_VRAM_ADD2 #$3F00
	ldy	#1
	lda	#$1F			; 黒
	jsr  SYS_VRAM_WLP



;===============================
;	サウンド・乱数初期化
;===============================
	jsr  INIT_SOUND

	jsr .INIT_RND


;===============================
; 初回キー入力チェック
;===============================
; WRAM 強制初期化操作に必要
; 連続 2 フレームで同じキーが押されていないと
; 「押された」と判定されないため、2 回の読込みを行わせる。
	jsr  KEY_RTN
	jsr  KEY_RTN

;=======================
	; 起動時ならマジックナンバー書込みとハイスコアクリア (2016-06-01 門真)
	ldx	#MAGIC_LEN -1		; カウンタ
	clc				; 初回起動でない、としておく
.magic_loop:
	lda	 ro_magic, x
	tay
	eor	 MAGIC, x
	beq	.magic_match
	tya
	sta	 MAGIC, x
	sec				; 初回起動確定
.magic_match:
	dex
	bpl	.magic_loop
	bcc	.magic_e		; リセット時なら何もしない

	lda	#0
	ldx	#(3+1)*2 -1		; (ハイスコア,キャラ)*レベル数-1
.boot_loop:
	sta	 HISCORES, x
	dex
	bpl	.boot_loop


.magic_e:




	rts



;*****************************************
;乱数システム初期化
;*****************************************
.INIT_RND:
	LDA	#0
	STA	RND_SEL
	LDA	#15
	STA	RND_WK0
	LDA	#162
	STA	RND_WK1
	LDA	#83
	STA	RND_WK2
	LDA	#230
	STA	RND_WK3
	RTS



