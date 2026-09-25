/**
 * @file UsrObj3d.h
 * @brief
 * @ingroup sample_app
 *
 *
 * @see @ref sample_game
 */

#ifndef UsrObj3d_H
#define UsrObj3d_H

#include "_build_option.h"

#include "res/res_id.h"
#include "res/res_id2.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"
#include "sys/Obj3d.h"


enum {
	OMD_FIELD = OMD_USR,	// フィールド描画モード

};

#define P_SIZE 2.0f
#define FLD_PW	16
#define FLD_PH	16

#define FLD_LIM_W	(P_SIZE * (FLD_PW/2))
#define FLD_LIM_H	(P_SIZE * (FLD_PH/2))




#endif

