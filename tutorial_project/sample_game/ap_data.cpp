/*
    ap_data.cpp
 */

/**
 * @file ap_data.cpp
 * @brief Where the resource archive and the 3D model data land in flash.
 * @ingroup sample_app
 *
 * Includes the generated `res/resdata.c` -- the packed archive of boot ROM, game
 * ROM, music, artwork and licence pages -- plus one generated `.c` per 3D model.
 * The models are separate arrays rather than archive entries because they are
 * linked structures, not opaque blobs.
 *
 * @see @ref generated_resources
 */

#include "ap_data.h"

#include "res/resdata.c"


#include "res/fcpico.c"
#include "res/mdl_enemy.c"
#include "res/mdl_enemy_col.h"
#include "res/mdl_enemy2.c"
#include "res/mdl_enemy2_col.h"
#include "res/mdl_enemy3.c"
#include "res/mdl_enemy3_col.h"
#include "res/mdl_player.c"
#include "res/mdl_player_col.h"
#include "res/mdl_bullet.c"


const unsigned char *_font;  ///< Resolved by initResData(); declared in ap_data.h.
const unsigned char *_acOBJ; ///< Resolved by initResData(); declared in ap_data.h.


void initResData() {
	_acOBJ = getResData( CHR_OBJ );
	_font = getResData( CHR_FONT );
}

/**
 * @brief Selects which archive a resource id refers to.
 * @param resid Resource id; `resid / 10000` picks the archive, 0 being the one
 *              linked into the firmware and anything else #RES_DATA_ADR.
 * @return Pointer to the archive's index table: pairs of `int`, offset then size,
 *         one pair per entry, in `binlink.lst` order.
 * @note The offsets are relative to the start of the archive, which is why the
 *       table pointer doubles as the base pointer in getResData().
 * @ingroup sample_app
 */
int*  getResHead( int resid ) {
	int sel = resid / 10000;
	if ( sel == 0 ) {
		return (int*)_resdata;
	} else {
		return (int*)RES_DATA_ADR;
	}
}

const unsigned char* getResData( int resid ) {
	int *head = getResHead( resid );
	resid %= 10000;
	const unsigned char *resbuf = (const unsigned char *)head;
	return &resbuf[  head[ resid * 2 + 0] ];
}


int getResDataSize( int resid ) {
	int *head = getResHead( resid );
	resid %= 10000;
	return head[ resid * 2 + 1];
}


void setModelDataObj( Obj3d *obj, int no ) {
	obj->init();
	switch( no ) {
	case MDL_TITLE_LOGO:
		obj->setModelData( FCPICO, DM_FCPICO );
		break;
	case MDL_PLAYER_NO:
		obj->setModelData( MDL_PLAYER, DM_MDL_PLAYER, mdl_player_col );
		obj->m_angle_x = 64;
		obj->m_scale = 0.7;
//		obj->m_scale = 2.0;
		break;

	case MDL_ENEMY_NO:
		obj->setModelData( MDL_ENEMY, DM_MDL_ENEMY, mdl_enemy_col );
		obj->m_angle_x = -64;
		obj->m_scale = 0.7;
		break;

	case MDL_ENEMY2_NO:
		obj->setModelData( MDL_ENEMY2, DM_MDL_ENEMY2, mdl_enemy2_col );
		obj->m_angle_x = -64;
		obj->m_scale = 0.7;
		break;

	case MDL_ENEMY3_NO:
		obj->setModelData( MDL_ENEMY3, DM_MDL_ENEMY3, mdl_enemy3_col );
		obj->m_angle_x = -64;
		obj->m_scale = 0.7;
		break;
	}


}


void setMP3data( int no ) {

	int idx =  MP3_RES_ID + no -1;	//BGM_BOSS = 1
	const uint8_t* mp3data = getResData( idx );
	int size =  getResDataSize( idx );


	switch( no ) {
	case 1:
	case 2:
		snd.setMP3( mp3data, size, true );
		break;
	case 3:
	case 4:
		snd.setMP3( mp3data, size, false );
		break;

	default:
		snd.stopMP3();
		break;
	}

}


