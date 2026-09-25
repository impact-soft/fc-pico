;/// @file cfgGame.h
;/// @brief Game-balance constants, gathered for tuning.
;/// @ingroup gamerom
;///
;/// Starting lives, continue limits, speeds and score values. Separated from the
;/// code so that balancing does not mean editing the logic.
;===============================================================================================
;	ゲームバランス調整用
;===============================================================================================
EVENT_MODE	EQU  0		; =1 イベントモード		;///< Set to 1 to build the event-only variant.

PLY_LIFE_INIT  EQU  10	; 自機の残数　初期値		;///< Lives at the start of a run.
;PLY_LIFE_INIT  EQU  1	; 自機の残数　初期値
COTI_MAX_LIFE  EQU  25  ; コンティニューによるLIFE上限アップ		;///< Ceiling that continuing can raise the life count to.

DEMO_STAGE_MAX  EQU 3	; デモステージの最大数		;///< Number of stages attract mode cycles through.
STAGE_MAX  EQU 3		; ステージの最大数		;///< Number of stages. The C++ side declares this value again in `ap_game.h`. @see @ref sample_game



PLY_HIT_ADD_X	EQU		4		; プレーヤー当たり判定位置調整Ｘ		;///< Player hitbox X offset.
PLY_HIT_ADD_Y	EQU		4		; プレーヤー当たり判定位置調整Ｙ		;///< Player hitbox Y offset.
PLY_HIT_SIZ_W	EQU		8		; プレーヤー当たり判定サイズ調整Ｗ		;///< Player hitbox width.
PLY_HIT_SIZ_H	EQU		8		; プレーヤー当たり判定サイズ調整Ｈ		;///< Player hitbox height.



;PLY_LIM_XL		EQU		16			; プレイヤーＸ座標リミッター
;PLY_LIM_XH		EQU		256-16		; プレイヤーＸ座標リミッター
PLY_LIM_XL		EQU		32			; プレイヤーＸ座標リミッター		;///< Player X limit, left.
PLY_LIM_XH		EQU		256-32		; プレイヤーＸ座標リミッター		;///< Player X limit, right.
OPT_LIM_XH		EQU		256-32		; オプションＸ座標リミッター		;///< Option satellite X limit, right.

PLY_LIM_YL		EQU		24			; プレイヤーＹ座標リミッター		;///< Player Y limit, top.
PLY_LIM_YH		EQU		208			; プレイヤーＹ座標リミッター		;///< Player Y limit, bottom.
OPT_LIM_YL		EQU		16			; オプションＹ座標リミッター		;///< Option satellite Y limit, top.
OPT_LIM_YH		EQU		200 -2		; オプションＹ座標リミッター		;///< Option satellite Y limit, bottom.


;IOSR_2			EQU		707/1000	; ルート２分の１ (0.707)
IOSR_2			EQU		100/100		; ルート２分の１ (0.707)		;///< One over root two, 0.707, as a fixed-point byte. Keeps diagonal movement the same speed as orthogonal.
;IOSR_2			EQU		1			; ルート２分の１ (0.707)

MV_PLY_BASE0		EQU 	$200*3/2		; 自機の移送速度ベース		;///< Player movement speed.
MV_PLY_BASE0_IR2	EQU 	$16a*3/2		; 自機の移送速度ベースのルート2分の1		;///< Player diagonal speed: #MV_PLY_BASE0 scaled by #IOSR_2.

;MV_PLY_BASE0		EQU 	$200*6/5		; 自機の移送速度ベース
;MV_PLY_BASE0_IR2	EQU 	$16a*6/5		; 自機の移送速度ベースのルート2分の1



MV_ENT_BASE0	EQU 120		; 敵の弾 スピード調整用 ※256ドットを指定フレームで移動する		;///< Enemy shot speed 0, expressed as the frames taken to cross 256 pixels.
MV_ENT_BASE1	EQU 100		; 敵の弾 スピード調整用 ※256ドットを指定フレームで移動する		;///< Enemy shot speed 1.
MV_ENT_BASE2	EQU  80		; 敵の弾 スピード調整用 ※256ドットを指定フレームで移動する		;///< Enemy shot speed 2.
MV_ENT_BASE3	EQU  60		; 敵の弾 スピード調整用 ※256ドットを指定フレームで移動する		;///< Enemy shot speed 3.


POS_PLY_X_INIT	EQU		128			; プレーヤー初期位置Ｘ		;///< Player starting X.
POS_PLY_Y_INIT	EQU		192			; プレーヤー初期位置Ｙ		;///< Player starting Y.


PSHOTA_SPD    EQU 8		; 自機通常弾速度		;///< Player normal-shot speed.



ENEMY_LINE_SUU	EQU		240-24		; 敵BG表示エリアライン数		;///< Playfield height in scanlines. Declared again on the C++ side. @see @ref sample_game

BAKU_EFC_CHR equ $01		;///< First CHR tile of the explosion animation.

MUTEKI_TIME		equ 60	;ダメージ時の無敵期間		;///< Invulnerability frames granted after a hit.
DAM_BG_FLASH_INIT equ 4 ; ダメージフラッシュタイム		;///< Frames the damage flash lasts.

PS_NOMAL_POW		equ -1	;通常弾のヒット時の敵ダメージ	(静止目標に、２回当たる）		;///< Damage a normal shot does; a stationary target takes two hits.

; シークレット取得時のライフ回復量
ADD_SC_LIFE_ST1	EQU 1		;///< Score threshold for the first extra life.
ADD_SC_LIFE_ST2	EQU 2		;///< Score threshold for the second extra life.
ADD_SC_LIFE_ST3	EQU 3		;///< Score threshold for the third extra life.
ADD_SC_LIFE_ST4	EQU 3		;///< Score threshold for the fourth extra life.
ADD_SC_LIFE_ST5	EQU 3		;///< Score threshold for the fifth extra life.




