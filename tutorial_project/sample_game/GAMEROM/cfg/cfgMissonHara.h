;/// @file cfgMissonHara.h
;/// @brief Mission scripts for the ordinary waves, written in `MC_*` bytecode.
;/// @ingroup gamerom
;///
;/// Shared by all stages. The header comment lists the control opcodes and their
;/// operands; note the restriction it records, that a called script may not itself
;/// call.
;===============================================================================================
;	ミッションコントロールテーブル（全ステージ共通）
;
;MC_END			
;MC_CALL	 		; MC_CALL, 呼び出し先アドレスラベル　　コールは先からコールは不可
;MC_RET			
;MC_LOOP_CNT		; MC_LOOP_CNT, （ループ回数）
;MC_JMP				; MC_JMP, ジャンプ条件, ジャンプ先アドレス
;MC_ZAKO			; 雑魚敵セット
;MC_PALSET			; パレット書き換え
;MC_MEMSET			; メモリーセット 1バイト版
;MC_MEMSET2			; メモリーセット 2バイト版
;MC_PGCALL			; プログラムコール プログラムを呼び出す
;
;MC_BG_ATR			;  BGアトリビュート書換え　アドレス、値
;MC_MEMADD			;  メモリー加算 1バイト版 アドレス、値
;MC_MEMCMP			;  メモリー比較 1バイト版 アドレス、値


;===============================================================================================

;

ZAKO_MOVE_DT MACRO		;///< Emits a minion movement-data row.
	DB	\1			; フレーム数(1-200)
	DW	( \2 ) * $100 / \1	; 移動量 X
	DW	( \3 ) * $100 / \1	; 移動量 Y
	ENDM



;ZFY_BASE = $2C00
ZFY_BASE = %010_00_00000_00000		;///< Base nametable address for minion spawn positions, as a packed PPU address.

;  ミッションコントロール：最大64種類登録可能

;/// @brief Script address for each HARADIUS sub-type, up to 64 of them.
;/// @ingroup gamerom
misson_hara_control_tbl:

	dw  HaraProg00
	dw  HaraProg01
	dw  HaraProg02
	dw  HaraProg03
	dw  HaraProg04
	dw  HaraProg05
	dw  HaraProg06
	dw  HaraProg07
	dw  HaraProg08
	dw  HaraProg09
	dw  HaraProg0A
	dw  HaraProg0B
	dw  HaraProg0C
	dw  HaraProg0D
	dw  HaraProg0E
	dw  HaraProg0F

	dw  HaraProg10


HaraProg00	; ステージ開始
	MC_PGCALL SET_FADE_IN_B,0
	MC_PGCALL playStageBGM,0	; ステージBGM再生
	MC_PGCALL .stege_init,0		; ステージ初期化
	MC_WAIT 60
	MC_END	; データエンド


.stege_init
	ldx  PLY_STAGE
	lda  .tbl_SECRET_LIFE_ADD,x
	sta  SECRET_LIFE_ADD
	lda  #0
	sta  SECRET_STAT
	sta  MISSON_ANM_NO
	rts

.tbl_SECRET_LIFE_ADD
	db 1				;0
	db ADD_SC_LIFE_ST1	;1
	db ADD_SC_LIFE_ST2	;2
	db ADD_SC_LIFE_ST3	;3
	db ADD_SC_LIFE_ST4	;4
	db ADD_SC_LIFE_ST5	;5
	db 1				;6
	db 1				;7
	db 1				;8


;=====================================
; ステージBGM再生
;=====================================
;/// @brief Mission subroutine: start the stage music.
;/// @ingroup gamerom
playStageBGM:
	lda  #BGM_STAGE
	jmp  PLAY_BGM



HaraProg01
	MC_CALL waitClearSPE2
	MC_PGCALL playBossBGM,0	; ステージBGM再生
	MC_END	; データエンド

;/// @brief Mission subroutine: start the boss music.
;/// @ingroup gamerom
playBossBGM:
	lda  #BGM_BOSS	; 1
	jmp  PLAY_BGM


;-------------------------------
; 上から攻撃　NTK_SPZK0
;-------------------------------
;/// @brief Script: attack from above with #NTK_SPZK0.
;/// @ingroup gamerom
HaraProg02:
	MC_MEMSET PRM_1, NTK_SPZK0
;/// @brief Body of #HaraProg02.
;/// @ingroup gamerom
HaraProg02_0:
	MC_MEMPUSH MISSON_ANM_NO
	MC_CALL waitClearSPE2


	MC_LOOP_CNT 5
.p11_loop
	MC_PGCALL selSPE_MP,0
	MC_WAIT 8
	MC_PGCALL2 setSPE,128-50,8
	MC_WAIT 8
	MC_PGCALL2 setSPE,128-50,8
	MC_WAIT 8
	MC_PGCALL2 setSPE,128-50,8

	MC_WAIT 16


	MC_PGCALL selSPE_MP,1

	MC_WAIT 8
	MC_PGCALL2 setSPE,128+50,8
	MC_WAIT 8
	MC_PGCALL2 setSPE,128+50,8
	MC_WAIT 8
	MC_PGCALL2 setSPE,128+50,8

	MC_JMP .p11_loop, MCJ_LOOP_CNT

	MC_WAIT 16

	MC_MEMPOP MISSON_ANM_NO
	MC_END	; データエンド


