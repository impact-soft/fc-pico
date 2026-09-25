;/// @file defMission.h
;/// @brief Mission bytecode: the opcode numbers and the macros that emit them.
;/// @ingroup gamerom
;///
;/// Each `MC_*` macro writes one instruction: an opcode byte and its operands.
;/// The tables in `cfg/` are written entirely in these macros, so a stage script
;/// reads as assembly but assembles to data.
;///
;/// The mission types at the top -- `MT_HARA` and the rest -- select which script
;/// table a mission runs from. The `$FB`-`$FF` range is reserved for control
;/// rather than content, with `MT_END` at `$FF`.

;==========================================================
; ミッション制御関連定義
;==========================================================
MT_HARA		EQU  0		; HARADIUSミッション		;///< Ordinary wave mission. Runs a script from cfgMissonHara.h.
MT_FLET		EQU  1		; 艦隊ミッション		;///< Fleet mission.
MT_BOSS		EQU  2		; ボス敵ミッション		;///< Boss mission.
MT_EVNT		EQU  3		; イベント		;///< Scripted event.

MT_MAX		EQU  4		; ミッションタイプMAX		;///< Number of real mission types; the values above are control codes.

;---- 特殊ミッション制御 ----
MT_ATK_NO	EQU  $FB	; ミッション攻撃番号セット		;///< Control: set the mission attack number.
MT_ANM_NO	EQU  $FC	; ミッションアニメ番号セット		;///< Control: set the mission animation number.
MT_BOSSJMP	EQU  $FD	; デモ時の開始ボスミッションにジャンプ		;///< Control: in attract mode, jump to the boss mission.
MT_DEMOJMP	EQU  $FE	; デモ時の開始ミッションにジャンプ		;///< Control: in attract mode, jump to the demo's first mission.
MT_END		EQU  $FF	; ミッション終了		;///< Halt. Also what the cartridge reads as stage-cleared.


;==========================================================
; ミッションサブ　バリエーションタイプ
;==========================================================
MTSV_0		EQU  $00		;///< Mission sub-value 0.
MTSV_1		EQU  $40		;///< Mission sub-value 1.
MTSV_2		EQU  $80		;///< Mission sub-value 2.
MTSV_3		EQU  $C0		;///< Mission sub-value 3.

