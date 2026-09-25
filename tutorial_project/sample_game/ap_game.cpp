/*
    ap_game.h
 */

/**
 * @file ap_game.cpp
 * @brief The play screen: drives the emulated game ROM and renders its state.
 * @ingroup sample_app
 *
 * One frame here is: post the controller bytes into the emulated RAM, run the
 * ROM's `$E004` entry until it hits `brk`, then walk the object tables it left
 * behind and rebuild the 3D scene from them.
 *
 * Sound is relayed the same way. The ROM raises a request byte, this side plays
 * it and writes the byte back to zero, which is the acknowledgement.
 */


#include "ap_main.h"

ap_game ap_g;


/**
 * @brief NES palette for the play screen: four background sets, then four sprite sets.
 * @ingroup sample_app
 * @note Sent to the console with rp_system::setPalData(). These are real NES
 *       palette indices; the cartridge does not choose the colours, the console's
 *       PPU does.
 */
const uint8_t pal_game[] = {
	// BG PAL
	0x0F, 0x15, 0x11, 0x30,
	0x0F, 0x2A, 0x2A, 0x2A,
	0x0F, 0x12, 0x2C, 0x30,
	0x0F, 0x1A, 0x1A, 0x1A,
	// OBJ PAL
	0x0F, 0x0F, 0x20, 0x3c,
	0x0F, 0x00, 0x10, 0x20,
	0x0F, 0x15, 0x19, 0x20,
	0x0F, 0x21, 0x10, 0x20
};




void ap_game::init(void) {

	//Serial.printf("ap_game:init\n");
	ap.m_timer = 0;

	m_over_wait = 0;
	m_clear_wait = 0;

	/* Pass the canvas to the OpenGL environment */
	glUseCanvas(&c);

	c.setSprData( _acOBJ );

	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(4);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//  glOrtho(-5, 5, -5, 5, 0.1, 999.f);
	gluPerspective(30.0, c.width() / c.height(), 0.1f, 9999.f);

	glMatrixMode(GL_MODELVIEW);


	sys.setPalData(pal_game);
	sys.startDataMode();

	initLookAt(1);
	//	setLight( 1.0f, 0.0f, 0.0f);
	setLight(1.0f, 0.0f, 0.5f);

	ap.initObj();

	memset(m_emobj_kind, 0, ENEMY_NT_SUU);

	memset( &emu.m_RAM[PSHOT_A_X], 0, PSHOT_A_SUU*2);
	memset( &emu.m_RAM[ENEMY_NT_WORK], 0, ENEMY_NT_SIZE * ENEMY_NT_SUU );
	memset( &emu.m_RAM[BAKU_EFC_X], 0, BAKU_EFC_SUU * 3 );



	// game init
	emu.run(0xE000 + 4 * 0, 0, 0x00, 0x00);

	emu.m_RAM[EM_DEMO_FG] = ap.m_DemoFG;

	ap.initStarObjGame(OBJ_GAM_STAR, 80, -0.8f);

	Obj3d *obj = &ap.m_obj[OBJ_GAM_PLAYER];
	setModelDataObj(obj, MDL_PLAYER_NO);


//	Serial.printf("ap_game:init:stage : m_DemoFG %d:%d\n", emu.m_RAM[EM_PLY_STAGE] ,ap.m_DemoFG );

	//Serial.printf("ap_game:init end\n");
}


