/*
    UsrObj3d.cpp
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

#include "UsrObj3d.h"

const uint8_t test_dungeon[] = {
	1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
	1,0,0,1, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,1, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,1, 0,0,0,0, 0,0,0,0, 0,0,0,1,

	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 1,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 1,0,0,0, 0,0,0,1,

	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,

	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
	1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
};

void drawField() {

	const uint8_t *pMap = test_dungeon;

	if ( ap_g.objPLY == NULL ) return;

#if 0
    float modelviewProjection[16];
    multMatrix(modelviewProjection, glMatrices[GL_PROJECTION], glMatrices[GL_MODELVIEW]);

    int frameWidth  = c.width();
    int frameHeight = c.height();

	int bx[ FLD_PW ];
	int by[ FLD_PW ];

    GLVertex v;
    v.y = 0.0f;
    v.z = P_SIZE * -(FLD_PH/2);
    v.w = 1.0f;

	
	for ( int y = 0 ; y < FLD_PH; y++, v.z += P_SIZE ) {
		v.x = P_SIZE * -(FLD_PW/2);
	
		for ( int x = 0 ; x < FLD_PW; x++, v.x += P_SIZE ) {
			GLVertex aux = multVertex(modelviewProjection, v);
			if ( aux.w == 0 ) {
				bx[ x ] = -10000;	//描画しない
				continue;
			}
			aux.x = aux.x/aux.w;
			aux.y = aux.y/aux.w;
			aux.z = aux.z/aux.w;

			if( !isDispArea( &aux ) ) {
				bx[ x ] = -10000;	//描画しない
				continue;
			}

            int px = (((aux.x + 1.0)/2.0) * (frameWidth - 1));
            int py = ((1.0 - ((aux.y + 1.0)/2.0)) * (frameHeight - 1));
			c.setZval( aux.z );
			// 横ライン描画
			if ( x > 0 ) {
				if (  bx[ x -1 ] != -10000 ) {
					c.drawLine( bx[ x -1 ], by[ x -1 ], px, py);
				}
			}

			// 縦ライン描画
			if ( y > 0 ) {
				if (  bx[ x ] != -10000 ) {
					c.drawLine( bx[ x ], by[ x ], px, py);
				}
			}

			bx[ x ] = px;
			by[ x ] = py;
		}
	}

#else
	GLDrawMode gl_mode = GL_POLYGON;
///*
	float sy = P_SIZE * -8;
	float dislimit =  P_SIZE*P_SIZE*6*6;
	
	for ( int y = 0 ; y < 16; y++,sy += P_SIZE ) {
		float sx = P_SIZE * -8;
	
		for ( int x = 0 ; x < 16; x++,sx += P_SIZE ) {
//			if ( ap_g.objPLY->getDistanceSQpxz( sx, -sy ) > dislimit ) continue;

				glBegin(GL_LINE_LOOP);
			    glVertex3f(sx, 0, sy);
			    glVertex3f(sx+P_SIZE, 0, sy);
			    glVertex3f(sx+P_SIZE, 0, sy+P_SIZE);
			    glVertex3f(sx, 0, sy+P_SIZE);
				glEnd();

#if 0
			if ( pMap[ y * FLD_PW + x] == 0 ) {
				glBegin(GL_LINE_LOOP);
			    glVertex3f(sx, 0, sy);
			    glVertex3f(sx+P_SIZE, 0, sy);
			    glVertex3f(sx+P_SIZE, 0, sy+P_SIZE);
			    glVertex3f(sx, 0, sy+P_SIZE);
				glEnd();
			} else {
				// 上板
/*
				glBegin(GL_POLYGON);
			    glVertex3f(sx, P_SIZE, sy);
			    glVertex3f(sx+P_SIZE, P_SIZE, sy);
			    glVertex3f(sx+P_SIZE, P_SIZE, sy+P_SIZE);
			    glVertex3f(sx, P_SIZE, sy+P_SIZE);
				glEnd();
*/
				// 左壁
				glBegin(GL_POLYGON);
			    glVertex3f(sx, 0,  sy);
			    glVertex3f(sx, 0,  sy+P_SIZE);
			    glVertex3f(sx, P_SIZE, sy+P_SIZE);
			    glVertex3f(sx, P_SIZE, sy);
				glEnd();

				// 右壁
				glBegin(GL_POLYGON);
			    glVertex3f(sx+P_SIZE, 0,  sy);
			    glVertex3f(sx+P_SIZE, 0,  sy+P_SIZE);
			    glVertex3f(sx+P_SIZE, P_SIZE, sy+P_SIZE);
			    glVertex3f(sx+P_SIZE, P_SIZE, sy);
				glEnd();
			
				// 上壁
				glBegin(GL_POLYGON);
			    glVertex3f(sx, 0,  sy);
			    glVertex3f(sx+P_SIZE, 0,  sy);
			    glVertex3f(sx+P_SIZE, P_SIZE, sy);
			    glVertex3f(sx, P_SIZE, sy);
				glEnd();

				// 下壁
				glBegin(GL_POLYGON);
			    glVertex3f(sx, 0,  sy+P_SIZE);
			    glVertex3f(sx+P_SIZE, 0,  sy+P_SIZE);
			    glVertex3f(sx+P_SIZE, P_SIZE, sy+P_SIZE);
			    glVertex3f(sx, P_SIZE, sy+P_SIZE);
				glEnd();
			
			}
#endif
/*
			if ( x == 0 ) {
				glBegin(GL_POLYGON);
			    glVertex3f(sx, 0,  sy);
			    glVertex3f(sx, 0,  sy+P_SIZE);
			    glVertex3f(sx, P_SIZE, sy+P_SIZE);
			    glVertex3f(sx, P_SIZE, sy);
				glEnd();
			}
*/
			
			
		}
		
	}
#endif

}

void drawUsrObj3d( uint8_t m_mode ) {
	switch ( m_mode ) {
	case OMD_FIELD:
		drawField();
		break;
	

	default:
		break;
	}




}

