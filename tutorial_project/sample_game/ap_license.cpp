/*
    ap_license.cpp
*/

/**
 * @file ap_license.cpp
 * @brief Licence screen: two BPE-compressed nametable pages.
 * @ingroup sample_app
 *
 * Each page is a full 32x30 NES nametable assembled from
 * `res/NamLicense0.asm` and `NamLicense1.asm`, compressed by `res/bpe_asm.bat`
 * and stored in the archive. The page is decompressed into a 1152-byte stack
 * buffer and drawn one tile at a time, every frame.
 *
 * The page number lives in ap_main's sub-step, so it is 1 or 2 and the archive
 * id is formed as `BPE_NAMLICENSE0 - 1 + sel`.
 */


#include "ap_main.h"


ap_license ap_li;



/// @brief NES palette for the licence screen. @ingroup sample_app
const uint8_t ap_pal[] = {
	// BG PAL
	0x0F,0x01,0x15,0x20,
	0x0F,0x2A,0x2A,0x2A,
	0x0F,0x12,0x2C,0x30,
	0x0F,0x1A,0x1A,0x1A,
	// OBJ PAL
	0x0F,0x0F,0x20,0x3c,
	0x0F,0x00,0x10,0x20,
	0x0F,0x15,0x19,0x20,
	0x0F,0x21,0x10,0x20
};

/**
 * @brief Unused. Inherited verbatim from the ap_option screen this one was copied from.
 * @ingroup sample_app
 * @warning Six pairs against ap_license::op_dt's seven entries. Harmless only
 *          because neither table is read here; the ap_option.cpp original has
 *          the seventh pair. @see @ref sample_game
 */
const uint8_t op_cfg[] = {
	// MIN,MAX
	1,3,
	0,SND_SEL_MAX-1,
	0,1,
	0,40,
	0,1,
	0,0,
};

void ap_license::resetDemoTime(void) {
	demo_time = micros();
}

void ap_license::init(void) {


  snd.stopBGM();
  /* Pass the canvas to the OpenGL environment */
  glUseCanvas(&c);
  

  glClear(GL_COLOR_BUFFER_BIT);
  glPointSize(4);
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

//  glOrtho(-5, 5, -5, 5, 0.1, 999.f);
  gluPerspective(30.0, c.width()/c.height(), 0.1f, 9999.f);

  glMatrixMode(GL_MODELVIEW);

	sys.setPF_COM( PF_COM_BGM + BGM_STOP );

	resetDemoTime();
	option_sel = 0;
	ap.m_timer = 0;

	sys.clearAtrData();
	sys.setPalData( ap_pal );
	sys.startDataMode();

	initLookAt( 0 );
	setLight( 1.0f, 0.0f, 0.5f);

	ap.initObj();
}


void ap_license::main() {

	glClear(GL_COLOR_BUFFER_BIT); 

	uint8_t sel = ap.getStepSub();
	bool bExit = false;

	//--------------------------------
	// time out
	//--------------------------------
	if( (micros() - demo_time ) > 60 * 1 * MICROS_1S ) {
		bExit = true;
	}

	if ( sys.getKeyTrg() & ( KEY_UP | KEY_DOWN) ) {
		if ( sel == 1 ) {
			sel = 2;
		} else {
			sel = 1;
		}
		ap.setStepSub( sel );
		resetDemoTime();
		snd.playSE( SE_CUR_SEL );
	}

	if ( sys.getKeyTrg() & ( KEY_A | KEY_B ) ) {
		resetDemoTime();
		if ( sel == 1 ) {
			sel = 2;
			ap.setStepSub( sel );
			snd.playSE( SE_CUR_SEL );
		} else {
			bExit = true;
		}
	}


	if( bExit ) {
		snd.playSE( SE_CUR_ENT );
		snd.stopBGM();
		sys.FadeOut();
		sys.SleepMS( 500 );
		ap.setStep( ST_TITLE );
	}


	{ // BPE TEXT DRAW
		char wbuf[ 0x480 ];
		bpe_decode( (uint8_t*)getResData( BPE_NAMLICENSE0 -1 + sel ), (uint8_t*)wbuf );
		for ( int i = 0; i < 0x400 ; i++ ) {
			Serial.printf( "%c", wbuf[i] );
		}


		for ( int i = 0; i < 32*30; i++ ) {
			char d = wbuf[i];
			if ( d == 0 ) continue;
			c.drawCHR( d,  (i % 32) * 8, (i /32) *8 , _font );
		}
	
	}



}


