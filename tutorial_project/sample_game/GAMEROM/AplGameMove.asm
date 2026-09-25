;/// @file AplGameMove.asm
;/// @brief Object movement: enemies, enemy shots and player shots.
;/// @ingroup gamerom
;///
;/// The simulation proper, and the largest file in the ROM. Movement patterns are
;/// table-driven: `tblMoveENTsin` and its siblings hold the sine and direction
;/// tables that the aimed, homing and semi-homing shots steer by.
;///
;/// `hitEnemyNTObj` is the collision pass. It runs here, on the 6502, in both
;/// modes -- the cartridge renders the outcome but does not decide it.
;=======================================================================
;=======================================================================
;=======================================================================
;
;  各種ゲームOBJ移動処理
;




;/// @brief Moves every live object, then runs the collision pass.
;/// @ingroup gamerom
;///
;/// Called from `FCP_GAME_MAIN`, so this is the simulation the cartridge sees.
moveGameObj:
	; 無敵タイマーカウントダウン
	lda  PLY_MUTEKI_TM
	beq  .mg00
	dec  PLY_MUTEKI_TM
.mg00
	lda  MISSON_TYPE
	cmp  #MT_EVNT
	beq  .skip_00

;	jsr movePlyMoveObj

	jsr movePlyHorming
	jsr movePlyMoveObj

	jsr movePlyShotAObj
	jsr moveEnemyNTObj

	jsr hitEnemyNTObj

.skip_00
	rts



;-----------------------------------
; 敵弾移動
;-----------------------------------
;/// @brief Steps each enemy and enemy shot along its movement pattern.
;/// @ingroup gamerom
moveEnemyNTObj:

	; 移動処理ループ
	lda  <FLM_TIMER
	sta  <TMP_LOOP_CNT
	ldx  #0
	stx  <CACHE_GET_NENMY_NT_FG
.loop
	lda ENEMY_NT_KIND,x
	bne .movejob
.next
	inc  <TMP_LOOP_CNT
	txa
	clc
	adc  #ENEMY_NT_SIZE
	tax
	cpx #ENEMY_NT_SIZE*ENEMY_NT_SUU
	bne .loop


.end
	rts


.movejob
	jsr  .moveSpcSub
	lda  ENEMY_NT_KIND,x
	beq .next
	cmp #NTK_WARP		; ワープは移動しない
	beq .next

	lda  ENEMY_NT_MP,x
	jsr  setMoveDirData

	; X移動計算
	lda  ENEMY_NT_X+0,x
	clc
	adc  <W_AR
	sta  ENEMY_NT_X+0,x
	lda  ENEMY_NT_X+1,x
	adc  <W_AR+1
	sta  ENEMY_NT_X+1,x
	
	;  X画面外チェック
	lda  <W_AR+1
	bpl  .movx1
	jsr  revCFlag
.movx1
	bcs  .clear
	
	; Y移動計算
	lda  ENEMY_NT_Y+0,x
	clc
	adc  <W_BR
	sta  ENEMY_NT_Y+0,x
	lda  ENEMY_NT_Y+1,x
	adc  <W_BR+1
	sta  ENEMY_NT_Y+1,x

	
	;  Y画面外チェック
;	lda ENEMY_NT_Y+1,x
	cmp #ENEMY_LINE_SUU
;	cmp #240
	bcc .next

	lda  ENEMY_NT_KIND,x
	cmp  #NTK_MISS
	bne  .clear
	; ミサイル時特殊処理
	lda  #1
	sta  ENEMY_NT_Y+1,x
	jmp  .next

	; 移動終了
.clear
	lda  #0
	sta  ENEMY_NT_KIND,x
	jmp  .next

