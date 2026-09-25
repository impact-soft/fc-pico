
void ap_game::main() {

	if ( ap.m_DemoFG == 0 ) {
		ap.EM_KEY_TRG = sys.getKeyTrg();
		ap.EM_KEY_NEW = sys.getKeyNew();
	} else {
		if ( (ap.m_timer & 0x30) == 0 ) {
//			ap.EM_KEY_NEW = KEY_B | KEY_RIGHT;
			ap.EM_KEY_NEW = KEY_B | KEY_UP;
		} else {
//			ap.EM_KEY_NEW = KEY_A | KEY_LEFT;
			ap.EM_KEY_NEW = KEY_A;
		}
	}
	


	mt.updateMission();


	// ゲームオーバー判定
	if ( PLY_ANM == PLY_AN_DEAD ) {
		m_over_wait++;
		if ( m_over_wait & 1 ) {
			objPLY->m_mode = OMD_NONE;
			setBakuEfc( objPLY, 0 );
			objTmp->m_x += 0.1f * random( -12, 12 );
			objTmp->m_z += 0.1f * random( -12, 12 );
		} else {
			objPLY->m_mode = OMD_MODEL;
		}
		
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
	if ( mt.MISSON_TYPE == MT_END ) {
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


	move();

	updateCamera();


	hit();

	draw();

}


