/*
    ap_debug.cpp
*/

/**
 * @file ap_debug.cpp
 * @brief Debug menu: stage select, sound test, MP3 settings and the save.
 * @ingroup sample_app
 *
 */


#include "ap_main.h"


ap_debug ap_db;



/// @brief NES palette for the debug menu. @ingroup sample_app
const uint8_t pal_debug[] = {
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
const uint8_t db_cfg[] = {
	// MIN,MAX
	1,3,
	0,SND_SEL_MAX-1,
	0, 99,		// ミッションタイプ選択
	0, 99,		// ミッションタイプ サブ選択
	0,99,		// モデルビューワー
	0,0,
};


void ap_debug::init(void) {


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

	debug_sel = 0;
	ap.m_timer = 0;

	sys.clearAtrData();
	sys.setPalData( pal_debug );
	sys.startDataMode();

	initLookAt( 0 );
	setLight( 1.0f, 0.0f, 0.5f);

	ap.initObj();
	ap.initStarObj( 0, 80,  -1.0f );


	for ( int i=0; i < DEBUG_MENU_MAX ; i++ ) {
		db_dt[ i ] = db_cfg[i*2];
	}

//	db_dt[ 2 ] = sys.SaveData[ SDT_MP3_ENA ];
//	db_dt[ 3 ] = sys.SaveData[ SDT_MP3_VOL ];
//	db_dt[ 4 ] = sys.SaveData[ SDT_GAME_MODE ];

//	if ( db_dt[ 4 ] > 1 ) {
//		db_dt[ 4 ] = 0;
//	}

}


void ap_debug::main() {

	glClear(GL_COLOR_BUFFER_BIT); 


	//--------------------------------
	// select menu items
	//--------------------------------
	if ( sys.getKeyTrg() & KEY_UP ) {
		debug_sel += (DEBUG_MENU_MAX-1);
		snd.playSE( SE_CUR_SEL );
	}
	if ( sys.getKeyTrg() & KEY_DOWN ) {
		debug_sel++;
		snd.playSE( SE_CUR_SEL );
	}
	debug_sel = (debug_sel +DEBUG_MENU_MAX) % DEBUG_MENU_MAX;


	if ( sys.getKeyTrg() & KEY_RIGHT ) {
		if ( db_dt[ debug_sel ] >= db_cfg[debug_sel*2+1] ) {
			db_dt[ debug_sel ] = db_cfg[debug_sel*2];
		} else {
			db_dt[ debug_sel ]++;
		}
	}

	if ( sys.getKeyTrg() & KEY_LEFT ) {
		if ( db_dt[ debug_sel ] == db_cfg[debug_sel*2] ) {
			db_dt[ debug_sel ] = db_cfg[debug_sel*2+1];
		} else {
			db_dt[ debug_sel ]--;
		}
	}

	snd.m_MP3_ENA = db_dt[ 2 ];
	snd.m_MP3_VOL = db_dt[ 3 ];


	if ( sys.getKeyTrg() & ( KEY_A | KEY_B) ) {
		switch( debug_sel ) {
		case 0:
			snd.playSE( SE_CUR_ENT );
			sys.FadeOut();
			sys.SleepMS( 500 );
			ap.PLY_STAGE = db_dt[ 0 ];
			ap.m_DemoFG = 0;
			ap.GM_SCORE = 0; // スコアクリアー
			ap.setStep( ST_GAME );
			break;

		case 1:
			if ( db_dt[ 1 ] == 0) {
				snd.stopBGM();
				snd.stopMP3();
			} else {
				snd.playBGM( db_dt[ 1 ] );
			}
			break;
		case 2:
		case 3:
			snd.playSE( SE_CUR_ENT );
			sys.FadeOut();
			sys.SleepMS( 500 );
			ap.PLY_STAGE = db_dt[ 0 ];
			ap.m_DemoFG = 0;
			ap.GM_SCORE = 0; // スコアクリアー
			ap.setStep( ST_DEBUG_MT );
			break;

		case 4:	// モデルビューワー

			break;


		case 5:	// EXIT
			snd.playSE( SE_CUR_ENT );
			sys.FadeOut();
			sys.SleepMS( 500 );
			ap.setStep( ST_TITLE );
			break;
		}
	
	}


	ap.move();

	ap.moveStarObj();

	ap.draw();


	c.drawString( "[ DEBUG MODE ]", 8*9, 8*5, _font );
	{
		char buffer[40];
		sprintf(buffer, "STAGE  %02d", db_dt[ 0 ] );
		c.drawString( buffer, 8*12, 8*8, _font );
	}
	{
		char buffer[40];
		sprintf(buffer, "SOUND  %02d", db_dt[ 1 ] );
		c.drawString( buffer, 8*12, 8*10, _font );
	}
	{
		char buffer[40];
		sprintf(buffer, "M TYPE %02d", db_dt[ 2 ] );
		c.drawString( buffer, 8*12, 8*12, _font );
	}
	{
		char buffer[40];
		sprintf(buffer, "MT SUB %02d", db_dt[ 3 ] );
		c.drawString( buffer, 8*12, 8*14, _font );
	}
	{
		char buffer[40];
		sprintf(buffer, "MODEL  %02d", db_dt[ 4 ] );
		c.drawString( buffer, 8*12, 8*16, _font );
	}

	c.drawString( "EXIT",  8*14, 8*20, _font );


	if ( ap.m_timer & 0x08 ) {
		const uint8_t sx[] = {
			8*10,
			8*10,
			8*10,
			8*10,
			8*10,
			8*12,
		};
		const uint8_t sy[] = {
			8*8,
			8*10,
			8*12,
			8*14,
			8*16,
			8*20,
		};
		
		c.drawCHR( 0x01, sx[debug_sel], sy[debug_sel], _font );
	}

}