;-------------------------------
; 上から攻撃　NTK_SPZK1
;-------------------------------
;/// @brief Script: attack from above with #NTK_SPZK1.
;/// @ingroup gamerom
HaraProg03:
	MC_MEMSET PRM_1, NTK_SPZK1
	MC_JMP HaraProg02_0, MCJ_JMP

;-------------------------------
; 上から攻撃　NTK_SPZK2
;-------------------------------
;/// @brief Script: attack from above with #NTK_SPZK2.
;/// @ingroup gamerom
HaraProg04:
	MC_MEMSET PRM_1, NTK_SPZK2
	MC_JMP HaraProg02_0, MCJ_JMP


;-------------------------------
; ウェイト：ミッションアニメのみで使用
;-------------------------------
HaraProg05
	MC_MEMCMP  ENEMY_ATK_LV, MTSV_0
	MC_JMP .atk00, MCJ_CMP_Z
	MC_MEMCMP  ENEMY_ATK_LV, MTSV_1
	MC_JMP .atk01, MCJ_CMP_Z
	MC_MEMCMP  ENEMY_ATK_LV, MTSV_2
	MC_JMP .atk02, MCJ_CMP_Z

	MC_WAIT 120
	MC_WAIT 120
.atk02
	MC_WAIT 120
	MC_WAIT 120
.atk01
	MC_WAIT 120
	MC_WAIT 120
.atk00
	MC_WAIT 120
	MC_WAIT 120

	MC_END	; データエンド

;-------------------------------
; 下から攻撃　NTK_SPZK0
;-------------------------------
HaraProg06
	MC_MEMSET PRM_1, NTK_SPZK0

;/// @brief Script body, sub-type 6.
;/// @ingroup gamerom
HaraProg06_0:
	MC_MEMPUSH MISSON_ANM_NO
	MC_CALL waitClearSPE2


	MC_LOOP_CNT 5
.p11_loop
	MC_PGCALL selSPE_MP,2
	MC_WAIT 8
	MC_PGCALL2 setSPE,128-50,ENEMY_LINE_SUU-6
	MC_WAIT 8
	MC_PGCALL2 setSPE,128-50,ENEMY_LINE_SUU-6
	MC_WAIT 8
	MC_PGCALL2 setSPE,128-50,ENEMY_LINE_SUU-6

	MC_WAIT 16


	MC_PGCALL selSPE_MP,3

	MC_WAIT 8
	MC_PGCALL2 setSPE,128+50,ENEMY_LINE_SUU-6
	MC_WAIT 8
	MC_PGCALL2 setSPE,128+50,ENEMY_LINE_SUU-6
	MC_WAIT 8
	MC_PGCALL2 setSPE,128+50,ENEMY_LINE_SUU-6

	MC_JMP .p11_loop, MCJ_LOOP_CNT

	MC_WAIT 16

	MC_MEMPOP MISSON_ANM_NO
	MC_END	; データエンド




;-------------------------------
; 下から攻撃　NTK_SPZK1
;-------------------------------
HaraProg07
	MC_MEMSET PRM_1, NTK_SPZK1
	MC_JMP HaraProg06_0, MCJ_JMP

;-------------------------------
; 下から攻撃　NTK_SPZK2
;-------------------------------
HaraProg08
	MC_MEMSET PRM_1, NTK_SPZK2
	MC_JMP HaraProg06_0, MCJ_JMP




;-------------------------------
; 斜め上左右から攻撃用　NTK_SPZK0
;-------------------------------
HaraProg09
	MC_MEMSET PRM_1, NTK_SPZK0

;/// @brief Script body, sub-type 9.
;/// @ingroup gamerom
HaraProg09_0:
	MC_MEMPUSH MISSON_ANM_NO
	MC_CALL waitClearSPE2


	MC_LOOP_CNT 5
.p11_loop
	MC_PGCALL selSPE_MP,4
	MC_WAIT 8
	MC_PGCALL2 setSPE,6,8+32
	MC_WAIT 8
	MC_PGCALL2 setSPE,6,8+16
	MC_WAIT 8
	MC_PGCALL2 setSPE,6,8

	MC_WAIT 16


	MC_PGCALL selSPE_MP,5

	MC_WAIT 8
	MC_PGCALL2 setSPE,250,8+32
	MC_WAIT 8
	MC_PGCALL2 setSPE,250,8+16
	MC_WAIT 8
	MC_PGCALL2 setSPE,250,8

	MC_JMP .p11_loop, MCJ_LOOP_CNT

	MC_WAIT 16

	MC_MEMPOP MISSON_ANM_NO
	MC_END	; データエンド



