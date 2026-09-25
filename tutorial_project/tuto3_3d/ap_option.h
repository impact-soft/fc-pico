/**
 * @file ap_option.h
 * @brief Options menu: stage select, sound test, MP3 settings and the save.
 * @ingroup sample_app
 *
 * Seven rows, driven by a parallel pair of tables: `op_cfg[]` in ap_option.cpp
 * holds a minimum and maximum for each row, and ap_option::op_dt holds the current value.
 * Left and right wrap around inside those bounds.
 *
 * Only rows 2, 3 and 4 are persisted, and only on SAVE & EXIT. Sound and volume
 * changes take effect immediately but are lost if the menu times out.
 */

#ifndef ap_option_h
#define ap_option_h

#include "_build_option.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"

/**
 * @brief Object slots and the row count for the options menu.
 * @ingroup sample_app
 * @note #OBJ_OPT_LOGC and #OBJ_OPT_CUBE are both unused: ap_option.cpp starts its
 *       starfield at ap_title.h's `OBJ_TIT_CUBE`, which holds the same value.
 * @warning #OPTION_MENU_MAX must match the number of `MIN,MAX` pairs in
 *          ap_option.cpp's `op_cfg[]`. Nothing enforces it, and ap_license.cpp
 *          declares an array of this size against a shorter table of its own.
 */
enum {
	OBJ_OPT_LOGC = 0, ///< Unused. A logo slot, by analogy with the title screen.
	OBJ_OPT_CUBE = 1, ///< Unused. A starfield slot, by analogy with the title screen.

	OPTION_MENU_MAX = 7, ///< Number of menu rows, and the size of ap_option::op_dt.
};


/**
 * @brief Options menu.
 * @ingroup sample_app
 */
class ap_option {
    
public:
    /// @brief Trivial; state is established by init().
    ap_option() {};
    /**
     * @brief Builds the scene and seeds ap_option::op_dt from the saved settings.
     * @note Rows without a saved setting fall back to their `op_cfg[]` minimum.
     */
    void init(void);
    /// @brief One frame: row and value input, the row actions, then the menu text.
    void main();

private:
    /// @brief Restarts the five-minute idle timeout. Called on every key press.
    void resetDemoTime();

	uint8_t option_sel;               ///< Highlighted row, 0 to #OPTION_MENU_MAX - 1.
	unsigned long demo_time;          ///< `micros()` when the timeout last restarted.
	uint8_t op_dt[ OPTION_MENU_MAX ]; ///< Current value of each row.

};

extern ap_option ap_op; ///< The one options-menu instance.

#endif

