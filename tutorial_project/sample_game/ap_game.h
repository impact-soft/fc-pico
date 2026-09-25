/**
 * @file ap_game.h
 * @brief Play screen, and the C++ mirror of the game ROM's RAM map.
 * @ingroup sample_app
 *
 * The game itself is not written here. It runs as 6502 code inside rp_fcemu,
 * and this screen is the half that drives it: each frame it posts the controller
 * state into the emulated RAM, calls the ROM's `$E004` entry point, then reads
 * the object tables back out and turns them into 3D objects.
 *
 * Everything from #EM_DEMO_FG down is a hand-copied mirror of the addresses in
 * `GAMEROM/defRAM.h`. The two files are the same map written twice, in two
 * languages, and nothing checks that they still agree.
 *
 * @see @ref sample_game for the whole arrangement.
 */

#ifndef ap_game_h
#define ap_game_h

#include "_build_option.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"



/**
 * @brief How the play screen carves up the shared ap_main::m_obj pool.
 * @ingroup sample_app
 *
 * Each range is sized to the matching table in the emulated RAM: 15 bullets
 * (`PSHOT_A_SUU`), 22 enemies (#ENEMY_NT_SUU) and 13 explosions
 * (#BAKU_EFC_SUU). The ranges are laid out by hand and are not checked against
 * those counts.
 */
enum {
	OBJ_GAM_MODEL = 0,      ///< Unused here; kept so the slot numbering matches the other screens.
	OBJ_GAM_PLAYER = 2,     ///< The player ship.
	OBJ_GAM_BULLET = 3,     ///< First of 15 player-shot slots. // 自機の弾のオブジェ番号
	OBJ_GAM_ENEMY = 64,     ///< First of 22 enemy and enemy-shot slots.
	OBJ_GAM_EXPEFC = 110,   ///< First of 13 explosion slots. // 爆発エフェクト

	OBJ_GAM_STAR = 128,     ///< First of 80 starfield slots.
};


/**
 * @brief Game-progress codes used by the 6502 side.
 * @ingroup sample_app
 * @note Declared for completeness. This screen does not read them: it detects the
 *       end of a run from #PLY_ANM_NO and #MISSON_TYPE instead.
 */
enum {
	GMST_MAIN		= 5, ///< Playing.
	GMST_CLEAR		= 6, ///< Stage cleared.
	GMST_OVER		= 7, ///< Player out of lives.
};


//==========================================================
// プレーヤーアニメ制御関連定義
//==========================================================
/**
 * @brief Player animation states, as written to #PLY_ANM_NO by the 6502.
 * @ingroup sample_app
 * @note Only #PLY_AN_DEAD is tested on this side; it is what starts the
 *       three-second hold before the game-over screen.
 */
enum {
	PLY_AN_WAIT		= 0,	///< Idle. // 待機
//	PLY_AN_CHARGE	EQU  1		; チャージ
//	PLY_AN_SHOTA	EQU  2		; ショットA
//	PLY_AN_SHOTB	EQU  3		; ショットB
	PLY_AN_DEAD		= 4,		///< Death animation running. // 死亡アニメ
};


/**
 * @brief Addresses inside rp_fcemu::m_RAM, mirroring `GAMEROM/defRAM.h`.
 * @ingroup sample_app
 *
 * This is the interface between the two halves of the game. Values below `$100`
 * are 6502 zero page; `$100`-`$1FF` is its stack page, which the game also uses
 * for variables; everything above is ordinary work RAM.
 *
 * Traffic runs both ways. The C++ side writes the controller state and the demo
 * flag, and consumes the sound requests by zeroing them once played; it also
 * advances #BAKU_EFC_CNT, so the explosion animation is timed on this side even
 * though the table belongs to the 6502.
 *
 * @warning A hand-maintained copy. At this revision all 51 mirrored constants
 *          agree with `GAMEROM/defRAM.h`, but nothing in the build checks it, and
 *          the two files are edited independently. #PLY_STAGE is the fragile one:
 *          on the 6502 side it is an alias for a debug slot, `DEBUG_DT0`.
 */
enum {
	EM_DEMO_FG = 0xB7,	///< Attract-mode flag. Written by this side each time play starts. // デモフラグ

	EM_KEY_TRG = 0xC3,  ///< Newly pressed keys this frame. Written by this side.
	EM_KEY_NEW = 0xC5,  ///< Keys currently held. Written by this side; also read back to bank the ship.

	EM_STG_COD	= 0xE8, ///< Stage progress code, owned by the 6502.
	EM_STG_COD_SUB	= 0xE9, ///< Sub-code within the stage.


	EM_REQ_SE_NO	= 0xF5, ///< Sound-effect request slot 1. Zeroed by this side once played.
	EM_REQ_SE_NO2	= 0xF6, ///< Sound-effect request slot 2.
	EM_REQ_SE_NO3	= 0xF7, ///< Sound-effect request slot 3. Only the first non-zero slot is served per frame.
	EM_REQ_BGM_NO	= 0xFA, ///< Music request. Zeroed by this side once started.

	
	GM_SCORE		= 0x110,	///< Score, 4 bytes of packed BCD, least significant first. // 4 bytes
	PLY_LIFE		= 0x11e,	///< Lives remaining. // B 残機数
	PLY_STAGE		= 0x121,	///< Current stage, 1-based. Aliases `DEBUG_DT0` on the 6502 side. // ステージ番号

