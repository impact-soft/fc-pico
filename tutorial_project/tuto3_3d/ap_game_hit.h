
//=====================================================
//
//		当たり処理
//
//=====================================================

void ap_game::hit() {


	// 敵の当たり判定

//	for ( int j=0; j < ENEMY_NT_SUU ; j++ ) {
	for ( int j=0; j < ENEMY_SUU ; j++ ) {
		Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY+j];
		if ( obj->m_mode == OMD_NONE ) continue;

		Obj3d *obj2 = &ap.m_obj[OBJ_GAM_BULLET];
		for ( int i = 0 ; i < PSHOT_A_SUU ; i++, obj2++ ){
			if ( obj2->m_mode != OMD_NONE ) {
				if ( obj->getDistanceSQ( obj2 ) < 1.5f * 1.5f ) {
					switch ( m_emobj_hp[j] ) {
					case 255:
						snd.playSE( SE_NO_DAME );
						setBakuEfc( obj2, 32 );
						obj2->m_mode = OMD_NONE;
						break;
					case 254:
						// 当たり判定なし
						break;

					default:
						m_emobj_hp[j]--;
						if ( m_emobj_hp[j] == 0 ) {
							m_emobj_kind[j] = 0;
							ap.GM_SCORE += 10;
							obj->m_mode = OMD_NONE;
							setBakuEfc( obj2, 0 );
							snd.playSE( SE_BAKU_S );
						} else {
							setBakuEfc( obj2, 32 );
							snd.playSE( SE_DAME );
						}
						obj2->m_mode = OMD_NONE;
						break;
					}

				} 
			}
		}
	}

	// 自機の当たり判定
	for ( int j=0; j < ENEMY_NT_SUU ; j++ ) {
		Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY+j];
		if ( obj->m_mode == OMD_NONE ) continue;
		if ( m_emobj_hp[j] == 254 ) continue;
		if ( PLY_ANM == PLY_AN_DEAD ) continue;

		if ( obj->getDistanceSQ( objPLY ) < 1.5f * 1.5f ) {


//			if ( j >=  ENEMY_SUU ) {
				// 敵の弾だったら消す
				obj->m_mode = OMD_NONE;
				m_emobj_kind[j] = 0;
//				Serial.printf("et clr %02x\n", j );
//			}

			if ( ap.m_DemoFG == 0 ) {
				if ( ap.PLY_LIFE > 0 ) {
					ap.PLY_LIFE--;
				}
			}
			m_PlyDamFg = 2;

			if ( ap.PLY_LIFE == 0 ) {
				PLY_ANM = PLY_AN_DEAD;
				m_over_wait = 0;
				snd.playSE( SE_PLY_DEAD );
			} else {
				snd.playSE( SE_PLY_DAME );
			}
		}
	}


	anmBakuEfc();
}


