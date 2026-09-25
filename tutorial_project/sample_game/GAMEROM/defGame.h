;/// @file defGame.h
;/// @brief Game-wide constants: PPU flags, key bits, sound slots and object kinds.
;/// @ingroup gamerom
;///
;/// @warning The sound slot numbers here are the same numbering the C++ side
;///          declares in `ap_main.h`, and the key bits are the same bits the
;///          cartridge writes into `KEY_NEW`. Both are duplicated by hand.
;///          @see @ref sample_game

;----------------
; PPU
;----------------
; MMC3のIRQを使う場合は BG を$0000番地, Spr を$1000番地に配置しなければならない
FLG_PPU2000	EQU	%101_01_0_00		;///< PPU control value: NMI on, 8x8 sprites, BG and sprites both at pattern table 0.
				; NMI gen,master,SP8x8,BG$0000,SP$0000,+1,v0,h0


FLG_PPU2001	EQU	%000_11_11_0		;///< PPU mask value: background and sprites enabled, including the leftmost column.


;----------------
; KEY BIT CODE
;----------------
KEY_A		EQU	$80		;///< Controller bit, A.
KEY_B		EQU	$40		;///< Controller bit, B.
KEY_SEL		EQU	$20		;///< Controller bit, Select.
KEY_RUN		EQU	$10		;///< Controller bit, Start.
KEY_UP		EQU	$08		;///< Controller bit, up.
KEY_DOWN	EQU	$04		;///< Controller bit, down.
KEY_LEFT	EQU	$02		;///< Controller bit, left.
KEY_RIGHT	EQU	$01		;///< Controller bit, right.

KEY_AB		EQU	$C0		;///< Mask matching either action button.
KEY_ABRS	EQU	$F0		;///< Mask matching A, B, Select or Start.


;----------------
; KEY DIR
;----------------
KDIR_N		EQU $FF		;///< Direction: neutral.
KDIR_U		EQU $00		;///< Direction: up. The eight directions run clockwise from here.
KDIR_UR		EQU $01		;///< Direction: up-right.
KDIR_R		EQU $02		;///< Direction: right.
KDIR_DR		EQU $03		;///< Direction: down-right.
KDIR_D		EQU $04		;///< Direction: down.
KDIR_DL		EQU $05		;///< Direction: down-left.
KDIR_L		EQU $06		;///< Direction: left.
KDIR_UL		EQU $07		;///< Direction: up-left.


;----------------
; キーリピート設定
;----------------
REP_WAIT	EQU	24	; リピート開始までの時間 (フレーム数)		;///< Frames held before auto-repeat starts.
REP_INTERVAL	EQU	 8	; リピート間隔 (フレーム数)		;///< Frames between auto-repeat presses.


;----------------
; バンク定義
;----------------
PBNK_SYS	EQU  $00		;///< PRG bank holding the system code.



;----------------
; 各種P定義
;----------------
SP_CLR_Y	EQU 240		; スプライトクリアーY		;///< Y value that parks a sprite off-screen.

;----------------
; STEP定義
;----------------
ST_INIT		EQU	 0	; 初期化		;///< Console-side screen code: initialise.
ST_EXA00	EQU	 1	; 拡張システム起動チェック		;///< Console-side screen code: probe for the expansion adapter.
ST_TITLE	EQU	 2		;///< Console-side screen code: title.
ST_OPTION	EQU	 3		;///< Console-side screen code: options.
ST_DEBUG	EQU	 4		;///< Console-side screen code: debug menu.
ST_MAIN		EQU	 5		;///< Console-side screen code: play. The only one the cartridge sets, in `FCP_GAME_INIT`.
ST_CLEAR	EQU	 6		;///< Console-side screen code: stage clear.
ST_OVER		EQU	 7		;///< Console-side screen code: game over.
ST_LICENSE	EQU	 8	; ライセンス		;///< Console-side screen code: licence.

ST_MAX		EQU	 9	; ステップの最大値		;///< Number of console-side screen codes.



;==========================================================
; デモタイマー関連定義
;==========================================================
TITLE_DEMO_TM	EQU	(7*60/16)	; 約7秒		;///< Title idle time before attract mode, about 7 seconds. Counted in units of 16 frames.
GAME_DEMO_TM	EQU	(15*60/16)	; 約15秒		;///< Attract-mode play length, about 15 seconds.
CREDIT_DEMO_TM	EQU	(5*60/16)	; 約5秒		;///< Credit screen length, about 5 seconds.