	EM_PLY_STAGE = 0x121,		///< The same byte as #PLY_STAGE, under the name the menus use to set it. // ステージ番号
//	EM_MISSON_TYPE = 0x3A1,		// B ミッションタイプ
//	EM_MISSON_TYPE_SUB = 0x3A2,	// B ミッションタイプ
//	EM_MISSON_WAIT	= 0x3A3,    // B MISSON_PCの次の処理までのウェイト
//	EM_MISSON_PC	= 0x3A4,	// W ミッションPC

	

	STAGE_MAX = 3,				///< Number of stages. Attract mode wraps back to 1 past this. // ステージの最大数

	//----------------
	// ミッション制御系ワーク
	//----------------
	MISSON_NO		= 0x3A0,	///< Mission index within the stage. // B ステージ内ミッション番号
	MISSON_TYPE		= 0x3A1,	///< Mission opcode. `0xff` means the stage is cleared, which is how this side detects it. // B ミッションタイプ
	MISSON_TYPE_SUB	= 0x3A2,	///< Mission opcode qualifier. // B ミッションタイプ
	MISSON_WAIT		= 0x3A3,	///< Frames until the mission script advances. // B MISSON_PCの次の処理までのウェイト
	MISSON_PC		= 0x3A4,	///< Mission script program counter, 16-bit. // W ミッションPC

	MISSON_LOOP_CNT	= 0x3A6,	///< Loop counter for the mission script. // B ミッション ループカウンタ
	MISSON_FLG		= 0x3A9,	///< Mission flags. // B ミッション フラグ
	MISSON_STEP		= 0x3AA,	///< Mission step within the current opcode. // B ミッション 処理ステップ
	MISSON_TMP		= 0x3AB,	///< Mission scratch byte. // B ミッション 汎用

	DAM_BG_FLASH	= 0x3B2,	///< Frames of red background flash owed. The code that acted on it is commented out. // ダメージＢＧフラッシュ

	PLY_ANM_NO		= 0x3B3,	///< Player animation state; one of the `PLY_AN_*` values. // アニメーション番号



//	PLY_FORM		= 0x3B4,	// フォーメーション
//	PLY_DISP_FG		= 0x3B5,	// プレーヤー表示制御
	PLY_MUTEKI_TM	= 0x3B6,	///< Invulnerability frames left. Drives the blink on this side. // 無敵タイマー

	PLY_OBJ_KIND	= 0x500, ///< Player object kind.
	PLY_OBJ_DIR		= 0x501, ///< Player facing.
	PSHOT_KIND		= 0x502, ///< First player-shot kind. Stride 2, as for the position tables.
	PSHOT_DIR		= 0x503, ///< First player-shot direction; selects the sprite tile on this side.
	
	POS_PLY_X		= 0x520, ///< Player X, in NES screen pixels.
	POS_PLY_Y		= 0x521, ///< Player Y, in NES screen pixels.

	PSHOT_A_X		= (POS_PLY_X+2), ///< First player-shot X. Stride 2 bytes per shot.
	PSHOT_A_Y		= (POS_PLY_Y+2), ///< First player-shot Y. A zero Y marks the slot free.
	PSHOT_A_SUU	 = 15, ///< Number of player-shot slots.

	PLY_OBJ_WX		= 0x540, ///< Player X, sub-pixel fraction.
	PLY_OBJ_WY		= 0x541, ///< Player Y, sub-pixel fraction.
	PSHOT_A_WX		= (PLY_OBJ_WX+2), ///< First player-shot X fraction.
	PSHOT_A_WY		= (PLY_OBJ_WY+2), ///< First player-shot Y fraction.

	
	//----------------
	// 敵のノーマル弾ワーク
	// $600-$6AF 8x22セット
	//----------------
	ENEMY_NT_WORK	= 0x600, ///< Base of the enemy table: #ENEMY_NT_SUU entries of #ENEMY_NT_SIZE bytes.
	ENEMY_NT_KIND	= 0x600, ///< Enemy kind; 0 means the slot is free. One of the `NTK_*` values.
	ENEMY_NT_MP		= 0x601,	///< Movement pattern number. // 移動パターン番号
	ENEMY_NT_DT		= 0x602,	///< Per-pattern control byte. // 特殊制御用データ
	ENEMY_NT_HP		= 0x603,	///< Hit points. // 耐久力
	ENEMY_NT_X		= 0x604,	///< Enemy X, 16-bit; this side reads the high byte for screen pixels. // W
	ENEMY_NT_Y		= 0x606,	///< Enemy Y, 16-bit; high byte is the screen pixel. // W

