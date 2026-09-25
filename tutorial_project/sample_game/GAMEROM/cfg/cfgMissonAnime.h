;/// @file cfgMissonAnime.h
;/// @brief Mission animation sequences.
;/// @ingroup gamerom
;===============================================================================================
;	ミッションアニメ処理
;
;===============================================================================================
;/// @brief Runs the HARADIUS mission animation.
;/// @ingroup gamerom
MissionAnimeHARA:
	lda  <PRM_1
	pha
	lda  <PRM_2
	pha
	jsr  .misson_anm_sub
	jsr  .misson_atk_sub
	pla
	sta  <PRM_2
	pla
	sta  <PRM_1
	rts


;--------------------------
;  SP敵攻撃パターン
;--------------------------
.misson_atk_sub
	lda  MISSON_ATK_CNT
	beq  .setAtk00
	dec  MISSON_ATK_CNT
	rts
.setAtk00
	lda  MISSON_ATK_NO
	tay
	and  #$03
	tax
	lda  .tblCntAddAtk,x
	sta  MISSON_ATK_CNT
	tya
	lsr  a
	lsr  a
	jmp  shotSpEnemy

.tblCntAddAtk
	db  20,15,10,5


.misson_anm_sub
	lda  MISSON_ANM_NO
	bmi  .misson_anm_tblsel
	lsr  a
	lsr  a
	TBL_JUMP
	JPTBL	.no_mission_anm		; 0
;	JPTBL	.misson_anm_01		; 1 次元潜航エフェクト


.no_mission_anm
.end
	rts

;--------------------------
;  テーブル選択型アニメ
;--------------------------
.misson_anm_tblsel
	lda  MISSON_ANM_CNT
	beq  .setAnmSP
	dec  MISSON_ANM_CNT
	rts
.setAnmSP
	; 出現頻度乱数＋固定値
	lda  MISSON_ANM_NO
	tay
	and  #$03
	tax
	jsr  GET_RND
	and  #$0F
	adc  .tblCntAdd,x
	sta  MISSON_ANM_CNT
	tya
	; セットテーブル選択
	and  #%0111_1100
	sta  <TMP_WRK0
	jsr  GET_RND
	and  #$03
	clc
	adc  <TMP_WRK0
	jmp  .setEnemyNT_hara

.tblCntAdd
;	db  40,30,20,10
	db  32,16,8,4



;------------------------------------
; 敵セット命令 areg 座標番号
;------------------------------------
.setEnemyNT_hara:
	asl a
	asl a
	tay
	lda  .tblData+2,y
	sta  <PRM_1
	lda  .tblData+3,y
	sta  <PRM_2

	lda  .tblData+1,y
	cmp  #-1
	bne  .no_rndy

	lda  #ENEMY_LINE_SUU -24
	jsr  GET_RND_N
	adc  #8
	and  #%1111_1000
.no_rndy
	sta  <PRM_Y_POS

	lda  .tblData+0,y
	cmp  #-3
	beq  .tirple_set
	cmp  #-2
	beq  .duble_set
	cmp  #-1
	bne  .no_rndx
.db00
	lda  #ENEMY_LINE_SUU -24
	jsr  GET_RND_N
	adc  #16
	and  #%1111_1000

.no_rndx
	sta  <PRM_X_POS
	jmp  .setEnamy

.duble_set
	jsr .db00
	lda  <PRM_X_POS
	clc
	adc  #24
	sta  <PRM_X_POS
	jmp  .setEnamy

.tirple_set
	jsr .db00
	lda  <PRM_X_POS
	clc
	adc  #24
	sta  <PRM_X_POS
	jsr  .setEnamy
	lda  <PRM_X_POS
	clc
	adc  #24
	sta  <PRM_X_POS

.setEnamy
	lda  <PRM_X_POS
	ldy  <PRM_Y_POS
	jsr  setEnemyNT3
	bcs  .end2

	lda  #0
	sta  <CACHE_GET_NENMY_NT_FG

	lda  <PRM_1
	sta  ENEMY_NT_KIND,y
	jsr  getEnemyNTcfg
	lda  <TMP_WRK1
	sta  ENEMY_NT_HP,y
	
	lda  <PRM_2
	sta  ENEMY_NT_MP,y

.end2
	rts



.tblData
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
	db  -2, 6,NTK_SPZK0,$40*2+$10+0	;21
	db  -1, 6,NTK_SPZK0,$40*3+$10-0	;22
	db  -3, 6,NTK_SPZK1,$40*1+$10+0	;23

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



;------------------------------
;	SP敵から弾発射
;
;  Areg = 攻撃パターン
;------------------------------
;/// @brief Fires from a special enemy; the accumulator selects the attack pattern.
;/// @ingroup gamerom
shotSpEnemy:
	TBL_JUMP
	JPTBL	shotSp00		; 0		攻撃しない
	JPTBL	shotSp01		; 1		全敵が自機方向に1発撃つ
	JPTBL	shotSp02		; 2		全敵がホーミング弾を1発撃つ

shotSp01
	jsr  shotSpTargetIDX
	bcc  .end

	; 弾発射
	lda  ENEMY_NT_X+1,x
	ldy  ENEMY_NT_Y+1,x
	jsr  setEnemyNT2

.end
shotSp00
	rts



shotSp02
	jsr  shotSpTargetIDX
	bcc  .end

	; 弾発射
	lda  ENEMY_NT_X+1,x
	ldy  ENEMY_NT_Y+1,x
	jsr  setEnemyNT2

	lda  ENEMY_NT_X+1,x
	ldy  ENEMY_NT_Y+1,x
	jsr  setEnemyNT

	lda  #$40-2
	sta  ENEMY_NT_MP,y

	lda  ENEMY_NT_X+1,x
	ldy  ENEMY_NT_Y+1,x
	jsr  setEnemyNT

	lda  #2
	sta  ENEMY_NT_MP,y

.end
	rts
	
;----------------------------------
;  発射するSP敵のIDXを取得
;----------------------------------
;/// @brief Returns the index of the special enemy that should fire.
;/// @ingroup gamerom
shotSpTargetIDX:
	lda  MISSON_ATK_IDX
	inc  MISSON_ATK_IDX
	and  #$07
	asl  a
	asl  a
	asl  a
	tax
	lda  ENEMY_NT_KIND,x
	beq  .noshot
	lda  ENEMY_NT_HP,x
	beq  .noshot
	bmi  .noshot
	sec
	rts

.noshot
	clc
	rts