;==========================================================
; プレーヤーアニメ制御関連定義
;==========================================================
PLY_AN_WAIT		EQU  0		; 待機		;///< Player animation: idle.
PLY_AN_CHARGE	EQU  1		; チャージ		;///< Player animation: charging.
PLY_AN_SHOTA	EQU  2		; ショットA		;///< Player animation: shot A.
PLY_AN_SHOTB	EQU  3		; ショットB		;///< Player animation: shot B.
PLY_AN_DEAD		EQU  4		; 死亡アニメ		;///< Player animation: dying. The cartridge watches #PLY_ANM_NO for this value.



;----------------
; サウンド定義
;----------------
BGM_BOSS	EQU  1	;		;///< Music slot: boss.
BGM_STAGE	EQU  2	;		;///< Music slot: stage. The C++ side calls the same slot `BGM_MAIN`. @see @ref sample_game
BGM_CLEAR	EQU  3	;		;///< Music slot: stage clear.
BGM_OVER	EQU  4	;		;///< Music slot: game over.

SE_TOP_NO	EQU  5		;///< First sound-effect slot. Everything below it is music.

SE_CUR_SEL		EQU  (SE_TOP_NO+0)	; 00 カーソル 移動		;///< Menu cursor moved.
SE_CUR_ENT		EQU  (SE_TOP_NO+1)	; 01 カーソル 決定		;///< Menu item confirmed.
SE_CUR_CAN		EQU  (SE_TOP_NO+2)	; 02 カーソル キャンセル　(オプション 使用)		;///< Menu cancelled.
SE_SHOT_A		EQU  (SE_TOP_NO+3)	; 03 自機ショット音		;///< Player shot.
SE_PLY_DAME		EQU  (SE_TOP_NO+4)	; 04 自機ダメージ音		;///< Player hit.
SE_PLY_DEAD		EQU  (SE_TOP_NO+5)	; 05 自機死亡		;///< Player destroyed.
SE_PLY_FORM		EQU  (SE_TOP_NO+6)	; 06 自機フォーメーションチェンジ		;///< Formation change.

SE_BAKU_S		EQU  (SE_TOP_NO+7)	; 07 敵 撃破 敵サイズ小 ザコ		;///< Small enemy destroyed.
SE_BAKU_M		EQU  (SE_TOP_NO+8)	; 08 敵 撃破 敵サイズ中 ザコ		;///< Medium enemy destroyed.
SE_BAKU_L		EQU  (SE_TOP_NO+9)	; 09 敵 撃破 敵サイズ大 ボス		;///< Boss destroyed.

SE_NO_DAME		EQU  (SE_TOP_NO+10)	; 10 敵無敵音		;///< Hit an invulnerable enemy.
SE_DAME			EQU  (SE_TOP_NO+11)	; 11 ダメージ受け音		;///< Enemy took damage.

SE_BOSS_MOVE1	EQU  (SE_TOP_NO+12)	; 12 ボス移動1		;///< Boss movement, variant 1.
SE_BOSS_MOVE2	EQU  (SE_TOP_NO+13)	; 13 ボス移動2		;///< Boss movement, variant 2.

SE_BOSS_ATK1	EQU  (SE_TOP_NO+14)	; 14 ボス攻撃		;///< Boss attack.
SE_TITLE		EQU  (SE_TOP_NO+15)	; 15 タイトルＳＥ		;///< Title-screen sting.
SE_START_JET	EQU  (SE_TOP_NO+16)	; 16 スタートジェット		;///< Launch thruster.
SE_HADOU_CHG	EQU  (SE_TOP_NO+17)	; 17 波動砲　チャージ		;///< Wave cannon charging.
SE_HADOU_SHT	EQU  (SE_TOP_NO+18)	; 18 波動砲　発射		;///< Wave cannon fired.
SE_DM_DIVE		EQU  (SE_TOP_NO+19)	; 19 次元潜航		;///< Dimensional dive.
SE_YAMATO_S		EQU  (SE_TOP_NO+20)	; 20 ヤマト発進		;///< Launch sequence.

SNDTST_MAX   EQU (SE_TOP_NO+21)		;///< One past the last sound slot; the sound test counts to it.


SE_TITLE_START	EQU  SE_CUR_ENT		;///< Alias for #SE_CUR_ENT, used when starting a game.
SE_POWUP		EQU  SE_CUR_ENT		;///< Alias for #SE_CUR_ENT, used for a power-up.
SE_BAKU_EFC		EQU  SE_BAKU_S		;///< Alias for #SE_BAKU_S.
SE_BAKU_BG		EQU  SE_BAKU_M		;///< Alias for #SE_BAKU_M.

SE_SPECIAL	EQU	 SE_PLY_FORM	;		;///< Alias for #SE_PLY_FORM.

BGM_GAME_CLEAR  EQU  BGM_CLEAR		;///< Alias for #BGM_CLEAR.




