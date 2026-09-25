;/// @file defRAM.h
;/// @brief The RAM map: every variable the game owns, by address.
;/// @ingroup gamerom
;///
;/// Zero page first, since the 6502 addresses it in one byte fewer, then the stack
;/// page -- which this game also uses for variables that must survive a reset --
;/// and then work RAM.
;///
;/// @warning The C++ side keeps its own copy of part of this map, in
;///          `sample_game/ap_game.h`. At this revision the 51 addresses that appear
;///          in both agree, but nothing in the build checks it and the two files are
;///          edited independently. @see @ref sample_game
;========================================


;----------------
; サウンドドライバーのワーク
;----------------
SND_WK0		EQU	$00	; size $32		;///< NSD.Lib driver work area, `$32` bytes of zero page.

;========================================
;  NSF再生用
;========================================
__ptr		EQU	$05	; 汎用ポインタ 2byte		;///< NSD.Lib general-purpose pointer, 2 bytes.
__tmp		EQU	$07		;///< NSD.Lib scratch byte.

__flag		EQU $0A		;///< NSD.Lib state flags: playback inhibit, fast-forward, SE priority, SE and BGM busy.
 .if 0
	lda	#nsd_flag::BGM + nsd_flag::SE
	sta	__flag		;BGM, SE処理を禁止（RAM未初期化対策）

	__flag
		D... .... : 再生制御無効
		...J .... : 早送り中
		.... PP.. : 効果音の優先度
		.... ..S. : ＳＥ再生中
		.... ...B : ＢＧＭが再生中 
 .endif

;========================================
W_AR		EQU	$40	; 16 bit 計算用  2 bytes		;///< 16-bit accumulator A for the `defMacro.h` word operations, 2 bytes.
W_BR		EQU	$42	; 16 bit 計算用  2 bytes		;///< 16-bit accumulator B for the `defMacro.h` word operations, 2 bytes.

TMP_SVA		EQU	$44	; 汎用 A レジスタ保存用アドレス		;///< Saved A register.
TMP_SVX		EQU	$45	; 汎用 X レジスタ保存用アドレス		;///< Saved X register.
TMP_SVY		EQU	$46	; 汎用 Y レジスタ保存用アドレス		;///< Saved Y register.
TMP_LOOP_CNT	EQU	$47	; 汎用ループカウンタ		;///< General-purpose loop counter.

TMP_SV0		EQU	$48	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 0.
TMP_SV1		EQU	$49	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 1.
TMP_SV2		EQU	$4A	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 2.
TMP_SV3		EQU	$4B	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 3.
TMP_SV4		EQU	$4C	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 4.
TMP_SV5		EQU	$4D	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 5.
TMP_SV6		EQU	$4E	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 6.
TMP_SV7		EQU	$4F	; 汎用レジスタ保存用アドレス		;///< General-purpose save slot 7.


TMP_WRK0	EQU	$50		;///< Scratch byte 0.
TMP_WRK1	EQU	$51		;///< Scratch byte 1.
TMP_WRK2	EQU	$52		;///< Scratch byte 2.
TMP_WRK3	EQU	$53		;///< Scratch byte 3.

TMP_DISP2	EQU	TMP_WRK3	; 1 byte  表示汎用		;///< Display scratch; an alias for #TMP_WRK3.


TMP_COUNT	EQU $54		; 2バイト		;///< General-purpose counter, 2 bytes.


SRC_ADR		EQU	$56		; 汎用ソースアドレス  2 bytes		;///< Source pointer for the copy and table-walk routines, 2 bytes.
DST_ADR		EQU	$58		; 汎用デスティネーションアドレス  2 bytes		;///< Destination pointer for the copy routines, 2 bytes.

TMP_ADR0		EQU	$5A		; 汎用アドレス  2 bytes		;///< General-purpose pointer 0, 2 bytes.
TMP_ADR0_IDX	EQU	$5C		; 汎用アドレス  1 bytes		;///< Index that goes with #TMP_ADR0.
TMP_ADR1		EQU	$5D		; 汎用アドレス  2 bytes		;///< General-purpose pointer 1, 2 bytes.
TMP_ADR1_IDX	EQU	$5F		; 汎用アドレス  1 bytes		;///< Index that goes with #TMP_ADR1.

