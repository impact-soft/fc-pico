/*
    ap_game.h
 */

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
	m_PlyDamFg = 0;

	ap.PLY_LIFE = 10;
	PLY_ANM = PLY_AN_WAIT;
	/* Pass the canvas to the OpenGL environment */
	glUseCanvas(&c);

	c.setSprData( _acOBJ );

	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(4);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//  glOrtho(-5, 5, -5, 5, 0.1, 999.f);
	gluPerspective(45.0, c.width() / c.height(), 0.1f, 9999.f);
//	gluPerspective(50.0, c.width() / c.height(), 0.1f, 9999.f);

	glMatrixMode(GL_MODELVIEW);


	sys.setPalData(pal_game);
	sys.startDataMode();

//	initLookAt(1);
	initLookAt(0);
	setLight(1.0f, 0.0f, 0.5f);

	ap.initObj();

	memset(m_emobj_kind, 0, ENEMY_NT_SUU);

//	memset( &emu.m_RAM[PSHOT_A_X], 0, PSHOT_A_SUU*2);
//	memset( &emu.m_RAM[ENEMY_NT_WORK], 0, ENEMY_NT_SIZE * ENEMY_NT_SUU );
//	memset( &emu.m_RAM[BAKU_EFC_X], 0, BAKU_EFC_SUU * 3 );



	// game init
//	emu.run(0xE000 + 4 * 0, 0, 0x00, 0x00);

//	ap.initStarObjGame(OBJ_GAM_STAR, 80, -0.8f);
//	ap.initStarObj( OBJ_GAM_STAR, 80,  -1.0f );

	objPLY = &ap.m_obj[OBJ_GAM_PLAYER];
	objSWD = NULL;
	objATK = NULL;

	setModelDataObj(objPLY, MDL_PLAYER_NO);
	objPLY->m_z = -FLD_LIM_H + 1.0f;

/*
	{
		Obj3d *obj = &ap.m_obj[OBJ_GAM_DEBUG];
		obj->m_mode = OMD_CUBE;
		obj->m_scale = 0.2f;
		obj->m_color = 3;
		obj->setAngle(32, 32, 32);
		obj->m_angle_vx = random(-5, 5);
		obj->m_angle_vy = random(-5, 5);
		obj->m_DitherAdd = 2;
	}

	{
		Obj3d *obj = &ap.m_obj[OBJ_GAM_DEBUG+1];
		obj->m_mode = OMD_CUBE;
		obj->m_scale = 0.2f;
		obj->m_color = 3;
		obj->setAngle(32, 32, 32);
		obj->m_angle_vx = random(-5, 5);
		obj->m_angle_vy = random(-5, 5);
		obj->m_DitherAdd = 2;
	}
*/

//	setModelDataObj(obj, MDL_PLAYER_NO);
//	obj->m_angle_z = 180.0f;
	{
		Obj3d *obj = &ap.m_obj[OBJ_GAM_FIELD];
		obj->m_mode = OMD_FIELD;
		obj->m_angle_y = 180.0f;
		obj->m_y = -0.5f;
		obj->m_color = 2;
	}

	DashCounter = 0;


	mt.initMission();


//	Serial.printf("ap_game:init:stage : m_DemoFG %d:%d\n", emu.m_RAM[EM_PLY_STAGE] ,ap.m_DemoFG );

	//Serial.printf("ap_game:init end\n");
}

