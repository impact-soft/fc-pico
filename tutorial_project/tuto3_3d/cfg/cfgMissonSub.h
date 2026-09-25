;/// @file cfgMissonSP.h
;/// @brief Mission subroutines for the special enemies.
;/// @ingroup gamerom
;===============================================================================================
;	ミッションSP敵関連
;
;===============================================================================================

;------------------------------------------------------------------
; 敵　消滅待ち
;------------------------------------------------------------------
;/// @brief Wait entry point that skips the setup.
;/// @ingroup gamerom
waitClearSPE2:
	MC_MEMSET MISSON_ANM_NO, 0
waitClearSPE:
	MC_WAIT 1
	MC_JMP waitClearSPE, MCJ_ENEMY_NZ
	MC_RET




