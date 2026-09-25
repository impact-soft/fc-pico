
//----------------------------------------------------
//
//		描画処理
//
//----------------------------------------------------

void ap_game::draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	if ( m_PlyDamFg ) {	// ダメージフラッシュ
		m_PlyDamFg--;
		memset(c.bitmap(), 1, FRAME_BUF_SIZE);
	}

	// 敵の弾描画特殊処理
	for (int i = 0; i < ENEMY_NT_SUU; i++) {
		uint8_t kind = m_emobj_kind[i];
		if ( kind == 0) {
			continue;
		}
		// キャラクターアニメ
		if (kind == NTK_WARP) {
			Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY + i];
			obj->m_mode = OMD_SPR16;
			obj->m_chrNo = 0x68 + (ap.m_timer & 0x6);
			obj->m_scale = 1.5f;
		}
		if (kind < 0x80 ) {
			Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY + i];
			obj->m_mode = OMD_SPR8;
			obj->m_chrNo = 0x54 + (ap.m_timer & 0x1);
			obj->m_scale = 1.0f;
		}
	}

	ap.draw();


	// レーダー描画
	drawRader();


	c.setDefCol(3);
	{
		char buffer[40];  // バッファを確保
		sprintf(buffer, "SCORE %08d0  %c%02d  STAGE%02d", ap.GM_SCORE, 0x7f, ap.PLY_LIFE, ap.PLY_STAGE);
		c.drawString(buffer, 8, 8, _font);
	}


#if 0
	{
		Obj3d *obj = &ap.m_obj[OBJ_GAM_PLAYER];


		char buffer[40];  // バッファを確保
//		sprintf(buffer, "%3.3f : %3.3f : %3.3f",db0 , db1, db2 );
		sprintf(buffer, "%3.3f : %3.3f : %3.3f",obj->m_x , obj->m_z,obj->m_angle_y );
//		sprintf(buffer, "%3.3f : %3.3f : %3.3f",cam_prm[0] , cam_prm[1],cam_prm[2] );
//		sprintf(buffer, "%3.3f : %3.3f : %3.3f",vlootAt[0] , vlootAt[1],vlootAt[2] );
		c.drawString(buffer, 8, 8*28, _font);
	}
#endif



#if 0
	{
		char buffer[40];  // バッファを確保
		sprintf(buffer, "DB %02x %02x ", emu.m_RAM[MISSON_NO] , emu.m_RAM[EM_DEMO_FG] );
		c.drawString(buffer, 8, 8*2, _font);
	}
#endif

}



#if 0

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
		//conv3Dxy(obj, x, y);
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
			//conv3Dxy(obj, x, y);
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
			//conv3Dxy(obj, x, y);
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

		//conv3Dxy(obj, x, y);
		//		obj->m_scale = 0.5f + cnt * 0.2f;
		//		obj->m_DitherAdd = cnt;
		//		obj->setAngle( random( 0, 255), random( 0, 255), random( 0, 255) );
		//		obj->m_color = random( 1, 3);
	}



}

#endif


//-----------------------------------------------
//			レーダー描画
//-----------------------------------------------
void ap_game::drawRader() {
	int bx = 185;
	int by = 30;
	Obj3d *obj2;

	// フィールド範囲
	c.setDefCol(3);
	c.drawSquare( bx - 5, by -5 , bx + 65, by + 65 );

	// 敵の弾描画
	c.setDefCol(1);
	obj2 = &ap.m_obj[OBJ_GAM_ENEMY];
	for ( int i = 0 ; i < ENEMY_NT_SUU ; i++, obj2++ ){
		uint8_t kind = m_emobj_kind[i];
		if ( kind == 0) {
			continue;
		}
		int x = obj2->m_x + FLD_LIM_W;
		int y = -obj2->m_z + FLD_LIM_H;
		x *= (4.0f / P_SIZE);
		y *= (4.0f / P_SIZE);
		if ( i < ENEMY_SUU ) {
			c.drawSquare(bx + x - 2, by + y - 2, bx + x + 2, by + y + 2);
		} else {
			c.drawSquare(bx + x - 1, by + y - 1, bx + x + 1, by + y + 1);
		}
	}

	// 自機弾

	obj2 = &ap.m_obj[OBJ_GAM_BULLET];
	c.setDefCol(2);

	for ( int i = 0 ; i < PSHOT_A_SUU ; i++, obj2++ ){
		if ( obj2->m_mode == OMD_NONE ) {
			continue;
		}
		int x = obj2->m_x + FLD_LIM_W;
		int y = -obj2->m_z + FLD_LIM_H;
		x *= (4.0f / P_SIZE);
		y *= (4.0f / P_SIZE);
		c.drawSquare(bx + x - 1, by + y - 1, bx + x + 1, by + y + 1);
	}

	// 自機描画
	c.setDefCol(2);
	{
		int x = objPLY->m_x + FLD_LIM_W;
		int y = -objPLY->m_z + FLD_LIM_H;
		x *= (4.0f / P_SIZE);
		y *= (4.0f / P_SIZE);
		c.drawSquare(bx + x - 3, by + y - 3, bx + x + 3, by + y + 3);
	}


}