;==========================================================
; ミッション　アニメ番号(下位２ビットはバリエーション番号）
;==========================================================
MTA_OFF			EQU  $00*4		; アニメオフ		;///< Mission animation: none.


MTA_ASTRO_F		EQU  $80+0*4	; 隕石　右から		;///< Mission animation: meteors from the right.
MTA_ASTRO_B		EQU  $80+1*4	; 隕石　左から		;///< Mission animation: meteors from the left.
MTA_ASTRO_U		EQU  $80+2*4	; 隕石　上から		;///< Mission animation: meteors from above.
MTA_ASTRO_D		EQU  $80+3*4	; 隕石　下から		;///< Mission animation: meteors from below.
MTA_ZAKO_F0		EQU  $80+4*4	; 敵　前から 弱ザコのみ		;///< Mission animation: minions head-on, weak only.
MTA_ZAKO_F1		EQU  $80+5*4	; 敵　前から 硬ザコMIX		;///< Mission animation: minions head-on, mixed with armoured ones.
MTA_ZAKO_F2		EQU  $80+6*4	; 敵　前から ブラックタイガー隊		;///< Mission animation: minions head-on, the Black Tiger squadron.
MTA_WARPIN0		EQU  $80+7*4	; ワープIN 弱ザコ		;///< Mission animation: weak minions warping in.
MTA_WARPIN1		EQU  $80+8*4	; ワープIN ブラックタイガー		;///< Mission animation: Black Tigers warping in.
MTA_WARPIN2		EQU  $80+9*4	; ワープIN 隕石		;///< Mission animation: meteors warping in.


;==========================================================
; ミッション　SP敵攻撃パターン(下位２ビットは攻撃頻度）
;==========================================================
MAA_OFF			EQU  $00*4		; 攻撃オフ		;///< Mission attack: none.
MAA_1SHOT		EQU  $01*4		; １発攻撃		;///< Mission attack: single shot.
MAA_3SHOT		EQU  $02*4		; ３発攻撃		;///< Mission attack: three-way shot.



;----------------------------------------------
; ミッションコントロールコード
;----------------------------------------------
_MC_END			EQU  $00		;///< Opcode: halt the script.
_MC_CALL	 	EQU  $FF	; MC_CALL, 呼び出し先アドレスラベル　　コールは先からコールは不可		;///< Opcode: call another script. A called script may not itself call.
_MC_RET			EQU  $FE		;///< Opcode: return from a call.
_MC_LOOP_CNT	EQU  $FD	; MC_LOOP_CNT, （ループ回数）		;///< Opcode: load the loop counter.
_MC_JMP			EQU  $FC	; MC_JMP, ジャンプ条件, ジャンプ先アドレス		;///< Opcode: conditional jump.
_MC_ZAKO		EQU  $FB	; 雑魚敵セット		;///< Opcode: spawn a minion.
_MC_MEMCPYN		EQU  $FA	; メモリーコピー Nバイト版		;///< Opcode: copy N bytes.
_MC_MEMCPY2		EQU  $F9	; メモリーコピー 2バイト版		;///< Opcode: copy 2 bytes.
_MC_BOSS_NS		EQU  $F8	; ボス通常弾発射		;///< Opcode: boss fires a normal shot.
_MC_PALSET		EQU  $F7	; パレット書き換え		;///< Opcode: rewrite part of the palette.
_MC_MEMSET		EQU  $F6	; メモリーセット 1バイト版		;///< Opcode: store a byte.
_MC_MEMSET2		EQU  $F5	; メモリーセット 2バイト版		;///< Opcode: store a word.
_MC_PGCALL		EQU  $F4	; プログラムコール プログラムを呼び出す		;///< Opcode: call a 6502 subroutine.
_MC_MEMCLR		EQU  $F3	; メモリークリアー		;///< Opcode: clear a region.

_MC_BG_ANIME	EQU  $F2	;  BGにアニメーションデータをセットする		;///< Opcode: install background animation data.
_MC_PGCALL2		EQU  $F1	;  バンク付きプログラムコール プログラムを呼び出す		;///< Opcode: call a 6502 subroutine in a named bank.
;_MC_VRAMSET		EQU  $F0	;  VRAMセット　アドレス、値
_MC_MEMADD		EQU  $EF	;  メモリー加算 1バイト版 アドレス、値		;///< Opcode: add to a byte.
_MC_MEMCMP		EQU  $EE	;  メモリー比較 1バイト版 アドレス、値		;///< Opcode: compare a byte, leaving the result in #MISSON_CMP_P.
_MC_MEMPUSH		EQU  $ED	;  メモリー 1バイト PUSH		;///< Opcode: push a byte onto the mission stack.
_MC_MEMPOP		EQU  $EC	;  メモリー 1バイト POP		;///< Opcode: pop a byte from the mission stack.


_MC_BASE	EQU  $EC		; コマンド予約の開始番号		;///< First reserved opcode number.


;----------------------------------------------
; ミッション拡張関数コード
;----------------------------------------------



;----------------------------------------------
; ミッションコントロールコード　ジャンプ条件
;----------------------------------------------
MCJ_JMP			EQU  0		; 無条件ジャンプ		;///< Jump condition: always.
MCJ_LOOP_CNT	EQU  1		; ループカウンターをマイナス１してゼロでなければジャンプ		;///< Jump condition: decrement the loop counter and jump while non-zero.
							; MC_LOOP_CNTであらかじめループ回数セット

;MCJ_BOSS_HP		EQU  2		; ボスＨＰがボスＨＰ比較データより大きければジャンプ
MCJ_ENEMY_Z		EQU  3		; BG敵の残りがゼロならジャンプ		;///< Jump condition: no background enemies left.
MCJ_EBG_TAG_Z	EQU  4		; 指定タグのＢＧ敵がゼロならジャンプ		;///< Jump condition: no tagged background enemies left.
MCJ_ESP_TAG_Z	EQU  5		; 指定タグのＳＰ敵がゼロならジャンプ		;///< Jump condition: no tagged special enemies left.
MCJ_CMP_Z		EQU  6		; メモリー比較結果がZならジャンプ		;///< Jump condition: last compare was equal.
MCJ_CMP_NZ		EQU  7		; メモリー比較結果がNZならジャンプ		;///< Jump condition: last compare was not equal.
MCJ_CMP_C		EQU  8		; メモリー比較結果がCならジャンプ		;///< Jump condition: last compare set carry.
MCJ_CMP_NC		EQU  9		; メモリー比較結果がNCならジャンプ		;///< Jump condition: last compare cleared carry.


;----------------
; ミッションコントロールマクロ
;----------------

MC_END MACRO		;///< Emits #_MC_END.
	DB	_MC_END
	ENDM

MC_CALL MACRO		;///< Emits #_MC_CALL and a target address.
	DB	_MC_CALL
	DW  \1			; 呼び出し先アドレスラベル
	ENDM

MC_RET MACRO		;///< Emits #_MC_RET.
	DB	_MC_RET
	ENDM

MC_LOOP_CNT MACRO		;///< Emits #_MC_LOOP_CNT and an iteration count.
	DB	_MC_LOOP_CNT
	DB  \1			; ループ回数
	ENDM

MC_JMP MACRO		;///< Emits #_MC_JMP, a target and one condition byte.
	DB	_MC_JMP
	DW  \1			; ジャンプ先アドレスラベル
	DB  \2			; ジャンプ条件
	DB  0
	ENDM

MC_JMP2 MACRO		;///< Emits #_MC_JMP, a target and two condition bytes.
	DB	_MC_JMP
	DW  \1			; ジャンプ先アドレスラベル
	DB  \2			; ジャンプ条件
	DB  \3			; ジャンプ条件
	ENDM



MC_ZAKO MACRO		;///< Emits #_MC_ZAKO: kind, movement pattern, and an X offset halved on the way in.
	DB	_MC_ZAKO
	DB  \1			; 種類
	DB  \2			; 移動パターン（最下位ビット=1 でX軸反転）
	DB  ((\3) /2)		; Ｘオフセット 0-511 (1/2してセットされる）
	DB  \4			; Ｙオフセット -128 から 127　まで
	ENDM


;-------------------------------
;  LBF_ZAKO 用弾発射
;-------------------------------
MC_BOSS_NS MACRO		;///< Emits #_MC_BOSS_NS: background index, target and shot kind.
	DB	_MC_BOSS_NS
	DB  \1			; LASTER_BG IDX
	DB  \2			; ターゲット
	DB  \3			; 弾の種類
	DB  \4			; 移動パターン
	ENDM

MC_BOSS_SS MACRO		;///< Emits a boss special shot: X and Y offsets and a parameter.
	DB	_MC_BOSS_SS
	DB  \1			; Ｘオフセット -128 から 127　まで
	DB  \2			; Ｙオフセット -128 から 127　まで
	DB  \3			; パラメーター
	ENDM

MC_BOSS_HS MACRO		;///< Emits a boss homing shot: X and Y offsets and a shot kind.
	DB	_MC_BOSS_HS
	DB  \1			; Ｘオフセット -128 から 127　まで
	DB  \2			; Ｙオフセット -128 から 127　まで
	DB  \3			; 弾の種類
	DB  \4			; 移動パターン
	ENDM

MC_PALSET MACRO		;///< Emits #_MC_PALSET: a palette slot and three colours.
	DB	_MC_PALSET
	DB  \1			; セット位置 4xN +1 (N= 0～7)
	DB  \2,\3,\4	; パレットデータ
	ENDM

MC_MEMSET MACRO		;///< Emits #_MC_MEMSET: address and byte.
	DB	_MC_MEMSET
	DW  \1			; 書き換えアドレス
	DB  \2			; 書き換えデータ
	ENDM

MC_MEMSET2 MACRO		;///< Emits #_MC_MEMSET2: address and word.
	DB	_MC_MEMSET2
	DW  \1			; 書き換えアドレス
	DW  \2			; 書き換えデータ
	ENDM

MC_MEMSET3B MACRO		;///< Emits a three-byte store.
	DB	_MC_MEMSET2
	DW  \1			; 書き換えアドレス
	DB  \2,\3		; 書き換えデータ
	DB	_MC_MEMSET
	DW  \1+2		; 書き換えアドレス
	DB  \4			; 書き換えデータ
	ENDM

MC_MEMADD MACRO		;///< Emits #_MC_MEMADD: address and addend.
	DB	_MC_MEMADD
	DW  \1			; アドレス
	DB  \2			; 加算データ
	ENDM

MC_MEMCMP MACRO		;///< Emits #_MC_MEMCMP: address and comparand.
	DB	_MC_MEMCMP
	DW  \1			; アドレス
	DB  \2			; 比較データ
	ENDM

MC_MEMCPYN MACRO		;///< Emits #_MC_MEMCPYN: source, destination and length.
	DB	_MC_MEMCPYN
	DW  \2			; アドレス SRC
	DW  \1			; アドレス DST
	DB  \3
	ENDM

MC_MEMCPY2 MACRO		;///< Emits #_MC_MEMCPY2: source and destination.
	DB	_MC_MEMCPY2
	DW  \2			; アドレス SRC
	DW  \1			; アドレス DST
	ENDM


MC_MEMPUSH MACRO		;///< Emits #_MC_MEMPUSH.
	DB	_MC_MEMPUSH
	DW  \1			; PUSH変数アドレス
	ENDM

MC_MEMPOP MACRO		;///< Emits #_MC_MEMPOP.
	DB	_MC_MEMPOP
	DW  \1			; POP変数アドレス
	ENDM


MC_PGCALL MACRO		;///< Emits #_MC_PGCALL and a subroutine address.
	DB	_MC_PGCALL
	DB  #high( \1 -1)	; コールアドレス
	DB  #low( \1 -1)	; コールアドレス
	DB  0			; コール時にYreg にセットする値
	DB  \2			; コール時にAreg にセットする値
	ENDM

MC_PGCALL2 MACRO		;///< Emits #_MC_PGCALL2, a bank and a subroutine address.
	DB	_MC_PGCALL
	DB  #high( \1 -1)	; コールアドレス
	DB  #low( \1 -1)	; コールアドレス
	DB  \3			; コール時にYreg にセットする値
	DB  \2			; コール時にAreg にセットする値
	ENDM

MC_PGCALL_A MACRO		;///< Emits a program call taking the accumulator as an argument.
	DB	_MC_PGCALL2
	DW  \3			; TMP_ADR0にセットする値
	DB  #high( \1 -1)	; コールアドレス
	DB  #low( \1 -1)	; コールアドレス
	DB  0			; コール時にYreg にセットする値
	DB  \2			; コール時にAreg にセットする値
	ENDM




MC_MEMCLR MACRO		;///< Emits #_MC_MEMCLR: address and length.
	DB	_MC_MEMCLR
	DW  \1			; アドレス
	DB  \2			; サイズ
	DB  \3			; クリア値
	ENDM

MC_BG_POS_CLR MACRO		;///< Emits a clear of the background enemy position table.
	DB	_MC_BG_POS_CLR
	DB  \1			; Ｘ
	DB  \2			; Ｙ
	ENDM


MC_BG_TAG_SBG MACRO		;///< Emits a background enemy tag assignment.
	DB	_MC_BG_TAG_SBG
	DB  \1			; タグ番号
	DB  \2			; BG化する敵の種類番号
	ENDM

MC_BG_POS_HS MACRO		;///< Emits a background enemy homing-shot position.
	DB	_MC_BG_POS_HS
	DB  \1			; Ｘ
	DB  \2			; Ｙ
	DB  \3			; 弾種類
	DB  \4			; 移動パターン
	ENDM






MC_WAIT MACRO		;///< Emits a wait: suspends the script for a number of frames via #MISSON_WAIT.
	DB  \1			; ウェイトフレーム数 1-200
	ENDM



;------------------------------------
; ミッション拡張関数
;  \1 ->拡張関数番号
;------------------------------------
MC_MISSION_FUNC MACRO		;///< Emits a call to one of the mission helper routines.
	DB	_MC_PGCALL
	DB  #high( MissionFunc -1)	; コールアドレス
	DB  #low( MissionFunc -1)	; コールアドレス
	DB  \1			; コール時にAreg にセットする値
	ENDM