;----------------
; サブル－チン呼び出しパラメーター
;----------------
PRM_0		EQU $60		;///< Subroutine parameter 0.
PRM_1		EQU $61		;///< Subroutine parameter 1.
PRM_2		EQU $62		;///< Subroutine parameter 2.
PRM_3		EQU $63		;///< Subroutine parameter 3.
PRM_4		EQU $64		;///< Subroutine parameter 4.
PRM_5		EQU $65		;///< Subroutine parameter 5.
PRM_6		EQU $66		;///< Subroutine parameter 6.
PRM_7		EQU $67		;///< Subroutine parameter 7.

PRM_X_POS	EQU $68		;///< Subroutine parameter: X position.
PRM_Y_POS	EQU $69		;///< Subroutine parameter: Y position.
PRM_W_POS	EQU $6A		;///< Subroutine parameter: width.
PRM_H_POS	EQU $6B		;///< Subroutine parameter: height.
PRM_WT_POS	EQU $6C		;///< Subroutine parameter: width in tiles.
PRM_HT_POS	EQU $6D		;///< Subroutine parameter: height in tiles.


;----------------
; 各画面毎に初期化して利用するワーク
;----------------
GM_TMP0	 	EQU	$70		;///< Per-screen scratch 0. Re-initialised whenever a screen starts.
GM_TMP1	 	EQU	$71		;///< Per-screen scratch 1.
GM_TMP2	 	EQU	$72		;///< Per-screen scratch 2.
GM_TMP3	 	EQU	$73		;///< Per-screen scratch 3.
GM_TMP4	 	EQU	$74		;///< Per-screen scratch 4.
GM_TMP5	 	EQU	$75		;///< Per-screen scratch 5.
GM_TMP6	 	EQU	$76		;///< Per-screen scratch 6.
GM_TMP7	 	EQU	$77		;///< Per-screen scratch 7.
GM_TMP8	 	EQU	$78		;///< Per-screen scratch 8.
GM_TMP9	 	EQU	$79		;///< Per-screen scratch 9.
GM_TMP10 	EQU	$7A		;///< Per-screen scratch 10.

; 選択画面系
DEBUG_KEY_CNT	EQU GM_TMP5	; デバッグ突入チェック用		;///< Counts the debug-entry key gesture. Alias for #GM_TMP5.
PUSH_CTR	EQU	GM_TMP7	; 文字＆カーソル点滅用		;///< Blink counter for menu text and cursors. Alias for #GM_TMP7.


DEBUG_0		EQU  GM_TMP7		;///< Debug menu scratch 0. Alias for #GM_TMP7, so it shares with #PUSH_CTR.
DEBUG_1		EQU  GM_TMP8		;///< Debug menu scratch 1. Alias for #GM_TMP8.
DEBUG_2		EQU  GM_TMP9		;///< Debug menu scratch 2. Alias for #GM_TMP9.
DEBUG_3		EQU  GM_TMP10		;///< Debug menu scratch 3. Alias for #GM_TMP10.

; 空き
DEBUG_COM	EQU	$80		;///< Debug command byte.



USR_PROG	EQU  $8A	; USRプログラム用		;///< Per-frame user hook called by `updateMission`; a null high byte disables it.
SP_LOCK		  EQU $8D	; スプライト更新制御用 （=1 更新しない）		;///< Non-zero suspends sprite updates.

CACHE_GET_NENMY_NT_FG  EQU $8E		;///< Set once the enemy-configuration lookup has been cached this frame.


PAL_WRK		EQU	$90 		 ;size $20	転送用		;///< Palette staging buffer, `$20` bytes, copied to the PPU during vertical blank.

;----------------
; デモ用
;----------------
DEMOMODE_NAM	EQU	$B0		;///< Nametable selection for attract mode.

DBD_BGTEST_FLG	EQU	$B1	; 非 0:BG テスト中 (3,4 面の水面制御抑制に使う)		;///< Non-zero while the BG test is running; suppresses the water effects on stages 3 and 4.


