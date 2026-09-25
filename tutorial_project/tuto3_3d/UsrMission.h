/**
 * @file UsrMission.h
 * @brief
 * @ingroup sample_app
 *
 *
 * @see @ref sample_game
 */

#pragma once

#include "_build_option.h"

#include "res/res_id.h"
#include "res/res_id2.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"
//#include "sys/rp_Misson.h"




extern void UsrMisson_PG_CALL( uint16_t pc, uint8_t a, uint8_t y, const uint8_t *tmpAdr );
extern bool UsrMisson_mcj_sub( uint8_t mcj );
extern const uint8_t *getEnemyNTcfg( uint8_t kind );
extern void MissionAnime();