;------------------------------
; 敵の弾の移動特殊処理
;------------------------------
.moveSpcSub
	jsr  getEnemyNTcfg2
	lda  <TMP_WRK3
	beq  .mss00
	; タイマーで弾種＋１する
	inc  ENEMY_NT_DT,x
	cmp  ENEMY_NT_DT,x
	bne  .mss00
	lda  ENEMY_NT_KIND,x
	cmp  #NTK_WARP
	bne  .mss01
	;----------------------
	; ワープ時特殊処理
	;----------------------
	lda  ENEMY_NT_MP,x
	sta  ENEMY_NT_KIND,x
	jsr  getEnemyNTcfg2
	lda  <TMP_WRK1
	sta  ENEMY_NT_HP,x
	lda  #$40*1
	sta  ENEMY_NT_MP,x
	jsr  .angle2
	jmp  .mss00

.mss01
	inc  ENEMY_NT_KIND,x
	lda  #0
	sta  ENEMY_NT_DT,x
.mss00
	lda  <TMP_WRK2
	TBL_JUMP
	JPTBL	.end2		; 0
	JPTBL	.angle		; 1
	JPTBL	.horming	; 2
	JPTBL	.missile_u	; 3	上昇
	JPTBL	.missile_d	; 4 下降

;-----------------------------------
; ミサイル上昇中
;-----------------------------------
.missile_u
	lda  ENEMY_NT_DT,x
	beq  .missile_d_00

	lda  POS_PLY_X
	cmp  ENEMY_NT_X+1,x
	bcc  .mu_left

	lda  #$C0+48+2
	cmp  ENEMY_NT_MP,x
	beq  .end2
	inc  ENEMY_NT_MP,x
	rts

.mu_left
	lda  #$C0+48-2
	cmp  ENEMY_NT_MP,x
	beq  .end2
	dec  ENEMY_NT_MP,x
.end2
	rts

;-----------------------------------
; ミサイル下降中
;-----------------------------------
.missile_d_00
	lda  POS_PLY_X
	sta  ENEMY_NT_X+1,x
	jsr  .md_down

.missile_d
	lda  POS_PLY_X
	cmp  ENEMY_NT_X+1,x
	beq  .md_down
	bcs  .md_left

	lda  #$C0+16+2
	cmp  ENEMY_NT_MP,x
	beq  .end2
	inc  ENEMY_NT_MP,x
	rts

.md_left
	lda  #$C0+16-2
	cmp  ENEMY_NT_MP,x
	beq  .end2
	dec  ENEMY_NT_MP,x
	rts

.md_down
	lda  #$C0+16
	sta  ENEMY_NT_MP,x
	rts


;-----------------------------------
; ホーミング弾特殊処理
;-----------------------------------
.horming
	lda  ENEMY_NT_DT,x
	bne  .skip_getang

	; 方向修正
	lda  ENEMY_NT_X+1,x
	sta  <PRM_X_POS
	lda  ENEMY_NT_Y+1,x
	sta  <PRM_Y_POS
	lda  POS_PLY_X
	sta  W_AR+0
	lda  POS_PLY_Y
	sta  W_AR+1
	jsr  getAngleENT

	ora  #$80
	sta  ENEMY_NT_DT,x

.skip_getang
	and  #%0011_1111
	sta  <TMP_SVA
	
	lda  ENEMY_NT_MP,x
	and  #%0011_1111
	sta  <W_BR+0
	sec
	sbc  <TMP_SVA
	beq  .hrm002

	and  #%0010_0000
	beq  .hrm001
	inc  <W_BR+0
	inc  <W_BR+0
.hrm001
	dec  <W_BR+0

.hrm002
	lda  <W_BR+0
	and  #%0011_1111
	sta  <W_BR+0

	lda  ENEMY_NT_MP,x
	and  #$C0
	ora  <W_BR+0
	sta  ENEMY_NT_MP,x
	

	lda  <TMP_LOOP_CNT
	and  #$1F
	bne  .end3
	lda  #0
	sta  ENEMY_NT_DT,x
.end3
	rts

.tbl_rot
	db  0, -1,1

;-----------------------------------
; 自機狙い弾
;-----------------------------------
.angle
	inc  ENEMY_NT_KIND,x

