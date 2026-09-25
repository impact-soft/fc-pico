;/// @file cfgStage.h
;/// @brief The stage tables: which missions run, and in what order.
;/// @ingroup gamerom
;///
;/// One table per stage, each a list of mission types and sub-types. The sub-type
;/// byte is overloaded: its top two bits set the enemy attack level, and the
;/// bottom six index the script table for that mission type.
;===============================================================================================
;	ステージのミッション定義テーブル
;
;  各ステージのミッションの進行順を定義する
;  MT_??? でミッションタイプを指定して、その後のデータでそのミッションタイプのサブタイプを指定する
;  サブタイプ部分の指定可能数値および仕様は各ミッションタイプにより異なる
;  ※サブタイプの詳細はあとでまとめます。
;===============================================================================================
MN_BOSS_BGM = $00		;///< Mission-list marker selecting the boss music.

;/// @brief Mission list for each stage, in play order.
;/// @ingroup gamerom
mission_prog_tbl:
	dw	0
	dw  stage1
	dw  stage2
	dw  stage3

;/// @brief Mission list for the first attract-mode run.
;/// @ingroup gamerom
mission_prog_tbl_demo1:
	dw	0
	dw  stage1_demo
	dw  stage2_demo
	dw  stage3_demo


;/// @brief Mission list for the second attract-mode run.
;/// @ingroup gamerom
mission_prog_tbl_demo2:
	dw	0
	dw  stage1_boss
	dw  stage2_boss
	dw  stage3_boss


;--------------------------------------------
; ステージ1
;--------------------------------------------
stage1
	db MT_HARA,$00	; ステージ開始

	db MT_ANM_NO, MTA_OFF
	db MT_ATK_NO, MTA_OFF

	db MT_HARA,MTSV_0 + $02	; 上から攻撃　NTK_SPZK0

;	db MT_END,0		; ミッション終了

	db MT_ANM_NO, MTA_ZAKO_F0+0	; ザコ前弱LV0アニメスタート
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_ATK_NO, MAA_1SHOT+0	; １発攻撃 LV0
	db MT_HARA,MTSV_0 + $02	; 上から攻撃　NTK_SPZK0

	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_HARA,MTSV_0 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト
	db MT_HARA,MTSV_2 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0

	db MT_ANM_NO, MTA_ASTRO_U+1	; 隕石上LV1アニメスタート
	db MT_HARA,MTSV_1 + $05	; 8秒ウェイト

stage1_demo
	db MT_ANM_NO, MTA_ZAKO_F0+0	; ザコ前弱LV0アニメスタート
	db MT_ATK_NO, MAA_1SHOT+1	; １発攻撃 LV1
	db MT_HARA,MTSV_2 + $02	; 上から攻撃　NTK_SPZK0
	db MT_HARA,MTSV_3 + $02	; 上から攻撃　NTK_SPZK0

	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_HARA,MTSV_0 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0　NTK_SPZK0
	db MT_HARA,MTSV_0 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0
	db MT_HARA,MTSV_0 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0　NTK_SPZK0

	db MT_ANM_NO, MTA_ZAKO_F1+0	; ザコ前硬LV0アニメスタート

	db MT_HARA,MTSV_1 + $05	; 8秒ウェイト

stage1_boss
	db MT_HARA,$01	; ボスBGM開始
	db MT_ATK_NO, MAA_1SHOT+2	; １発攻撃 LV2

	db MT_HARA,MTSV_0 + $0F	; ワープイン　／＼　NTK_SPZK0
	db MT_HARA,MTSV_0 + $10	; ワープイン　＼／　NTK_SPZK0

	db MT_HARA,MTSV_1 + $0F	; ワープイン　／＼　NTK_SPZK1
	db MT_HARA,MTSV_0 + $10	; ワープイン　＼／　NTK_SPZK0

	db MT_HARA,MTSV_1 + $0F	; ワープイン　／＼　NTK_SPZK1


	db MT_END,0		; ミッション終了


;--------------------------------------------
; ステージ2
;--------------------------------------------
stage2
	db MT_HARA,$00	; ステージ開始

	db MT_ANM_NO, MTA_OFF
	db MT_ATK_NO, MTA_OFF

	db MT_HARA,MTSV_0 + $02	; 上から攻撃　NTK_SPZK0
	db MT_HARA,MTSV_0 + $06	; 下から攻撃　NTK_SPZK0

;	db MT_END,0		; ミッション終了

	db MT_ANM_NO, MTA_ZAKO_F0+2	; ザコ前弱LV2アニメスタート
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_ATK_NO, MAA_1SHOT+1	; １発攻撃 LV1
	db MT_HARA,MTSV_0 + $06	; 下から攻撃　NTK_SPZK0
	db MT_HARA,MTSV_3 + $09	; 斜め上左右から攻撃用　NTK_SPZK0
	db MT_HARA,MTSV_3 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0

