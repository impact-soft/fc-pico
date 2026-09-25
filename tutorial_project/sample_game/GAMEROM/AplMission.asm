;/// @file AplMission.asm
;/// @brief The mission engine: a bytecode interpreter for stage scripts.
;/// @ingroup gamerom
;///
;/// A stage is not code, it is a script. `MISSON_PC` is a program counter into a
;/// byte stream built from the `MC_*` macros in defMission.h, and
;/// `mainMissionControl` is the interpreter that walks it.
;///
;/// The interpreter has more of a machine than the name suggests: a wait counter
;/// that suspends the script for a number of frames, a loop counter, conditional
;/// jumps, and a real call stack in `MISSON_STACK` indexed by `MISSON_PC_SP`, so
;/// one script can call another. Scripts may not nest calls, though; the comment
;/// at the head of cfgMissonHara.h says so plainly.
;///
;/// `MISSON_TYPE` doubles as the halt flag. Set to `$FF` the interpreter stops,
;/// and that is exactly what the C++ side watches for to decide a stage is clear.
;/// @see @ref sample_game
;=====================================
;
;	ミッション制御プログラム
;
;
;=====================================


;-------------------
;	初期化
;-------------------
;/// @brief Starts the first mission of the current stage.
;/// @ingroup gamerom
initMission:
	lda  #0
	sta  MISSON_NO
;/// @brief Reads the next entry from the stage table and starts that mission.
;/// @ingroup gamerom
getMission:
	lda  #0
	sta  MISSON_STEP
	sta  <USR_PROG+1
	sta  MISSON_PC_SP

	lda  DEBUG_MT_FLG
	beq  .no_mission_debug
	cmp  #2
	beq  .ret_debug
	
	lda  DBD_M_TYPE
	sta  MISSON_TYPE
	lda  DBD_MT_SUB
	sta  MISSON_TYPE_SUB
	inc  DEBUG_MT_FLG
	rts
.ret_debug
	lda  #ST_OPTION
	jmp  SET_STG_COD		; =jsr+rts

.no_mission_debug	
	lda  PLY_STAGE
	asl  a

	tay
	lda  <DEMO_FG
	bne  .tbl_sel0
	lda  mission_prog_tbl+1, y
	sta  <SRC_ADR+1
	lda  mission_prog_tbl, y
	sta  <SRC_ADR+0
	jmp  .tbl_sel_end

.tbl_sel0
	cmp  #1
	bne  .tbl_sel1

	lda  mission_prog_tbl_demo1+1, y
	sta  <SRC_ADR+1
	lda  mission_prog_tbl_demo1, y
	sta  <SRC_ADR+0
	jmp  .tbl_sel_end

.tbl_sel1
	lda  mission_prog_tbl_demo2+1, y
	sta  <SRC_ADR+1
	lda  mission_prog_tbl_demo2, y
	sta  <SRC_ADR+0
	jmp  .tbl_sel_end


.tbl_sel_end
	lda  MISSON_NO
	asl  a
	tay
	lda  [SRC_ADR],y
	sta  MISSON_TYPE
	iny
	lda  [SRC_ADR],y
	sta  MISSON_TYPE_SUB
	inc  MISSON_NO

	ldy  MISSON_TYPE
	cpy  #MT_ATK_NO
	beq  .atk_no
	cpy  #MT_ANM_NO
	beq  .anm_no
.end
	rts


; 攻撃番号セット
.atk_no
	lda  MISSON_TYPE_SUB
	sta  MISSON_ATK_NO
	jmp  getMission

; アニメーション番号セット
.anm_no
	lda  MISSON_TYPE_SUB
	sta  MISSON_ANM_NO
	jmp  getMission


;-------------------
;	ミッション更新処理（メインから毎フレームコール）
;-------------------
;/// @brief Advances the mission engine by one frame.
;/// @ingroup gamerom
;///
;/// Returns immediately once `MISSON_TYPE` reads `$FF`, which is the halt state
;/// and the signal the cartridge reads as stage-cleared.
updateMission:
	jsr  .u001
	lda  <USR_PROG+1
	beq  .end
	jmp  [USR_PROG]

.u001
	lda  MISSON_TYPE
	cmp  #$FF
	bne  .u000
.end
	rts