.angle2
	; 方向取得
	lda  ENEMY_NT_X+1,x
	sta  <PRM_X_POS
	lda  ENEMY_NT_Y+1,x
	sta  <PRM_Y_POS
	lda  POS_PLY_X
	sta  W_AR+0
	lda  POS_PLY_Y
	sta  W_AR+1
	jsr  getAngleENT

	lda  ENEMY_NT_MP,x
	and  #%0011_1111
	clc
	adc  <TMP_SVA
	and  #%0011_1111
	sta  <TMP_SVA
	lda  ENEMY_NT_MP,x
	and  #%1100_0000
	ora  <TMP_SVA
	sta  ENEMY_NT_MP,x
	rts



;-----------------------------------
; 自機誘導弾補正 8フレームに１回だけ誘導
;-----------------------------------
;/// @brief Steers the player's homing shots. Corrects once every eight frames.
;/// @ingroup gamerom
movePlyHorming:
	ldx  #0
.loop
	lda  PSHOT_A_Y,x
	beq  .next
	
	txa
	asl  a
	asl  a
	tay
	lda  ENEMY_NT_KIND,y
	beq .next
	jsr  .mph00
.next
	inx
	inx
	cpx  #2*8
	bne  .loop
.end
	rts

.mph00
	; 方向修正
	lda  PSHOT_A_Y,x
	sta  <PRM_X_POS
	lda  PSHOT_A_Y,x
	sta  <PRM_Y_POS

	lda  ENEMY_NT_X+1,y
	sta  W_AR+0
	lda  ENEMY_NT_Y+1,y
	sta  W_AR+1
	jsr  getAngleENT

	lda  PSHOT_DIR,x
	and  #%0011_1111
	sta  <W_BR+0
	sec
	sbc  <TMP_SVA
	beq  .hrm002

	and  #%0010_0000
	beq  .hrm001
	inc  <W_BR+0
	inc  <W_BR+0
.hrm001
	dec  <W_BR+0

.hrm002
	lda  <W_BR+0
	and  #%0011_1111
	sta  <W_BR+0

	lda  PSHOT_DIR,x
	and  #$C0
	ora  <W_BR+0
	sta  PSHOT_DIR,x
	rts

;-----------------------------------
; 自機ノーマル弾移動
;-----------------------------------
;/// @brief Steps the player's shots and retires any that leave the playfield.
;/// @ingroup gamerom
;/// @note A retired slot is marked by zeroing its Y, which is the same convention
;///       the C++ renderer tests. @see @ref sample_game
movePlyShotAObj:
	ldx #0
.loop
	lda  PSHOT_A_Y,x
	beq  .next

	lda  PSHOT_DIR,x
	jsr  setMoveDirData

	; X移動計算
	lda  PSHOT_A_WX,x
	clc
	adc  <W_AR
	sta  PSHOT_A_WX,x
	lda  PSHOT_A_X,x
	adc  <W_AR+1
	sta  PSHOT_A_X,x
	
	;  X画面外チェック
	lda  <W_AR+1
	bpl  .movx1
	jsr  revCFlag
.movx1
	bcs  .clear

	; Y移動計算
	lda  PSHOT_A_WY,x
	clc
	adc  <W_BR
	sta  PSHOT_A_WY,x
	lda  PSHOT_A_Y,x
	adc  <W_BR+1

	;  Y画面外チェック
	cmp #ENEMY_LINE_SUU
	bcc .me000
.clear
	lda  #0
.me000
	sta PSHOT_A_Y,x
.next
	inx
	inx
	cpx #2*PSHOT_A_SUU
	bne .loop
.end
	rts


;=======================================================================
;  敵当たり判定
;=======================================================================


;-----------------------------------
; 敵ノーマル弾当たり判定
;-----------------------------------
;/// @brief Collision: player shots against enemies, and enemies against the player.
;/// @ingroup gamerom
;///
;/// Runs on the 6502 in both modes. The cartridge renders the result but takes no
;/// part in deciding it.
hitEnemyNTObj:
	lda  PLY_ANM_NO
	cmp  #PLY_AN_DEAD
	bne  .hitchk
	rts

