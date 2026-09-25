/*
 NSDLIB専用 NSFプレイヤー
 
 */

#include "Arduino.h"
 
#include "rp_system.h"
#include "rp_nsfplayer.h"


/*

http://kevtris.org/nes/nsfspec.txt

nsf_init

これは非常に単純です。
希望の曲番号をアキュムレータにロードし、
1を減算した後、Xレジスタを設定してPAL（X=1）またはNTSC（X=0）を指定します。
単一規格の曲（つまりPALかNTSCのいずれか一方のみ）の場合、
Xレジスタの内容は影響しません。曲番号と
オプションのPAL/NTSC規格を読み込んだら、単にINITアドレスを呼び出します。
初期化が完了すると、RTSを実行するはずです

nsf_main

曲の初期化が完了すると、再生が可能になります。再生するには、
1秒間に数回、再生アドレスを呼び出すだけです。1秒あたりの呼び出し回数は、
ファイル内のオフセット006ehと006fhで決定されます。
これらのバイトは再生速度を1/1000000秒単位で示します。
「標準的な」60Hz再生速度の場合は、これを411ahに設定してください。

異なる再生速度を生成するには、次の式を使用します：



曲を初期化する「正しい」方法
---------------------------

1) 0000h-07ffh の全RAMをクリアする。

2) 6000h-7fffh の全RAMをクリアする。

3) サウンドレジスタを初期化：
   04000h-0400Fh に 00h、
   04010h に 10h、
   04011h-04013h に 00h を書き込む。

4) ボリュームレジスタ 04015h を 00fh に設定。

5) バンク化された曲の場合、ヘッダーからバンク値を
   5ff8-5fffh にロードする。

6) 対象の曲に対応するアキュムレータとXレジスタを設定する。

7) 音楽初期化ルーチンを呼び出す。


曲を再生する「正しい」方法
---------------------------

1) 速度指定語によって決定される周期的な間隔で、音楽の再生アドレスを呼び出します。使用する指定語は、
現在使用しているモード（PALまたはNTSC）によって決まります。


*/


uint16_t rp_nsfplayer::nsd_data_addr(  uint8_t song_no ) {
	uint16_t idx = _nsd_table_idx + 2 + song_no *2;
	return readByte( idx ) + (readByte( idx +1 )  << 8);
}


void rp_nsfplayer::setNSF( const uint8_t *nsf ) {
//	Serial.printf("nsf test %04x\n", sizeof(NSF_HEADER) );
	NSF_HEADER *head = (NSF_HEADER*)nsf;
	this->init();
	nsf_head = NULL;

	if ( head->NESM != NSF_NESM ) {
		Serial.printf("nsf NESM %04x Error\n", head->NESM );
		return;
	}
	nsf_head = head;
	this->setROM( &nsf[sizeof(NSF_HEADER) + head->load_address - 0x8000 ] );

}


void rp_nsfplayer::setNSF_NSDLIB( const uint8_t *nsf ) {
	this->setNSF( nsf );

}

void rp_nsfplayer::play( uint8_t song_no ) {
//	Serial.printf("nsf play %d\n", song_no );
	if ( nsf_head ) {
		this->run( _nsf_init, song_no -1,0,0 );
	}
}

void rp_nsfplayer::playBGM( uint8_t song_no ) {
//	Serial.printf("playBGM %d\n", song_no );
	if ( nsf_head ) {
		uint16_t addr = nsd_data_addr( song_no );
		this->run( _nsd_play_bgm, addr & 0xff, (addr >> 8) & 0xff,0 );
	}
}

void rp_nsfplayer::playSE( uint8_t song_no ) {
	if ( nsf_head ) {
		uint16_t addr = nsd_data_addr( song_no );
		this->run( _nsd_play_se, addr & 0xff, (addr >> 8) & 0xff,0 );
	}
}

void rp_nsfplayer::stopBGM() {
//	Serial.printf("stopBGM\n" );
	if ( nsf_head ) {
		this->run( _nsd_stop_bgm, 0,0,0 );
	}
}

void rp_nsfplayer::stopSE() {
//	Serial.printf("stopSE\n" );
	if ( nsf_head ) {
		this->run( _nsd_stop_se, 0,0,0 );
	}
}




void rp_nsfplayer::main( ) {
	if ( nsf_head ) {
		this->run( _nmi_main, 0,0,0 );
	}
}


rp_nsfplayer nsf;

