/*
  Created by Fabrizio Di Vittorio (fdivitto2013@gmail.com) - <http://www.fabgl.com>
  Copyright (c) 2019-2022 Fabrizio Di Vittorio.
  All rights reserved.
 
 
* Please contact fdivitto2013@gmail.com if you need a commercial license.
 
 
* This library and related software is available under GPL v3.
 
  FabGL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  FabGL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with FabGL.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 
#pragma once
 
 
#include "rp_fcemu.h"

#define NSF_NESM	0x4D53454E	// ヘッダーチェック用

typedef struct {
	uint32_t NESM;				// denotes an NES sound format file 0x4D53454E
	uint8_t NESM_E;				// $1A
	uint8_t Version_number;		// $01 (or $02 for NSF2)
	uint8_t Total_songs;		// (1=1 song, 2=2 songs, etc)
	uint8_t Starting_song;		// (1=1st song, 2=2nd song, etc)
	uint16_t load_address;		// (lo, hi) ($8000-FFFF)
	uint16_t init_address;		// (lo, hi) ($8000-FFFF)
	uint16_t play_address;		// (lo, hi) ($8000-FFFF)
	uint8_t  str_song[32];		// The name of the song, null terminated
	uint8_t  str_artist[32];	// The artist, if known, null terminated
	uint8_t  str_copyright[32];	// The copyright holder, null terminated
	uint16_t Play_speed_N;		// (lo, hi) Play speed, in 1/1000000th sec ticks, NTSC (see text)
	uint8_t  Bankswitch[8];		// Bankswitch init values
	uint16_t Play_speed_P;		// (lo, hi) Play speed, in 1/1000000th sec ticks, PAL (see text)
	uint8_t PAL_NTSC_bits;		// PAL/NTSC bits 
//                bit 0: if clear, this is an NTSC tune
//                bit 0: if set, this is a PAL tune
//                bit 1: if set, this is a dual PAL/NTSC tune
//                bits 2-7: reserved, must be 0
	uint8_t Extra_Sound;		// Extra Sound Chip Support
//                bit 0: if set, this song uses VRC6 audio
//                bit 1: if set, this song uses VRC7 audio
//                bit 2: if set, this song uses FDS audio
//                bit 3: if set, this song uses MMC5 audio
//                bit 4: if set, this song uses Namco 163 audio
//                bit 5: if set, this song uses Sunsoft 5B audio
//                bit 6: if set, this song uses VT02+ audio
//                bit 7: reserved, must be zero
	uint8_t RsvNSF2[4];		// Reserved for NSF2t

} NSF_HEADER;

enum {
	__ptr = 0x05,	// 汎用ポインタ 2byte
/*
	__tmp = 0x07,
*/

	__flag = 0x0A,
/*
	lda	#nsd_flag::BGM + nsd_flag::SE
	sta	__flag		;BGM, SE処理を禁止（RAM未初期化対策）

	__flag
		D... .... : 再生制御無効
		...J .... : 早送り中
		.... PP.. : 効果音の優先度
		.... ..S. : ＳＥ再生中
		.... ...B : ＢＧＭが再生中 
*/

	_eff  = 0x200,	// 効果音テーブル開始番号
	_play = 0x201,	// =0 フレームオーバー防止用変数
	
/*
	ax = Pointer 		; x = Hadr / a = Ladr
*/

	_nsf_init = 0x8010,		// NSF init address 
	_nmi_main = 0x8084,		// NSF play address
	_nsd_init = 0x80A1,
	_nsd_set_dpcm = 0x80AB,	// ax = Pointer of ⊿PCM infomation Struct
	_nsd_main = 0x80B2,
	_nsd_play_bgm = 0x8137,	// ax = Pointer of BGM
	_nsd_stop_bgm = 0x8219,
	_nsd_play_se = 0x8239,	// ax = Pointer of SE
	_nsd_stop_se = 0x82B7,
	_nsd_snd_init = 0x8AB6,

	_nsd_table_idx = 0x8F6E,	//テーブルインデックス

/*
+$0000	B 効果音テーブル開始番号？
+$0002	W DPCM情報テーブルアドレス
+$0004～ 2バイト単位でBGM,SEのデータアドレス
*/

};


class rp_nsfplayer : public rp_fcemu {
 
public:
	void setNSF( const uint8_t *nsf );
	void setNSF_NSDLIB( const uint8_t *nsf );
	void play( uint8_t song_no );
	void playBGM( uint8_t song_no );
	void playSE( uint8_t song_no );
	void stopBGM();
	void stopSE();
	void main( );
private:
	NSF_HEADER *nsf_head;
	uint16_t nsd_data_addr(  uint8_t song_no );

};
 

extern rp_nsfplayer nsf;


