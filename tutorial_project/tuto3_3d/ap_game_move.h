



//----------------------------------------------------
//		一番近い敵を探す
//----------------------------------------------------
Obj3d* ap_game::findNearEnemy( float LimitSQ ) {
	Obj3d* objTarget = NULL;
	for (int i = 0; i < ENEMY_SUU; i++ ) {
		if (m_emobj_kind[i] == 0) continue;
		if (m_emobj_hp[i] == 255) continue;
		if (m_emobj_hp[i] == 254) continue;
		Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY + i];
		float dst = objPLY->getDistanceSQ( obj );
		if ( dst < LimitSQ ) {
			LimitSQ = dst;
			objTarget = obj;
		}
	}
	return objTarget;
}



//----------------------------------------------------
//
//		移動処理
//
//----------------------------------------------------

void ap_game::move() {


	movePlayer();

	movePlayerShot();

	moveEnemy();

	ap.move();

}

//----------------------------------------------------
//
//		プレーヤー制御
//
//----------------------------------------------------

void  ap_game::movePlayer() {

/*
	{
	Obj3d *obj3 = &ap.m_obj[OBJ_GAM_DEBUG];
	Obj3d *obj4 = &ap.m_obj[OBJ_GAM_DEBUG+1];
	float dir = obj3->getObjAngleXZ( objPLY );
	obj4->m_x = obj3->m_x -sin( DEG2RAD * dir );
	obj4->m_z = obj3->m_z + cos( DEG2RAD * dir );

	int idx = setEnemy( NTK_NOMAL, obj3->m_x, obj3->m_x );
	if ( idx >= 0 ) {
		m_emobj_dir[idx] = dir;
		m_emobj_spd[idx] = 1.0f;
	}
	
	}
*/

	if ( PLY_ANM == PLY_AN_DEAD ) {
		
		return;
	}


	uint8_t read_dt = ap.EM_KEY_NEW;

	float vy_limit = 4.0f;

	
	if (read_dt & KEY_RIGHT) {
		if (read_dt & KEY_AB) {
			objPLY->m_x -= ply_side_x * 0.5f;
			objPLY->m_z -= ply_side_z * 0.5f;
		} else {
			objPLY->m_angle_vy -= 1;
			if ( objPLY->m_angle_vy < -vy_limit ) objPLY->m_angle_vy = -vy_limit;
		}
	} else if (read_dt & KEY_LEFT) {
		if (read_dt & KEY_AB) {
			objPLY->m_x += ply_side_x * 0.5f;
			objPLY->m_z += ply_side_z * 0.5f;
		} else {
			objPLY->m_angle_vy += 1;
			if ( objPLY->m_angle_vy > vy_limit ) objPLY->m_angle_vy = vy_limit;
		}
	} else {
		objPLY->m_angle_z = 0;
		if ( objPLY->m_angle_vy < 0 ) objPLY->m_angle_vy++;
		if ( objPLY->m_angle_vy > 0 ) objPLY->m_angle_vy--;
	}



	if (read_dt & KEY_UP) {
		objPLY->m_x += (ply_dir_x * 0.5f);
		objPLY->m_z += (ply_dir_z * 0.5f);
//		objPLY->m_vx = (ply_dir_x * 0.2f);
//		objPLY->m_vz = (ply_dir_z * 0.2f);
	} else if (read_dt & KEY_DOWN) {
		objPLY->m_x -= (ply_dir_x * 0.5f);
		objPLY->m_z -= (ply_dir_z * 0.5f);
//		objPLY->m_vx = -(ply_dir_x * 0.2f);
//		objPLY->m_vz = -(ply_dir_z * 0.2f);
	}


	if (read_dt & KEY_B) {
		DashCounter++;
		if ( DashCounter > 16 ) {
			DashCounter = 16;
		}
	}
		
	


	ply_dir_x = -sin( DEG2RAD * objPLY->m_angle_y );
	ply_dir_z = cos( DEG2RAD * objPLY->m_angle_y );

	ply_side_x = -sin( DEG2RAD * (objPLY->m_angle_y + 90.0f) );
	ply_side_z = cos( DEG2RAD * (objPLY->m_angle_y + 90.0f) );

	// 通常弾発射
	if ( ap.EM_KEY_NEW & KEY_A) {
		if ( (ap.m_timer & 0x3) == 0 ) {
		 	shotPlayer();
		 	shotPlayer( 15.0f);
		 	shotPlayer( -15.0f);
		 	shotPlayer( 180.0f);
		}
	}
	
	if ( ap.EM_KEY_NEW & KEY_B) {

		// 近い敵を探す
//		objATK = findNearEnemy( 100.0f * 100.0f );

//		if ( objATK != NULL ) {
//			atk_dir = objPLY->getObjAngleXZ( objATK );
//		}

		db1 = objPLY->m_angle_y;
		db2 = atk_dir;

		if ( objATK != NULL ) {
			objPLY->turnObjY( atk_dir, 8.0f );
		}
	
		objATK = findNearEnemy( 100.0f * 100.0f );
		if ( objATK != NULL ) {
			atk_dir = objPLY->getObjAngleXZ( objATK );
		}

		if ( DashCounter <= 2 ) {
		} else {
//			objPLY->m_x += (ply_dir_x * 0.8f);
//			objPLY->m_z += (ply_dir_z * 0.8f);
		 	shotPlayerB();
		 	shotPlayerB(30.0f);
		 	shotPlayerB(-30.0f);
		 	shotPlayerB(60.0f);
		 	shotPlayerB(-60.0f);
			PLY_ATK_CNT = 0;
		}
	} else {
		DashCounter = 0;
	}

	// 移動範囲リミッター
	moveLimitPly( objPLY );

}


