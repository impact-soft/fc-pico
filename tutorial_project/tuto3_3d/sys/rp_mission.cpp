
#include "Arduino.h"
 
#include "rp_system.h"
#include "rp_mission.h"

#ifdef USR_MISSON

#include "..\UsrMission.h"

#endif


uint8_t gMI_RAM[0x20];	///< ミッション用グローバルRAM


//=====================================================
//
//		ミッション処理
//
//=====================================================

void rp_mission::initMission() {
	MISSON_NO = 0;
	getMission();
	// デモモードだった場合は MT_DEMOJMP MT_BOSSJMPをサーチする
}

void rp_mission::getMission() {
	MISSON_STEP = 0;
	MISSON_PC_SP = 0;
	if ( ap.getStep() == ST_DEBUG_MT ) {
		if ( ap.getStepSub() == 0 ) {
			MISSON_TYPE = ap_db.db_dt[ 2 ];
			MISSON_TYPE_SUB = ap_db.db_dt[ 3 ];
			return;
		} else {
			sys.FadeOut();
			sys.SleepMS( 500 );
			ap.setStep( ST_DEBUG );
			return;
		}
	}

	ASSERT( ap.PLY_STAGE > 0 );
	ASSERT( ap.PLY_STAGE <= STAGE_MAX );

	const uint8_t *cfgStage = getResData( BIN_CFGSTAGE );
	int ofs = cfgStage[ ap.PLY_STAGE*2 ] + (cfgStage[ ap.PLY_STAGE*2 + 1 ] << 8);
	const uint8_t *stgtop = &cfgStage[ ofs ];

	MISSON_TYPE = stgtop[ MISSON_NO*2 + 0];
	MISSON_TYPE_SUB = stgtop[ MISSON_NO*2 + 1];

//	Serial.printf("getMission %02x %02x %02x %04x\n", MISSON_TYPE, MISSON_TYPE_SUB, MISSON_NO, ofs );

	
	MISSON_NO++;
	
	switch( MISSON_TYPE ) {
	case MT_HARA:
		Serial.printf("MT_HARA %02x %02x--------\n", MISSON_TYPE_SUB & 0x3f, MISSON_TYPE_SUB & 0xC0 );
		break;
	case MT_FLET:
		Serial.printf("MT_FLET %02x %02x--------\n", MISSON_TYPE_SUB & 0x3f, MISSON_TYPE_SUB & 0xC0 );
		break;
	case MT_BOSS:
		Serial.printf("MT_BOSS %02x %02x--------\n", MISSON_TYPE_SUB & 0x3f, MISSON_TYPE_SUB & 0xC0 );
		break;
	case MT_EVNT:
		Serial.printf("MT_EVNT %02x %02x--------\n", MISSON_TYPE_SUB & 0x3f, MISSON_TYPE_SUB & 0xC0 );
		break;

	
	case MT_ATK_NO:
		Serial.printf("MT_ATK_NO %02x\n", MISSON_TYPE_SUB );
		gMI_RAM[ MISSON_ATK_NO ] = MISSON_TYPE_SUB;
		break;
	case MT_ANM_NO:
		Serial.printf("MT_ANM_NO %02x\n", MISSON_TYPE_SUB );
		gMI_RAM[ MISSON_ANM_NO ] = MISSON_TYPE_SUB;
		break;

	case MT_END:
		Serial.printf("MT_END\n" );
		break;

//	case MT_DEMOJMP:
//	case MT_BOSSJMP:
	default:
		break;
	}



}



void rp_mission::updateMission() {
	switch( MISSON_TYPE ) {
	case MT_HARA:
	case MT_FLET:
	case MT_BOSS:
	case MT_EVNT:
		mtMain();
//		getMission();	// 次のミッションを取得
		break;
	case MT_END:		// ミッション終了
		break;

//	MT_ANM_NO   = 0xFC,	// ミッションアニメ番号セット
//	MT_BOSSJMP	= 0xFD,	// デモ時の開始ボスミッションにジャンプ
//	MT_DEMOJMP  = 0xFE,	// デモ時の開始ミッションにジャンプ
	default:
		getMission();	// 次のミッションを取得
		break;
	}
	

}


void rp_mission::mtMain() {
	switch( MISSON_STEP ) {
	case 0:	// INIT
		initMissionControl();
		MISSON_STEP++;
		break;
	
	case 1:	// MOVE
#ifdef USR_MISSON
		MissionAnime();
#endif
		if ( mainMissionControl() ) {
			MISSON_STEP++;
		}
		break;
	
	case 2:	// CLEAR
		MISSON_STEP++;
		break;
	
	case 3:	// OUT
		getMission();
		break;
	default:
		break;
	}


}


