/**
 * @file system.h
 * @brief The unity build: pulls every translation unit into one compilation.
 * @ingroup sample_app
 *
 * The same arrangement as the tutorial firmware's system.h, with `sys/rp_bpe.cpp`
 * added for the compressed licence pages. The platform sources under `sys/` are
 * brought in by textual inclusion rather than separate compilation.
 *
 * The `ap_*.cpp` files are **not** listed here. They sit in the sketch directory,
 * so the Arduino IDE compiles them as translation units in their own right. That
 * is why a `const` table such as `op_cfg[]` can be defined in both ap_option.cpp
 * and ap_license.cpp without colliding.
 *
 * @warning **Include order matters.** rp_core0.h and rp_core1.h define the Arduino
 *          entry points and reference the global instances (`sys`, `ap`, `snd`)
 *          defined at the bottom of the `.cpp` files above them. They must stay
 *          last.
 * @see @ref sample_game, @ref architecture
 */

#include "_build_option.h"

#include "sys/rp_system.h"

#include "ap_data.h"
#include "ap_main.h"

#include "sys/ArduinoGL.cpp"
#include "sys/Canvas.cpp"
#include "sys/Obj3d.cpp"
#include "sys/rp_debug.cpp"
#include "sys/rp_dma.cpp"
#include "sys/rp_fcemu.cpp"
#include "sys/rp_nsfplayer.cpp"
#include "sys/rp_sound.cpp"
#include "sys/rp_system.cpp"
#include "sys/rp_bpe.cpp"



#include "sys/rp_core0.h"
#include "sys/rp_core1.h"