.u000
	TBL_JUMP
	JPTBL	mi_Hara		; 0  BGザコ敵
	JPTBL	mi_Hara		; 1  BGザコ敵
	JPTBL	mi_Hara		; 2  BGザコ敵
	JPTBL	mi_Hara		; 3  BGザコ敵
	JPTBL	mi_Hara		; 4  BGザコ敵
;/// @brief Mission handler that does nothing. Fills unused jump-table slots.
;/// @ingroup gamerom
mi_dmy:
	rts


;=================================
; ミッションコントロールシステム
;=================================
;----------------------
;  初期化
;----------------------
;/// @brief Loads the script for a mission and points `MISSON_PC` at its start.
;/// @ingroup gamerom
;///
;/// The sub-type byte is read two ways at once: its top two bits become the enemy
;/// attack level, and its bottom six index the script table for the mission type.
initMissionControl:
	lda  MISSON_TYPE
	asl  a
	tax
	lda  .tbl_control_tbl_list+0,x
	sta  <TMP_ADR0 +0
	lda  .tbl_control_tbl_list+1,x
	sta  <TMP_ADR0 +1

	lda  MISSON_TYPE_SUB
	tay
	and  #$C0
	sta  ENEMY_ATK_LV	;  敵の攻撃レベル設定
	tya
	and  #$3F
	asl  a
	tay
	lda  [TMP_ADR0],y
	sta  MISSON_PC+0
	iny
	lda  [TMP_ADR0],y
	sta  MISSON_PC+1

	lda  #0
	sta  MISSON_WAIT

	rts


.tbl_control_tbl_list
	dw misson_hara_control_tbl		;0
	dw misson_hara_control_tbl		;1
	dw misson_hara_control_tbl		;2
	dw misson_hara_control_tbl		;3


;----------------------
;  ミッションスタック
;----------------------
pushMstack_areg
	ldx  MISSON_PC_SP
	sta  MISSON_STACK+0,x
	inc  MISSON_PC_SP
	rts

popMstack_areg
	dec  MISSON_PC_SP
	ldx  MISSON_PC_SP
	lda  MISSON_STACK+0,x
	rts


;----------------------
;  メイン
;----------------------
;/// @brief The bytecode interpreter: executes one step of the mission script.
;/// @ingroup gamerom
;///
;/// Counts `MISSON_WAIT` down first and does nothing while it is non-zero, which
;/// is how a script sleeps. Otherwise it fetches the opcode at `MISSON_PC` and
;/// dispatches on it. @see defMission.h for the opcodes.
mainMissionControl:
	lda  MISSON_WAIT
	beq  .control
	dec  MISSON_WAIT
	clc
	rts

.control
	lda  MISSON_PC+0
	sta  <SRC_ADR+0
	lda  MISSON_PC+1
	sta  <SRC_ADR+1

.loop
	jsr  getSCR_ADR_DATA
	cmp  #_MC_END
	beq  .mc_end
	cmp  #_MC_BASE
	bcc  .mc_wait

	sbc  #_MC_BASE
	TBL_JUMP
	JPTBL	.mc_mempop		; _MC_MEMPOP	  メモリー 1バイト POP
	JPTBL	.mc_mempush		; _MC_MEMPUSH	  メモリー 1バイト PUSH
	JPTBL	.mc_memcmp		; _MC_MEMCMP	  メモリー比較 1バイト版 アドレス、値
	JPTBL	.mc_memadd		; _MC_MEMADD	  メモリー加算 1バイト版 アドレス、値
	JPTBL	.mc_nouse		; @_MC_VRAMSET	  VRAMセット　アドレス、値
	JPTBL	.mc_pgcall2		;F1 _MC_PGCALL2
	JPTBL	.mc_dummy		;F2
	JPTBL	.mc_memclr		;F3 _MC_MEMCLR
	JPTBL	.mc_pgcall		;F4 _MC_PGCALL
	JPTBL	.mc_memset2		;F5 _MC_MEMSET2
	JPTBL	.mc_memset		;F6 _MC_MEMSET
	JPTBL	.mc_palset		;F7 _MC_PALSET
	JPTBL	.mc_dummy		;F8
	JPTBL	.mc_memcpy2		;F9 _MC_MEMCPY2
	JPTBL	.mc_memcpyn		;FA _MC_MEMCPYN
	JPTBL	.mc_nouse		;FB _MC_ZAKO
	JPTBL	.mc_jmp			;FC _MC_JMP
	JPTBL	.mc_loop_cnt	;FD _MC_LOOP_CNT
	JPTBL	.mc_ret			;FE _MC_RET
	JPTBL	.mc_call		;FF _MC_CALL


