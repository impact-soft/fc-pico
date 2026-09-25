/*
    ap_main.h
 */

/**
 * @file ap_main.h
 * @brief Scene state machine, shared object pool and the game-wide enumerations.
 * @ingroup sample_app
 *
 * ap_main is the top of the C++ application. It owns the single pool of 256
 * Obj3d slots that every screen draws out of, and it dispatches one frame at a
 * time to whichever screen module is current.
 *
 * Screens do not own their objects. Each one carves the shared pool up with its
 * own `OBJ_*` enumeration, calls ap_main::initObj() on entry, and trusts the
 * previous screen to have left nothing behind. The slot numbers of different
 * screens overlap deliberately.
 *
 * @see @ref sample_game for how this layer drives the emulated game ROM.
 */

#ifndef ap_main_h
#define ap_main_h

#include "_build_option.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"

#include "ap_title.h"
#include "ap_game.h"
#include "ap_over.h"
#include "ap_clear.h"
#include "ap_demo0.h"
#include "ap_data.h"
#include "ap_option.h"
#include "ap_license.h"

// プロセス間通信用
/**
 * @brief Core 0 to core 1 mailbox commands.
 * @ingroup sample_app
 *
 * Pushed through the RP2350 inter-core FIFO. #C1_SND_MP3PLAY is deliberately far
 * above the others so that a stray small value cannot be mistaken for it.
 */
enum {
	C1_RESET,                    ///< Restart core 1's work loop.
	C1_SNDJOB,                   ///< Run one iteration of the sound job.

	C1_SND_MP3PLAY = 0x10000000, ///< Refill the MP3 decoder from core 1.
	

};


/**
 * @brief Scene identifiers for ap_main::setStep().
 * @ingroup sample_app
 *
 * Each value except #ST_INIT and #ST_WAIT names one screen module. The dispatch
 * in ap_main::main() runs the module's `init()` on the first frame -- when the
 * sub-step is still 0 -- and its `main()` on every frame after that.
 */
enum {
	ST_INIT = 0,  ///< Entry state; falls straight through to #ST_TITLE.
	ST_TITLE,     ///< Title screen and menu. ap_title.
	ST_GAME,      ///< Interactive play. ap_game.
	ST_DEMO0,     ///< Attract mode: ap_game driven by a synthetic key pattern.
	ST_DEMO1,     ///< Attract mode: the model parade. ap_demo0.
	ST_OVER,      ///< Game-over screen. ap_over.
	ST_CLEAR,     ///< Stage-clear screen. ap_clear.
	ST_OPTION,    ///< Options menu. ap_option.
	ST_LICENSE,   ///< Licence text, decompressed from BPE. ap_license.

	ST_WAIT = 255,///< Idle: dispatch does nothing at all.
};

// セーブデータ
/**
 * @brief Byte offsets into rp_system::SaveData, the EEPROM-backed settings.
 * @ingroup sample_app
 *
 * ap_main::init() treats `0xff` at #SDT_HEAD as "never written" and installs the
 * defaults, which is what an erased EEPROM reads back as.
 */
enum {
	SDT_HEAD = 0,   ///< Initialised marker. Anything but `0xff` means the rest is valid.
	SDT_MP3_ENA,    ///< MP3 background music enabled.
	SDT_MP3_VOL,    ///< MP3 volume, 0-40.
	SDT_GAME_MODE,  ///< 0 = fixed camera, 1 = camera tracks the player.

};


/**
 * @brief Sound slot numbers shared by the NSF player, the MP3 path and the 6502.
 * @ingroup sample_app
 *
 * One flat numbering covers both music and effects: values up to #BGM_OVER are
 * background music, everything above is a sound effect. The split matters --
 * ap_game passes the emulated ROM's `REQ_BGM_NO` byte straight to
 * rp_sound::playBGM() and its `REQ_SE_NO` bytes to rp_sound::playSE(), so the
 * 6502 and this enumeration have to agree on the boundary.
 *
 * @warning The 6502 side keeps its own copy of these numbers in
 *          `GAMEROM/defGame.h`. Nothing checks that the two agree.
 *          @see @ref sample_game
 */
enum {
	BGM_STOP = 0,
	BGM_BOSS,   ///< Boss music. Also the track forced during attract mode 2.
	BGM_MAIN,   ///< Stage music.
	BGM_CLEAR,  ///< Stage-clear jingle.
	BGM_OVER,   ///< Game-over jingle. Last music slot; effects follow.

	SE_CUR_SEL,		///< Menu cursor moved. // 00 カーソル 移動
	SE_CUR_ENT,		///< Menu item confirmed. // 01 カーソル 決定
	SE_CUR_CAN,		///< Menu cancelled. // 02 カーソル キャンセル　(オプション 使用)
	SE_SHOT_A,		///< Player shot. // 03 自機ショット音
	SE_PLY_DAME,	///< Player hit. // 04 自機ダメージ音
	SE_PLY_DEAD,	///< Player destroyed. // 05 自機死亡
	SE_PLY_FORM,	///< Formation change. Unused by this build. // 06 自機フォーメーションチェンジ

	SE_BAKU_S,		///< Small enemy destroyed. // 07 敵 撃破 敵サイズ小 ザコ
	SE_BAKU_M,		///< Medium enemy destroyed. // 08 敵 撃破 敵サイズ中 ザコ
	SE_BAKU_L,		///< Boss destroyed. // 09 敵 撃破 敵サイズ大 ボス