GM_WAIT		EQU	$B2	; 2 bytes  ゲーム待ち		;///< General game wait counter, 2 bytes.

;----------------
; 処理落ち対策
;----------------
ENEMY_FLFG		EQU  $B4	;		;///< Slowdown guard: skips part of the enemy update when the frame is running long.
ENEMY_NT_FLFG	EQU  $B5	;		;///< Slowdown guard for the enemy-shot update.

;----------------
; 拡張アダプター モード
;----------------
EXA_MODE	EQU	$B6	; =0 拡張モード =1 スタンドアロンモード		;///< 0 = expansion-adapter mode, 1 = stand-alone.

;----------------
; ゲーム関連
;----------------
DEMO_FG		EQU	$B7	; デモフラグ		;///< Attract-mode flag. Written by the C++ side each time play starts. @see @ref sample_game
DEMO_TIMER	EQU	$B8	; デモタイマー		;///< Counts down to the attract-mode hand-off.




;----------------
; キー関連
;----------------
KEY_CH0D	EQU	$C0	; ΔPCM ノイズ除去用に増設 (新設)		;///< Raw port 0 read, kept for delta-PCM noise rejection.
KEY_CH2D	EQU	$C1	; ΔPCM ノイズ除去用に増設 (新設)		;///< Raw expansion-port read, kept for delta-PCM noise rejection.

KEY_REL		EQU	$C2	;		;///< Keys released this frame.
KEY_TRG		EQU	$C3	;		;///< Keys newly pressed this frame. Written directly by the cartridge. @see @ref sample_game
KEY_OLD		EQU	$C4	;		;///< Previous frame's held keys, for the edge detector.
KEY_NEW		EQU	$C5	;		;///< Keys currently held. Written directly by the cartridge. @see @ref sample_game
KEY_CH0		EQU	$C6	;		;///< Debounced controller 1.
KEY_CH1		EQU	$C7	;		;///< Debounced controller 2.
KEY_CH2		EQU	$C8	; 拡張パッド用に増設 (新設)		;///< Debounced expansion pad 1.
KEY_CH3		EQU	$C9	; 拡張パッド用に増設 (新設)		;///< Debounced expansion pad 2.

REP_KEY		EQU	$CA	; リピート用のキー		;///< Key currently being auto-repeated.
REP_NEW		EQU	$CB	; リピートによる押下状態		;///< Press state produced by auto-repeat.
REP_CNT		EQU	$CC	; ウェイト、インターバルのカウンタ		;///< Auto-repeat delay and interval counter.

BG_STAR_DISP	EQU	$CD	; ゲーム中BG_STAR最大表示数		;///< Number of background stars to draw during play.
BG_STAR_MODE	EQU	$CE	; ゲーム中BG_STARスクロールモード		;///< Background starfield scroll mode.

;----------------
; IRQ処理関連
;----------------

;HIRQ_ENA	EQU  $CF ; IRQ フラグ制御 (未使用=0)

SCR_LINE	EQU	$D0	; size 4 bytes	多重スクロール用 (各段の開始位置)		;///< Split-scroll start scanlines, 4 bytes, one per band.
BG_SCR_X	EQU	$D4 ; size 4 bytes		;///< Horizontal scroll per band, 4 bytes.

BG_BNK		EQU	$DA	; size 6 bytes		;///< CHR bank selection, 6 bytes.
BG0_BNK		EQU	BG_BNK		;///< First CHR bank slot. Alias for #BG_BNK.
BG1_BNK		EQU	BG_BNK+1		;///< Second CHR bank slot.



;----------------
; システム関連
;----------------
FLG_2000	EQU	$E0		;///< Shadow of the PPU control register `$2000`.
FLG_2001	EQU	$E1		;///< Shadow of the PPU mask register `$2001`.
BG_SCR_Y	EQU	$E2		;///< Vertical scroll.

NMI_FLG		EQU	$E3		;///< Set by the NMI handler; the main loop waits on it to pace itself to the frame.
PAL_CHG_FG	EQU	$E4		; パレット変更フラグ		;///< Non-zero asks the NMI handler to push #PAL_WRK to the PPU.

