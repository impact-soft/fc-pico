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

リソースをpicotoolでアップロードする仕組みが出来たのでこのビルドオプションは廃止

LINK_MP3

0: MP3データをリンクしない（MP3オプションに関係なくBGMを内蔵音源で再生する）
1: MP3データをリンクする
*/
//#define LINK_MP3  1


#endif
