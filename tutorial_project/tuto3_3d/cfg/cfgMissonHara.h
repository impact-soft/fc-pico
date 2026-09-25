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


;===============================================================================================

	.INCLUDE	"defCommon.h"



;




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
	dw  HaraProg11
	dw  HaraProg12
	dw  HaraProg13
	dw  HaraProg14
	dw  HaraProg15
	dw  HaraProg16
	dw  HaraProg17
	dw  HaraProg18
	dw  HaraProg19
	dw  HaraProg1A
	dw  HaraProg1B
	dw  HaraProg1C
	dw  HaraProg1D
	dw  HaraProg1E
	dw  HaraProg1F




	.INCLUDE	".\cfgMissonSub.h"



HaraProg00	; ステージ開始
	MC_PGCALL SET_FADE_IN_B,0
	MC_PGCALL PLAY_BGM, BGM_STAGE	; ステージBGM再生
	MC_PGCALL_A _STAGE_INIT,0,.tblSPE_MP			; ステージ初期化
	MC_PGCALL_A _STAGE_INIT2,0,tblEnemyNTcfg		; ステージ初期化
	MC_PGCALL_A _STAGE_INIT3,0,misson_anmtblData	; ステージ初期化
	MC_WAIT 60
	MC_END	; データエンド

;--------------------------------
;	敵移動パターンテーブル
;
;	MC_PGCALL selSPE_MP,(teble no)
;	上記命令で指定する移動パターンテーブル
;	cfgStage.hのMTSV_? の0-3がテーブル内の0-3の選択に使われる
;
;--------------------------------
.tblSPE_MP

	;------------------------
	; 上から攻撃用
	;------------------------
	; table 0
	db $40*1+$10,$40*2+$10,$40*2+$10-4,$40*2+$10+3	; 0
	; table 1
	db $40*1+$10,$40*2+$10,$40*2+$10+4,$40*2+$10-3	; 1

	; 下から攻撃用
	; table 2
	db $40*1+$30,$40*2+$30,$40*2+$30-4,$30*2+$10+3	; 2
	; table 3
	db $40*1+$30,$40*2+$30,$40*2+$30+4,$30*2+$10-3	; 3

	; 斜め上左右から攻撃用
	; table 4
	db $40*1+$10-8,$40*2+$10-8,$40*2+$10-12,$40*2+$10-4	; 4
	; table 5
	db $40*1+$10+8,$40*2+$10+8,$40*2+$10+12,$40*2+$10+4	; 5

	; 斜め下左右から攻撃用
	; table 6
	db $40*1+$30+8,$40*2+$30+8,$40*2+$30+12,$40*2+$30+4	; 6
	; table 7
	db $40*1+$30-8,$40*2+$30-8,$40*2+$30-12,$40*2+$30-4	; 7

	; ワープイン用
	; table 8
	db NTK_SPZK0,NTK_SPZK1,NTK_SPZK2,NTK_METEO	; 8



;===============================================================================================
; 敵ノーマル弾移動データテーブル
;
;===============================================================================================

NTK_ANGLE	EQU		1	; 自機狙い弾		;///< Aimed shot.
NTK_NOMAL	EQU		2	; 通常弾		;///< Plain shot.
NTK_HORMI	EQU		3	; ホーミング弾		;///< Homing shot.
NTK_MISS	EQU		4	; ミサイル弾		;///< Missile.
NTK_HHORM	EQU		6	; 半誘導弾		;///< Partially homing shot.
NTK_HHORM3	EQU		8	; 半誘導弾３分裂		;///< Partially homing shot that splits into three.
NTK_METEO	EQU		$80+11	; 隕石		;///< Meteor. Top bit set, so the cartridge draws it as a model rather than a sprite.
NTK_WARP	EQU		$80+12	; ワープエフェクト		;///< Warp-in effect.
NTK_SPZK0	EQU		$80+13	; SPザコ0		;///< Special minion 0.
NTK_SPZK1	EQU		$80+14	; SPザコ1		;///< Special minion 1.
NTK_SPZK2	EQU		$80+15	; SPザコ2		;///< Special minion 2.
NTK_LIFE	EQU		$80+16	; ライフ回復		;///< Extra-life pickup.


; データフォーマット
; 0:  見た目番号ｘ４＋パレット選択
; 1:  HP  0->当たり判定なし 255->無敵
; 2:  拡張番号
; 3:  拡張パラメータ:弾種切り替えタイマー

; 拡張番号メモ
; 0:特殊処理なし
; 1:自機方向に方向転換
; 2:ホーミング
; 3:ミサイル上昇 X軸によって方向補正　画面外消去特殊処理
; 4:ミサイル下降 X軸によって方向補正
; 5:進行方向に向けて３分裂
; 6:自己消滅
; 7:６方向炸裂
; 8:近接信管
;




