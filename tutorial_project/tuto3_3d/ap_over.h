/**
 * @file ap_over.h
 * @brief Game-over screen: tumbling enemy model and the final score.
 * @ingroup sample_app
 *
 * The score is read straight out of the emulated 6502's RAM at `GM_SCORE` and
 * printed as packed BCD with a fixed trailing zero, so the figure on screen is
 * ten times the one stored.
 */

#ifndef ap_over_h
#define ap_over_h

#include "_build_option.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"

/**
 * @brief Slot assignments inside the shared ap_main::m_obj pool.
 * @ingroup sample_app
 * @note #OBJ_OVE_STAR is declared but not used: ap_over.cpp passes
 *       `OBJ_DEM_STAR` from ap_demo0.h instead. Both are 1.
 */
enum {
	OBJ_OVE_MODEL = 0, ///< The tumbling enemy model.
	OBJ_OVE_STAR = 1,  ///< First of 80 starfield slots.
};


/**
 * @brief Game-over screen.
 * @ingroup sample_app
 */
class ap_over {
    
public:
    /// @brief Trivial; state is established by init().
    ap_over() {};
    /// @brief Builds the scene, starts the game-over music and asks the 6502 to stop its own.
    void init(void);
    /**
     * @brief One frame: tumbles the model, draws the score, waits for an exit.
     * @note Accepts a key press only after five seconds, and leaves unconditionally
     *       after eighteen.
     */
    void main();

private:

	unsigned long demo_time; ///< Set at entry, never read; ap_main::m_timer drives the timing instead.
};

extern ap_over ap_ov; ///< The one game-over instance.

#endif

