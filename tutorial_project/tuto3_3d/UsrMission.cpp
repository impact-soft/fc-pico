/*
    UsrMission.cpp
 */

/**
 * @file ap_data.cpp
 * @brief Where the resource archive and the 3D model data land in flash.
 * @ingroup sample_app
 *
 * Includes the generated `res/resdata.c` -- the packed archive of boot ROM, game
 * ROM, music, artwork and licence pages -- plus one generated `.c` per 3D model.
 * The models are separate arrays rather than archive entries because they are
 * linked structures, not opaque blobs.
 *
 * @see @ref generated_resources
 */

#include "ap_main.h"


enum {
	SET_FADE_IN_B	= 0xF000,
	PLAY_BGM		= 0xF001,
//	setEnemyNT3		= 0xF002,
//	getEnemyNTcfg	= 0xF003,
	_STAGE_INIT		= 0xF004,
	selSPE_MP		= 0xF005,
	setSPE			= 0xF006,
	_STAGE_INIT2	= 0xF007,
	_STAGE_INIT3	= 0xF008,
};


const uint8_t *tblSPE_MP;
const uint8_t *tblEnemyNTcfg;
const uint8_t *misson_anmtblData;
uint8_t MISSON_ATK_IDX;



const uint8_t *getEnemyNTcfg( uint8_t kind ) {
	kind--;
	kind &= 0x7f;
	return &tblEnemyNTcfg[ kind * 4 ];
}


//-----------------------------
// 敵が残っていたらtrue
//-----------------------------
bool _checkEnemy() {
	Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY];
	for ( int j=0; j < ENEMY_SUU ; j++,obj++ ) {
		if ( obj->m_mode != OMD_NONE ) return true;
	}
	return false;
};


bool UsrMisson_mcj_sub( uint8_t mcj ) {
	bool bl = false;
	
	switch( mcj ) {
	case MCJ_ENEMY_Z:
		bl = !_checkEnemy();
		Serial.printf("MCJ_ENEMY_Z %d\n", bl);
		break;
	
	case MCJ_ENEMY_NZ:
		bl = _checkEnemy();
		Serial.printf("MCJ_ENEMY_NZ %d\n", bl );
		break;
	}

/*
	条件はゲームを作りながら必要に応じて追加


*/
	return bl;
}



void UsrMisson_PG_CALL( uint16_t pc, uint8_t a, uint8_t y, const uint8_t *tmpAdr ) {
	pc++;		// pcには対象アドレス-1が入った状態で呼び出されている
	switch ( pc ) {
	case SET_FADE_IN_B:
		Serial.printf("SET_FADE_IN_B\n");
		break;
	case PLAY_BGM:
		Serial.printf("PLAY_BGM\n");
		snd.playBGM( a );
		break;
	case _STAGE_INIT:
		Serial.printf("_STAGE_INIT %x\n", tmpAdr);
		gMI_RAM[ MISSON_ANM_NO ] = 0;
		tblSPE_MP = tmpAdr;
/*

.stege_init
	ldx  PLY_STAGE
	lda  .tbl_SECRET_LIFE_ADD,x
	sta  SECRET_LIFE_ADD
	lda  #0
	sta  SECRET_STAT
	sta  MISSON_ANM_NO
	rts

.tbl_SECRET_LIFE_ADD
	db 1				;0
	db ADD_SC_LIFE_ST1	;1
	db ADD_SC_LIFE_ST2	;2
	db ADD_SC_LIFE_ST3	;3
	db ADD_SC_LIFE_ST4	;4
	db ADD_SC_LIFE_ST5	;5
	db 1				;6
	db 1				;7
	db 1				;8

*/
	
		break;

	case _STAGE_INIT2:
		Serial.printf("_STAGE_INIT2 %x\n", tmpAdr);
		tblEnemyNTcfg = tmpAdr;
		break;

	case _STAGE_INIT3:
		Serial.printf("_STAGE_INIT3 %x\n", tmpAdr);
		misson_anmtblData = tmpAdr;
		break;

	case selSPE_MP:
		Serial.printf("selSPE_MP %d\n", a);
		if ( tblSPE_MP == NULL ) break;
		gMI_RAM[ PRM_2 ] = tblSPE_MP[ (gMI_RAM[ ENEMY_ATK_LV ] >> 6) + a*4 ];
		break;

	case setSPE:
		{
			float x =  (( FLD_LIM_W*2 * a) / 256) - FLD_LIM_W;
			float z =  ((-FLD_LIM_H*2 * y) / 256) + FLD_LIM_H;
			Serial.printf("setSPE %d %d : %f %f\n", a, y, x, z);
			uint8_t kind = gMI_RAM[ PRM_1 ];
			ap_g.setEnemy( kind, x, z );
		}
		break;
	
	default:
		Serial.printf("USR_PG_CALL %04x\n", pc);
		break;
	}


}


