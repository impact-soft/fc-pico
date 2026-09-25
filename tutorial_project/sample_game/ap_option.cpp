/*
    ap_option.cpp
*/

/**
 * @file ap_option.cpp
 * @brief Options menu: stage select, sound test, MP3 settings and the save.
 * @ingroup sample_app
 *
 * Rows are described by `op_cfg[]` and held in ap_option::op_dt. Row 5 opens the
 * licence screen and row 6 saves and leaves; the rest only edit a value.
 *
 * @note Row 2's action falls through into row 3's on purpose -- toggling MP3
 *       playback also has to stop whatever is currently sounding.
 */


#include "ap_main.h"


ap_option ap_op;



/// @brief NES palette for the options menu. @ingroup sample_app
const uint8_t pal_option[] = {
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
 * @brief Inclusive minimum and maximum for each menu row, in row order.
 * @ingroup sample_app
 *
 * Seven pairs for #OPTION_MENU_MAX rows: stage, sound test, MP3 enable, MP3
 * volume, game mode, and then LICENSE and SAVE & EXIT, which take no value and
 * are pinned to `0,0`.
 *
 * @note Internal linkage, being `const` at namespace scope, so ap_license.cpp's
 *       table of the same name is a different object and the two do not collide.
 */
const uint8_t op_cfg[] = {
	// MIN,MAX
	1,3,
	0,SND_SEL_MAX-1,
	0,1,
	0,40,
	0,1,
	0,0,
	0,0,
};

void ap_option::resetDemoTime(void) {
	demo_time = micros();
}

void ap_option::init(void) {


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
	sys.setPalData( pal_option );
	sys.startDataMode();

	initLookAt( 0 );
	setLight( 1.0f, 0.0f, 0.5f);

	ap.initObj();
	ap.initStarObj( OBJ_TIT_CUBE, 80,  -1.0f );


	for ( int i=0; i < OPTION_MENU_MAX ; i++ ) {
		op_dt[ i ] = op_cfg[i*2];
	}

	op_dt[ 2 ] = sys.SaveData[ SDT_MP3_ENA ];
	op_dt[ 3 ] = sys.SaveData[ SDT_MP3_VOL ];
	op_dt[ 4 ] = sys.SaveData[ SDT_GAME_MODE ];

	if ( op_dt[ 4 ] > 1 ) {
		op_dt[ 4 ] = 0;
	}

}


void ap_option::main() {

	glClear(GL_COLOR_BUFFER_BIT); 

	//--------------------------------
	// time out
	//--------------------------------
	if( (micros() - demo_time ) > 60 * 5 * MICROS_1S ) {
		snd.playSE( SE_CUR_ENT );
		snd.stopBGM();
		sys.FadeOut();
		sys.SleepMS( 500 );
		ap.setStep( ST_TITLE );
	}

	//--------------------------------
	// select menu items
	//--------------------------------
	if ( sys.getKeyTrg() & KEY_UP ) {
		resetDemoTime();
		option_sel += (OPTION_MENU_MAX-1);
		snd.playSE( SE_CUR_SEL );
	}
	if ( sys.getKeyTrg() & KEY_DOWN ) {
		resetDemoTime();
		option_sel++;
		snd.playSE( SE_CUR_SEL );
	}
	option_sel = (option_sel +OPTION_MENU_MAX) % OPTION_MENU_MAX;


	if ( sys.getKeyTrg() & KEY_RIGHT ) {
		resetDemoTime();
		if ( op_dt[ option_sel ] >= op_cfg[option_sel*2+1] ) {
			op_dt[ option_sel ] = op_cfg[option_sel*2];
		} else {
			op_dt[ option_sel ]++;
		}
	}

	if ( sys.getKeyTrg() & KEY_LEFT ) {
		resetDemoTime();
		if ( op_dt[ option_sel ] == op_cfg[option_sel*2] ) {
			op_dt[ option_sel ] = op_cfg[option_sel*2+1];
		} else {
			op_dt[ option_sel ]--;
		}
	}

	snd.m_MP3_ENA = op_dt[ 2 ];
	snd.m_MP3_VOL = op_dt[ 3 ];


	if ( sys.getKeyTrg() & ( KEY_A | KEY_B) ) {
		resetDemoTime();
		switch( option_sel ) {
		case 0:
			snd.playSE( SE_CUR_ENT );
			sys.FadeOut();
			sys.SleepMS( 500 );
			emu.m_RAM[EM_PLY_STAGE] = op_dt[ 0 ];
			ap.m_DemoFG = 0;
			memset( &emu.m_RAM[GM_SCORE], 0, 4); // スコアクリアー
			ap.setStep( ST_GAME );
			break;

		case 1:
			if ( op_dt[ 1 ] == 0) {
				snd.stopBGM();
				snd.stopMP3();
			} else {
				snd.playBGM( op_dt[ 1 ] );
			}
			break;
		case 2:
			op_dt[ 2 ] ^= 1;
		case 3:
			snd.stopBGM();
			snd.stopMP3();
			break;

		case 4:
			break;

		case 5:	// LICENSE
			snd.playSE( SE_CUR_ENT );
			sys.FadeOut();
			sys.SleepMS( 500 );
			ap.setStep( ST_LICENSE );
			break;

		case 6:	// SAVE&EXIT
			snd.playSE( SE_CUR_ENT );
			sys.FadeOut();
			sys.SleepMS( 500 );
			{
				bool commit = false;
				if ( sys.SaveData[ SDT_MP3_ENA ] != op_dt[ 2 ] ) {
					sys.SaveData[ SDT_MP3_ENA ] = op_dt[ 2 ];
					commit = true;
				}
				if ( sys.SaveData[ SDT_MP3_VOL ] != op_dt[ 3 ] ) {
					sys.SaveData[ SDT_MP3_VOL ] = op_dt[ 3 ];
					commit = true;
				}
				if ( sys.SaveData[ SDT_GAME_MODE ] != op_dt[ 4 ] ) {
					sys.SaveData[ SDT_GAME_MODE ] = op_dt[ 4 ];
					commit = true;
				}
				if ( commit ) {
					sys.commitSaveData();
				}
			}
			ap.setStep( ST_TITLE );
			break;
		}
	
	}


	ap.move();

	ap.moveStarObj();

	ap.draw();


	c.drawString( "[ OPTION MODE ]", 8*9, 8*5, _font );
	{
		char buffer[40];
		sprintf(buffer, "STAGE %02d", op_dt[ 0 ] );
		c.drawString( buffer, 8*13, 8*8, _font );
	}
	{
		char buffer[40];
		sprintf(buffer, "SOUND %02d", op_dt[ 1 ] );
		c.drawString( buffer, 8*13, 8*10, _font );
	}
	{
		char buffer[40];
		if ( op_dt[ 2 ] == 0 ) {
			sprintf(buffer, "MP3 ENA OFF" );
		} else {
			sprintf(buffer, "MP3 ENA ON" );
		}
		c.drawString( buffer, 8*12, 8*12, _font );
	}
	{
		char buffer[40];
		sprintf(buffer, "MP3 VOL %02d", op_dt[ 3 ] );
		c.drawString( buffer, 8*12, 8*14, _font );
	}
	{
		char buffer[40];
		sprintf(buffer, "GAME MODE %d", op_dt[ 4 ] );
		c.drawString( buffer, 8*11, 8*16, _font );
	}

	c.drawString( "LICENSE",  8*13, 8*18, _font );

	c.drawString( "SAVE & EXIT",  8*11, 8*22, _font );

	if ( ap.m_timer & 0x08 ) {
		const uint8_t sx[] = {
			8*11,
			8*11,
			8*10,
			8*10,
			8*9,
			8*11,
			8*9,
		};
		const uint8_t sy[] = {
			8*8,
			8*10,
			8*12,
			8*14,
			8*16,
			8*18,
			8*22,
		};
		
		c.drawCHR( 0x01, sx[option_sel], sy[option_sel], _font );
	}

}