.mc_dummy
	rts

	;------------------
	; 処理共通化
	;------------------
.getSCR_DST_ADR
	jsr  getSCR_ADR_DATA
	sta   <DST_ADR+0
	jsr  getSCR_ADR_DATA
	sta   <DST_ADR+1
	rts
	


	;------------------
	; 未使用
	;------------------
.mc_nouse:

	;------------------
	; データエンド
	;------------------
.mc_end
	sec
	rts

	;-------------------------------------
	; ウェイトタイマーセットしてテーブルデコード終了
	;-------------------------------------
.mc_wait
	sta  MISSON_WAIT
	
	lda  <SRC_ADR+0
	sta  MISSON_PC+0
	lda  <SRC_ADR+1
	sta  MISSON_PC+1
	clc
	rts


	;------------------
	; コール
	;------------------
.mc_call
	jsr  getSCR_ADR_DATA
	pha
	jsr  getSCR_ADR_DATA
	pha

	; 戻り先アドレスをセット
	lda  <SRC_ADR+0
	jsr  pushMstack_areg
	lda  <SRC_ADR+1
	jsr  pushMstack_areg

	; ループカウンターをセット
	lda  MISSON_LOOP_CNT
	jsr  pushMstack_areg

	; コール先アドレスにPCを書き換え
	pla
	sta  <SRC_ADR+1
	pla
	sta  <SRC_ADR+0
	jmp  .loop

	;------------------
	; リターン
	;------------------
.mc_ret
	jsr  popMstack_areg
	sta  MISSON_LOOP_CNT
	jsr  popMstack_areg
	sta  <SRC_ADR+1
	jsr  popMstack_areg
	sta  <SRC_ADR+0

	jmp  .loop

	;------------------
	; ループカウンターセット
	;------------------
.mc_loop_cnt
	jsr  getSCR_ADR_DATA
	sta  MISSON_LOOP_CNT
	jmp  .loop


	;------------------
	; 条件付ジャンプ
	;------------------
.mc_jmp
	jsr  getSCR_ADR_DATA
	pha
	jsr  getSCR_ADR_DATA
	pha
	jsr  getSCR_ADR_DATA
	tax
	jsr  getSCR_ADR_DATA
	jsr  .mcj_sub
	bcs  .mc_jump00

	; ジャンプしないで、次の命令へ
	pla
	pla
	jmp  .loop

	; 指定アドレスにジャンプ
.mc_jump00
	pla
	sta  <SRC_ADR+1
	pla
	sta  <SRC_ADR+0
	jmp  .loop



;-------------------------------------
; 条件付ジャンプの条件判定サブ
;   ジャンプする場合 cフラグセット
;-------------------------------------
.mcj_sub
	txa
	TBL_JUMP
	JPTBL  .MCJ_JMP
	JPTBL  .MCJ_LOOP_CNT
	JPTBL  .MCJ_BOSS_HP
	JPTBL  .MCJ_JMP
	JPTBL  .MCJ_JMP
	JPTBL  .MCJ_JMP
	JPTBL  .MCJ_CMP_Z
	JPTBL  .MCJ_CMP_NZ
	JPTBL  .MCJ_CMP_C
	JPTBL  .MCJ_CMP_NC


; 無条件ジャンプ ----------

; BOSS HP比較　未使用 ----------
.MCJ_BOSS_HP
.MCJ_JMP
	sec
	rts

; ループカウントダウン ----------
.MCJ_LOOP_CNT
	dec  MISSON_LOOP_CNT
	bne  .MCJ_JMP
	clc
	rts


; メモリー比較結果がZならジャンプ ----------
.MCJ_CMP_Z
	lda  MISSON_CMP_P
	pha
	plp
	beq  .MCJ_JMP
	clc
	rts

; メモリー比較結果がNZならジャンプ ----------
.MCJ_CMP_NZ
	lda  MISSON_CMP_P
	pha
	plp
	bne  .MCJ_JMP
	clc
	rts

; メモリー比較結果がCならジャンプ ----------
.MCJ_CMP_C
	lda  MISSON_CMP_P
	pha
	plp
	bcs  .MCJ_JMP
	clc
	rts

