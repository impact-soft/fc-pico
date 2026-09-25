/*
    ap_title.cpp
*/

/**
 * @file ap_title.cpp
 * @brief Title screen: menu, attract-mode countdown and the tumbling logo.
 * @ingroup sample_app
 *
 * The logo flies in from `m_z = -20` and is stopped by hand once it reaches the
 * origin, rather than by any animation system; every 128 frames it is given a
 * fresh random tumble.
 */


#include "ap_main.h"


ap_title ap_t;


/// @brief NES palette for the title screen. @ingroup sample_app
const uint8_t pal_title[] = {
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

/*
const uint8_t atr_title[] = {
//	0x20,0x30,0x40,0x00,0x00,0x00,0x00,0x00,
//	0xaa,0xaa,0xaa,0xaa,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
*/

void ap_title::resetDemoTime(void) {
	demo_time = micros();
}

void ap_title::init(void) {

  snd.stopBGM();
  /* Pass the canvas to the OpenGL environment */
  glUseCanvas(&c);
  
//  char *s;
//  s = (char *)0x10200000;
//	Serial.printf("%s\n", s);


  glClear(GL_COLOR_BUFFER_BIT);
  glPointSize(4);
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

//  glOrtho(-5, 5, -5, 5, 0.1, 999.f);
  gluPerspective(30.0, c.width()/c.height(), 0.1f, 9999.f);

  glMatrixMode(GL_MODELVIEW);

	sys.setPF_COM( PF_COM_BGM + BGM_STOP );

	resetDemoTime();
	title_sel = 0;
	ap.m_timer = 0;

	sys.clearAtrData();
//	sys.setAtrData( atr_title );
//	sys.setAtr( 0, 0, 1 );
//	sys.setAtr( 1, 1, 2 );
//	sys.setAtr( 2, 2, 3 );
//	sys.setAtr( 3, 3, 1 );
	sys.setPalData( pal_title );
	sys.startDataMode();

	initLookAt( 0 );
//	setLight( 1.0f, 0.0f, 0.0f);
	setLight( 1.0f, 0.0f, 0.5f);

	ap.initObj();

	Obj3d *obj = &ap.m_obj[ OBJ_TIT_LOGC ];
	setModelDataObj( obj, MDL_TITLE_LOGO );
	obj->m_color = 2;
	obj->m_scale = 0.06f;
	obj->setAngle( 0, 128, 0 );
	obj->m_y = 1.5f;
	obj->m_z = -20.0f;
	obj->m_vz = 1.0f;
	
//	ap.initStarObj( OBJ_TIT_CUBE, 80,  -2.0f );
	ap.initStarObj( OBJ_TIT_CUBE, 80,  -1.0f );


}


void ap_title::main() {

	TRACE(DTR_TITLE)

	glClear(GL_COLOR_BUFFER_BIT); 

	TRACE(DTR_TITLE)
	//--------------------------------
	// 15 seconds goto demo
	//--------------------------------
	if( (micros() - demo_time ) > 10 * MICROS_1S ) {
//	if( (micros() - demo_time ) > 3 * MICROS_1S ) {
		TRACE(DTR_TITLE)
		sys.FadeOut();
		sys.SleepMS( 500 );
		memset( &emu.m_RAM[GM_SCORE], 0, 4); // スコアクリアー
		TRACE(DTR_TITLE)
		ap.m_DemoFG++;
		if ( ap.m_DemoFG == 3 ) {
			ap.m_DemoFG = 1;
		}
		ap.setStep( ST_DEMO0 );
	}

	//--------------------------------
	// select menu items
	//--------------------------------
	if ( sys.getKeyTrg() & (KEY_UP | KEY_DOWN)) {
		resetDemoTime();
		title_sel ^= 1;
		snd.playSE( SE_CUR_SEL );
	}

/*
	if ( sys.getKeyTrg() & ( KEY_A | KEY_B) ) {
		// ウォッチドック強制発動
		sleep_ms( 5000 );
	}
*/

/*
	if ( sys.getKeyTrg() & ( KEY_A | KEY_B) ) {
		ASSERT(0);
	}
*/
	if ( sys.getKeyTrg() & ( KEY_A | KEY_B) ) {
		snd.playSE( SE_CUR_ENT );
		sys.FadeOut();
		sys.SleepMS( 500 );
		if( title_sel == 0 ) {
			emu.m_RAM[EM_PLY_STAGE] = 1;
			ap.m_DemoFG = 0;
			memset( &emu.m_RAM[GM_SCORE], 0, 4); // スコアクリアー

			ap.setStep( ST_GAME );
//			ap.setStep( ST_CLEAR );
//			ap.setStep( ST_OVER );
		} else {
			ap.setStep( ST_OPTION );
		}
	}

	// パレット&BGアトリビュート更新システム動作テスト
/*
	for( int y=0; y < 240; y+=8 ) {
		for( int x=0; x < 256; x+=8 ) {
			c.drawCHR( 0x02, x, y, _font );
		}
	}
	sys.setPal( 1, random(0,0x40) );
	sys.setAtr( random(0,16), random(0,15), timer & 3 );
*/

	TRACE(DTR_TITLE)


	if ( ap.m_obj[ OBJ_TIT_LOGC ].m_angle_y == 128 ) {
		ap.m_obj[ OBJ_TIT_LOGC ].m_angle_vy = 0;
	}

	if ( (ap.m_timer & 0x7f) == 0x40 ) {
		ap.m_obj[ OBJ_TIT_LOGC ].m_angle_vy = random( -3, 3);
;		ap.m_obj[ OBJ_TIT_LOGC ].m_angle_vx = random( -3, 3);
		ap.m_obj[ OBJ_TIT_LOGC ].m_angle_vz = random( -3, 3);
	}

	ap.move();

	if ( ap.m_obj[ OBJ_TIT_LOGC ].m_z > 0.f ) {
		ap.m_obj[ OBJ_TIT_LOGC ].m_z = 0;
		ap.m_obj[ OBJ_TIT_LOGC ].m_vz = 0;
	}

	ap.moveStarObj();


	ap.draw();


	c.drawString( "FC-PICO TYPE ZERO V1.00", 40, 20, _font );
	c.drawString( "GAME START", 8*11, 8*20, _font );
	c.drawString( "OPTION", 8*13,  8*22, _font );

	c.drawString( "~IMPACT SOFT", 8*18, 8*24, _font );
	c.drawString( "2026 V1.06F",8*20, 8*26, _font );

	TRACE(DTR_TITLE)

	if ( ap.m_timer & 0x08 ) {
		if( title_sel == 0 ) {
			c.drawCHR( 0x01, 8*11 - 12, 8*20, _font );
		} else {
			c.drawCHR( 0x01, 8*13 - 12, 8*22, _font );
		}
	}


#if 0
	{
		char buffer[40]; // バッファを確保
		sprintf(buffer, "LT:%f", c.debug_flt );
		c.drawString( buffer, 80, 40, _font );
	}
#endif

#if 0
	{
		char buffer[40]; // バッファを確保
		sprintf(buffer, "LT:%f D:%d", c.debug_flt, c.debug_u8t );
		c.drawString( buffer, 80, 40, _font );
		sprintf(buffer, "db:%f %f", c.debug_f2, c.debug_f3 );
		c.drawString( buffer, 80, 48, _font );
		sprintf(buffer, "%f %f %f", c.dv0[0], c.dv0[1], c.dv0[2] );
		c.drawString( buffer, 16, 48+8*1, _font );
		sprintf(buffer, "%f %f %f", c.dv1[0], c.dv1[1], c.dv1[2] );
		c.drawString( buffer, 16, 48+8*2, _font );
	}
#endif
#if 0
//	String strBuf = "Hello, world";
//	c.drawString( strBuf.c_str(), 80, 60, _font );
	{
		char buffer[40]; // バッファを確保
		sprintf(buffer, "NEW:%02x TRG:%02x", sys.getKeyNew(), sys.getKeyTrg() );
		c.drawString( buffer, 80, 60, _font );
	}
#endif

	TRACE_END(DTR_TITLE)
}


