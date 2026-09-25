/*
 */
 
 
#pragma once
 

#include "rp_sound.h"

enum {
	MP3_BUFSIZE = 512,
 
};

 
class rp_sound {
 
public:
	void init();
	void jobSound();

	void playBGM( uint8_t bgmno );
	void stopBGM();
	void playSE( uint8_t seno );
	void stopSE();
	
	void setReg( uint16_t addr, uint8_t value );

	void setMP3( const uint8_t* mp3data, int size, bool loop );
	void stopMP3();
	void jobMP3();

	uint8_t   m_MP3_ENA;
	uint8_t   m_MP3_VOL;


private:
	uint8_t   m_APU[ 0x18 ];
	uint8_t   m_BgmRQ;		// BGMリクエスト用
	uint8_t   m_SeRQ[8];	// SEリクエスト用

	const uint8_t*  m_pMP3data;	// MP3データアドレス
	int m_MP3_idx;		// MP3データ インデックス
	int m_MP3_size;		// MP3データ サイズ
	bool m_bMP3loop;	// MP3ループ再生
	int m_loopWait;		// ループ再生時のウェイト


};
 
extern rp_sound snd;