.hitchk
    ldy  #ENEMY_NT_SUU
	sty  <TMP_LOOP_CNT

	ldx #0
.loop
	lda  ENEMY_NT_KIND,x
	beq  .next

	lda  ENEMY_NT_X+1,X
	cmp  #8		; 当たり判定の左側リミッター
	bcc  .next

	ldy  #2*(PSHOT_A_SUU+1)
	lda  ENEMY_NT_HP,x
	beq  .he10
	cmp  #254		; 判定無し
	bne  .he00

.next
	txa
	clc
	adc  #ENEMY_NT_SIZE
	tax
	dec  <TMP_LOOP_CNT
	bne .loop
.end
	rts


.he10
	ldy  #2			; 自機だけチェック
.he00
	lda  ENEMY_NT_Y+1,x
	sta  <PRM_Y_POS
	lda  ENEMY_NT_X+1,x
	sta  <PRM_X_POS

	jsr  hit_ShotA
	bcc  .next

	cpy  #0
	bne  .chkShotAB

	lda  PLY_MUTEKI_TM
	bne  .muteki
	jsr  setPlayerDead		; プレーヤーにダメージ

.muteki
	lda  #0
	sta  ENEMY_NT_KIND,x	; 弾を消す
	jmp  .next


.chkShotAB
	lda  ENEMY_NT_HP,X
	beq  .next			;当たり判定無し、破壊不能弾

	cmp  #255
	bne  .ch30

	; 当たり判定あり、破壊不能弾
	lda  #SE_NO_DAME
	jsr  PLAY_SE
	jmp  .next

.ch30
	dec  ENEMY_NT_HP,X
	beq  .hit
	
	lda  #SE_DAME
	jsr  PLAY_SE
	jmp  .next

.hit
	; SPザコならスコア+1
	lda  ENEMY_NT_KIND,x
	bpl  .hit2
	lda  #1
	jsr  SCR_ADD
.hit2
	; 爆発演出セット

	ldy ENEMY_NT_Y+1,x
	lda ENEMY_NT_X+1,x
	jsr setBakuEfc

	lda  #SE_BAKU_EFC
	jsr  PLAY_SE


	lda  #0
	sta  ENEMY_NT_KIND,x
	jmp  .next


;====================================================================
;    指定方向の移動データ取得
; IN -> Areg 移動方向+速度
; OUT -> X移動データ W_AR
;        Y移動データ W_BR
;====================================================================
;/// @brief Loads the movement delta for a direction into #W_BR.
;/// @ingroup gamerom
setMoveDirData:
	php
	asl  a
	sta  <TMP_SVY
	asl  a
	php
	clc
	adc  #$40
	plp
	ror  a
	tay

	; X移動データ
	lda  tblMoveENTsin,y
	sta  <W_AR
	lda  tblMoveENTsin+1,y
	sta  <W_AR+1

	; Ｙ移動データ
	ldy  <TMP_SVY
	lda  tblMoveENTsin,y
	sta  <W_BR
	lda  tblMoveENTsin+1,y
	sta  <W_BR+1

	plp
	bpl  .speedx1

	; 倍速
	asl  <W_AR
	rol  <W_AR+1
	asl  <W_BR
	rol  <W_BR+1

.speedx1
	rts


;====================================================================
;    敵の弾移動データ
;====================================================================

; 敵の弾移動テーブル 64方向一周分sinテーブル

NT_SIN MACRO		;///< Emits one entry of the enemy movement sine table.
	DW	( \1 ) * $100 / MV_ENT_BASE0
	ENDM

NT_SIN1 MACRO		;///< Emits one entry of the first alternate sine table.
	DW	( \1 ) * $100 / MV_ENT_BASE1
	ENDM

NT_SIN2 MACRO		;///< Emits one entry of the second alternate sine table.
	DW	( \1 ) * $100 / MV_ENT_BASE2
	ENDM

