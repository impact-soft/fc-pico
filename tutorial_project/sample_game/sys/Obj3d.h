/*
    Obj3d.h
 */

#ifndef Obj3d_h
#define Obj3d_h

#include "Arduino.h"

enum {
	OMD_NONE = 0,
	OMD_MODEL,		// モデル描画モード
	OMD_CUBE,		// CUBE描画モード

	OMD_2D,			// ---- 以下は2D描画モード ---
	OMD_PSET,		// 点描画モード
	OMD_SPR16,		// 16x16スプライト描画モード
	OMD_SPR8,		// 8x8スプライト描画モード

};



class Obj3d {
public:
    Obj3d();
	void init();
	void setModelData( const float *dt, int pn, const uint8_t *col_dt = NULL );
	void setAngle( uint8_t ax, uint8_t ay,uint8_t az ) {
		m_angle_x = ax;
		m_angle_y = ay;
		m_angle_z = az;
	}
	void draw();
	void move();

	uint8_t m_mode;
	uint8_t m_color;
	uint8_t m_chrNo;
	uint8_t m_angle_x;
	uint8_t m_angle_y;
	uint8_t m_angle_z;
	uint8_t m_angle_vx;
	uint8_t m_angle_vy;
	uint8_t m_angle_vz;
	int  m_DitherAdd;


	float m_scale;
	float m_x;
	float m_y;
	float m_z;
	float m_vx;
	float m_vy;
	float m_vz;

private:
	void drawPSET();
	void drawSPR16();
	void drawSPR8();
	void drawCube();
	void glVertexFromMemory(int i);
	void drawModel(void);

	const float *m_model_dt;
	int  m_model_pn;
	const uint8_t *m_model_col_dt;

};





extern void initLookAt( int mode );
extern void reloadLookAt();
extern void moveLookAt( int x, int y );

#endif

