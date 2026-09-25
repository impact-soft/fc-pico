;/// @file cfgMissonSP.h
;/// @brief Mission subroutines for the special enemies.
;/// @ingroup gamerom
;===============================================================================================
;	ミッションSP敵関連
;
;===============================================================================================

;------------------------------------------------------------------
; sp敵　消滅待ち
;------------------------------------------------------------------
;/// @brief Wait entry point that skips the setup.
;/// @ingroup gamerom
waitClearSPE2:
	MC_MEMSET MISSON_ANM_NO, 0
;@	MC_MEMSET MISSON_ATK_NO, 0
;/// @brief Mission subroutine: wait until the special enemies are gone.
;/// @ingroup gamerom
waitClearSPE:
	MC_WAIT 1
	MC_PGCALL  .checkSub,0
	MC_JMP waitClearSPE, MCJ_CMP_NC
	MC_RET

.checkSub
	ldy  #0
.loop
	lda  ENEMY_NT_KIND,y
	bne  .end
.next
	tya
	clc
	adc  #ENEMY_NT_SIZE
	tay
	cpy  #ENEMY_NT_SIZE*8
	bne .loop
	sec
	rts

.end
	clc
	rts


;------------------------------------------------------------------
; sp敵　セット
; a reg = x座標
; y reg = y座標
;------------------------------------------------------------------
setSPE
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


;------------------------------------------------------------------
; sp敵の種類をENEMY_ATK_LVで切り替え
; a reg = 切り替えテーブル番号
;------------------------------------------------------------------
 .if 0
;/// @brief Selects a special-enemy kind from the table indexed by the accumulator.
;/// @ingroup gamerom
selSPE_KIND:
	jsr  selSPE_SUB
	lda  .tbl,y
	sta  <PRM_1
	rts
.tbl
	db NTK_SPZK0,NTK_SPZK0,NTK_SPZK0,NTK_SPZK0	; 0

 .endif
;------------------------------------------------------------------
; sp敵の移動方向をENEMY_ATK_LVで切り替え
; a reg = 切り替えテーブル番号
;------------------------------------------------------------------
;/// @brief Selects a special-enemy movement pattern.
;/// @ingroup gamerom
selSPE_MP:
	jsr  selSPE_SUB
	lda  .tbl,y
	sta  <PRM_2
	rts
.tbl
	; 上から攻撃用
	db $40*1+$10,$40*2+$10,$40*2+$10-4,$40*2+$10+3	; 0
	db $40*1+$10,$40*2+$10,$40*2+$10+4,$40*2+$10-3	; 1

	; 下から攻撃用
	db $40*1+$30,$40*2+$30,$40*2+$30-4,$30*2+$10+3	; 2
	db $40*1+$30,$40*2+$30,$40*2+$30+4,$30*2+$10-3	; 3

	; 斜め上左右から攻撃用
	db $40*1+$10-8,$40*2+$10-8,$40*2+$10-12,$40*2+$10-4	; 4
	db $40*1+$10+8,$40*2+$10+8,$40*2+$10+12,$40*2+$10+4	; 5

	; 斜め下左右から攻撃用
	db $40*1+$30+8,$40*2+$30+8,$40*2+$30+12,$40*2+$30+4	; 6
	db $40*1+$30-8,$40*2+$30-8,$40*2+$30-12,$40*2+$30-4	; 7

	; ワープイン用
	db NTK_SPZK0,NTK_SPZK1,NTK_SPZK2,NTK_METEO	; 8


;/// @brief Special-enemy warp-in helper.
;/// @ingroup gamerom
selSPE_SUB:
	sta  <TMP_SVA
	lda  ENEMY_ATK_LV
	asl  a
	rol  <TMP_SVA
	asl  a
	rol  <TMP_SVA
	ldy  <TMP_SVA
	rts