	ENEMY_NT_SIZE	= 8,  ///< Bytes per enemy entry.
	ENEMY_NT_SUU	= 22, ///< Number of enemy slots.

	//----------------
	// 爆発演出ワーク
	// 3x13セット
	//----------------
	BAKU_EFC_X		= 0x6B0, ///< Base of the explosion table; stride 3 bytes.
	BAKU_EFC_Y		= 0x6B1, ///< Explosion Y.
	BAKU_EFC_CNT	= 0x6B2, ///< Animation frame, 1-15; 0 is a free slot. Advanced by this side, not the 6502.

	BAKU_EFC_SUU = 13, ///< Number of explosion slots.


	ENEMY_LINE_SUU	= 240-24,	///< Playfield height in scanlines. Used only by the disabled radar. // 敵BG表示エリアライン数

};

//;========================================
//  敵種類定義
//;========================================
/**
 * @brief Enemy kinds, as stored in #ENEMY_NT_KIND.
 * @ingroup sample_app
 *
 * The top bit is the discriminator this side cares about: values below `0x80`
 * are shots, drawn as a two-frame 8x8 sprite, and values from `0x80` up are
 * craft, drawn as 3D models. Everything else about the kind is the 6502's
 * business.
 */
enum {
	NTK_ANGLE	= 1,	///< Aimed shot. // 自機狙い弾
	NTK_NOMAL	= 2,	///< Plain shot. // 通常弾
	NTK_HORMI	= 3,	///< Homing shot. // ホーミング弾
	NTK_MISS	= 4,	///< Missile. // ミサイル弾
	NTK_HHORM	= 6,	///< Partially homing shot. // 半誘導弾
	NTK_HHORM3	= 8,	///< Partially homing shot, splits into three. // 半誘導弾３分裂
	NTK_METEO	= 0x80+11,	///< Meteor. Drawn as a dithered tumbling cube rather than a model. // 隕石
	NTK_WARP	= 0x80+12,	///< Warp-in effect. Drawn as a four-frame 16x16 sprite. // ワープエフェクト
	NTK_SPZK0	= 0x80+13,	///< Special minion 0. Falls to the default model. // SPザコ0
	NTK_SPZK1	= 0x80+14,	///< Special minion 1. Drawn with #MDL_ENEMY3_NO. // SPザコ1
	NTK_SPZK2	= 0x80+15,	///< Special minion 2. Drawn with #MDL_ENEMY2_NO. // SPザコ2
	NTK_LIFE	= 0x80+16,	///< Extra-life pickup. // ライフ回復

};


/**
 * @brief The play screen: drives the emulated game ROM and renders its state.
 * @ingroup sample_app
 */
class ap_game {
    
public:
    /// @brief Trivial; state is established by init().
    ap_game() {};
    /**
     * @brief Builds the scene and runs the ROM's `$E000` initialisation entry.
     * @note Clears the shot, enemy and explosion tables in the emulated RAM
     *       first. Nothing else zeroes them between runs.
     */
    void init(void);
    /**
     * @brief One frame: post input, run the ROM's `$E004` entry, render the result.
     *
     * In attract mode the controller bytes are replaced by a square wave that
     * alternates left and right every 64 frames, which is the whole of the demo
     * "AI".
     */
    void main();

private:
	// アセンブラソースから移植
	/// @brief Empty. A stub left over from the port of the 6502 mission code.
	void initMission() {
		
	};
	/// @brief Declared but never defined or called; the mission script runs on the 6502.
	void getMission();
	
	/**
	 * @brief Draws a 2D overhead radar of the object tables.
	 * @note Complete, but the call site in main() is commented out, so it never runs.
	 */
	void drawRader();
	/**
	 * @brief Turns the emulated RAM's object tables into 3D objects.
	 *
	 * The core of the screen. Walks the enemy, player-shot, player and explosion
	 * tables and gives each live entry an object slot, choosing a model, a sprite
	 * or a cube by kind. Model changes are made only when an entry's kind changes,
	 * which is what #m_emobj_kind is for.
	 */
	void conv3DObje();
	/**
	 * @brief Maps a NES screen position onto the 3D ground plane.
	 * @param obj Object to place.
	 * @param x NES screen X, 0-255.
	 * @param y NES screen Y, 0-239.
	 * @note Screen Y becomes world Z, not world Y: the 2D playfield is laid flat
	 *       and viewed in perspective. That one substitution is what turns a 2D
	 *       shooter into the 3D scene on the television.
	 */
	void conv3Dxy( Obj3d  *obj, int x, int y);

	int m_over_wait;   ///< Frames since the player died; the game-over screen follows after 180.
	int m_clear_wait;  ///< Frames since the stage was cleared; the clear screen follows after 180.
	uint8_t m_PlyDamFg;

	uint8_t m_emobj_kind[ ENEMY_NT_SUU ]; ///< Last kind seen in each enemy slot, so models are rebuilt only on a change.

};

extern ap_game ap_g; ///< The one play-screen instance. ///< The one play-screen instance.

#endif

