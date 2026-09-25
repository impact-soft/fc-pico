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
; サブル－チン呼び出しパラメーター
;----------------
PRM_0		EQU $00		;///< Subroutine parameter 0.
PRM_1		EQU $01		;///< Subroutine parameter 1.
PRM_2		EQU $02		;///< Subroutine parameter 2.

MISSON_ANM_NO	EQU  $03	; B ミッションアニメ番号		;///< Mission animation number.

ENEMY_ATK_LV	EQU  $04	; B 敵の攻撃LV		;///< Enemy attack level: 0 none, 1 aimed, 2 homing, 3 both, 4 fast homing.
							;  0:攻撃しない 1:自機狙い弾 2:ホーミング弾
							;  3:自機狙い＆ホーミング 4: 高速ホーミング


	.list			; リスティングファイル出力
	.mlist			; リスティングファイル上でマクロを展開

        .inesprg 2		; プログラムバンク数
        .ineschr 1		; CHR バンク数
        .inesmir 1		; 0:V 垂直２画面（ 水平ミラー） 1:H 水平２画面（垂直ミラー）
        .inesmap 0		; mapper #0

	.INCLUDE	"defMission.h"

ENEMY_LINE_SUU	EQU		240-24		; 敵BG表示エリアライン数		;///< Playfield height in scanlines. Declared again on the C++ side. @see @ref sample_game

;----------------
; サウンド定義
;----------------
BGM_BOSS	EQU  1	;		;///< Music slot: boss.
BGM_STAGE	EQU  2	;		;///< Music slot: stage. The C++ side calls the same slot `BGM_MAIN`. @see @ref sample_game
BGM_CLEAR	EQU  3	;		;///< Music slot: stage clear.
BGM_OVER	EQU  4	;		;///< Music slot: game over.


;----------------
; PG_CALL
;----------------
SET_FADE_IN_B	EQU $F000
PLAY_BGM		EQU $F001
;setEnemyNT3		EQU $F002
;getEnemyNTcfg	EQU $F003
_STAGE_INIT		EQU $F004
selSPE_MP		EQU $F005
setSPE			EQU $F006
_STAGE_INIT2	EQU $F007
_STAGE_INIT3	EQU $F008




	.code

	;========================================
	; ゲームバンク0
	;========================================
	.BANK		0
	ORG  $0000