NT_SIN3 MACRO		;///< Emits one entry of the third alternate sine table.
	DW	( \1 ) * $100 / MV_ENT_BASE3
	ENDM


; 360 / 64 = 5.625

;/// @brief Sine table for enemy movement; 64 steps to the turn, 5.625 degrees each.
;/// @ingroup gamerom
tblMoveENTsin:
	NT_SIN 0		; 0  0
	NT_SIN 25		; 1  5.625
	NT_SIN 50		; 2  11.25
	NT_SIN 74		; 3  16.875
	NT_SIN 98		; 4  22.5
	NT_SIN 121		; 5  28.125
	NT_SIN 142		; 6  33.75
	NT_SIN 162		; 7  39.375
	NT_SIN 181		; 8  45
	NT_SIN 198		; 9  50.625
	NT_SIN 213		; 10  56.25
	NT_SIN 226		; 11  61.875
	NT_SIN 237		; 12  67.5
	NT_SIN 245		; 13  73.125
	NT_SIN 251		; 14  78.75
	NT_SIN 255		; 15  84.375

	NT_SIN 256		; 16  90
	NT_SIN 255		; 17  84.375
	NT_SIN 251		; 18  78.75
	NT_SIN 245		; 19  73.125
	NT_SIN 237		; 20  67.5
	NT_SIN 226		; 21  61.875
	NT_SIN 213		; 22  56.25
	NT_SIN 198		; 23 50.625
	NT_SIN 181		; 24 45
	NT_SIN 162		; 25  39.375
	NT_SIN 142		; 26  33.75
	NT_SIN 121		; 27  28.125
	NT_SIN 98		; 28  22.5
	NT_SIN 74		; 29
	NT_SIN 50		; 30
	NT_SIN 25		; 31

	NT_SIN 0		; 32
	NT_SIN -25		; 33
	NT_SIN -50		; 34
	NT_SIN -74		; 35
	NT_SIN -98		; 36  22.5
	NT_SIN -121		; 37  28.125
	NT_SIN -142		; 38  33.75
	NT_SIN -162		; 39  39.375
	NT_SIN -181		; 40  45
	NT_SIN -198		; 41  50.625
	NT_SIN -213		; 42  56.25
	NT_SIN -226		; 43  61.875
	NT_SIN -237		; 44  67.5
	NT_SIN -245		; 45  73.125
	NT_SIN -251		; 46  78.75
	NT_SIN -255		; 47  84.375

	NT_SIN -256		; 48  90
	NT_SIN -255		; 49  84.375
	NT_SIN -251		; 50  78.75
	NT_SIN -245		; 51  73.125
	NT_SIN -237		; 52  67.5
	NT_SIN -226		; 53  61.875
	NT_SIN -213		; 54  56.25
	NT_SIN -198		; 55  50.625
	NT_SIN -181		; 56  45
	NT_SIN -162		; 57  39.375
	NT_SIN -142		; 58  33.75
	NT_SIN -121		; 59  28.125
	NT_SIN -98		; 60  22.5
	NT_SIN -74		; 61
	NT_SIN -50		; 62
	NT_SIN -25		; 63

