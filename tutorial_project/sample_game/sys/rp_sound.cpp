#include "rp_system.h"

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/regs/rosc.h"


//#include <BackgroundAudio.h>
#include <BackgroundAudioMP3.h>
#include <PWMAudio.h>


extern void BLINK_LED();

#define PWM_AUDIO_L    (28)


// Create the PWM audio device on GPIO 1.   Hook amp/speaker between GPIO1 and convenient GND.
PWMAudio pwm( PWM_AUDIO_L );

//BackgroundAudioMixer<640> mixer(pwm, 44100);
//ROMBackgroundAudioWAV wav;
ROMBackgroundAudioMP3 BMP;


void rp_sound::init() {
	memset( m_APU, 0, sizeof(m_APU));
	memset( m_SeRQ, 0, sizeof(m_SeRQ));
	m_BgmRQ = 0;
	m_loopWait = 0;
	stopMP3();

	// NSFプレーヤー
	nsf.init();
	nsf.setNSF( getResData( NSF_SOUND ) );
//	wav.setDevice(mixer.add());
//	wav.setDevice( &pwm );
//	BMP.setDevice(mixer.add());
	BMP.setDevice( &pwm );

  // Start the background player
//	mixer.begin();
//	wav.begin();

}

void rp_sound::jobSound() {
	if ( m_BgmRQ == 255 ) {
		nsf.stopBGM();
		stopMP3();
	} else if ( m_BgmRQ > 0 ) {
		if ( m_MP3_ENA ) {
			if ( m_BgmRQ < (BGM_OVER +1) ) {
				setMP3data( m_BgmRQ );
			} else {
				stopMP3();
				nsf.playBGM( m_BgmRQ );
			}
		} else {
			stopMP3();
			nsf.playBGM( m_BgmRQ );
		}
	}
	m_BgmRQ = 0;

	for( int i = 0; i < sizeof(m_SeRQ); i++ ) {
		if ( m_SeRQ[i] == 0 ) continue;
		if ( m_SeRQ[i] == 255 ) {
			memset( m_SeRQ, 0, sizeof(m_SeRQ));
			nsf.stopSE();
			break;
		} else {
			nsf.playSE( m_SeRQ[i] );
		}
		m_SeRQ[i] = 0;
	}

	nsf.main();

}

void rp_sound::setMP3( const uint8_t* mp3data, int size, bool loop ) {
//	audio.playMP3(mp3data, size);
	m_pMP3data = mp3data;
	m_MP3_idx = 0;
	m_MP3_size = size;
	m_bMP3loop = loop;

	BMP.flush();
	BMP.begin();
	BMP.setGain( ( 1.0 / 40 ) * sys.SaveData[ SDT_MP3_VOL ] );
//@
	BMP.write( m_pMP3data, m_MP3_size );

//	Serial.printf("setMP3data %d\n", BMP.availableForWrite() );

}

void rp_sound::stopMP3() {
	BMP.flush();
	m_pMP3data = NULL;
	BMP.end();
}




void rp_sound::jobMP3() {
//	Serial.printf("jobMP3 %d\n", BMP.available() );
	if ( m_loopWait >  0 ) {
		m_loopWait--;
		if ( m_loopWait == 0 ) {
			//Serial.printf("jobMP3 loop\n" );
			// ループ再生
			setMP3( m_pMP3data, m_MP3_size, m_bMP3loop );
		}
		return;
	}

	
	if (m_pMP3data && (BMP.available() == 0) ) {
		if ( m_bMP3loop ) {
			m_loopWait = 60;
		} else {
			// 再生終了
			m_pMP3data = NULL;
//			stopMP3();
		}
	}

}



void rp_sound::playSE( uint8_t seno ) {
	for( int i = 0; i < sizeof(m_SeRQ); i++ ) {
		if ( m_SeRQ[i] != 0 ) continue;
		m_SeRQ[i] = seno;
		break;
	}
}


void rp_sound::playBGM( uint8_t bgmno ) {
	m_BgmRQ = bgmno;
}

void rp_sound::stopBGM( ) {
	m_BgmRQ = 255;
}

void rp_sound::stopSE( ) {
	memset( m_SeRQ, 0, sizeof(m_SeRQ));
	m_SeRQ[ 0 ] = 255;
}


void rp_sound::setReg( uint16_t addr, uint8_t value ) {
	if ( (addr >= 0x4000) && (addr <= 0x4017) ) {
		m_APU[ addr & 0x1F ] = value;
//		Serial.printf("SR 0x%04x %02x\n", addr , value );
	}
}



rp_sound snd;


