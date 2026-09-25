/**
 * @file ap_demo0.h
 * @brief Attract mode: a slow parade of the ship and enemy models.
 * @ingroup sample_app
 *
 * The second of the two demo screens. Where #ST_DEMO0 replays the game itself
 * under synthetic input, this one simply rotates each model in turn against the
 * starfield, swapping the subject at fixed frame counts.
 */

#ifndef ap_demo0_h
#define ap_demo0_h

#include "_build_option.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"


/**
 * @brief Slot assignments inside the shared ap_main::m_obj pool.
 * @ingroup sample_app
 * @note ap_over and ap_clear both reach for these names instead of their own
 *       equivalents. The values coincide, so it works. @see @ref sample_game
 */
enum {
	OBJ_DEM_MODEL = 0, ///< The model currently on show.
	OBJ_DEM_STAR = 1,  ///< First of 80 starfield slots.
};


/**
 * @brief Model-parade attract screen.
 * @ingroup sample_app
 */
class ap_demo0 {
    
public:
    /// @brief Trivial; state is established by init().
    ap_demo0() {};
    /// @brief Builds the scene and shows the player ship first.
    void init(void);
    /**
     * @brief One frame: swaps the model on a frame-count schedule, then draws.
     * @note Does not decide when to leave. ap_main::main() ends this screen after
     *       five seconds or on any key.
     */
    void main();

private:

	unsigned long demo_time; ///< Set at entry, never read. Left over from the shared screen template.
};

extern ap_demo0 ap_d0; ///< The one model-parade instance.

#endif