void  ap_game::shotPlayer( float aofs ) {
	Obj3d *obj = &ap.m_obj[OBJ_GAM_PLAYER];
	Obj3d *obj2 = &ap.m_obj[OBJ_GAM_BULLET];
	for ( int i = 0 ; i < PSHOT_A_SUU ; i++, obj2++ ){
		if ( obj2->m_mode == OMD_NONE ) {
			obj2->m_mode = OMD_SPR8;
			obj2->m_chrNo = 0x40;
			obj2->m_vx = -sin( DEG2RAD * (objPLY->m_angle_y +aofs) );
			obj2->m_vz = cos( DEG2RAD * (objPLY->m_angle_y +aofs) );

			obj2->m_x = obj->m_x;
			obj2->m_y = obj->m_y;
			obj2->m_z = obj->m_z;
			obj2->m_scale = 0.5f;
			obj2->m_cnt = 25;
			break;
		}
	}
	snd.playSE( SE_SHOT_A );
}

void  ap_game::shotPlayerB( float aofs ) {
	Obj3d *obj = &ap.m_obj[OBJ_GAM_PLAYER];
	Obj3d *obj2 = &ap.m_obj[OBJ_GAM_BULLET];
	for ( int i = 0 ; i < PSHOT_A_SUU ; i++, obj2++ ){
		if ( obj2->m_mode == OMD_NONE ) {
			obj2->m_mode = OMD_SPR8;
			obj2->m_chrNo = 0x40;

			obj2->m_vx = -sin( DEG2RAD * (objPLY->m_angle_y +aofs) );
			obj2->m_vz = cos( DEG2RAD * (objPLY->m_angle_y +aofs) );

			obj2->m_x = obj->m_x + obj2->m_vx *1.2;
			obj2->m_y = obj->m_y;
			obj2->m_z = obj->m_z + obj2->m_vz *1.2;
			obj2->m_scale = 0.8f;
			obj2->m_cnt = 8;
			break;
		}
	}
	snd.playSE( SE_SHOT_A );
}

void  ap_game::movePlayerShot() {
	
	Obj3d *obj2 = &ap.m_obj[OBJ_GAM_BULLET];
	for ( int i = 0 ; i < PSHOT_A_SUU ; i++, obj2++ ){
		if ( obj2->m_mode != OMD_NONE ) {
			obj2->m_cnt--;
			if ( moveLimit( obj2 ) ) {
				obj2->m_cnt = 0;
			}
			if ( obj2->m_cnt == 0 ) {
				obj2->m_mode = OMD_NONE;
			}
		}
	}


}

bool  ap_game::moveLimitPly( Obj3d *obj ) {
	bool limfg = false;
	float limW = (FLD_LIM_W - 1.0f);
	float limH = (FLD_LIM_H - 1.0f);

	
	if ( obj->m_x < -limW ) {
		obj->m_x = -limW;
		limfg = true;
	} else if ( obj->m_x >  limW ) {
		obj->m_x =  limW;
		limfg = true;
	}

	if ( obj->m_z < -limH ) {
		obj->m_z = -limH;
		limfg = true;
	} else if ( obj->m_z >  limH ) {
		obj->m_z =  limH;
		limfg = true;
	}
	return limfg;
}

