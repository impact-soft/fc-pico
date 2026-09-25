/*
    ap_game.h
 */

/**
 * @file ap_game.cpp
 * @brief The play screen: drives the emulated game ROM and renders its state.
 * @ingroup sample_app
 *
 * One frame here is: post the controller bytes into the emulated RAM, run the
 * ROM's `$E004` entry until it hits `brk`, then walk the object tables it left
 * behind and rebuild the 3D scene from them.
 *
 * Sound is relayed the same way. The ROM raises a request byte, this side plays
 * it and writes the byte back to zero, which is the acknowledgement.
 */


#include "ap_main.h"

#define DEG2RAD (M_PI/180.0)

ap_game ap_g;


#include "ap_game_init.h"
#include "ap_game_main.h"
#include "ap_game_move.h"
#include "ap_game_hit.h"
#include "ap_game_camera.h"
#include "ap_game_draw.h"