; メモリー比較結果がNCならジャンプ ----------
.MCJ_CMP_NC
	lda  MISSON_CMP_P
	pha
	plp
	bcc  .MCJ_JMP
	clc
	rts


	;------------------
	; プログラムコールアドレスセット版
	;------------------
.mc_pgcall2
	jsr  getSCR_ADR_DATA
	sta  <TMP_ADR0 +0
	jsr  getSCR_ADR_DATA
	sta  <TMP_ADR0 +1

	;------------------
	; プログラムコール
	;------------------
.mc_pgcall
	jsr  .mc_pgcall_sub
	php
	pla
	sta  MISSON_CMP_P
	jmp  .loop

.mc_pgcall_sub
	jsr  getSCR_ADR_DATA
	pha
	jsr  getSCR_ADR_DATA
	pha
	jsr  getSCR_ADR_DATA
	tay
	jmp  getSCR_ADR_DATA

	;------------------
	; メモリーコピー ２バイト版
	;------------------
.mc_memcpy2
	jsr  .getSCR_DST_ADR
	ldy  #1
	lda  [DST_ADR],Y
	dey
	pha
	lda  [DST_ADR],Y
	pha

	jsr  .getSCR_DST_ADR

	pla
	sta  [DST_ADR],Y
	iny
	pla
	sta  [DST_ADR],Y
	jmp  .loop

	;------------------
	; メモリーコピー Nバイト版
	;------------------
.mc_memcpyn
	jsr  getSCR_ADR_DATA
	sta   <W_AR+0
	jsr  getSCR_ADR_DATA
	sta   <W_AR+1

	jsr  .getSCR_DST_ADR

	jsr  getSCR_ADR_DATA
	tay
	dey
.loop_mc_memcpyn
	lda  [W_AR],Y
	sta  [DST_ADR],Y
	dey
	bpl  .loop_mc_memcpyn
	jmp  .loop


	;------------------
	; メモリーセット ２バイト版
	;------------------
.mc_memset2
	jsr  .getSCR_DST_ADR
	jsr  getSCR_ADR_DATA
	pha
	jsr  getSCR_ADR_DATA
	ldy   #1
	sta   [DST_ADR],Y
	dey
	pla
	sta   [DST_ADR],Y
	jmp  .loop

	;------------------
	; メモリーセット
	;------------------
.mc_memset
	jsr  .getSCR_DST_ADR
	jsr  getSCR_ADR_DATA
	ldy   #0
	sta   [DST_ADR],Y
	jmp  .loop

	;------------------
	; メモリークリア
	;------------------
.mc_memclr
	jsr  .getSCR_DST_ADR
	jsr  getSCR_ADR_DATA
	tay
	jsr  getSCR_ADR_DATA

.mc_mclr_loop
	dey
	sta   [DST_ADR],y
	bne   .mc_mclr_loop
	jmp  .loop


	;------------------
	; メモリー加算
	;------------------
.mc_memadd
	jsr  .getSCR_DST_ADR
	jsr  getSCR_ADR_DATA
	ldy   #0
	clc
	adc  [DST_ADR],Y
	sta  [DST_ADR],Y
	jmp  .loop

	;------------------
	; メモリー比較
	;------------------
.mc_memcmp
	jsr  .getSCR_DST_ADR
	jsr  getSCR_ADR_DATA
	ldy   #0
	cmp  [DST_ADR],Y
	php
	pla
	sta  MISSON_CMP_P
	jmp  .loop


	;------------------
	; メモリーPUSH
	;------------------
.mc_mempush
	jsr  .getSCR_DST_ADR
	ldy   #0
	lda  [DST_ADR],Y
	jsr  pushMstack_areg
	jmp  .loop


	;------------------
	; メモリーPOP
	;------------------
.mc_mempop
	jsr  .getSCR_DST_ADR
	jsr  popMstack_areg
	ldy   #0
	sta  [DST_ADR],Y
	jmp  .loop


	;------------------
	; パレットセット
	;------------------
.mc_palset
	jsr  getSCR_ADR_DATA
	tax
	jsr  getSCR_ADR_DATA
	sta  PAL_WRK,x
	jsr  getSCR_ADR_DATA
	sta  PAL_WRK+1,x
	jsr  getSCR_ADR_DATA
	sta  PAL_WRK+2,x
	PAL_CHG
	jmp  .loop

