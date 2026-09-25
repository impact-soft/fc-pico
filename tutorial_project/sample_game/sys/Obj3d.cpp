/*
    Canvas.h - Simple canvas.
 */

#include "Arduino.h"
#include "ArduinoGL.h"
#include "Canvas.h"
#include "rp_system.h"

#include "Obj3d.h"

//========================================================
//
//				global func
//
//========================================================


float LookAtMatrices[16];

void initLookAt( int mode ) {
	glLoadIdentity();
	static int timer = 0;
	float zoom = 20.0f;
//	float zoom = 30.0f;

	switch( mode ) {
	case 0:
		gluLookAt(0, 0, -15, 0, 1.0f, 0, 0, 1, 0);
		break;
	case 1:
//		gluLookAt(0.0f, 0.3f*zoom, -1.0*zoom, 0, 0.0f, 0, 0, 1, 0);
		gluLookAt(0.0f, 1.3f*zoom, -1.0*zoom, 0, 0.0f, 0, 0, 1, 0);
//		gluLookAt(-5.0f, 0.7f*zoom, -1.0*zoom, 5.0f, 0.0f, 0, 0, 1, 0);
//		gluLookAt(0.0f, 2.0f*zoom, -2*zoom, 0, 0.0f, 0, 0, 1, 0);
//		gluLookAt(0, 3, -15, 0, 1.0f, 0, 0, 1, 0);
		break;
	case 2:
		gluLookAt(0, 0, -15 -(timer & 0x3f), 0, 0, 0, 0, 1, 0);
		break;
	}

	copyMatrix( LookAtMatrices, NULL );

	timer++;
}

void moveLookAt( int x, int y ) {
	glLoadIdentity();
	float zoom = 20.0f;
	float mx = 0.0f + (x -128) * 0.06f;
	gluLookAt( -mx, 0.3f*zoom, -1.0*zoom, -mx, 0.0f, 0, 0, 1, 0);
//	gluLookAt(0.0f + (x -128) * 0.05f, 0.3f*zoom, -1.0*zoom, 0, 0.0f, 0, 0, 1, 0);
	copyMatrix( LookAtMatrices, NULL );
}

void reloadLookAt( ) {
	glLoadMatrixf( LookAtMatrices );
}



//========================================================
//
//				public class func
//
//========================================================


Obj3d::Obj3d(void) {
}


void Obj3d::init() {
	m_mode = OMD_NONE;
	m_model_dt = NULL;
	m_model_pn = 0;
	m_scale = 1.0f;
	m_angle_x = 0;
	m_angle_y = 0;
	m_angle_z = 0;
	m_angle_vx = 0;
	m_angle_vy = 0;
	m_angle_vz = 0;
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_vx = 0;
	m_vy = 0;
	m_vz = 0;

	m_color = 3;
	m_DitherAdd = 0;
}

void Obj3d::setModelData( const float *dt, int pn, const uint8_t *col_dt ) {
	m_mode = OMD_MODEL;
	m_model_dt = dt;
	m_model_pn = pn;
	m_model_col_dt = col_dt;
}


void Obj3d::move() {
	if ( m_mode == OMD_NONE ) return;

	m_angle_x += m_angle_vx;
	m_angle_y += m_angle_vy;
	m_angle_z += m_angle_vz;
	m_x += m_vx;
	m_y += m_vy;
	m_z += m_vz;

}


void Obj3d::draw() {
	if ( m_mode == OMD_NONE ) return;

	reloadLookAt();

	glTranslatef( m_x, m_y, m_z );
	if ( m_mode < OMD_2D ) {
		if ( m_angle_z != 0 ) {
			glRotatef( 360.f * m_angle_z / 256 , 0.f, 0.f, 1.f);
		}
		if ( m_angle_y != 0 ) {
			glRotatef( 360.f * m_angle_y / 256 , 0.f, 1.f, 0.f);
		}
		if ( m_angle_x != 0 ) {
			glRotatef( 360.f * m_angle_x / 256 , 1.f, 0.f, 0.f);
		}
		
		if ( m_scale != 1.f ) {
			glScalef( m_scale, m_scale, m_scale);
		}
	} else {
		c.setSprZoom( m_scale, m_scale );
	}

	c.setDefCol( m_color );
	c.setDitherAdd( m_DitherAdd );


	switch ( m_mode ) {
	case OMD_MODEL:
		drawModel();
		break;
	
	case OMD_CUBE:
		drawCube();
		break;
		
	case OMD_PSET:
		drawPSET();
		break;
	case OMD_SPR16:
		drawSPR16();
		break;
	case OMD_SPR8:
		drawSPR8();
		break;
	
	default:
		break;
	}


}


//========================================================
//
//				private class func
//
//========================================================

void Obj3d::drawCube() {
	GLDrawMode gl_mode = GL_POLYGON;
///*

//	gl_mode = GL_LINE_LOOP;
  glBegin(gl_mode);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
  glEnd();

  glBegin(gl_mode);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
  glEnd();

  glBegin(gl_mode);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
  glEnd();
//*/

	// test
	gl_mode = GL_POLYGON;

  glBegin(gl_mode);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
  glEnd();

//	gl_mode = GL_LINE_LOOP;

///*
  glBegin(gl_mode);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    glVertex3f(-1, -1, -1);
  glEnd();

  glBegin(gl_mode);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
  glEnd();
//*/
}


void Obj3d::drawPSET() {
	GLDrawMode gl_mode = GL_POINTS;
	glBegin(gl_mode);
    glVertex3f( 0, 0, 0 );
	glEnd();
}

void Obj3d::drawSPR16() {
	c.setSprChr( m_chrNo );
	GLDrawMode gl_mode = GL_SPR16;
	glBegin(gl_mode);
    glVertex3f( 0, 0, 0 );
	glEnd();
}

void Obj3d::drawSPR8() {
	c.setSprChr( m_chrNo );
	GLDrawMode gl_mode = GL_SPR8;
	glBegin(gl_mode);
    glVertex3f( 0, 0, 0 );
	glEnd();
}


void Obj3d::glVertexFromMemory(int i) {
    glVertex3f(pgm_read_float(&m_model_dt[i*3 + 0]), pgm_read_float(&m_model_dt[i*3 + 1]), pgm_read_float(&m_model_dt[i*3 + 2]));
}


void Obj3d::drawModel(void) {

	if ( m_model_col_dt == NULL ) {
		for(int i = 0; i < m_model_pn; i++) {
			glBegin(GL_POLYGON);
			glVertexFromMemory(i*3);
			glVertexFromMemory(i*3 + 1);
			glVertexFromMemory(i*3 + 2);
			glEnd();
		}
	} else {
		for(int i = 0; i < m_model_pn; i++) {
			uint8_t col = (uint8_t)m_model_col_dt[i];
			int add = (col >> 2) & 0x1f;
			c.setDefCol( col & 0x3 );
			c.setDitherAdd( add );

			glBegin(GL_POLYGON);
			glVertexFromMemory(i*3);
			glVertexFromMemory(i*3 + 1);
			glVertexFromMemory(i*3 + 2);
			glEnd();
		}
	}
}



