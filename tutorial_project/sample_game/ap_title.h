#include <dummy_rp2350.h>

/**
 * @file ap_title.h
 * @brief Title screen: menu, attract-mode timer and the tumbling logo.
 * @ingroup sample_app
 *
 * Two menu items, GAME START and OPTION. Idling for ten seconds hands over to
 * attract mode, cycling ap_main::m_DemoFG between 1 and 2 so the two demo runs
 * alternate.
 *
 * @note The comment above the timeout says fifteen seconds; the code says ten.
 */

#ifndef ap_title_h
#define ap_title_h

#include "_build_option.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"

/**
 * @brief Slot assignments inside the shared ap_main::m_obj pool.
 * @ingroup sample_app
 * @note ap_option reuses #OBJ_TIT_CUBE for its own starfield rather than
 *       defining an equivalent of its own.
 */
enum {
	OBJ_TIT_LOGC = 0, ///< The FC-PICO logo model.
	OBJ_TIT_CUBE = 1, ///< First of 80 starfield slots. Named for an earlier version that drew cubes.
};


/**
 * @brief Title screen.
 * @ingroup sample_app
 */
class ap_title {
    
public:
    /// @brief Trivial; state is established by init().
    ap_title() {};
    /// @brief Builds the scene: projection, palette, logo model and starfield.
    void init(void);
    /// @brief One frame: menu input, idle timeout, logo animation and text.
    void main();

private:
    /// @brief Restarts the attract-mode countdown. Called on every key press.
    void resetDemoTime();

	uint8_t title_sel;       ///< Highlighted menu item: 0 = GAME START, 1 = OPTION.
	unsigned long demo_time; ///< `micros()` when the countdown last restarted.
};

extern ap_title ap_t; ///< The one title-screen instance.

#endif

