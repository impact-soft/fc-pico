/**
 * @file _build_option.h
 * @brief Retired build switches, kept for the record.
 * @ingroup sample_app
 *
 * `LINK_MP3` used to decide whether the MP3 tracks were linked into the
 * firmware. They are now uploaded separately with `picotool` and addressed
 * through #RES_DATA_ADR, so the switch was withdrawn; the note below says so in
 * the author's words. The header is still included widely and is now empty of
 * effect.
 *
 * @see @ref generated_resources
 */

#ifndef _build_option_h
#define _build_option_h


/*
	デバッグ用コンパイルの可否
*/
//#define NDEBUG	1


/*
	ユーザー定義の３Dオブジェクトを使う場合定義を有効にする

*/

#define USR_OBJ_3D


/*
	ユーザー定義のミッション処理を使う場合定義を有効にする

*/

#define USR_MISSON



#endif

