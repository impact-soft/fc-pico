/**
 * @file ap_clear.h
 * @brief Stage-clear screen.
 * @ingroup sample_app
 *
 * Reached when the mission engine in the emulated ROM sets `MISSON_TYPE` to
 * `0xff` and ap_game has waited out its three-second hold.
 *
 * @note This header, alone among the screens, includes nothing of its own. It is
 *       only ever reached through ap_main.h, which has already pulled in
 *       Arduino.h and the platform headers it needs.
 */

#ifndef ap_clear_h
#define ap_clear_h

#include "_build_option.h"

/**
 * @brief Slot assignments inside the shared ap_main::m_obj pool.
 * @ingroup sample_app
 * @note Neither value is actually used: ap_clear.cpp reaches for ap_demo0.h's
 *       `OBJ_DEM_MODEL` and `OBJ_DEM_STAR`, which hold the same numbers.
 */
enum {
	OBJ_CLR_MODEL = 0, ///< The player-ship model.
	OBJ_CLR_STAR = 1,  ///< First of 80 starfield slots.
};


/**
 * @brief Stage-clear screen.
 * @ingroup sample_app
 */
class ap_clear {
    
public:
    /// @brief Trivial; state is established by init().
    ap_clear() {};
    /// @brief Builds the scene and starts the clear jingle.
    void init(void);
    /// @brief One frame: animates the ship, then returns to the title screen on a timer.
    void main();

private:

	unsigned long demo_time; ///< Set at entry, never read.
};

extern ap_clear ap_cl; ///< The one stage-clear instance.

#endif

