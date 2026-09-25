/*
    Canvas.h - Simple canvas.
 */

#ifndef Canvas_h
#define Canvas_h

#include "Arduino.h"

#define CANVAS_WIDTH 256
#define CANVAS_HEIGHT 240
#define FRAME_BUF_SIZE (CANVAS_WIDTH * CANVAS_HEIGHT)

#define FLIP_W	1		// 左右反転
#define FLIP_H	2		// 上限反転


class Canvas {
    
public:
    Canvas();
    void clear();
    void setClip(int xl, int yl,int xh, int yh ) {
    	clipXL = xl;
    	clipXH = xh;
    	clipYL = yl;
    	clipYH = yh;
    };
    void setClipWH(int x, int y,int w, int h ) {
		setClip( x, y ,x + w, y+h );
    };

    // スプライトキャラデータアドレスセット
    void setSprData( const uint8_t *pData ) {
		pSprData = pData;
	};

   	// スプライト描画キャラ番号セット
    void setSprChr( const uint8_t c ) {
		SprChr = c;
	};

   	// スプライト描画倍率セット
    void setSprZoom( float mgw, float mgh );

   	// w値でスプライト描画倍率セット
    void setSprZoomW( float w );

   	// スプライト反転描画セット
    void setSprFlip( const uint8_t flip ) {
		SprFlip = flip;
	};

    void setPixel(int x, int y );
    void drawLine(int startX, int startY, int endX, int endY);
    void drawCircle(int centreX, int centreY, int radius);
    void drawSquare(int startX, int startY, int endX, int endY);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);

	void draw_triangle( int x1, int y1, int x2, int y2, int x3, int y3);
//	void draw_triangle2( int x1, int y1, int x2, int y2, int x3, int y3);

	void drawSPR16( int x, int y );
	void drawSPR8(  int x, int y );
	void drawSPR_WH( int x, int y, int cw, int ch );

	void drawCHR( uint8_t c, int x, int y, const uint8_t* pData );
	void drawString( const char *str, int x, int y, const uint8_t* pData );
    
    uint8_t* bitmap();
    
    int width();
    int height();

    void setDefCol( uint8_t c ) {
		defCol = c;
	}

    void setDitherAdd( int add ) {
		m_DitherAdd = add;
	}
    void setZval( float fz );
    void setDitherNo( int no );
    uint8_t getDitherCol( int x, int y );

	
private:
	void draw_Xaxis( int y, int x2, int x3 );
	void draw_flatTriangle( int x1, int y1, int x2, int y2, int x3);
	void makeCBUF(uint8_t c, uint8_t* pCBUF , const uint8_t* pData );

	uint8_t frame_buff[FRAME_BUF_SIZE *2];
	const uint8_t *pSprData;
	uint8_t defCol;
	uint8_t DitherNo;
	uint8_t zval_L;
	uint8_t zval_H;
	int  m_DitherAdd;

	int clipXL;
	int clipXH;
	int clipYL;
	int clipYH;

	uint8_t SprChr;
	uint8_t SprFlip;
	uint8_t Spr_nw;
	uint8_t Spr_nh;
	bool  Spr_mg;
	float Spr_mgw;
	float Spr_mgh;

};

extern Canvas c;

#endif