bool  ap_game::moveLimit( Obj3d *obj ) {
	bool limfg = false;
	
	if ( obj->m_x < -FLD_LIM_W ) {
		obj->m_x = -FLD_LIM_W;
		limfg = true;
	} else if ( obj->m_x >  FLD_LIM_W ) {
		obj->m_x =  FLD_LIM_W;
		limfg = true;
	}

	if ( obj->m_z < -FLD_LIM_H ) {
		obj->m_z = -FLD_LIM_H;
		limfg = true;
	} else if ( obj->m_z >  FLD_LIM_H ) {
		obj->m_z =  FLD_LIM_H;
		limfg = true;
	}
	return limfg;
}



//----------------------------------------------------
//
//		爆発エフェクト
//
//----------------------------------------------------
void  ap_game::setBakuEfc( Obj3d *obj, uint8_t cnt ) {
	Obj3d *obj2 = &ap.m_obj[OBJ_GAM_EXPEFC];
	for ( int i = 0 ; i < BAKU_EFC_SUU ; i++,obj2++ ){
		if ( obj2->m_mode == OMD_NONE ) {
			obj2->m_mode = OMD_SPR16;
			obj2->m_scale = 1.0f;
			obj2->m_chrNo = cnt;
			obj2->m_cnt  = cnt;
			obj2->m_x = obj->m_x;
			obj2->m_y = obj->m_y;
			obj2->m_z = obj->m_z;
			objTmp = obj2;
			break;
		}
	}
}

void  ap_game::anmBakuEfc() {
	Obj3d *obj2 = &ap.m_obj[OBJ_GAM_EXPEFC];
	for ( int i = 0 ; i < BAKU_EFC_SUU ; i++,obj2++ ){
		if ( obj2->m_mode != OMD_NONE ) {
			obj2->m_chrNo = obj2->m_cnt & 0xFE;
			obj2->m_cnt++;
			if ( (obj2->m_cnt == 16) || (obj2->m_cnt == 40) ) {
				obj2->m_mode = OMD_NONE;
			}
		}
	}
}

//----------------------------------------------------
//
//		敵制御
//
//----------------------------------------------------
float ap_game::getEnemySpd( uint8_t spd ) {
	// 移動スピードテーブル
	const float tblSpd[] = {
		0.1f,0.2f,0.3f,0.4f,
	};
	return tblSpd[ spd ];
}


//---------------------------------------
//		敵セット
//---------------------------------------
int ap_game::setEnemy( uint8_t kind, float x, float z ) {

	int start = 0;
	int end = ENEMY_SUU;
	if (kind < 0x80) {
		start = end;
		end = ENEMY_NT_SUU;
	}
	objTmp = NULL;

	for (int i = start; i < end; i++ ) {
		if (m_emobj_kind[i] != 0) {
			continue;
		}
		setEnemyIdx( i, kind, x, z );
		return i;
	}
	return -1;
}

void ap_game::setEnemyIdx( int idx, uint8_t kind, float x, float z ) {
	int i = idx;

	m_emobj_kind[i] = kind;

	const uint8_t *NTcfg = getEnemyNTcfg( kind );
	ap_g.m_emobj_hp[ i ] = NTcfg[1];
	ap_g.m_emobj_dt[ i ] = 0;

	uint8_t mp =  gMI_RAM[ PRM_2 ];
	m_emobj_spd[i] = getEnemySpd( (mp >> 6) & 3 );
	int dir = (mp & 0x3f);
	//Serial.printf("dir %02x\n", dir );
	dir += 0x10;
	dir = 0x40 - dir;
	dir &= 0x3f;
	//Serial.printf("dir2 %02x\n", dir );
	float dirF = dir * 360.0f / 64;
	m_emobj_dir[i] = dirF;
	//Serial.printf("dirF %f\n", dirF );

	Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY + i];
	objTmp = obj;
	obj->m_cnt = 0;


	switch (kind) {

	case NTK_ANGLE:		// 自機狙い弾
		m_emobj_dir[i] = obj->getObjAngleXZ( objPLY );;
		m_emobj_spd[i] = getEnemySpd( 1 );
		setMoveDirData( i, obj );
		m_emobj_kind[i]++;
		Serial.printf("NTK_ANGLE %f %f\n", m_emobj_dir[i] ,m_emobj_spd[i] );


	case NTK_NOMAL:		// 通常弾
	case NTK_HORMI:		// ホーミング弾
//		NTK_MISS	= 4,	///< Missile. // ミサイル弾
//		NTK_HHORM	= 6,	///< Partially homing shot. // 半誘導弾
//		NTK_HHORM3	= 8,	///< Partially homing shot, splits into three. // 半誘導弾３分裂
		obj->m_mode = OMD_SPR8;
		obj->m_chrNo = 0x55;
		obj->m_cnt = 60;
		break;
			
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
		m_emobj_wp[ i ] = mp;
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

	obj->m_x = x;
	obj->m_y = 0;
	obj->m_z = z;
}