void ap_game::main() {

	if ( ap.getStep() == ST_GAME ) {
		emu.m_RAM[EM_KEY_TRG] = sys.getKeyTrg();
		emu.m_RAM[EM_KEY_NEW] = sys.getKeyNew();
	} else {
		if ( ap.m_timer & 0x40 ) {
			emu.m_RAM[EM_KEY_NEW] = KEY_RIGHT;
		} else {
			emu.m_RAM[EM_KEY_NEW] = KEY_LEFT;
		}
	}

	// game main
	emu.run(0xE000 + 4 * 1, 0, 0x00, 0x00);

	// 自機ダメージBGフラッシュ（BGが赤くなりっぱなしの時があるので封印）
	if ( emu.m_RAM[DAM_BG_FLASH] > 0 ) {
		emu.m_RAM[DAM_BG_FLASH] = 0;
		m_PlyDamFg = 2;
	}

	// ゲームオーバー判定
	if ( emu.m_RAM[PLY_ANM_NO] == PLY_AN_DEAD ) {
		m_over_wait++;
		if ( m_over_wait > 180 ) {
			sys.setPF_COM( PF_COM_BGM + BGM_STOP );
			snd.stopBGM();
			sys.FadeOut();
			sys.SleepMS( 500 );
			ap.setStep( ST_OVER );
			return;
		}
	}

	// ゲームクリアー判定
	if ( emu.m_RAM[MISSON_TYPE] == 0xff ) {
		m_clear_wait++;
		if ( m_clear_wait > 180 ) {
			sys.setPF_COM( PF_COM_BGM + BGM_STOP );
			snd.stopBGM();
			sys.FadeOut();
			sys.SleepMS( 500 );
			ap.setStep( ST_CLEAR );
			return;
		}
	}


	int bgm_rq = emu.m_RAM[EM_REQ_BGM_NO];
	if (bgm_rq != 0) {
		//Serial.printf("bgm_rq %d\n", bgm_rq );
		if ( ap.m_DemoFG == 2 ) {
			bgm_rq = 1;
		}

		snd.playBGM( bgm_rq );
		emu.m_RAM[EM_REQ_BGM_NO] = 0;
	}

	for (int i = 0; i < 3; i++) {
		if (emu.m_RAM[EM_REQ_SE_NO + i]) {
			snd.playSE( emu.m_RAM[EM_REQ_SE_NO + i] );
			//			Serial.printf("%d\n", emu.m_RAM[ EM_REQ_SE_NO + i ] +4 );
			emu.m_RAM[EM_REQ_SE_NO + i] = 0;
			break;
		}
	}


	ap.move();

	ap.moveStarObjGame();

	conv3DObje();

	glClear(GL_COLOR_BUFFER_BIT);

	if ( m_PlyDamFg ) {	// ダメージフラッシュ
		m_PlyDamFg--;
		memset(c.bitmap(), 1, FRAME_BUF_SIZE);
	}


	ap.draw();

	// レーダー描画
//	drawRader();

	if ( m_over_wait == 0 ) {
		Obj3d *obj = &ap.m_obj[OBJ_GAM_PLAYER];
		uint8_t read_dt = emu.m_RAM[EM_KEY_NEW];

		if (read_dt & KEY_RIGHT) {
			obj->m_angle_z = 16;
		} else if (read_dt & KEY_LEFT) {
			obj->m_angle_z = -16;
		} else {
			obj->m_angle_z = 0;
		}
	}

	{
		char buffer[40];  // バッファを確保
		sprintf(buffer, "SCORE %02x%02x%02x%02x0  %c%02d  STAGE%02d", emu.m_RAM[GM_SCORE + 3], emu.m_RAM[GM_SCORE + 2], emu.m_RAM[GM_SCORE + 1], emu.m_RAM[GM_SCORE + 0], 0x7f, emu.m_RAM[PLY_LIFE], emu.m_RAM[PLY_STAGE]);
		c.drawString(buffer, 8, 8, _font);
	}
#if 0
	{
		char buffer[40];  // バッファを確保
		sprintf(buffer, "DB %02x %02x ", emu.m_RAM[MISSON_NO] , emu.m_RAM[EM_DEMO_FG] );
		c.drawString(buffer, 8, 8*2, _font);
	}
#endif

#if 0
//	String strBuf = "Hello, world";
//	c.drawString( strBuf.c_str(), 80, 60, _font );
	{
		char buffer[40]; // バッファを確保
//		sprintf(buffer, "x:%03d y:%03d", emu.m_RAM[ POS_PLY_X ], emu.m_RAM[ POS_PLY_Y ] );
		sprintf(buffer, "MT:%02x:%02x WT:WT%03d PC%02x%02x"
			, emu.m_RAM[ EM_MISSON_TYPE ],emu.m_RAM[ EM_MISSON_TYPE_SUB ]
			, emu.m_RAM[ EM_MISSON_WAIT ]
			, emu.m_RAM[ EM_MISSON_PC+1 ]
			, emu.m_RAM[ EM_MISSON_PC+0 ]
			 );
		c.drawString( buffer, 20, 60, _font );
	}
#endif
}