void rp_mission::initMissionControl() {
//	mission = getResData( BIN_CFGMISSONHARA + MISSON_TYPE );
	mission = getResData( BIN_CFGMISSONHARA );
//	Serial.printf("mission %x" , mission);

	int tblno = (MISSON_TYPE_SUB & 0x3F) << 1;
	MISSON_PC = mission[ tblno] + (mission[ tblno + 1 ] << 8);

	gMI_RAM[ ENEMY_ATK_LV ] = MISSON_TYPE_SUB & 0xC0;	//  敵の攻撃レベル設定
	MISSON_WAIT = 0;
//	Serial.printf("MPC %04x %02x\n" , MISSON_PC, mission[MISSON_PC] );
}


bool rp_mission::mainMissionControl() {
	uint16_t pc;
	uint8_t a;
	uint8_t y;
	uint16_t tmp;
	const uint8_t *tmpAdr;

	if ( MISSON_WAIT > 0 ) {
		MISSON_WAIT--;
		return false;
	}

	for(;;) {
		//Serial.printf("MPC %04x %02x\n" , MISSON_PC, mission[MISSON_PC] );

		uint8_t mc = mission[ MISSON_PC++ ];
		
		switch( mc ) {
		case _MC_END:
			Serial.printf("MC_END\n");
			//MISSON_PC--;
			return true;

		case _MC_CALL:
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			pushMISSON_PC();
			pushMstack_areg( MISSON_LOOP_CNT );
			MISSON_PC = pc;
			Serial.printf("MC_CALL %04x\n", pc);
			break;

		case _MC_RET:
			Serial.printf("MC_RET\n");
			MISSON_LOOP_CNT = popMstack_areg();
			popMISSON_PC();
			break;

		case _MC_LOOP_CNT:
			MISSON_LOOP_CNT = mission[ MISSON_PC++ ];
			Serial.printf("MC_LOOP_CNT %d\n", MISSON_LOOP_CNT);
			break;

		case _MC_JMP:
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			a = mission[ MISSON_PC++ ];
			y = mission[ MISSON_PC++ ]; 
			Serial.printf("MC_JMP %02x\n", a );
			if ( mcj_sub( a ) ) {
				MISSON_PC = pc;
				Serial.printf(">JMP %04x\n", pc );
			}
			break;

		case _MC_ZAKO:

			// 未実装

			break;


		case _MC_MEMCPYN:
			Serial.printf("MC_MEMCPYN\n");
			// 未実装
			MISSON_PC += 5;
			break;

		case _MC_MEMCPY2:
			Serial.printf("MC_MEMCPY2\n");
			// 未実装
			MISSON_PC += 4;
			break;

		case _MC_BOSS_NS:
			Serial.printf("MC_BOSS_NS\n");
			// 未実装
			MISSON_PC += 4;
			break;
		case _MC_PALSET:
			Serial.printf("MC_PALSET\n");
			// 未実装
			MISSON_PC += 4;
			break;

		case _MC_MEMSET:		// F6
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			a = mission[ MISSON_PC++ ];
			if ( pc < sizeof(gMI_RAM) ) {
				gMI_RAM[ pc ] = a;
			} else {
				Serial.printf("Error MC_MEMSET\n");
			}
			Serial.printf("MC_MEMSET %04x %02x\n", pc, a);
			break;

		case _MC_MEMSET2:		// F5
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			a = mission[ MISSON_PC++ ];
			y = mission[ MISSON_PC++ ]; 
			if ( pc < (sizeof(gMI_RAM) -1) ) {
				gMI_RAM[ pc   ] = a;
				gMI_RAM[ pc+1 ] = y;
			} else {
				Serial.printf("Error MC_MEMSET\n");
			}
			Serial.printf("MC_MEMSET2 %04x %04%x\n", pc, (a + y << 8));
			break;

		case _MC_PGCALL:
			// コール先アドレスは上位、下位の順番で入っている
			pc = (mission[ MISSON_PC++ ] << 8);
			pc |= mission[ MISSON_PC++ ];
			y = mission[ MISSON_PC++ ]; 
			a = mission[ MISSON_PC++ ];
//			Serial.printf("MC_PGCALL %04x %02x\n", pc+1 , a );
#ifdef USR_MISSON
			UsrMisson_PG_CALL( pc, a, y, NULL );
#endif
			break;

		case _MC_PGCALL2:
			tmp = mission[ MISSON_PC++ ];
			tmp |= (mission[ MISSON_PC++ ] << 8);
			tmpAdr = &mission[ tmp ];
			// コール先アドレスは上位、下位の順番で入っている
			pc = (mission[ MISSON_PC++ ] << 8);
			pc |= mission[ MISSON_PC++ ];
			y = mission[ MISSON_PC++ ]; 
			a = mission[ MISSON_PC++ ];
			Serial.printf("MC_PGCALL2 %04x %02x %02x %04x\n", pc+1 , a, y , tmp);
#ifdef USR_MISSON
			UsrMisson_PG_CALL( pc, a, y, tmpAdr );
#endif
			break;

		case _MC_MEMCLR:
			Serial.printf("MC_MEMCLR\n");
			MISSON_PC += 4;
			// 未実装
			break;

		case _MC_MEMPUSH:
			Serial.printf("MC_MEMPUSH\n");
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			pushMstack_areg( gMI_RAM[ pc ] );
			break;

		case _MC_MEMPOP:
			Serial.printf("MC_MEMPOP\n");
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			gMI_RAM[ pc ] = popMstack_areg();
			break;

		case _MC_MEMCMP:
			Serial.printf("MC_MEMCMP\n");
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			a = mission[ MISSON_PC++ ];
			MISSON_CMP_P = 0;
			if ( gMI_RAM[ pc ] == a ) {
				MISSON_CMP_P = 2;	// Z flag set
			}
			if ( gMI_RAM[ pc ] > a ) {
				MISSON_CMP_P++;		// C flag set	※要チェック　条件が逆かも？
			}
			break;

		case _MC_MEMADD:
			Serial.printf("MC_MEMADD\n");
			pc = mission[ MISSON_PC++ ];
			pc |= (mission[ MISSON_PC++ ] << 8);
			a = mission[ MISSON_PC++ ];
			gMI_RAM[ pc ] += a;
			break;
		
		
		default:	// mc_wait
			if ( mc < _MC_BASE ) {
				Serial.printf("WAIT %d\n", mc);
				MISSON_WAIT = mc;
			} else {
				Serial.printf("MC_%02x\n", mc);
			}
			return false;
		}
	}

	return false;
}