SYS_TIMER	EQU	$E5		; 2 bytes		;///< Free-running frame counter, 2 bytes. Advanced in both run modes.
FLM_TIMER	EQU	$E7		; フレームタイマー		;///< Frame counter reset per screen.

STG_COD		EQU	$E8		;///< Console-side screen code; the index the `PLY_MAIN` jump table dispatches on.
STG_COD_SUB	EQU	$E9		;///< Sub-step within the current console-side screen.

;----------------
; バンク関連
;----------------
SPT_BNK		EQU	$EA		;///< Sprite CHR bank.
SPT_BNK2	EQU	$EB		;///< Second sprite CHR bank.
A0_BNK		EQU	$EC		; バンク切り替えリクエスト用　実際にはVBankで切り替わる		;///< Requested PRG bank; the switch itself happens in the vertical-blank handler.


;----------------
; NMIからコールするプログラムのアドレス
;----------------
NMI_CALL_BNK	EQU $ED ; 1byte ０ならコールしない		;///< Bank of the routine the NMI handler should call. Zero means call nothing.
NMI_CALL_ADR	EQU $EE ; 2byte コールするプログラムアドレス		;///< Address of the routine the NMI handler should call, 2 bytes.


TMP_SYS		EQU	$F0		;システムで使うTMP		;///< System scratch 0.
TMP_SYS2	EQU	$F1		;システムで使うTMP		;///< System scratch 1.
TMP_SYS3	EQU	$F2		;システムで使うTMP		;///< System scratch 2.
TMP_SYS4	EQU	$F3		;システムで使うTMP		;///< System scratch 3.


;----------------
; サウンド関連
;----------------
REQ_TMPUP		EQU	$F4	; テンポアップ		;///< Request a tempo increase.
REQ_SE_NO		EQU	$F5		;///< Sound-effect request 1. The cartridge plays it and writes back zero. @see @ref sample_game
REQ_SE_NO2		EQU	$F6	;		;///< Sound-effect request 2.
REQ_SE_NO3		EQU	$F7	;		;///< Sound-effect request 3. Only the first non-zero request is served per frame.

;LAST_SE_LOCK	EQU	$F8	; 同一効果音の最低再生フレーム数
;LAST_SE_NO		EQU	$F9	; 最後に再生したSE
REQ_BGM_NO		EQU	$FA		;///< Music request. Cleared once started, in either run mode.
;REQ_SE_NO		EQU	$FB
SEQ_CTR			EQU	$FC	; カウンタ		;///< Sound sequence counter.
;SND_FLG			EQU	$FD
__MusBank		EQU $FD		;///< Bank the NSD.Lib driver expects its music data in.


MASTER_VOL		EQU $FF		;///< Master volume for the driver.

WRAM_EXIST	EQU	$100	; 1 byte  非 0: WRAM が存在		;///< Non-zero once battery-backed WRAM has been detected.
				; 1 byte
HISCORES	EQU	$102	; 8 bytes LV1 のハイスコア,キャラ,		;///< High scores and their initials, 8 bytes. Kept in the stack page so a reset does not clear it.
				;         LV2 のハイスコア,キャラ
MAGIC		EQU	$10a	; 6 bytes 起動/リセット判別用マジックナンバー		;///< 6-byte signature distinguishing a cold boot from a reset.


;----------------
; ランダムシステム
;----------------
RND_SEL		EQU	$10b		;///< Selects which random word is read next.
RND_WK0		EQU	$10c		;///< Random generator state 0.
RND_WK1		EQU	$10d		;///< Random generator state 1.
RND_WK2		EQU	$10e		;///< Random generator state 2.
RND_WK3		EQU	$10f		;///< Random generator state 3.


;----------------
; ゲーム内表示関連
;----------------
GM_SCORE	EQU	$110	; 4 bytes		;///< Score, 4 bytes of packed BCD, least significant first. Read by the cartridge for the game-over screen.
GM_HISCORE	EQU	$114	; 4 bytes ハイスコア実作業用		;///< High score being edited, 4 bytes.
SCR_CHG_SW	EQU	$118	; 1 byte  スコア変化フラグ		;///< Set when the score changed, so the display is redrawn.