;/// @brief Second sine table, at a different amplitude.
;/// @ingroup gamerom
tblMoveENTsin1:
	NT_SIN1 0		; 0
	NT_SIN1 25		; 1
	NT_SIN1 50		; 2
	NT_SIN1 74		; 3
	NT_SIN1 98		; 4  22.5
	NT_SIN1 121		; 5  28.125
	NT_SIN1 142		; 6  33.75
	NT_SIN1 162		; 7  39.375
	NT_SIN1 181		; 8  45
	NT_SIN1 198		; 9  50.625
	NT_SIN1 213		; 10  56.25
	NT_SIN1 226		; 11  61.875
	NT_SIN1 237		; 12  67.5
	NT_SIN1 245		; 13  73.125
	NT_SIN1 251		; 14  78.75
	NT_SIN1 255		; 15  84.375

	NT_SIN1 256		; 16  90
	NT_SIN1 255		; 17  84.375
	NT_SIN1 251		; 18  78.75
	NT_SIN1 245		; 19  73.125
	NT_SIN1 237		; 20  67.5
	NT_SIN1 226		; 21  61.875
	NT_SIN1 213		; 22  56.25
	NT_SIN1 198		; 23 50.625
	NT_SIN1 181		; 24 45
	NT_SIN1 162		; 25  39.375
	NT_SIN1 142		; 26  33.75
	NT_SIN1 121		; 27  28.125
	NT_SIN1 98		; 28  22.5
	NT_SIN1 74		; 29
	NT_SIN1 50		; 30
	NT_SIN1 25		; 31

	NT_SIN1 0		; 32
	NT_SIN1 -25		; 33
	NT_SIN1 -50		; 34
	NT_SIN1 -74		; 35
	NT_SIN1 -98		; 36  22.5
	NT_SIN1 -121	; 37  28.125
	NT_SIN1 -142	; 38  33.75
	NT_SIN1 -162	; 39  39.375
	NT_SIN1 -181	; 40  45
	NT_SIN1 -198	; 41  50.625
	NT_SIN1 -213	; 42  56.25
	NT_SIN1 -226	; 43  61.875
	NT_SIN1 -237	; 44  67.5
	NT_SIN1 -245	; 45  73.125
	NT_SIN1 -251	; 46  78.75
	NT_SIN1 -255	; 47  84.375

	NT_SIN1 -256	; 48  90
	NT_SIN1 -255	; 49  84.375
	NT_SIN1 -251	; 50  78.75
	NT_SIN1 -245	; 51  73.125
	NT_SIN1 -237	; 52  67.5
	NT_SIN1 -226	; 53  61.875
	NT_SIN1 -213	; 54  56.25
	NT_SIN1 -198	; 55  50.625
	NT_SIN1 -181	; 56  45
	NT_SIN1 -162	; 57  39.375
	NT_SIN1 -142	; 58  33.75
	NT_SIN1 -121	; 59  28.125
	NT_SIN1 -98		; 60  22.5
	NT_SIN1 -74		; 61
	NT_SIN1 -50		; 62
	NT_SIN1 -25		; 63


;------------------------------------------
;		 2点間方向判定システム
;------------------------------------------
; 入力
; PRM_X_POS	-> 基準点X
; PRM_Y_POS	-> 基準点Y
; W_AR+0    -> ターゲットX
; W_AR+1    -> ターゲットY
;
; 破壊
;	W_BR,PRM_W_POS,PRM_H_POS
;
; 出力
;	TMP_SVA 0-63 方向

;/// @brief Returns the direction from an enemy to the player, for aimed shots.
;/// @ingroup gamerom
getAngleENT:
	lda  #0
	sta  <W_BR+0

	; X軸チェック
	lda  <W_AR+0
	sec
	sbc  <PRM_X_POS
	bcs  .gae00
	inc  <W_BR+0
	eor  #$FF
.gae00
	sta  <PRM_W_POS

	; Y軸チェック
	lda  <W_AR+1
	sec
	sbc  <PRM_Y_POS
	bcs  .gae01
	inc  <W_BR+0
	inc  <W_BR+0
	eor  #$FF
.gae01
	sta  <PRM_H_POS

	lda  #15
.loop
	cmp  <PRM_H_POS
	bcc  .next
	cmp  <PRM_W_POS
	bcc  .next
	bcs  .exit
.next
	lsr  <PRM_H_POS
	lsr  <PRM_W_POS
	jmp  .loop

.exit

	lda  <PRM_H_POS
	asl  a
	asl  a
	asl  a
	asl  a
	ora  <PRM_W_POS
	tay
	lda  tblDirCheck,y
	asl  a
	asl  a
	ora  <W_BR+0
	tay
	lda  tblDirCnv,y

	sta  <TMP_SVA
	rts