//-----------------------------------------------
//			レーダー描画
//-----------------------------------------------
void ap_game::drawRader() {

	int bx = 170;
	int by = 40;

	// 敵の弾描画
	c.setDefCol(1);
	for (int ofs = 0; ofs < ENEMY_NT_SUU * ENEMY_NT_SIZE; ofs += ENEMY_NT_SIZE) {
		uint8_t kind = emu.m_RAM[ENEMY_NT_KIND + ofs];
		if (kind == 0) continue;
		if (emu.m_RAM[ENEMY_NT_Y + 1 + ofs] >= ENEMY_LINE_SUU) continue;
		if (emu.m_RAM[ENEMY_NT_X + 1 + ofs] < 8) continue;
		if (emu.m_RAM[ENEMY_NT_X + 1 + ofs] > 248) continue;
		int x = emu.m_RAM[ENEMY_NT_X + 1 + ofs];
		int y = emu.m_RAM[ENEMY_NT_Y + 1 + ofs];
		x /= 3;
		y /= 3;
		if (kind < 0x80) {
			c.drawSquare(bx + x - 1, by + y - 1, bx + x + 1, by + y + 1);
		} else {
			c.drawSquare(bx + x - 3, by + y - 3, bx + x + 3, by + y + 3);
		}
	}

	// 自機弾
	for (int ofs = 0; ofs < 2 * PSHOT_A_SUU; ofs += 2) {
		c.setDefCol(2);
		int y = emu.m_RAM[PSHOT_A_Y + ofs];
		if (y == 0) continue;
		int x = emu.m_RAM[PSHOT_A_X + ofs];
		x /= 3;
		y /= 3;
		c.drawSquare(bx + x - 1, by + y - 1, bx + x + 1, by + y + 1);
	}
	// 自機描画
	{
		c.setDefCol(2);
		int x = emu.m_RAM[POS_PLY_X];
		int y = emu.m_RAM[POS_PLY_Y];
		x /= 3;
		y /= 3;
		c.drawSquare(bx + x - 3, by + y - 3, bx + x + 3, by + y + 3);
	}
}


void ap_game::conv3Dxy(Obj3d *obj, int x, int y) {
	//	obj->m_x = 0.1f * ( 128 - x);
	obj->m_x = 0.08f * (128 - x);
	obj->m_z = 0.1f * (128 - y);
}


