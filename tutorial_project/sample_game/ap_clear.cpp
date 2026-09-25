/*
    ap_clear.cpp
 */

/**
 * @file ap_clear.cpp
 * @brief Stage-clear screen.
 * @ingroup sample_app
 *
 * @note Configures the model through ap_demo0.h's `OBJ_DEM_MODEL` and then
 *       adjusts it through its own `OBJ_CLR_MODEL`. Both are 0, so the two names
 *       refer to the same slot and the screen works.
 */


#include "ap_main.h"

ap_clear ap_cl;


/// @brief NES palette for the stage-clear screen. @ingroup sample_app
const uint8_t pal_clear[] = {
	// BG PAL
	0x0F,0x15,0x11,0x30,
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
const uint8_t atr_over[] = {
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


void ap_clear::init(void) {

//	Serial.printf("ap_clear:init\n" );
  /* Pass the canvas to the OpenGL environment */
  glUseCanvas(&c);
  

  glClear(GL_COLOR_BUFFER_BIT);
  glPointSize(4);
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

//  glOrtho(-5, 5, -5, 5, 0.1, 999.f);
  gluPerspective(30.0, c.width()/c.height(), 0.1f, 9999.f);

  glMatrixMode(GL_MODELVIEW);

	sys.setPF_COM( PF_COM_BGM + BGM_CLEAR );


	demo_time = micros();

	ap.m_timer = 0;

	sys.setPalData( pal_clear );
	sys.startDataMode();

	initLookAt( 0 );
//	setLight( 1.0f, 0.0f, 0.0f);
	setLight( 1.0f, 0.0f, 0.5f);

	ap.initObj();
	setModelDataObj( &ap.m_obj[ OBJ_DEM_MODEL ], MDL_PLAYER_NO );
	ap.m_obj[ OBJ_CLR_MODEL ].m_scale = 1.50f;
//	ap.m_obj[ OBJ_CLR_MODEL ].setAngle( -64, 128, 0 );
	ap.m_obj[ OBJ_CLR_MODEL ].m_angle_x = 64 + 32;
	ap.m_obj[ OBJ_CLR_MODEL ].m_angle_vy = 4;


	ap.initStarObj( OBJ_DEM_STAR, 80,  -1.0f );

	snd.playBGM( BGM_CLEAR );

}


void ap_clear::main() {

	bool exit = false;
	
	if ( (ap.m_timer & 0x7f) == 2 ) {
		ap.m_obj[ OBJ_CLR_MODEL ].m_angle_vy = random( -2, 2);
		ap.m_obj[ OBJ_CLR_MODEL ].m_angle_vx = random( -2, 2);
		ap.m_obj[ OBJ_CLR_MODEL ].m_angle_vz = random( -2, 2);
	}


	ap.move();

	ap.moveStarObj();

  glClear(GL_COLOR_BUFFER_BIT); 

	// テスト用グリッド
/*
	c.setDefCol( 1 );

	c.drawLine( 0,1, 255,1 );
	c.drawLine( 0,239, 255,239 );
	c.drawLine( 0,0, 0,239 );
	c.drawLine( 255,0,255,239 );
	c.drawLine( 0,0, 255,239 );
	c.drawLine( 0,239, 255,0 );
*/
	ap.draw();



  /* Ask the PDC8544 display to print our frame buffer */
//  display.printBitmap(c.bitmap());

	if ( ap.m_timer > 60*5 ) {
		if ( sys.getKeyTrg() ) {
			exit = true;
		}
	}


	if ( ap.m_timer > 60*23 ) {
		exit = true;
	}

	if ( exit ) {
		sys.setPF_COM( PF_COM_BGM + BGM_STOP );
		snd.stopBGM();
		snd.playSE( SE_CUR_ENT );
		sys.FadeOut();
		sys.SleepMS( 500 );
		if ( emu.m_RAM[ PLY_STAGE ] == STAGE_MAX ) {
			ap.setStep( ST_TITLE );
		} else {
			emu.m_RAM[ PLY_STAGE ]++;
			ap.setStep( ST_GAME );
			return;
		}
	}




	{
		char buffer[40]; // バッファを確保
		sprintf(buffer, "STAGE %02d CLEAR", emu.m_RAM[ PLY_STAGE ] );
		c.drawString( buffer, 8*10-4, 8*6, _font );

		sprintf(buffer, "SCORE %02x%02x%02x%02x0"
			, emu.m_RAM[ GM_SCORE + 3 ], emu.m_RAM[ GM_SCORE + 2 ], emu.m_RAM[ GM_SCORE + 1 ], emu.m_RAM[ GM_SCORE + 0 ]
		);
		c.drawString( buffer, 8*9, 8*10, _font );
	}

}


