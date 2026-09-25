;/// @file cfgEnemyNT.h
;/// @brief Enemy and enemy-shot definitions, and their movement tables.
;/// @ingroup gamerom
;///
;/// The `NTK_*` kinds declared here are the same numbers the C++ side switches on
;/// when it decides whether a table entry should be drawn as a sprite or a model.
;/// @see @ref sample_game
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
	NT_CFG 2,1,1,0,0		; 13 SPザコ0
	NT_CFG 3,2,2,0,0		; 14 SPザコ1
	NT_CFG 4,1,2,2,0		; 15 SPザコ2 ブラックタイガー　ホーミング


