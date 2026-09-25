/*
    ap_demo0.cpp
 */

/**
 * @file ap_demo0.cpp
 * @brief Attract mode: a slow parade of the ship and enemy models.
 * @ingroup sample_app
 *
 * The subject changes on a fixed schedule -- the ship at frame 2, then an enemy
 * at frame 180 -- and the enemy cycles through the three types on each visit via
 * a function-local `static`, so it survives between runs of the screen.
 */


#include "ap_main.h"

ap_demo0 ap_d0;


/// @brief NES palette for the model parade. @ingroup sample_app
const uint8_t pal_demo0[] = {
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

/// @brief All-zero background attribute table: every tile uses palette set 0. @ingroup sample_app
const uint8_t atr_demo0[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};



void ap_demo0::init(void) {

  snd.stopBGM();

//	Serial.printf("ap_demo0:init\n" );
  /* Pass the canvas to the OpenGL environment */
  glUseCanvas(&c);
  

  glClear(GL_COLOR_BUFFER_BIT);
  glPointSize(4);
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

//  glOrtho(-5, 5, -5, 5, 0.1, 999.f);
  gluPerspective(30.0, c.width()/c.height(), 0.1f, 9999.f);

  glMatrixMode(GL_MODELVIEW);

	demo_time = micros();

	ap.m_timer = 0;

	sys.setAtrData( atr_demo0 );
	sys.setPalData( pal_demo0 );
	sys.startDataMode();

	initLookAt( 0 );
//	setLight( 1.0f, 0.0f, 0.0f);
	setLight( 1.0f, 0.0f, 0.5f);

	ap.initObj();
	ap.m_obj[ OBJ_DEM_MODEL ].m_angle_vy = 4;


//	ap.initStarObj( OBJ_DEM_STAR, 80,  -3.0f );
	ap.initStarObj( OBJ_DEM_STAR, 80,  -1.0f );

	setModelDataObj( &ap.m_obj[ OBJ_DEM_MODEL ], MDL_PLAYER_NO );
	ap.m_obj[ OBJ_DEM_MODEL ].m_scale *= 3;

}


void ap_demo0::main() {

	if ( ap.m_timer  == 2 ) {
		setModelDataObj( &ap.m_obj[ OBJ_DEM_MODEL ], MDL_PLAYER_NO );
		ap.m_obj[ OBJ_DEM_MODEL ].m_scale *= 4;

		ap.m_obj[ OBJ_DEM_MODEL ].m_angle_x = 64 + 32;

//		ap.m_obj[ OBJ_DEM_MODEL ].m_angle_vx = random( -2, 2);
		ap.m_obj[ OBJ_DEM_MODEL ].m_angle_vy = 2;
	}
	if ( ap.m_timer == 60*3 ) {
		static uint8_t sel = 0;
		setModelDataObj( &ap.m_obj[ OBJ_DEM_MODEL ], MDL_ENEMY_NO + sel );
		ap.m_obj[ OBJ_DEM_MODEL ].m_scale *= 4;

		ap.m_obj[ OBJ_DEM_MODEL ].m_angle_x = 64 + 32;

		ap.m_obj[ OBJ_DEM_MODEL ].m_angle_vy = 2;

		sel++;
		sel %= 3;
	}

	ap.move();

	ap.moveStarObj();

	glClear(GL_COLOR_BUFFER_BIT); 

	ap.draw();


}