DEBUG_FLG		EQU	$11a	; デバッグモード突入フラグ		;///< Debug mode entered.
DEBUG_MT_FLG	EQU	$11b	; デバッグモード突入フラグ		;///< Mission-type debug mode entered.


PLY_LIFE		EQU $11e	; B 残機数		;///< Lives remaining. Drawn by the cartridge in the status line.
PLY_CONTINUE	EQU $11f	; コンティニュー回数 カウント上限99		;///< Continues used, counted to 99.

;----------------
; その他
;----------------
DEBUG_SEL	EQU	$120		;///< Selected row in the debug menu.

DEBUG_DT0	EQU	$121		;///< Debug data slot 0.
DEBUG_DT1	EQU	$122		;///< Debug data slot 1.
DEBUG_DT2	EQU	$123		;///< Debug data slot 2.
DEBUG_DT3	EQU	$124		;///< Debug data slot 3.
DEBUG_DT4	EQU	$125		;///< Debug data slot 4.
DEBUG_DT5	EQU	$126		;///< Debug data slot 5.
DEBUG_DT6	EQU	$127		;///< Debug data slot 6.
DEBUG_DT7	EQU	$128		;///< Debug data slot 7.

DEBUG_DT	EQU	DEBUG_DT0		;///< Base of the debug data slots. Alias for #DEBUG_DT0.


PLY_STAGE		EQU	DEBUG_DT0	; ステージ番号		;///< Current stage, 1-based. @warning An alias for #DEBUG_DT0: real game state living in a debug slot. The cartridge writes it by that address. @see @ref sample_game
DBD_SOUND_TST	EQU	DEBUG_DT1	; サウンドテスト		;///< Debug sound test selection. Alias for #DEBUG_DT1.
DBD_M_TYPE		EQU	DEBUG_DT2	; ミッションタイプ		;///< Debug mission type. Alias for #DEBUG_DT2.
DBD_MT_SUB		EQU	DEBUG_DT3	; ミッションタイプサブ		;///< Debug mission sub-type. Alias for #DEBUG_DT3.
DBD_STEP_JUMP	EQU	DEBUG_DT4	; ステップジャンプ		;///< Debug step jump. Alias for #DEBUG_DT4.



;========================================
; サウンドワーク
; $200-$328
;========================================
SND_WK1		EQU	$200	; size $128		;///< NSD.Lib driver work area, `$128` bytes.

;========================================
;  NSF再生用
;========================================
_eff		EQU $200		; 効果音テーブル開始番号		;///< First entry of the sound-effect table.
_play		EQU $201		; フレームオーバー防止用変数		;///< Guard the driver uses to avoid overrunning a frame.








;-----------------------------------------------------
; ここから下の$300台のワークは ステージ開始時に０クリアー
;-----------------------------------------------------
CLEAR_300W_TOP  EQU  $32E		;///< First address of the block `FCP_GAME_INIT` zeroes at the start of a stage.



MISSON_ATK_NO	EQU  $384	; B ミッション攻撃番号		;///< Mission attack number.
MISSON_ATK_IDX	EQU  $385	; B ミッション攻撃インデックス		;///< Index into the mission attack table.
MISSON_ATK_CNT	EQU  $386	; B ミッション攻撃カウンター		;///< Mission attack counter.


MISSON_LDBG0	EQU  $38A	; B ミッションBG番号		;///< Mission background number 0.
MISSON_LDBG1	EQU  $38B	; B ミッションBG番号		;///< Mission background number 1.
MISSON_LDBG2	EQU  $38C	; B ミッションBG番号		;///< Mission background number 2.

MISSON_ANM_NO	EQU  $38D	; B ミッションアニメ番号		;///< Mission animation number.
MISSON_ANM_CNT	EQU  $38E	; B ミッションアニメカウンター		;///< Mission animation counter.
;---- ミッション用ソフトスタック -------
MISSON_PC_SP	EQU  $38F	; B ミッションPCスタックポインタ		;///< Stack pointer into #MISSON_STACK.

