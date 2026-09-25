/**
 * @file ap_license.h
 * @brief Licence screen: two pages of text, stored BPE-compressed.
 * @ingroup sample_app
 *
 * The pages are not glyph strings but whole 32x30 NES nametables, assembled by
 * `res/NamLicense0.asm` and `NamLicense1.asm`, compressed with `bpe_fc`, and
 * decompressed here into a stack buffer one frame at a time.
 *
 * @note This header has no include guard problem of its own but does depend on
 *       ap_option.h having been included first: it sizes ap_license::op_dt with
 *       `OPTION_MENU_MAX`, which ap_option.h defines. ap_main.h includes them in
 *       the order that makes this work.
 */

#ifndef ap_license_h
#define ap_license_h

#include "_build_option.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"



/**
 * @brief Licence screen.
 * @ingroup sample_app
 *
 * A copy of the ap_option skeleton with the menu machinery left in place but
 * unused; only #demo_time and the page selector do any work here. The page
 * number is kept in ap_main's sub-step rather than in a member of this class.
 */
class ap_license {
    
public:
    /// @brief Trivial; state is established by init().
    ap_license() {};
    /// @brief Builds the scene and stops the music.
    void init(void);
    /**
     * @brief One frame: page selection, then decompress and draw the page.
     * @warning Decompresses the whole page and writes all 1024 bytes to the serial
     *          port on every frame. That is left-over debug output, and it is slow
     *          enough to be visible.
     */
    void main();

private:
    /// @brief Restarts the sixty-second idle timeout.
    void resetDemoTime();

	uint8_t option_sel;               ///< Written on entry, never read. The page number lives in ap_main's sub-step.
	unsigned long demo_time;          ///< `micros()` when the timeout last restarted.
	uint8_t op_dt[ OPTION_MENU_MAX ]; ///< Unused. Inherited from the ap_option skeleton this screen was copied from.

};

extern ap_license ap_li; ///< The one licence-screen instance.

#endif