//-------------------------------------
// 条件付ジャンプの条件判定サブ
//   ジャンプする場合 true
//-------------------------------------
bool rp_mission::mcj_sub( uint8_t mcj ) {
	switch( mcj ) {
	case MCJ_JMP:
		return true;
	
	case MCJ_LOOP_CNT:
		MISSON_LOOP_CNT--;
		return ( MISSON_LOOP_CNT == 0 );

	case MCJ_CMP_Z:		// メモリー比較結果がZならジャンプ
		return ( (MISSON_CMP_P & 2) != 0 );

	case MCJ_CMP_NZ:	// メモリー比較結果がNZならジャンプ
		return ( (MISSON_CMP_P & 2) == 0 );

	case MCJ_CMP_C:		// メモリー比較結果がCならジャンプ
		return ( (MISSON_CMP_P & 1) != 0 );
		
	case MCJ_CMP_NC:	// メモリー比較結果がNCならジャンプ
		return ( (MISSON_CMP_P & 1) == 0 );


	default:
#ifdef USR_MISSON
		return  UsrMisson_mcj_sub( mcj );
#endif
		break;
	}

/*
	条件はゲームを作りながら必要に応じて追加

MCJ_JMP			EQU  0		; 無条件ジャンプ		;///< Jump condition: always.
MCJ_LOOP_CNT	EQU  1		; ループカウンターをマイナス１してゼロでなければジャンプ		;///< Jump condition: decrement the loop counter and jump while non-zero.
							; MC_LOOP_CNTであらかじめループ回数セット
MCJ_ENEMY_Z		EQU  2		; 敵の残りがゼロならジャンプ		;///< Jump condition: no enemies left.
MCJ_ENEMY_NZ	EQU  3		; 敵の残りがゼロでないならジャンプ	;///< Jump condition: enemies left.


*/
	return false;
}


//----------------------
//  ミッションスタック
//----------------------
void rp_mission::pushMstack_areg( uint8_t a ) {
	ASSERT( ap.PLY_STAGE > 0 );
	MISSON_STACK[ MISSON_PC_SP++ ] = a;
}

uint8_t rp_mission::popMstack_areg() {
	ASSERT( MISSON_PC_SP > 0 );
	MISSON_PC_SP--;
	return MISSON_STACK[ MISSON_PC_SP ];
}

void rp_mission::pushMISSON_PC() {
	pushMstack_areg( MISSON_PC & 0xff );
	pushMstack_areg( MISSON_PC >> 8 );
}

void rp_mission::popMISSON_PC() {
	MISSON_PC = popMstack_areg();
	MISSON_PC <<= 8;
	MISSON_PC |= popMstack_areg();
}



