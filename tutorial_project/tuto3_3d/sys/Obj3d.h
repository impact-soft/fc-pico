/*
    Obj3d.h
 */

#ifndef Obj3d_h
#define Obj3d_h

#include "Arduino.h"

enum {
	OMD_NONE = 0,
	OMD_MODEL,		// モデル描画モード
	OMD_WIRE,		// ワイヤーフレームモデル描画モード
	OMD_CUBE,		// CUBE描画モード

	OMD_2D,			// ---- 以下は2D描画モード ---
	OMD_PSET,		// 点描画モード
	OMD_SPR16,		// 16x16スプライト描画モード
	OMD_SPR8,		// 8x8スプライト描画モード
	OMD_USR,		// USR定義描画モード

};



class Obj3d {
public:
    Obj3d();
	void init();
	void setModelData( const float *dt, int pn, const uint8_t *col_dt = NULL );
	void setAngle( float ax, float ay,float az ) {
		m_angle_x = ax;
		m_angle_y = ay;
		m_angle_z = az;
	}
	float getDistanceSQ( Obj3d *obj );
	float getDistanceSQpxz( float x, float z );
	float getDistance( Obj3d *obj );
	float getObjAngleXZ( Obj3d *obj );
	float getObjAngleXZpxz( float x, float z );
	bool turnObjY( float TargetAngle, float spd );

	void draw();
	void move();

	uint8_t m_mode;
	uint8_t m_color;
	uint8_t m_chrNo;
	uint8_t m_cnt;		///< 汎用カウンター
	float m_angle_x;
	float m_angle_y;
	float m_angle_z;
	float m_angle_vx;
	float m_angle_vy;
	float m_angle_vz;
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
	void drawWire(void);

	const float *m_model_dt;
	int  m_model_pn;
	const uint8_t *m_model_col_dt;

};





extern void initLookAt( int mode );
extern void reloadLookAt();
extern bool moveCamera();
extern void setCameraMatrices();
extern void setCamera(int flm ,float *p );
extern float cam_prm[9];

extern float turnAngele( float BaseAngle ,float TargetAngle, float spd );


#endif

