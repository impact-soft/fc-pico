;/// @file AplMiHara.asm
;/// @brief The HARADIUS mission type: the ordinary waves of minions.
;/// @ingroup gamerom
;=====================================
;
;	ミッション　HARADIUS制御プログラム
;
;
;=====================================







;/// @brief The HARADIUS mission handler; the ordinary waves.
;/// @ingroup gamerom
mi_Hara:
	lda MISSON_STEP
	TBL_JUMP
	JPTBL	mi_HaraInit		; 0
	JPTBL	mi_HaraMove		; 1
	JPTBL	mi_HaraClear	; 2
	JPTBL	mi_HaraOUT		; 3



;-----------------------
;		ボス初期化
;-----------------------

;/// @brief Initialises the mission's boss.
;/// @ingroup gamerom
mi_HaraInit:
	jsr  initMissionControl
	jsr  setGameCommonPal

	inc MISSON_STEP
	
	rts


;-----------------------
;		移動
;-----------------------
;/// @brief Moves the mission's boss.
;/// @ingroup gamerom
mi_HaraMove:
	jsr  MissionAnimeHARA
	jsr  mainMissionControl
	bcc .next

	lda #0
	sta <BG_SCR_Y

	lda #120
	sta MISSON_TMP




	inc MISSON_STEP
	rts

.next
;	jsr  moveLasterBG
;	jsr  hitLasterBG
	rts


;-----------------------
;		BG消去
;-----------------------
;/// @brief Erases the mission's background.
;/// @ingroup gamerom
mi_HaraClear:
	inc MISSON_STEP
.wait
	rts

;-----------------------
;		ボス消滅
;-----------------------

;/// @brief Runs the boss's departure.
;/// @ingroup gamerom
mi_HaraOUT:
	jmp getMission


