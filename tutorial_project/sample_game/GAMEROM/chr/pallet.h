;/// @file pallet.h
;/// @brief Palette definitions and the macros that emit them.
;/// @ingroup gamerom
PAL_BG = $0F		;///< The backdrop colour every palette set repeats in its first entry.

;------------------------------------------------------------------------------
;  各種画面のパレットを集中管理するファイル
;------------------------------------------------------------------------------
PAL_PLYER		MACRO		;///< Palette: player ship.
	DB   PAL_BG,$21,$10,$20
	ENDM

PAL_PLYER2		MACRO		;///< Palette: player ship, second set.
	DB   PAL_BG,$1A,$17,$29
	ENDM


PAL_EFFECT		MACRO		;///< Palette: effects.
	DB   PAL_BG,$15,$19,$20
	ENDM

PAL_GAGE_EFC	MACRO		;///< Palette: gauge effect.
	DB  PAL_BG,$21,$21,$20			; BG 3  ゲージ＆爆発エフェクト
	ENDM

PAL_POWUP	MACRO		;///< Palette: power-up.
	DB   PAL_BG,$21,$10,$20		; BG 2
	ENDM


;------------------------------------------------------------------------------
;  タイトル画面パレット
;------------------------------------------------------------------------------
PAL_TITLE	MACRO		;///< Palette: title screen.
	; PAL_DAT
	DB	PAL_BG,$20,$20,$20 ;(緑) 
	DB	PAL_BG,$2A,$2A,$2A ;(赤) 
	DB	PAL_BG,$15,$27,$30 ;(青) 
	DB	PAL_BG,$1A,$1A,$1A ; 
	;	スプライト用パレット
	DB  PAL_BG,$0F,$20,$20 ;スプライト0
	PAL_PLYER2
	PAL_EFFECT
	PAL_PLYER
	ENDM



;------------------------------------------------------------------------------
;  クリアー画面パレット
;  ゲームオーバー画面パレット
;------------------------------------------------------------------------------
PAL_CLEAR	MACRO		;///< Palette: stage clear.
	; PAL_DAT
	DB  PAL_BG,$15,$21,$20	;(黒) グラフィック
	DB  PAL_BG,$00,$10,$20	;(青)
	DB  PAL_BG,$00,$10,$20	;(緑)
	DB  PAL_BG,$0F,$20,$20	;(赤) 文字
	;	スプライト用パレット
	DB   PAL_BG,$00,$10,$20 ;スプライト0
	PAL_PLYER2
	PAL_EFFECT
	PAL_PLYER
	ENDM

;------------------------------------------------------------------------------
;  オプション画面＆デバッグ画面パレット
;------------------------------------------------------------------------------
PAL_OPTION	MACRO		;///< Palette: options menu.
	; PAL_DAT
	DB	PAL_BG,$20,$20,$20 ;(白) 
	DB  PAL_BG,$07,$17,$28			; BG 1  ステージテーマサブ色
	DB	PAL_BG,$0f,$20,$20 ;(緑) 
	DB	PAL_BG,$0f,$20,$20 ;(緑) 
	; スプライト用パレット　デバッグ
	DB	PAL_BG,$00,$21,$20 ;(青) 
	PAL_PLYER2
	PAL_EFFECT
	PAL_PLYER
	ENDM


;------------------------------------------------------------------------------
;  エンディング画面パレット
;------------------------------------------------------------------------------
PAL_ENDING	MACRO		;///< Palette: ending.
	; PAL_DAT
	DB  PAL_BG,$20,$20,$20	;(黒)
	DB  PAL_BG,$00,$10,$20	;(青)
	DB  PAL_BG,$00,$10,$20	;(緑)
	DB  PAL_BG,$0F,$20,$20	;(赤) 文字
	;	スプライト用パレット
	DB  PAL_BG,$00,$10,$20 ;スプライト0
	DB  PAL_BG,$00,$10,$20 ;スプライト1
	DB  PAL_BG,$20,$10,$20 ;スプライト2　背景 星 色
	DB  PAL_BG,$00,$10,$20 ;スプライト3
	ENDM


;------------------------------------------------------------------------------
;  スタッフ画面パレット
;------------------------------------------------------------------------------
PAL_STAFF	MACRO		;///< Palette: staff roll.
	; PAL_DAT
	DB  PAL_BG,$00,$10,$20	;(黒)
	DB	PAL_BG,$20,$05,$15 ;(青) 
	DB	PAL_BG,$11,$21,$2C ;(黒)
	DB  PAL_BG,$20,$12,$20	;(赤) 文字
	;	スプライト用パレット
	DB  PAL_BG,$00,$10,$20 ;スプライト0
	DB  PAL_BG,$00,$10,$20 ;スプライト1
	PAL_EFFECT
	PAL_PLYER
	ENDM


;------------------------------------------------------------------------------
;  ステージ用パレット
;------------------------------------------------------------------------------

; ステージ共通パレット
PAL_STAGE_COM	MACRO		;///< Palette: shared by every stage.
	PAL_POWUP					; BG2
	PAL_GAGE_EFC				; BG3

	DB  $0F,$0C,$1C,$2C			; SP 0  隕石 ザコ敵　緑系
	PAL_PLYER2
	PAL_EFFECT					; SP 2  敵弾
	PAL_PLYER					; SP 3  プレーヤー系
	ENDM



PAL_STAGE01	MACRO		;///< Palette: stage 1.
	; STAGE 1
	DB  PAL_BG,$06,$16,$26			; BG 0  ステージテーマメイン色
	DB  PAL_BG,$2D,$27,$37			; BG 1  ステージテーマサブ色
	ENDM


PAL_STAGE02	MACRO		;///< Palette: stage 2.
	; STAGE 2
	DB  PAL_BG,$0A,$1A,$2A			; BG 0  ステージテーマメイン色
	DB  PAL_BG,$05,$15,$25			; BG 1  ステージテーマサブ色
	ENDM


PAL_STAGE03	MACRO		;///< Palette: stage 3.
	; STAGE 3
	DB  PAL_BG,$04,$14,$24			; BG 0  ステージテーマメイン色
	DB  PAL_BG,$07,$17,$28			; BG 1  ステージテーマサブ色
	ENDM


PAL_STAGE04	MACRO		;///< Palette: stage 4.
	; STAGE 4
	DB  PAL_BG,$11,$21,$20			; BG 0  ステージテーマメイン色
	DB  PAL_BG,$07,$17,$28			; BG 1  ステージテーマサブ色
	ENDM


PAL_STAGE05	MACRO		;///< Palette: stage 5.
	; STAGE 5
	DB  PAL_BG,$02,$12,$22			; BG 0  ステージテーマメイン色
	DB  PAL_BG,$07,$17,$28			; BG 1  ステージテーマサブ色
	ENDM


PAL_STAGE06	MACRO		;///< Palette: stage 6.
	; STAGE 6
	DB  PAL_BG,$16,$27,$20			; BG 0  ザコ敵　オレンジ系
	DB  PAL_BG,$25,$36,$20			; BG 1  ザコ敵　ピンク系
	ENDM

