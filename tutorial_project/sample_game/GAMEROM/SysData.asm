;/// @file SysData.asm
;/// @brief Static tables: nametables, string data and the pointer lists.
;/// @ingroup gamerom
;///
;/// Read-only data the rest of the ROM indexes into. Kept in one place so the
;/// bank budget for data is visible at a glance.

;***********************************************************************
;	データ転送関連システム
;***********************************************************************

;=======================
; ROMの指定バンクのデータをRAMにコピーする
;  IN: SRC_ADR 転送元アドレス 16bit
;  IN: DST_ADR 転送先アドレス 16bit
;  IN: Y 転送サイズ
;  破壊 A,Y
;=======================
;/// @brief Copies #SRC_ADR to #DST_ADR. Destroys A and Y.
;/// @ingroup gamerom
memcpy:
.loop
	dey
	lda  [SRC_ADR],y
	sta  [DST_ADR],y
	cpy  #0
	bne  .loop
	rts



;=======================
; 背景データ 2C00       *
;=======================
;/// @brief Clears the nametable at `$2C00`.
;/// @ingroup gamerom
CLEAR_BG_2C:
	PHA
	SET_VRAM_ADD2 #$2C00
	PLA
	jmp CLEAR_BG_00
;/// @brief Common tail of the background clear routines.
;/// @ingroup gamerom
BAK_CLR_RTN:
	LDA	#$00
;/// @brief Clears a nametable.
;/// @ingroup gamerom
CLEAR_BG:
	PHA
	SET_VRAM_ADD2 #$2000
	PLA

;/// @brief Clears a nametable with tile 0.
;/// @ingroup gamerom
CLEAR_BG_00:

	ldx	#3
;/// @brief Clears nametable 1.
;/// @ingroup gamerom
CLEAR_BG_DATA_L1:
	ldy	#0
;/// @brief Clears nametable 0.
;/// @ingroup gamerom
CLEAR_BG_DATA_L0:
	sta	$2007
	dey
	bne	CLEAR_BG_DATA_L0
	dex
	bne	CLEAR_BG_DATA_L1

	LDY	#$C0
;	JSR  CLEAR_VRAM
;/// @brief Clears nametable 2.
;/// @ingroup gamerom
CLEAR_BG_DATA_L2:
	sta	$2007
	dey
	bne	CLEAR_BG_DATA_L2


	LDA	#$00
	LDY	#$40
;	JSR  CLEAR_VRAM
;/// @brief Clears nametable 3.
;/// @ingroup gamerom
CLEAR_BG_DATA_L3:
	sta	$2007
	dey
	bne	CLEAR_BG_DATA_L3

	RTS


;=======================
; パレット初期セットサブ
;  IN: SRC_ADR 転送元アドレス 16bit
;=======================
;/// @brief Stages a palette from #SRC_ADR into #PAL_WRK.
;/// @ingroup gamerom
setPalData:
	ldy  #32
	SET_DATA_DST PAL_WRK
;/// @brief Palette staging entry point that skips the address setup.
;/// @ingroup gamerom
setPalData2:
	PAL_CHG
	jmp  memcpy

;------------------------------------------------
;	BPE デコード
;
;  SRC_ADR =  データ格納アドレス
;  DST_ADR =  展開先VRAMアドレス
;------------------------------------------------
BPE_pass		EQU  TMP_SV7		;///< BPE frame header: dictionary entry count.
BPE_decompsize	EQU  TMP_WRK0		; 展開サイズ		;///< BPE frame header: decompressed size.
BPE_compsize	EQU  TMP_WRK2		; 圧縮サイズ		;///< BPE frame header: compressed size.

BPE_DIC_H		EQU  TMP_SV0		;///< BPE dictionary: high halves of the replaced pairs.
BPE_DIC_L		EQU  TMP_SV2		;///< BPE dictionary: low halves.
BPE_DIC_O		EQU  TMP_SV4		;///< BPE dictionary: the replacement bytes.
BPE_DT_BUF		EQU  BPE_BUF		;///< BPE decode stack: pending data bytes.
BPE_IX_BUF		EQU  BPE_BUF+$80		;///< BPE decode stack: pending dictionary indices.


