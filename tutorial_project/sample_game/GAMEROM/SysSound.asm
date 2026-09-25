;/// @file SysSound.asm
;/// @brief Music and sound-effect requests, on top of the NSD.Lib driver.
;/// @ingroup gamerom
;///
;/// The game never touches the APU. It raises a request byte and this layer hands
;/// it to the sound driver linked in at `$8000`.
;///
;/// Under the cartridge the same request bytes are read and cleared by the C++
;/// side instead, which plays the sound through its own NSF player. The two
;/// arrangements share the slot numbering and nothing enforces that they agree.
;/// @see @ref sample_game
;======================================================
;			サウンドシステム
;======================================================

_nsf_init		EQU $8010	; NSF init address		;///< NSD.Lib entry: NSF init.
_nmi_main		EQU $8084	; NSF play address		;///< NSD.Lib entry: NSF play, called once per frame.
_nsd_init		EQU $80A1		;///< NSD.Lib entry: driver init.
_nsd_set_dpcm	EQU $80AB	; ax = Pointer of ⊿PCM infomation Struct		;///< NSD.Lib entry: install the DPCM information struct.
_nsd_main		EQU $80B2		;///< NSD.Lib entry: per-frame driver tick.
_nsd_play_bgm	EQU $8137	; ax = Pointer of BGM		;///< NSD.Lib entry: start music, pointer in A and X.
_nsd_stop_bgm	EQU $8219		;///< NSD.Lib entry: stop music.
_nsd_play_se	EQU $8239	; ax = Pointer of SE		;///< NSD.Lib entry: start a sound effect.
_nsd_stop_se	EQU $82B7		;///< NSD.Lib entry: stop the sound effect.
_nsd_snd_init	EQU $8AB6		;///< NSD.Lib entry: reset the sound state.

_nsd_table_idx	EQU $8F6E	; テーブルインデックス		;///< NSD.Lib table index.


bgm_addr	EQU (_nsd_table_idx+2)		;///< Address of the music table.
se_addr		EQU (_nsd_table_idx+2)		;///< Address of the sound-effect table.

STOP_SE_SYS	 EQU  _nsd_stop_se		;///< Value that means stop the current effect.

;/// @brief Stops the current sound effect.
;/// @ingroup gamerom
STOP_SE:
	jmp  STOP_SE_SYS



;===============================
; BGM再生
;===============================
;/// @brief Requests a music track from the sound driver.
;/// @ingroup gamerom
PLAY_BGM:
	STA  <REQ_BGM_NO
    RTS

;===============================
; SE再生
;===============================
;/// @brief Requests a sound effect unconditionally, pre-empting the current one.
;/// @ingroup gamerom
PLAY_SE_FORCE:
	STA	<REQ_SE_NO
	rts

;/// @brief Requests a sound effect, if nothing more urgent is already sounding.
;/// @ingroup gamerom
PLAY_SE:
	phxy
;	ldy  <DEMO_FG
;	bne  .end

	ldy  #2
	tax
.loop
	ldx  REQ_SE_NO,y
	beq  .set
	dey
	bne  .loop
.set
	sta  REQ_SE_NO,y
.end
	plxy
	RTS



;===============================
; サウンド初期化
;===============================
;/// @brief Initialises the NSD.Lib driver linked in at `$8000`.
;/// @ingroup gamerom
INIT_SOUND:
	lda  #15
	sta  <MASTER_VOL
	jmp  _nsd_init

;---------------------------------------------
;	A	bit 0	BGM Status (0:Stop / 1:Play)
;		bit 1	SE  Status (0:Stop / 1:Play)

;/// @brief Returns the driver's busy flags; bit 1 is the sound-effect channel.
;/// @ingroup gamerom
MUSDRV_GET_STATE:
	lda  __flag
	rts





;/// @brief Stops the music.
;/// @ingroup gamerom
STOP_BGM:
;/// @brief Music stop, entered directly. Also forces the DPCM channel off, which stopping the driver alone does not do.
;/// @ingroup gamerom
STOP_BGM_SYS:
	; BGM 停止時に DPCM が止まらないので無理矢理止める (2016-05-15 門真)
	lda	#%00001111
	sta	 $4015
	jmp  _nsd_stop_bgm




;*****************************************
;サウンドシステム
;*****************************************
;/// @brief Per-frame sound service: hands pending requests to the driver.
;/// @ingroup gamerom
;/// @note Called from the NMI handler, so under the cartridge it never runs. The
;///       request bytes are consumed by the C++ side instead.
;///       @see @ref sample_game
SOUND_SYSTEM:
	lda  <DEMO_FG
	beq  .snd_sys40
	lda  #0
	sta  <REQ_BGM_NO
.snd_sys40
	lda  <SEQ_CTR
	beq  .snd_sys02
	dec  <SEQ_CTR		; BGM再生ウェイト
	jmp  .snd_sys00
.snd_sys02:
	lda  <REQ_BGM_NO
	beq  .snd_sys00
	jsr  _playBGM
.skip00
	lda  #0
	sta  <REQ_BGM_NO
.snd_sys00:
	lda  <REQ_SE_NO3
	beq  .snd_sys01_2
	jsr  _playSE
.snd_sys01_2:
	lda  <REQ_SE_NO2
	beq  .snd_sys01_3
	jsr  _playSE
.snd_sys01_3:
	lda  <REQ_SE_NO
	beq  .snd_sys01_4
	jsr  _playSE
.snd_sys01_4:
	lda  #0
	sta  <REQ_SE_NO
	sta  <REQ_SE_NO2
	sta  <REQ_SE_NO3

	; --- main start -----

 	jmp  _nsd_main

	
_playSE
	asl	a
	tay
	;SE再生開始
	lda  se_addr + 0,y
	ldx  se_addr + 1,y
	jmp  _nsd_play_se
.end
	rts

_playBGM
	asl	a
	tay
	lda  bgm_addr + 0,y
	ldx  bgm_addr + 1,y
	jmp  _nsd_play_bgm
	rts