MISSON_STACK	EQU $390		; 16byte ミッション用スタック		;///< The mission interpreter's call stack, 16 bytes.

;----------------
; ミッション制御系ワーク
;----------------
MISSON_NO		EQU $3A0	; B ステージ内ミッション番号		;///< Mission index within the stage.
MISSON_TYPE		EQU $3A1	; B ミッションタイプ		;///< Mission opcode. `$FF` halts the interpreter, and is what the cartridge reads as stage-cleared. @see @ref sample_game
MISSON_TYPE_SUB	EQU $3A2	; B ミッションタイプ		;///< Mission sub-type. Top two bits set #ENEMY_ATK_LV, bottom six index the script table.
MISSON_WAIT		EQU $3A3	; B MISSON_PCの次の処理までのウェイト		;///< Frames the script is suspended for.
MISSON_PC		EQU $3A4	; W ミッションPC		;///< Mission script program counter, 2 bytes.

MISSON_LOOP_CNT	EQU $3A6	; B ミッション ループカウンタ		;///< Loop counter for `MC_LOOP_CNT`.
MISSON_FLG		EQU $3A9	; B ミッション フラグ		;///< Mission flags.
MISSON_STEP		EQU $3AA	; B ミッション 処理ステップ		;///< Step within the current mission opcode.
MISSON_TMP		EQU $3AB	; B ミッション 汎用		;///< Mission scratch byte.

SECRET_STAT		EQU $3AC	; B シークレット状態(0:初期値 1:解放 2:取得)		;///< Secret item state: 0 initial, 1 revealed, 2 collected.
SECRET_LIFE_ADD	EQU $3AD	; B シークレットアイテム獲得時のライフボーナス		;///< Life bonus awarded for the secret item.

MISSON_ASM		EQU $3AE	; W ミッション毎フレーム割込み処理		;///< Per-frame mission hook, 2 bytes.

ENEMY_ATK_LV	EQU $3B0	; B 敵の攻撃LV		;///< Enemy attack level: 0 none, 1 aimed, 2 homing, 3 both, 4 fast homing.
							;  0:攻撃しない 1:自機狙い弾 2:ホーミング弾
							;  3:自機狙い＆ホーミング 4: 高速ホーミング

MISSON_CMP_P	EQU $3B1    ; B 比較命令時のフラグ保存		;///< Saved flags from the last mission compare, for the conditional jumps.

DAM_BG_FLASH	EQU $3B2	; ダメージＢＧフラッシュ		;///< Frames of damage flash owed. The cartridge reads it but the code acting on it is commented out.

;----------------
; プレーヤーワーク
;----------------
PLY_ANM_NO		EQU $3B3	; アニメーション番号		;///< Player animation state. `PLY_AN_DEAD` here is what ends a run under the cartridge.
PLY_FORM		EQU $3B4	; フォーメーション		;///< Player formation.
PLY_DISP_FG		EQU $3B5	; プレーヤー表示制御		;///< Player display control.
PLY_MUTEKI_TM	EQU $3B6	; 無敵タイマー		;///< Invulnerability frames left. Drives the blink on the cartridge side.

;---- ボスミッション用 ワーク -------
BM_DEATH_ANM	EQU $3B7		; w ボス死亡アニメ		;///< Boss death animation pointer, 2 bytes.
BM_DEATH_MSC	EQU $3B9		; w ボス死亡ミッションスクリプト		;///< Boss death mission script pointer, 2 bytes.

SHOT_TARGET		EQU $3BB		;///< Current homing-shot target.


;----------------
; プレーヤーワーク
;----------------

PLY_OBJ_KIND	EQU	$500		;///< Player object kind.
PLY_OBJ_DIR		EQU	$501		;///< Player facing.
PSHOT_KIND		EQU	$502		;///< First player-shot kind; stride 2.
PSHOT_DIR		EQU	$503		;///< First player-shot direction; selects the sprite tile on the cartridge side.

POS_PLY_X		EQU $520		;///< Player X in screen pixels.
POS_PLY_Y		EQU $521		;///< Player Y in screen pixels.