//---------------------------------------
//		敵＆敵弾移動
//---------------------------------------
void ap_game::moveEnemy( ) {

	Obj3d *obj2 = &ap.m_obj[OBJ_GAM_ENEMY];
	for ( int i = 0 ; i < ENEMY_NT_SUU ; i++, obj2++ ){
		uint8_t kind = m_emobj_kind[i];
		if ( kind == 0) {
			continue;
		}
		if ( moveLimit( obj2 ) ) {
			m_emobj_kind[i] = 0;
			obj2->m_mode = OMD_NONE;
			continue;
		}
		_moveSpcSub( i, obj2 );

		if ( kind == NTK_WARP ) { // ワープは移動しない
			objTmp->m_vx = 0;
			objTmp->m_vz = 0;
			continue;
		}
		setMoveDirData( i, obj2 );

		if ( obj2->m_cnt > 0 ) {
			obj2->m_cnt--;
			continue;
		}

		if (kind < 0x80) {	// 弾の移動処理
			obj2->m_mode = OMD_NONE;
			m_emobj_kind[i] = 0;
		}

	}

}


/*====================================================================
;    指定方向の移動データ取得
; IN -> Areg 移動方向+速度
; OUT -> X移動データ W_AR
;        Y移動データ W_BR
;====================================================================*/

void ap_game::setMoveDirData( int idx, Obj3d *obj ) {
	float spdF = m_emobj_spd[ idx ];
	obj->m_vx = -sin( DEG2RAD * m_emobj_dir[idx] ) * spdF;
	obj->m_vz = cos( DEG2RAD * m_emobj_dir[idx] ) * spdF;
}



//---------------------------------------
//	敵の弾の移動特殊処理
//---------------------------------------
void ap_game::_moveSpcSub( int idx, Obj3d *obj ) {
	uint8_t kind = m_emobj_kind[idx];
	uint8_t a;
	const uint8_t *NTcfg = getEnemyNTcfg( kind );
	a = NTcfg[3];
	if ( a == 0 ) goto _mss00;
	m_emobj_dt[idx]++;
	if ( a != m_emobj_dt[idx] ) goto _mss00;
	
	if ( kind != NTK_WARP ) goto _mss01;
	//----------------------
	// ワープ時特殊処理
	//----------------------
	setEnemyIdx( idx, m_emobj_wp[ idx ], obj->m_x, obj->m_z );
	m_emobj_dir[idx] = obj->getObjAngleXZ( objPLY );
	m_emobj_spd[idx] = getEnemySpd( 1 );	// ワープイン時の速度
//	_angle2( idx, obj );
	goto _mss00;
_mss01:
	m_emobj_kind[idx]++;
	m_emobj_dt[idx] = 0;
_mss00:
	a = NTcfg[2];
	if ( a == 1 ) {
//		_angle( idx, obj );
		return;
	}
	if ( a == 2 ) {
		_horming( idx, obj );
		return;
	}
}


void ap_game::_horming( int idx, Obj3d *obj ) {
	if ( ((ap.m_timer + idx ) & 0x3 ) == 0 ) {
		// ターゲットの方向を再チェック
		m_emobj_Targetdir[idx] = obj->getObjAngleXZ( objPLY );
		//Serial.printf("_horming %f %f\n", m_emobj_Targetdir[idx] , m_emobj_dir[idx] );
	}

	m_emobj_dir[idx] = turnAngele( m_emobj_dir[idx] ,m_emobj_Targetdir[idx], 5.0f);
}