;/// @brief Decompresses a BPE stream straight into VRAM.
;/// @ingroup gamerom
bpe_dec_vram:
	; 転送先VRAMアドレスセット
	lda  <DST_ADR+1
    sta  $2006
	lda  #$FF
	sta  <DST_ADR+1
	lda <DST_ADR+0
	sta $2006

;/// @brief Decompresses a BPE stream into #BPE_BUF.
;/// @ingroup gamerom
bpe_dec:

.loop
	jsr  getSCR_ADR_DATA
	bne  .no_end
	rts
.no_end
	sta  <BPE_pass

	; 展開サイズ
	jsr  getSCR_ADR_DATA
	sta  <BPE_decompsize+0
	jsr  getSCR_ADR_DATA
	sta  <BPE_decompsize+1

	; 圧縮サイズ
	jsr  getSCR_ADR_DATA
	sta  <BPE_compsize+0
	jsr  getSCR_ADR_DATA
	sta  <BPE_compsize+1

	; 辞書アドレスH
	LD_W <BPE_DIC_H , <SRC_ADR
	ldx  #0
	lda  <BPE_pass
	jsr  addSCR_ADR

	; 辞書アドレスL
	LD_W <BPE_DIC_L , <SRC_ADR
	ldx  #0
	lda  <BPE_pass
	jsr  addSCR_ADR

	; 辞書アドレスO
	LD_W <BPE_DIC_O , <SRC_ADR
	ldx  #0
	lda  <BPE_pass
	jsr  addSCR_ADR

	jsr  frame_decode
	
	jmp .loop

;------------------------------------------------
;	BPE 1フレームデコード
;------------------------------------------------
;/// @brief Decodes one BPE frame.
;/// The 6502 counterpart of `bpe_decode()` in `sys/rp_bpe.cpp`; the two read the
;/// same wire format. @see @ref sample_game
;/// @ingroup gamerom
frame_decode:
	lda  <DST_ADR+1
	bpl  frame_decode2_ram

;/// @brief Second-stage BPE frame decode.
;/// @ingroup gamerom
frame_decode2:
	lda  <BPE_compsize+0
	bne  .fd00
	lda  <BPE_compsize+1
	beq  .end
	dec  <BPE_compsize+1
.fd00
	dec  <BPE_compsize+0
	jsr  getSCR_ADR_DATA
	
	ldx  #0
	ldy  <BPE_pass

.loop
	cpy  #0
	beq  .next
	dey
	cmp  [BPE_DIC_O],y
	bne  .loop

	lda  [BPE_DIC_L],y
	sta  BPE_DT_BUF,x
	tya
	sta  BPE_IX_BUF,x
	lda  [BPE_DIC_H],y
	inx
	bne  .loop

.next
	sta  $2007
	cpx  #0
	beq  frame_decode2

	dex
	ldy  BPE_IX_BUF,x
	lda  BPE_DT_BUF,x
	jmp  .loop
.end
	rts


;/// @brief BPE frame decode with a RAM destination.
;/// @ingroup gamerom
frame_decode2_ram:
	lda  <BPE_compsize+0
	bne  .fd00
	lda  <BPE_compsize+1
	beq  .end
	dec  <BPE_compsize+1
.fd00
	dec  <BPE_compsize+0
	jsr  getSCR_ADR_DATA
	
	ldx  #0
	ldy  <BPE_pass

.loop
	cpy  #0
	beq  .next
	dey
	cmp  [BPE_DIC_O],y
	bne  .loop

	lda  [BPE_DIC_L],y
	sta  BPE_DT_BUF,x
	tya
	sta  BPE_IX_BUF,x
	lda  [BPE_DIC_H],y
	inx
	bne  .loop

.next
	sty  <TMP_SVY
	ldy  #0
	sta  [DST_ADR],y
	inc  <DST_ADR+0
	bne  .sk00
	inc  <DST_ADR+1
.sk00
	ldy  <TMP_SVY
	cpx  #0
	beq  frame_decode2_ram

	dex
	ldy  BPE_IX_BUF,x
	lda  BPE_DT_BUF,x
	jmp  .loop
.end
	rts


