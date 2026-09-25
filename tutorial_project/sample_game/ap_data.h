/**
 * @file ap_data.h
 * @brief Resource access: the packed archive, the 3D models and the MP3 tracks.
 * @ingroup sample_app
 *
 * Two archives are in play. The one built into the firmware holds the boot ROM,
 * the game ROM, the music, the artwork and the licence pages; a second, larger
 * one holds the MP3 tracks and is flashed separately at #RES_DATA_ADR. An id of
 * 10000 or more selects the second. See @ref generated_resources.
 *
 * @see @ref sample_game
 */

#ifndef ap_data_h
#define ap_data_h

#include "_build_option.h"

#include "res/res_id.h"
#include "res/res_id2.h"

#include <Arduino.h>
#include "sys/ArduinoGL.h"
#include "sys/Canvas.h"
#include "sys/rp_system.h"
#include "sys/Obj3d.h"


/**
 * @brief Flash address of the separately uploaded resource archive.
 * @ingroup sample_app
 *
 * `res/upload.bat` writes `res2.bin` here with `picotool load -o 0x10200000`.
 * The address is a raw XIP-mapped flash offset, so nothing in the build checks
 * that the firmware image has not grown into it.
 */
#define RES_DATA_ADR   0x10200000

/**
 * @brief Model identifiers accepted by setModelDataObj().
 * @ingroup sample_app
 * @note The three enemy identifiers are consecutive, and ap_demo0 relies on it:
 *       it cycles the parade with `MDL_ENEMY_NO + sel`.
 */
enum {
	MDL_TITLE_LOGO = 0, ///< The FC-PICO title logo, from `res/fcpico.stl`.
	MDL_PLAYER_NO,      ///< Player ship, from `res/mdl_player.stl`. Carries a collision table.
	MDL_ENEMY_NO,       ///< Enemy type 1, from `res/mdl_enemy.stl`.
	MDL_ENEMY2_NO,      ///< Enemy type 2, from `res/mdl_enemy2.stl`.
	MDL_ENEMY3_NO,      ///< Enemy type 3, from `res/mdl_enemy3.stl`.

};

//extern const unsigned char _font[4096];
/// @brief Text glyphs: 256 NES 2bpp tiles, from `res/font.chr`. Set by initResData(). @ingroup sample_app
extern const unsigned char *_font;
/// @brief Sprite sheet: 256 NES 2bpp tiles, from `res/OBJ.chr`. Set by initResData(). @ingroup sample_app
extern const unsigned char *_acOBJ;
/// @brief Vestigial: declared but never defined or referenced. The music is fetched
///        with `getResData( NSF_SOUND )` instead. @ingroup sample_app
extern const unsigned char *sound_nsf;


/**
 * @brief Resolves #_acOBJ and #_font out of the built-in archive.
 * @ingroup sample_app
 * @note Called from rp_system::init(). Both pointers are null until it runs.
 */
extern void initResData();

/**
 * @brief Returns a pointer to one resource inside an archive.
 * @param resid An id from `res/res_id.h` or `res/res_id2.h`; `resid / 10000`
 *              selects the archive, 0 being the one linked into the firmware.
 * @return Pointer into the archive. The data is read in place, never copied.
 * @note No bounds or signature check: an id past the end of an archive returns a
 *       pointer built from whatever the index table happens to hold there.
 * @note In this program both archives are real: ids from `res/res_id.h` reach the
 *       one linked into the firmware, and the `MP3_*` ids from `res/res_id2.h`
 *       reach the separately flashed `res2.bin` at #RES_DATA_ADR.
 * @ingroup sample_app
 */
extern const unsigned char* getResData( int resid );

/**
 * @brief Returns the byte length of one resource inside an archive.
 * @param resid Same encoding as getResData().
 * @return Length in bytes, as recorded in the archive's index table.
 * @note Used with the MP3 ids, where the decoder needs the length as well as the
 *       address; the fixed-size assets never ask for it.
 * @ingroup sample_app
 */
extern int getResDataSize( int resid );

/**
 * @brief Attaches mesh, colour and collision data for a model to an object.
 * @param obj Object to configure.
 * @param no One of the `MDL_*` identifiers.
 * @note Unlike the tutorial's stub, this one really loads geometry: one case per
 *       `MDL_*` identifier, each binding a generated vertex array and, for the
 *       ships, a collision table. It also sets a default orientation and scale,
 *       so a caller wanting its own must override them afterwards.
 * @ingroup sample_app
 */
extern void setModelDataObj( Obj3d *obj, int no );

/**
 * @brief Starts playback of one of the MP3 tracks from the uploaded archive.
 * @param no Track number, 1-based.
 * @note Live in this program, where the tutorial's copy is an empty stub. The
 *       track number is 1-based and maps onto the `BGM_*` slots: 1 and 2 loop,
 *       3 and 4 play once, anything else stops playback.
 * @warning Reads from #RES_DATA_ADR whether or not `res2.bin` was ever flashed
 *          there. Without it the decoder is handed whatever is in flash.
 * @ingroup sample_app
 */
extern void setMP3data( int no );

#endif