	SE_NO_DAME,		///< Hit an invulnerable enemy. // 10 敵無敵音
	SE_DAME,		///< Enemy took damage. // 11 ダメージ受け音

	SE_BOSS_MOVE1,	///< Boss movement, variant 1. // 12 ボス移動1
	SE_BOSS_MOVE2,	///< Boss movement, variant 2. // 13 ボス移動2

	SE_BOSS_ATK1,	///< Boss attack. // 14 ボス攻撃
	SE_TITLE,		///< Title-screen sting. // 15 タイトルＳＥ
	SE_START_JET,	///< Launch thruster. // 16 スタートジェット
	SE_HADOU_CHG,	///< Wave cannon charging. // 17 波動砲　チャージ
	SE_HADOU_SHT,	///< Wave cannon fired. // 18 波動砲　発射
	SE_DM_DIVE,		///< Dimensional dive. // 19 次元潜航
	SE_YAMATO_S,	///< Launch sequence. // 20 ヤマト発進

	SND_SEL_MAX,    ///< Count of slots; the options menu uses it as the sound-test ceiling.
};



enum {

	OBJ_MAX = 256, ///< Size of the shared Obj3d pool. Screens index it with their own `OBJ_*` values.
};


/**
 * @brief The application root: scene dispatch, the object pool and the starfield.
 * @ingroup sample_app
 */
class ap_main {
    
public:

    /// @brief Trivial; the real work is in init().
    ap_main() {};

    /**
     * @brief Boots the application: object pool, emulated game ROM and save data.
     *
     * Loads `NES_MAP0DEMO` into the emulated 6502 and resets it, then installs
     * default save data if the EEPROM has never been written.
     * @note The emulator's RAM is zeroed here and never again. Every screen that
     *       needs a clean slate clears its own region.
     */
    void init();
    /// @brief Resets all #OBJ_MAX object slots to their inert state.
    void initObj();

    /**
     * @brief Runs one frame: samples the keys, then dispatches to the current screen.
     * @note Also increments #m_timer, which screens use as their frame clock; every
     *       screen resets it to 0 in its own `init()`.
     */
    void main();
    /// @brief Steps every object in the pool by its velocity.
    void move();
    /// @brief Draws every object in the pool, then restores the shared sprite state.
    void draw();



    /**
     * @brief Switches to another screen.
     * @param step One of the `ST_*` values.
     * @note Also clears the sub-step, which is what makes the dispatcher call the
     *       new screen's `init()` on the next frame.
     */
	void setStep( uint8_t step );
    /**
     * @brief Sets the sub-step within the current screen.
     * @param step_sub Screen-defined; 0 uniquely means "not yet initialised".
     */
	void setStepSub( uint8_t step_sub );

	uint8_t getStep() { return m_step; }        ///< @brief Current `ST_*` screen. @return The scene id.
	uint8_t getStepSub() { return m_step_sub; } ///< @brief Current sub-step. @return The sub-step, 0 before `init()` has run.
	Obj3d  m_obj[ OBJ_MAX ];                    ///< The shared object pool. Every screen draws out of this one array.

    /**
     * @brief Scatters a block of object slots into a drifting starfield.
     * @param StartStarObjNo First slot to use.
     * @param StarObjNum How many slots.
     * @param StarSpd Z velocity per frame; negative moves towards the viewer.
     * @note Only one starfield can be active: the parameters are stored in the
     *       single #m_StarObjNum / #m_StartStarObjNo pair, so a second call
     *       retargets moveStarObj() rather than adding a second field.
     */
    void initStarObj( uint16_t StartStarObjNo, uint16_t StarObjNum, float StarSpd );
    /// @brief Recycles any star that has passed the viewer back to the far plane.
    void moveStarObj();
    /**
     * @brief initStarObj() with the narrower spread used in-game.
     * @param StartStarObjNo First slot to use.
     * @param StarObjNum How many slots.
     * @param StarSpd Z velocity per frame.
     * @note The Y spread is a quarter of the menu version's and biased downwards,
     *       so the field reads as ground rushing past rather than open space.
     */
    void initStarObjGame( uint16_t StartStarObjNo, uint16_t StarObjNum, float StarSpd );
    /// @brief moveStarObj() for the in-game field, recycling at a nearer plane.
    void moveStarObjGame();

	uint16_t m_timer;  ///< Frames since the current screen started. Wraps at 65536.
	uint8_t m_DemoFG;  ///< Attract-mode counter: 0 = real play, 1 and 2 = the two demo runs.

private:
	uint8_t m_step;             ///< Current `ST_*` screen.
	uint8_t m_step_sub;         ///< Sub-step; 0 means the screen's `init()` still has to run.
	uint16_t m_StartStarObjNo;  ///< First slot of the active starfield.
	uint16_t m_StarObjNum;      ///< Number of slots in the active starfield.
	float m_StarSpd;            ///< Z velocity handed to the active starfield.
	unsigned long ap_main_time; ///< `micros()` at the top of the frame; feeds the disabled FPS readout.



};

extern ap_main ap;   ///< The one application instance.
/**
 * @brief The emulated 6502 running the game ROM.
 * @ingroup sample_app
 *
 * Not a music player here, as it is in the tutorial firmware: it executes
 * `GAMEROM/map0demo.NES`, and `emu.m_RAM` is the shared surface through which
 * the C++ and the 6502 halves of the game talk to each other.
 * @see @ref sample_game
 */
extern rp_fcemu emu;

#endif