;-------------------------------
; 斜め上左右から攻撃用　NTK_SPZK1
;-------------------------------
HaraProg0A
	MC_MEMSET PRM_1, NTK_SPZK1
	MC_JMP HaraProg09_0, MCJ_JMP

;-------------------------------
; 斜め上左右から攻撃用　NTK_SPZK2
;-------------------------------
HaraProg0B
	MC_MEMSET PRM_1, NTK_SPZK2
	MC_JMP HaraProg09_0, MCJ_JMP



;-------------------------------
; 斜め下左右から攻撃用　NTK_SPZK0
;-------------------------------
HaraProg0C
	MC_MEMSET PRM_1, NTK_SPZK0

;/// @brief Script body, sub-type 12.
;/// @ingroup gamerom
HaraProg0C_0:
	MC_MEMPUSH MISSON_ANM_NO
	MC_CALL waitClearSPE2


	MC_LOOP_CNT 5
.p11_loop
	MC_PGCALL selSPE_MP,6
	MC_WAIT 8
	MC_PGCALL2 setSPE,6,ENEMY_LINE_SUU-6-32
	MC_WAIT 8
	MC_PGCALL2 setSPE,6,ENEMY_LINE_SUU-6-16
	MC_WAIT 8
	MC_PGCALL2 setSPE,6,ENEMY_LINE_SUU-6

	MC_WAIT 16


	MC_PGCALL selSPE_MP,7

	MC_WAIT 8
	MC_PGCALL2 setSPE,250,ENEMY_LINE_SUU-6-32
	MC_WAIT 8
	MC_PGCALL2 setSPE,250,ENEMY_LINE_SUU-6-16
	MC_WAIT 8
	MC_PGCALL2 setSPE,250,ENEMY_LINE_SUU-6

	MC_JMP .p11_loop, MCJ_LOOP_CNT

	MC_WAIT 16

	MC_MEMPOP MISSON_ANM_NO
	MC_END	; データエンド

;-------------------------------
; 斜め下左右から攻撃用　NTK_SPZK1
;-------------------------------
HaraProg0D
	MC_MEMSET PRM_1, NTK_SPZK1
	MC_JMP HaraProg0C_0, MCJ_JMP

;-------------------------------
; 斜め下左右から攻撃用　NTK_SPZK2
;-------------------------------
HaraProg0E
	MC_MEMSET PRM_1, NTK_SPZK2
	MC_JMP HaraProg0C_0, MCJ_JMP


;-------------------------------
; ワープイン　／＼
;-------------------------------
HaraProg0F
	MC_MEMSET PRM_1, NTK_WARP
	MC_MEMPUSH MISSON_ANM_NO

	MC_PGCALL selSPE_MP,8
	MC_CALL waitClearSPE2

	MC_LOOP_CNT 10
.p11_loop
	MC_PGCALL2 setSPE,128-15-30*0,20
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*0,20
	MC_WAIT 5

	MC_PGCALL2 setSPE,128-15-30*1,20+30
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*1,20+30

	MC_WAIT 5

	MC_PGCALL2 setSPE,128-15-30*2,20+60
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*2,20+60

	MC_WAIT 5

	MC_PGCALL2 setSPE,128-15-30*3,20+90
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*3,20+90

	MC_WAIT 5

	MC_JMP .p11_loop, MCJ_LOOP_CNT

	MC_CALL waitClearSPE2

	MC_MEMPOP MISSON_ANM_NO
	MC_END	; データエンド

;-------------------------------
; ワープイン　＼／
;-------------------------------
HaraProg10
	MC_MEMSET PRM_1, NTK_WARP
	MC_MEMPUSH MISSON_ANM_NO

	MC_PGCALL selSPE_MP,8
	MC_CALL waitClearSPE2

	MC_LOOP_CNT 10
.p11_loop
	MC_PGCALL2 setSPE,128-15-30*0,ENEMY_LINE_SUU -20
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*0,ENEMY_LINE_SUU -20
	MC_WAIT 5

	MC_PGCALL2 setSPE,128-15-30*1,ENEMY_LINE_SUU -20-30
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*1,ENEMY_LINE_SUU -20-30

	MC_WAIT 5

	MC_PGCALL2 setSPE,128-15-30*2,ENEMY_LINE_SUU-20-60
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*2,ENEMY_LINE_SUU-20-60

	MC_WAIT 5

	MC_PGCALL2 setSPE,128-15-30*3,ENEMY_LINE_SUU-20-90
	MC_WAIT 5
	MC_PGCALL2 setSPE,128+15+30*3,ENEMY_LINE_SUU-20-90

	MC_WAIT 5

	MC_JMP .p11_loop, MCJ_LOOP_CNT

	MC_CALL waitClearSPE2

	MC_MEMPOP MISSON_ANM_NO
	MC_END	; データエンド


HaraProg11

HaraProg12

HaraProg13

HaraProg14


HaraProg15


HaraProg16


HaraProg17

HaraProg18

HaraProg19


HaraProg1A




HaraProg1B

HaraProg1C

HaraProg1D



HaraProg1E

HaraProg1F
	MC_END	; データエンド