stage2_demo
	db MT_ANM_NO, MTA_ASTRO_D+2	; 隕石下LV2アニメスタート
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_ANM_NO, MTA_OFF
	db MT_HARA,MTSV_0 + $03	; 上から攻撃　NTK_SPZK1

	db MT_HARA,MTSV_0 + $09	; 斜め上左右から攻撃用　NTK_SPZK0
	db MT_HARA,MTSV_2 + $09	; 斜め上左右から攻撃用　NTK_SPZK0
	db MT_HARA,MTSV_2 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0
	db MT_HARA,MTSV_0 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0

	db MT_ANM_NO, MTA_ZAKO_F1+3	; ザコ前硬LV3アニメスタート

	db MT_HARA,MTSV_1 + $05	; 8秒ウェイト
	
	db MT_HARA,MTSV_0 + $04	; 上から攻撃　NTK_SPZK2
	db MT_HARA,MTSV_3 + $08	; 下から攻撃　NTK_SPZK2

	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト


stage2_boss
	db MT_HARA,$01	; ボスBGM開始
	db MT_ATK_NO, MAA_1SHOT+2	; １発攻撃 LV2
	db MT_ANM_NO, MTA_WARPIN2+3	; ワープIN 隕LV3アニメスタート

	db MT_HARA,MTSV_1 + $0F	; ワープイン　／＼　NTK_SPZK1
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_HARA,MTSV_1 + $0F	; ワープイン　／＼　NTK_SPZK1
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_HARA,MTSV_1 + $10	; ワープイン　＼／　NTK_SPZK1


	db MT_END,0		; ミッション終了



;--------------------------------------------
; ステージ3　隕石＆上から攻撃
;--------------------------------------------
stage3
	db MT_HARA,$00	; ステージ開始

	db MT_ANM_NO, MTA_OFF
	db MT_ATK_NO, MTA_OFF

	db MT_ATK_NO, MAA_1SHOT+2	; １発攻撃 LV2

	db MT_HARA,MTSV_3 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0
	db MT_HARA,MTSV_2 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0
	db MT_HARA,MTSV_0 + $0D	; 斜め下左右から攻撃用　NTK_SPZK1

;	db MT_END,0		; ミッション終了
	
	db MT_HARA,MTSV_2 + $03	; 上から攻撃　NTK_SPZK1
	db MT_HARA,MTSV_3 + $03	; 上から攻撃　NTK_SPZK1

	db MT_ANM_NO, MTA_ASTRO_U+1	; 隕石上LV1アニメスタート
	db MT_HARA,MTSV_1 + $05	; 8秒ウェイト

	db MT_ATK_NO, MAA_3SHOT+1	; 3発攻撃 LV1
	db MT_HARA,MTSV_0 + $03	; 上から攻撃　NTK_SPZK1


	db MT_ATK_NO, MAA_3SHOT+2	; 3発攻撃 LV3
	db MT_ANM_NO, MTA_ZAKO_F0+2	; ザコ前弱LV2アニメスタート
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_HARA,MTSV_0 + $02	; 上から攻撃　NTK_SPZK0

	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト
	db MT_HARA,MTSV_2 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0

	db MT_HARA,MTSV_0 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0
	db MT_HARA,MTSV_0 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0
	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト
	db MT_HARA,MTSV_2 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0

	db MT_ANM_NO, MTA_ASTRO_U+1	; 隕石上LV1アニメスタート
	db MT_HARA,MTSV_1 + $05	; 8秒ウェイト

stage3_demo
	db MT_ANM_NO, MTA_OFF
	db MT_ATK_NO, MAA_1SHOT+3	; 1発攻撃 LV3

	db MT_HARA,MTSV_0 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0　NTK_SPZK0
	db MT_HARA,MTSV_0 + $09	; 斜め上左右から攻撃用　NTK_SPZK0　NTK_SPZK0
	db MT_HARA,MTSV_0 + $0C	; 斜め下左右から攻撃用　NTK_SPZK0　NTK_SPZK0

	db MT_HARA,MTSV_0 + $05	; 4秒ウェイト

stage3_boss
	db MT_HARA,$01	; ボスBGM開始
	db MT_ATK_NO, MAA_1SHOT+1	; 1発攻撃 LV1


	db MT_HARA,MTSV_2 + $0F	; ワープイン　／＼　NTK_SPZK2
	db MT_HARA,MTSV_2 + $10	; ワープイン　＼／　NTK_SPZK2

	db MT_HARA,MTSV_2 + $0F	; ワープイン　／＼　NTK_SPZK2
	db MT_HARA,MTSV_2 + $10	; ワープイン　＼／　NTK_SPZK2

	db MT_HARA,MTSV_2 + $0F	; ワープイン　／＼　NTK_SPZK2

	db MT_END,0		; ミッション終了