NT_CFG MACRO		;///< Emits one row of #tblEnemyNTcfg.
	db \1 * 4 + \2	; 見た目＋パレット
	db \3			; HP (=0当たり判定なし）
	db \4			; 拡張番号
	db \5			; 拡張パラメータ

	ENDM


;  最大64種類登録可能
;/// @brief Enemy configuration rows, up to 64 kinds.
;/// @ingroup gamerom
tblEnemyNTcfg:
	NT_CFG 2,2,0,1,0		; 1 自機狙い弾 発射時
	NT_CFG 2,2,0,0,0		; 2 自機狙い弾 移動中(通常弾)
	NT_CFG 1,2,1,2,0		; 3 ホーミング
	NT_CFG 3,2,8,3,90		; 4 ミサイル上昇
	NT_CFG 3,2,4,4,0		; 5 ミサイル下降

	NT_CFG 0,2,4,2,0		; 6 半誘導弾
	NT_CFG 0,2,4,2,0		; 7

	NT_CFG 2,2,0,2,16		; 8 半誘導弾３分裂
	NT_CFG 2,2,0,5,1		; 9
	NT_CFG 2,2,0,0,0		; 10

	NT_CFG 0,0,255,0,0		; 11 隕石
	NT_CFG 1,3,254,0,30		; 12 ワープINエフェクト
	NT_CFG 2,1,3,0,0		; 13 SPザコ0
	NT_CFG 3,2,5,0,0		; 14 SPザコ1
	NT_CFG 4,1,8,2,0		; 15 SPザコ2 ブラックタイガー　ホーミング



misson_anmtblData:
	; X,Y,弾の種類,弾の移動パターン
	; 右から隕石
	db  250,-1,NTK_METEO,$40*1+$20		;0
	db  250,-1,NTK_METEO,$40*1+$20-1	;1
	db  250,-1,NTK_METEO,$40*2+$20+1	;2
	db  250,-1,NTK_METEO,$40*0+$20-2  	;3

	; 左から隕石
	db  6,-1,NTK_METEO,$40*1+$00	;4
	db  6,-1,NTK_METEO,$40*2+$00-1	;5
	db  6,-1,NTK_METEO,$40*0+$00+1	;6
	db  6,-1,NTK_METEO,$40*1+$40-2	;7

	; 上から隕石
	db  -1,6,NTK_METEO,$40*1+$10	;8
	db  -1,6,NTK_METEO,$40*1+$10-1	;9
	db  -1,6,NTK_METEO,$40*2+$10+1	;10
	db  -1,6,NTK_METEO,$40*0+$10	;11

	; 下から隕石
	db  -1,ENEMY_LINE_SUU-6,NTK_METEO,$40*1+$30		;12
	db  -1,ENEMY_LINE_SUU-6,NTK_METEO,$40*1+$30-1	;13
	db  -1,ENEMY_LINE_SUU-6,NTK_METEO,$40*2+$30+1	;14
	db  -1,ENEMY_LINE_SUU-6,NTK_METEO,$40*0+$30-2	;15

	; 上からザコ 弱いザコのみ
	db  -1, 6,NTK_SPZK0,$40*2+$10+0	;16
	db  -1, 6,NTK_SPZK0,$40*3+$10+0	;17
	db  -1, 6,NTK_SPZK0,$40*1+$10-0	;18
	db  -1, 6,NTK_SPZK0,$40*2+$10+0	;19

	; 上からザコ 硬いザコミックス
	db  -1, 6,NTK_SPZK0,$40*3+$10+0	;20
	db  -1, 6,NTK_SPZK0,$40*2+$10+0	;21
	db  -1, 6,NTK_SPZK0,$40*3+$10-0	;22
	db  -1, 6,NTK_SPZK1,$40*1+$10+0	;23

	; 前からザコ ブラックタイガー隊
	db  -1, 6,NTK_SPZK2,$40*3+$10+0	;24
	db  -1, 6,NTK_SPZK2,$40*3+$10+0	;25
	db  -1, 6,NTK_SPZK2,$40*3+$10-0	;26
	db  -1, 6,NTK_SPZK2,$40*3+$10+0	;27

	; ワープイン 弱ザコ
	db  -1,-1,NTK_WARP,NTK_SPZK0		;28
	db  -1,-1,NTK_WARP,NTK_SPZK0		;29
	db  -1,-1,NTK_WARP,NTK_SPZK0		;30
	db  -1,-1,NTK_WARP,NTK_SPZK0		;31

	; ワープイン 弱ザコ
	db  -1,-1,NTK_WARP,NTK_SPZK2		;32
	db  -1,-1,NTK_WARP,NTK_SPZK2		;33
	db  -1,-1,NTK_WARP,NTK_SPZK2		;34
	db  -1,-1,NTK_WARP,NTK_SPZK2		;35

	; ワープイン 隕石
	db  -1,-1,NTK_WARP,NTK_METEO		;36
	db  -1,-1,NTK_WARP,NTK_METEO		;37
	db  -1,-1,NTK_WARP,NTK_METEO		;38
	db  -1,-1,NTK_WARP,NTK_METEO		;39



;=======================================================================

; ボス登場前処理
HaraProg01
	MC_CALL waitClearSPE2
	MC_PGCALL PLAY_BGM, BGM_BOSS	; BOSS BGM再生
	MC_END	; データエンド

;=======================================================================

;-------------------------------
; 上から攻撃　NTK_SPZK0
;-------------------------------
HaraProg02:
	MC_MEMSET PRM_1, NTK_SPZK0
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


;=======================================================================
;-------------------------------
; 上から攻撃　NTK_SPZK1
;-------------------------------
;/// @brief Script: attack from above with #NTK_SPZK1.
;/// @ingroup gamerom
HaraProg03:
	MC_MEMSET PRM_1, NTK_SPZK1
	MC_JMP HaraProg02_0, MCJ_JMP

;=======================================================================
;-------------------------------
; 上から攻撃　NTK_SPZK2
;-------------------------------
;/// @brief Script: attack from above with #NTK_SPZK2.
;/// @ingroup gamerom
HaraProg04:
	MC_MEMSET PRM_1, NTK_SPZK2
	MC_JMP HaraProg02_0, MCJ_JMP


;=======================================================================
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

;=======================================================================
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




;=======================================================================
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