void _misson_atk_sub() {
	const uint8_t tblCntAddAtk[] = {
		 10,8,6,4
//		 20,15,10,5
	};

	uint8_t a;
	if ( gMI_RAM[ MISSON_ATK_CNT ] > 0 ) {
		gMI_RAM[ MISSON_ATK_CNT ]--;
		return;
	}
	a = gMI_RAM[ MISSON_ATK_NO ];
	gMI_RAM[ MISSON_ATK_CNT ] = tblCntAddAtk[ a & 3 ];
	a >>= 2;
	if ( a == 0 ) { //  0 攻撃しない
		return;
	}

	// 発射する敵を選択

	uint8_t shot_idx = MISSON_ATK_IDX;
	MISSON_ATK_IDX++;
	shot_idx %= ENEMY_SUU;
	if ( ap_g.m_emobj_kind[ shot_idx ] == 0 ) return;
	if ( ap_g.m_emobj_hp[ shot_idx ] == 0 ) return;
	if ( ap_g.m_emobj_hp[ shot_idx ] >= 0x80 ) return;

	Obj3d *obj = &ap.m_obj[OBJ_GAM_ENEMY + shot_idx];

	int idx = ap_g.setEnemy( NTK_NOMAL, obj->m_x, obj->m_z );
	if ( idx >= 0 ) {
		float dir = obj->getObjAngleXZ( ap_g.objPLY );
		ap_g.m_emobj_dir[idx] = dir;
		ap_g.m_emobj_spd[idx] = 0.1f;

		if ( a == 1 ) { //  1 1発発射
			return;
		}
		//  1 ３ウェイ発射
		int idx2 = ap_g.setEnemy( NTK_NOMAL, obj->m_x, obj->m_z );
		if ( idx2 >= 0 ) {
			ap_g.m_emobj_dir[idx2] = dir - 5.0f;
			ap_g.m_emobj_spd[idx2] = 0.1f;
		}
		int idx3 = ap_g.setEnemy( NTK_NOMAL, obj->m_x, obj->m_z );
		if ( idx3 >= 0 ) {
			ap_g.m_emobj_dir[idx3] = dir + 5.0f;
			ap_g.m_emobj_spd[idx3] = 0.1f;
		}
	}

}

void _misson_anm_sub() {
	const uint8_t tblCntAdd[] = {
//		40,30,20,10
//		32,16,8,4
		16,8,4,2
	};


	uint8_t a = gMI_RAM[ MISSON_ANM_NO ];
	if ( a < 0x80 ) return;
	
	if ( gMI_RAM[ MISSON_ANM_CNT ] > 0 ) {
		gMI_RAM[ MISSON_ANM_CNT ]--;
		return;
	}
	
	gMI_RAM[ MISSON_ANM_CNT ] = tblCntAdd[ a & 3 ] + random(16);
	a = (a & 0b01111100) + random(4);
	uint8_t idx = (a << 2);

	gMI_RAM[ PRM_1 ] = misson_anmtblData[ idx + 2 ];
	gMI_RAM[ PRM_2 ] = misson_anmtblData[ idx + 3 ];

	uint8_t y = misson_anmtblData[ idx + 1 ];
	uint8_t x = misson_anmtblData[ idx + 0 ];

	if ( y == 255 ) {
		y = random(12, 255-12);
		y &= 0b11111000;

	}
	if ( x == 255 ) {
		x = random(12, 255-12);
		x &= 0b11111000;
	}

	uint8_t kind = gMI_RAM[ PRM_1 ];
	float fx =  (( FLD_LIM_W*2 * x) / 256) - FLD_LIM_W;
	float fz =  ((-FLD_LIM_H*2 * y) / 256) + FLD_LIM_H;
	Serial.printf("_misson_anm_sub %f %f\n", fx, fz);
	ap_g.setEnemy( kind, fx, fz );
}

/*


;------------------------------------
; 敵セット命令 areg 座標番号
;------------------------------------
.setEnemyNT_hara:
	asl a
	asl a
	tay
	lda  .tblData+2,y
	sta  <PRM_1
	lda  .tblData+3,y
	sta  <PRM_2

	lda  .tblData+1,y
	cmp  #-1
	bne  .no_rndy

	lda  #ENEMY_LINE_SUU -24
	jsr  GET_RND_N
	adc  #8
	and  #%1111_1000
.no_rndy
	sta  <PRM_Y_POS

	lda  #ENEMY_LINE_SUU -24
	jsr  GET_RND_N
	adc  #16
	and  #%1111_1000

.no_rndx
	sta  <PRM_X_POS
	jmp  .setEnamy

.setEnamy
	lda  <PRM_X_POS
	ldy  <PRM_Y_POS
	jsr  setEnemyNT3
	bcs  .end2

	lda  #0
	sta  <CACHE_GET_NENMY_NT_FG

	lda  <PRM_1
	sta  ENEMY_NT_KIND,y
	jsr  getEnemyNTcfg
	lda  <TMP_WRK1
	sta  ENEMY_NT_HP,y
	
	lda  <PRM_2
	sta  ENEMY_NT_MP,y

.end2
	rts




*/




void MissionAnime() {
	uint8_t bkPRM_1 = gMI_RAM[ PRM_1 ];
	uint8_t bkPRM_2 = gMI_RAM[ PRM_2 ];
	_misson_anm_sub();
	_misson_atk_sub();
	gMI_RAM[ PRM_2 ] = bkPRM_2;
	gMI_RAM[ PRM_1 ] = bkPRM_1;
}