//-----------------------------------------------
//			3Dオブジェクト変換
//-----------------------------------------------
void ap_game::conv3DObje() {

	int bx = 170;
	int by = 40;

	// 敵の弾描画
	for (int i = 0; i < ENEMY_NT_SUU; i++) {
		int ofs = i * ENEMY_NT_SIZE;
		Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY + i];

		uint8_t kind = emu.m_RAM[ENEMY_NT_KIND + ofs];
		if (m_emobj_kind[i] != kind) {
			m_emobj_kind[i] = kind;
			if (kind == 0) {
				obj->m_mode = OMD_NONE;
				continue;
			}
			if (kind < 0x80) {
				obj->m_mode = OMD_SPR8;
				obj->m_chrNo = 0x55;
//				setModelDataObj(obj, MDL_BULLET_E_NO);
			} else {
				switch (kind) {
					case NTK_METEO:
						obj->m_mode = OMD_CUBE;
						obj->m_scale = 0.5f;
						obj->m_color = 3;
						obj->setAngle(32, 32, 32);
						obj->m_angle_vx = random(-5, 5);
						obj->m_angle_vy = random(-5, 5);
						obj->m_DitherAdd = 2;
						break;
					case NTK_WARP:
						break;
					case NTK_SPZK1:
						setModelDataObj(obj, MDL_ENEMY3_NO);
						obj->m_angle_vz = random(-5, 5);
						break;
					case NTK_SPZK2:
						setModelDataObj(obj, MDL_ENEMY2_NO);
						obj->m_angle_vz = random(-5, 5);
						break;
					default:
						setModelDataObj(obj, MDL_ENEMY_NO);
						obj->m_angle_vz = random(-5, 5);
						break;
				}
			}
		}
		//		uint8_t m_mode_bak = obj->m_mode;

		if (kind == 0) {
			obj->m_mode = OMD_NONE;
			continue;
		}
		if (kind == NTK_WARP) {
			obj->m_mode = OMD_SPR16;
			obj->m_chrNo = 0x68 + (ap.m_timer & 0x6);
			obj->m_scale = 1.5f;
		}
		if (kind < 0x80 ) {
			obj->m_mode = OMD_SPR8;
			obj->m_chrNo = 0x54 + (ap.m_timer & 0x1);
			obj->m_scale = 1.0f;
		}

		//		if ( emu.m_RAM[ ENEMY_NT_Y + 1 + ofs ] >= ENEMY_LINE_SUU ) continue;
		//		if ( emu.m_RAM[ ENEMY_NT_X + 1 + ofs ] < 8 ) continue;
		//		if ( emu.m_RAM[ ENEMY_NT_X + 1 + ofs ] > 248 ) continue;
		int x = emu.m_RAM[ENEMY_NT_X + 1 + ofs];
		int y = emu.m_RAM[ENEMY_NT_Y + 1 + ofs];
		conv3Dxy(obj, x, y);
		//		obj->m_mode = m_mode_bak;
	}

	// 自機弾
	for (int i = 0; i < PSHOT_A_SUU; i++) {
		int ofs = i * 2;
		Obj3d *obj = &ap.m_obj[OBJ_GAM_BULLET + i];
		obj->m_mode = OMD_NONE;

		int y = emu.m_RAM[PSHOT_A_Y + ofs];
		if (y == 0) continue;
		int x = emu.m_RAM[PSHOT_A_X + ofs];
		int dir = emu.m_RAM[PSHOT_DIR + ofs];
		obj->m_scale = 1.0f;

//@		setModelDataObj(obj, MDL_BULLET_P_NO);
//		if ( (x >= 16) && ( x <= 256 -16 )) {
			obj->m_mode = OMD_SPR8;
			dir += 2;
			dir >>= 2;
			dir &= 7;
			obj->m_chrNo = 0x40+ dir;
			conv3Dxy(obj, x, y);
//		}
	}
	// 自機描画
	{
		bool disp = true;
		Obj3d *obj = &ap.m_obj[OBJ_GAM_PLAYER];
		obj->m_mode = OMD_NONE;
		int muteki_tm = emu.m_RAM[PLY_MUTEKI_TM];
		// 無敵点滅
		if (muteki_tm > 0) {
			if (muteki_tm & 2) {
				disp = false;
			}
		}
		// 死亡点滅
		if ( m_over_wait > 0 ) {
			if (m_over_wait & 2) {
				disp = false;
			}
		}

		int x = emu.m_RAM[POS_PLY_X];
		int y = emu.m_RAM[POS_PLY_Y];
		if (disp) {
			obj->m_mode = OMD_MODEL;
			conv3Dxy(obj, x, y);
		}

		// 自機追尾カメラテスト
		if ( sys.SaveData[ SDT_GAME_MODE ] ) {
			moveLookAt( x, y );
		}
	}


	// 爆発エフェクト
	for (int i = 0; i < BAKU_EFC_SUU; i++) {
		int ofs = i * 3;
		Obj3d *obj = &ap.m_obj[OBJ_GAM_EXPEFC + i];
		obj->m_mode = OMD_NONE;
		int cnt = emu.m_RAM[BAKU_EFC_CNT + ofs];
		if (cnt == 0) continue;
		emu.m_RAM[BAKU_EFC_CNT + ofs]++;
		if (cnt == 16) {
			emu.m_RAM[BAKU_EFC_CNT + ofs] = 0;
			continue;
		}
		int x = emu.m_RAM[BAKU_EFC_X + ofs];
		int y = emu.m_RAM[BAKU_EFC_Y + ofs];
		obj->m_mode = OMD_SPR16;
		obj->m_chrNo = cnt & 0x0E;
		obj->m_scale = 1.5f;

		conv3Dxy(obj, x, y);
		//		obj->m_scale = 0.5f + cnt * 0.2f;
		//		obj->m_DitherAdd = cnt;
		//		obj->setAngle( random( 0, 255), random( 0, 255), random( 0, 255) );
		//		obj->m_color = random( 1, 3);
	}
}
