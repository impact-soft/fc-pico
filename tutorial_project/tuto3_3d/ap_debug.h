/**
 * @file ap_debug.h
 * @brief Debug menu: stage select
 * @ingroup sample_app
 *
 */

#ifndef ap_debug_h
#define ap_debug_h

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

	DEBUG_MENU_MAX = 6, ///< Number of menu rows, and the size of ap_option::op_dt.
};


/**
 * @brief Options menu.
 * @ingroup sample_app
 */
class ap_debug {
    
public:
    /// @brief Trivial; state is established by init().
    ap_debug() {};
    /**
     * @brief Builds the scene and seeds ap_option::op_dt from the saved settings.
     * @note Rows without a saved setting fall back to their `op_cfg[]` minimum.
     */
    void init(void);
    /// @brief One frame: row and value input, the row actions, then the menu text.
    void main();

	uint8_t debug_sel;               ///< Highlighted row, 0 to #OPTION_MENU_MAX - 1.
	uint8_t db_dt[ DEBUG_MENU_MAX ]; ///< Current value of each row.

private:

};

extern ap_debug ap_db; ///< The one debug-menu instance.

#endif

