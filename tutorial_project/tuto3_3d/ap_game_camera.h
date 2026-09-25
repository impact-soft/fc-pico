
//-----------------------------------------------
//			カメラ制御
//-----------------------------------------------
void ap_game::updateCamera() {
	float p[9];
	Obj3d *obj = &ap.m_obj[OBJ_GAM_PLAYER];

	if ( ap.m_DemoFG == 0 ) {
		float x = obj->m_x;
		float y = obj->m_y;
		float z = obj->m_z;
		// game play camera
		if ( sys.SaveData[ SDT_GAME_MODE ] == 0 ) {
			float zoom = 8.0f;
	//		float zoom = 6.0f;
			p[0] = -x + ply_dir_x*zoom;
	//		p[1] = y + 1.3f*zoom;
	//		p[1] = y + 1.0f*zoom;
	//		p[1] = y + 12.0f;
			p[1] = y + 16.0f;
			p[2] = z - ply_dir_z*zoom;
	//		p[0] = -x;
	//		p[1] = y + 1.3f*zoom;
	//		p[2] = z +  -1.0*zoom;
			p[3] = -x;
			p[4] = y +0.5f;
			p[5] = z;
			p[6] = 0;
			p[7] = 1;
			p[8] = 0;
		} else {
			float zoom = 8.0f;
			p[0] = -x + ply_dir_x*zoom;
			p[1] = y +  8.0f;
			p[2] = z - ply_dir_z*zoom;
	//		p[0] = -x;
	//		p[1] = y + 1.3f*zoom;
	//		p[2] = z +  -1.0*zoom;
			p[3] = -x;
			p[4] = y +0.5f;
			p[5] = z;
			p[6] = 0;
			p[7] = 1;
			p[8] = 0;
		}
		setCamera( 0, p );
	
	} else {
		cam_prm[3] = -obj->m_x;
		cam_prm[4] = obj->m_y;
		cam_prm[5] = obj->m_z;

		// demo play camera
		if ( moveCamera() ) {

			float x = -obj->m_x;
			float y = obj->m_y;
			float z = obj->m_z;
			float zoom = 10.0f;
		
	//		p[0] = x;
	//		p[1] = y+(zoom/4);
	//		p[2] = z-zoom;

			p[0] = x + random( -3 , 3 );
			p[1] = y + random(  0 , 10 );
			p[2] = z + random( -3 , 3 );
			
			p[3] = x;
			p[4] = y;
			p[5] = z;
			p[6] = 0;
			p[7] = 1;
			p[8] = 0;
		
	//		setLookAtObj( obj, 10.0f, 0.0f, 0.0f, 0.0f );

	//	gluLookAt( x, y+(zoom/4), z-zoom, x, y +1.0f, z, 0, 1, 0);
	//	gluLookAt( x, y+(zoom/4), z-zoom, x, y, z, 0.5f, 1, 0);
	//	gluLookAt( -mx, 0.3f*zoom, -1.0*zoom, -mx, 0.0f, 0, 0, 1, 0);
	//	gluLookAt(0.0f + (x -128) * 0.05f, 0.3f*zoom, -1.0*zoom, 0, 0.0f, 0, 0, 1, 0);

			setCamera( 0, p );

			p[0] = x + random( -5 , 5 );
			p[1] = y + random( -2 , 8 );
			p[2] = z + random( -10 , 10 );

	//		p[6] = 1;
	//		p[7] = 0;
			setCamera( 180, p );
		
		}
	}
	setCameraMatrices();
}