;----------------
; 自機の通常弾ワーク
;----------------
PSHOT_A_X	 EQU (POS_PLY_X+2)		;///< First player-shot X; stride 2 bytes per shot.
PSHOT_A_Y	 EQU (POS_PLY_Y+2)	; =0 の時はスタンバイ状態		;///< First player-shot Y. Zero marks the slot standing by.
PSHOT_A_SUU	 EQU 15		;///< Number of player-shot slots.


PLY_OBJ_WX		EQU	$540		;///< Player X sub-pixel fraction.
PLY_OBJ_WY		EQU	$541		;///< Player Y sub-pixel fraction.
PSHOT_A_WX		 EQU (PLY_OBJ_WX+2)		;///< First player-shot X fraction.
PSHOT_A_WY		 EQU (PLY_OBJ_WY+2)		;///< First player-shot Y fraction.




;----------------
; 敵のノーマル弾ワーク
; $600-$6AF 8x22セット
;----------------
ENEMY_NT_WORK	EQU	$600		;///< Base of the enemy table: #ENEMY_NT_SUU entries of #ENEMY_NT_SIZE bytes, `$600`-`$6AF`.
ENEMY_NT_KIND	EQU	$600		;///< Enemy kind; 0 marks the slot free. One of the `NTK_*` values.
ENEMY_NT_MP		EQU	$601	; 移動パターン番号		;///< Movement pattern number.
ENEMY_NT_DT		EQU	$602	; 特殊制御用データ		;///< Per-pattern control byte.
ENEMY_NT_HP		EQU	$603	; 耐久力		;///< Hit points.
ENEMY_NT_X		EQU $604	; W		;///< Enemy X, 16-bit; the high byte is the screen pixel.
ENEMY_NT_Y		EQU $606	; W		;///< Enemy Y, 16-bit; the high byte is the screen pixel.

ENEMY_NT_SIZE	 EQU 8		;///< Bytes per enemy entry.
ENEMY_NT_SUU	 EQU 22		;///< Number of enemy slots.

;----------------
; 爆発演出ワーク
; $6B0-$6D7 3x13セット
;----------------
BAKU_EFC_X	 EQU $6B0		;///< Base of the explosion table, `$6B0`-`$6D7`; stride 3 bytes.
BAKU_EFC_Y	 EQU $6B1		;///< Explosion Y.
BAKU_EFC_CNT EQU $6B2		;///< Animation frame; 0 is a free slot. Advanced by the cartridge, not by this ROM. @see @ref sample_game

BAKU_EFC_SUU EQU 13		;///< Number of explosion slots.



;----------------
; パレット関連
;----------------
PALFADE_TIME	EQU	$6D7	; パレットフェード速度		;///< Frames between fade steps.
PALFADE_CNT		EQU	$6D8	; パレットフェードカウンタ		;///< Counts down to the next fade step.
PALFADE_VAL		EQU	$6D9	; 加算値、減算値		;///< Current fade level.
PALFADE_ADD		EQU	$6DA	; 変化の加算値		;///< Amount added to #PALFADE_VAL each step; its sign picks fade-in or fade-out.
PALFADE_MASK	EQU	$60B	; 変化させないパレットビット指定		;///< Bit mask of palette entries to leave alone during a fade. @warning Almost certainly a typo for `$6DB`. At `$60B` it lands inside #ENEMY_NT_WORK -- slot 1's #ENEMY_NT_HP -- and the only `sta` to it, in SysPallet.asm, is commented out. So a console-side fade masks itself with whatever hit points enemy 1 happens to have. Not reachable under the cartridge, which fades with `rp_system::FadeOut()` instead. @see @ref sample_game

;--- 空きあり ---
; 6DC-6DF


PAL_WRK2	EQU	$6E0 	;size $20	フェード中転送用		;///< Second palette staging buffer used during a fade, `$20` bytes.


OBJ_BUF		EQU	$700		; 256 bytes		;///< Sprite assembly buffer, 256 bytes.
BPE_BUF 	EQU	$700		; BEP展開バッファ		;///< BPE decompression buffer. Shares its address with #OBJ_BUF, so the two may not be live at once.