;------------------------------------------
;		 方向変換テーブル
;------------------------------------------
;/// @brief Direction conversion table.
;/// @ingroup gamerom
tblDirCnv:
	db $00,$20,$00,$20
	db $01,$1F,$3F,$21
	db $02,$1E,$3E,$22
	db $03,$1D,$3D,$23
	db $04,$1C,$3C,$24
	db $05,$1B,$3B,$25
	db $06,$1A,$3A,$26
	db $07,$19,$39,$27
	db $08,$18,$38,$28
	db $09,$17,$37,$29
	db $0A,$16,$36,$2A
	db $0B,$15,$35,$2B
	db $0C,$14,$34,$2C
	db $0D,$13,$33,$2D
	db $0E,$12,$32,$2E
	db $0F,$11,$31,$2F
	db $10,$10,$30,$30
	

;------------------------------------------
;		 方向判定テーブル 16x16
;------------------------------------------
;	NT_SIN 0		; 0  0
;	NT_SIN 25		; 1  5.625
;	NT_SIN 50		; 2  11.25
;	NT_SIN 74		; 3  16.875
;	NT_SIN 98		; 4  22.5
;	NT_SIN 121		; 5  28.125
;	NT_SIN 142		; 6  33.75
;	NT_SIN 162		; 7  39.375
;	NT_SIN 181		; 8  45
;	NT_SIN 198		; 9  50.625
;	NT_SIN 213		; A  56.25
;	NT_SIN 226		; B  61.875
;	NT_SIN 237		; C  67.5
;	NT_SIN 245		; F  73.125
;	NT_SIN 251		; E  78.75
;	NT_SIN 255		; F  84.375

;/// @brief Direction test table.
;/// @ingroup gamerom
tblDirCheck:
	db $08,$00,$00,$00,$00,$00,$00,$00,  $00,$00,$00,$00,$00,$00,$00,$00
	db $10,$08,$06,$05,$04,$03,$03,$03,  $02,$02,$02,$02,$02,$01,$01,$01
	db $10,$0A,$08,$07,$06,$05,$04,$04,  $03,$03,$03,$03,$02,$02,$02,$02
	db $10,$0B,$09,$08,$07,$06,$05,$05,  $04,$04,$04,$03,$03,$03,$03,$03
	db $10,$0C,$0A,$09,$08,$07,$06,$06,  $05,$05,$04,$04,$03,$03,$03,$03
	db $10,$0D,$0B,$0A,$09,$08,$07,$07,  $06,$06,$05,$05,$04,$04,$04,$04
	db $10,$0D,$0C,$0B,$0A,$09,$08,$07,  $07,$06,$06,$05,$05,$05,$04,$04
	db $10,$0D,$0C,$0B,$0A,$09,$09,$08,  $07,$07,$06,$06,$06,$05,$05,$05

	db $10,$0E,$0D,$0C,$0B,$0A,$09,$09,  $08,$07,$07,$07,$06,$06,$05,$05
	db $10,$0E,$0D,$0C,$0B,$0A,$0A,$09,  $09,$08,$08,$07,$07,$06,$06,$06
	db $10,$0E,$0D,$0C,$0C,$0B,$0A,$0A,  $09,$08,$08,$08,$07,$07,$06,$06
	db $10,$0E,$0D,$0D,$0C,$0B,$0B,$0A,  $09,$09,$08,$08,$08,$07,$07,$07
	db $10,$0E,$0E,$0D,$0D,$0C,$0B,$0A,  $0A,$09,$09,$08,$08,$08,$07,$07
	db $10,$0F,$0E,$0D,$0D,$0C,$0B,$0B,  $0A,$0A,$09,$09,$08,$08,$08,$07
	db $10,$0F,$0E,$0D,$0D,$0C,$0C,$0B,  $0B,$0A,$0A,$09,$09,$08,$08,$08
	db $10,$0F,$0E,$0D,$0D,$0C,$0C,$0B,  $0B,$0A,$0A,$09,$09,$09,$08,$08


