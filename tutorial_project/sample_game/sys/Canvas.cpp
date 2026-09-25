/*
    Canvas.h - Simple canvas.
 */

#include "Arduino.h"
#include "Canvas.h"


Canvas::Canvas(void) {
}

void Canvas::clear(void) {

	uint32_t *pFB = (uint32_t *)frame_buff;
	for (int i = 0 ; i < FRAME_BUF_SIZE*2 / sizeof( uint32_t ) ; i++ ) {
		pFB[ i ] = 0;
	}
	zval_L = zval_H = 0;
	setClip( 0,0,CANVAS_WIDTH,CANVAS_HEIGHT );
	Spr_mg = false;
    setSprFlip( 0 );
}

int Canvas::width(void) {
    
    return CANVAS_WIDTH;
}

int Canvas::height(void) {
    
    return CANVAS_HEIGHT;
}

uint8_t* Canvas::bitmap(void) {
    
    return frame_buff;
}

uint8_t Canvas::getDitherCol( int x, int y ) {
	const uint16_t DitherTbl[] = {
	//FEDCBA9876543210
	0b1111111111111111,  // 0
	0b1111111111111110,  // 1
	0b1111101111111110,  // 2
	0b1111101111111010,  // 3
	0b1111101011111010,  // 4
	0b1111101011011010,  // 5
	0b0111101011011010,  // 6
	0b0111101001011010,  // 7
	0b0101101001011010,  // 8
	0b0101101001011000,  // 9
	0b0101001001011000,  // 10
	0b0101001001010000,  // 11
	0b0101000001010000,  // 12
	0b0101000001000000,  // 13
	0b0001000001000000,  // 14
	0b0001000000000000,  // 15
	};

	uint8_t c = defCol;
	if ( DitherNo == 0 ) {
	    return c;
	}
	int idx = (x & 3) | (( y & 3 ) <<2);
	if ( DitherTbl[ DitherNo ] & (1<<idx) ) {
	    return c;
	}
    return 0;
}


void Canvas::setPixel(int x, int y ) {
	
    if((x >= clipXL) && (x < clipXH) && (y >= clipYL) && (y < clipYH)) {
        frame_buff[x + y*CANVAS_WIDTH] = getDitherCol( x, y );
    }
}


void Canvas::drawLine(int startX, int startY, int endX, int endY) {
    
    int diffX = (endX - startX);
    int diffY = (endY - startY);
    
    if(abs(diffX) > abs(diffY)) {
        
        float dy = diffY/(float)diffX;
        
        if(endX > startX) {
            
            for(int x = startX; x <= endX; x++) {
                
                this->setPixel(x, floor(startY + dy*(x - startX)) );
            }
        }
        
        else {
            
            for(int x = startX; x >= endX; x--) {
                
                this->setPixel(x, floor(startY + dy*(x - startX)) );
            }
        }
    }
    
    else {
        
        float dx = diffX/(float)diffY;
        
        if(endY > startY) {
            
            for(int y = startY; y <= endY; y++) {
                
                this->setPixel(floor(startX + dx*(y - startY)), y );
            }
        }
        
        else {
            
            for(int y = startY; y >= endY; y--) {
                
                this->setPixel(floor(startX + dx*(y - startY)), y );
            }
        }
    }
}

void Canvas::drawCircle(int centreX, int centreY, int radius) {
    
    radius = abs(radius);
    
    for(float angle = 0; angle < 2*M_PI; angle+=(M_PI/(4*radius))) {
        
        this->setPixel(centreX + cos(angle)*radius, centreY + sin(angle)*radius );
    }
}

void Canvas::drawSquare(int startX, int startY, int endX, int endY) {
    
    this->drawLine(startX, startY, startX, endY);
    this->drawLine(startX, endY, endX, endY);
    this->drawLine(endX, endY, endX, startY);
    this->drawLine(endX, startY, startX, startY);
}

void Canvas::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    
    this->drawLine(x1, y1, x2, y2);
    this->drawLine(x2, y2, x3, y3);
    this->drawLine(x3, y3, x1, y1);
}

//---- impact soft add -----

// スプライト描画倍率セット
void Canvas::setSprZoom( float mgw, float mgh ) {
	Spr_mgw = mgw;
	Spr_mgh = mgh;
	Spr_mg = false;
	if (mgw != 1.0f) Spr_mg = true;
	if (mgh != 1.0f) Spr_mg = true;
};

// w値でスプライト描画倍率セット
void Canvas::setSprZoomW( float w ) {
	this->setSprZoom( w * Spr_mgw, w * Spr_mgw );
};



void Canvas::makeCBUF(uint8_t c, uint8_t* pCBUF , const uint8_t* pData ) {
	const unsigned char* pDP = &pData[ c * 16 ];
	int dpp = 1;
	if ( SprFlip & FLIP_H ) {
		pDP += 7;
		dpp = -1;
	}

	if ( !(SprFlip & FLIP_W ) ) {
		for ( int idx = 0; idx < 64 ; ) {
			uint16_t p = pDP[0] | (pDP[8]<<8);
			for ( int f = 0; f < 8 ; f++ ) {
				uint8_t pd = 0;
				if ( p & 0x0080 ) pd++;
				if ( p & 0x8000 ) pd+=2;
				pd &= defCol;
				pCBUF[ idx++ ] = pd;
				p <<= 1;
			}
			pDP += dpp;
		}
	} else {
		for ( int idx = 0; idx < 64 ; ) {
			uint16_t p = pDP[0] | (pDP[8]<<8);
			for ( int f = 0; f < 8 ; f++ ) {
				uint8_t pd = 0;
				if ( p & 0x0001 ) pd++;
				if ( p & 0x0100 ) pd+=2;
				pd &= defCol;
				pCBUF[ idx++ ] = pd;
				p >>= 1;
			}
			pDP += dpp;
		}
	}

}



void Canvas::drawCHR(uint8_t c, int x, int y, const uint8_t* pData ) {

	uint8_t cbuf[8*8];
	makeCBUF( c, cbuf , pData );
	uint8_t c_bak = defCol;

	if ( Spr_mg ) {
		if ( Spr_mgw <= 0.0f ) return;
		if ( Spr_mgh <= 0.0f ) return;

		float dw = 8.0f / (8.0f  * Spr_mgw);
		float dh = 8.0f / (8.0f  * Spr_mgh);
		float sx,sy;

		uint8_t cbuf[8*8];
		makeCBUF( c, cbuf , pData );

		sy = 0.0f;
		for ( int dy = 0; sy < 8.0f ; dy++ ) {
			sx = 0.0f;
			for ( int dx = 0; sx < 8.0f ; dx++ ) {
				uint8_t pd = cbuf[ int(sx) + int(sy)*8 ];
				if( pd != 0 ) {
					this->setDefCol( pd );
					this->setPixel( x + dx, y +dy );
				}
				sx += dw;
			}
			sy += dh;
		}
	} else {
		uint8_t *pCB = cbuf;
		for ( int dy = 0; dy < 8 ; dy++ ) {
			for ( int dx = 0; dx < 8 ; dx++ ) {
				uint8_t pd = *(pCB++);
				if( pd != 0 ) {
					this->setDefCol( pd );
					this->setPixel( x + dx, y +dy );
				}
			}
		}
	}
	defCol = c_bak;
}


void Canvas::drawSPR16( int x, int y ) {
	drawSPR_WH( x, y, 2, 2 );
}

void Canvas::drawSPR8( int x, int y ) {
	int dx = 4;
	int dy = 4;
	if ( Spr_mg ) {
		dx = int(4.0f * Spr_mgw);
		dy = int(4.0f * Spr_mgh);
	}
	this->drawCHR( SprChr , x-dx, y-dy, pSprData ); 
}


void Canvas::drawString( const char *str, int x, int y, const uint8_t* pData ) {
	for( int i = 0 ; ; i++ ) {
		uint8_t c = str[i];
		if ( c == 0 ) return;
		this->drawCHR( c, x, y, pData );
		if ( Spr_mg ) {
			x += int(8.0f * Spr_mgw);
		} else {
			x += 8;
		}
	}
}

void Canvas::drawSPR_WH( int x, int y, int cw, int ch ) {

	int idx = 0;
	int dx = 8;
	int dy = 8;

	int bx = x - (cw*4);
	int by = y - (ch*4);
	if ( Spr_mg ) {
		dx = int(8.0f * Spr_mgw);
		dy = int(8.0f * Spr_mgh);
		bx = x - (dx*cw/2);
		by = y - (dy*ch/2);
	}


	if ( (SprFlip & FLIP_W) == FLIP_W ) {
		bx += dx * (cw -1);
		dx *= -1;
	}

	if ( (SprFlip & FLIP_H) == FLIP_H ) {
		by += dy * (ch -1);
		dy *= -1;
	}


	for ( int oy = 0; oy < ch ; oy++ ) {
		for ( int ox = 0; ox < cw ; ox++ ) {
			this->drawCHR( SprChr + ox + oy*0x10, bx + ox*dx, by+ oy*dy	, pSprData );
		}
	}
}

//#define swap(type,a,b) do{type _c;_c=a;a=b;b=_c;}while(0)
#define swap_int(a,b) do{int _c;_c=a;a=b;b=_c;}while(0)
#define ABS(x) ((x) < 0 ? -(x) : (x))

// この関数は将来的にサブコアに負荷分散する
void Canvas::draw_Xaxis( int y, int x2, int x3 ) {
	uint8_t *vram = &frame_buff[ y * CANVAS_WIDTH ];
	uint8_t *zbuff = &frame_buff[ y * CANVAS_WIDTH + FRAME_BUF_SIZE ];
	int d = 0;
	if ( x2 > x3 ) {
		swap_int(x2,x3);
	}

	if ( x2 < 0 ) {
		x2 = 0;
	}
	if ( x3 >= CANVAS_WIDTH ) {
		x3 = CANVAS_WIDTH -1;
	}

	if ( x2 == x3 ) {
    // 書き込みが一点のケース
	    if( zbuff[x2] > zval_H ) return;
	    zbuff[x2] = zval_H;
	    if( zbuff[x2] == zval_H ) {
		    if( vram[x2] > zval_L ) return;
		}
		vram[x2] = getDitherCol( x2, y ) | zval_L;
		return;
	}

	for (int x = x2;  x <= x3; x++ ) {
	    if( zbuff[x] > zval_H ) continue;
	    if( zbuff[x] == zval_H ) {
		    if( vram[x] > zval_L ) continue;
		}
	    zbuff[x] = zval_H;
		vram[x] = getDitherCol( x, y ) | zval_L;
	}
}

// この関数は将来的にサブコアに負荷分散する
void Canvas::draw_flatTriangle( int x1, int y1, int x2, int y2, int x3 ) {
	int ady = ABS(y1 - y2);
	if ( ady == 0 ) {
	    draw_Xaxis( y2, x2, x3 );
		return;
	}

	int sdy = 1;
	if (y1 < y2) sdy = -1;

	int adxa = ((x1 - x2) << 16) / ady;
	int adxb = ((x1 - x3) << 16) / ady;
	int xa = (x2 << 16);
	int xb = (x3 << 16);
	int y = y2;

	for (int ypos = 0; ypos <= ady; y += sdy, ypos++) {
		if ( (y < 0) || ( y >= CANVAS_HEIGHT ) ) break;

		draw_Xaxis( y, (xa >> 16), (xb >> 16) );
		xa += adxa;
	    xb += adxb;
	}
}

// この関数は CPU で計算する
void Canvas::draw_triangle( int x1, int y1, int x2, int y2, int x3, int y3 ) {
  // y1 < y2 < y3 となるように並べ直す
  if (y1 > y2) {
    swap_int(x1, x2);
    swap_int(y1, y2);
  }
  if (y1 > y3) {
    swap_int(x1, x3);
    swap_int(y1, y3);
  }
  if (y2 > y3) {
    swap_int(x2, x3);
    swap_int(y2, y3);
  }

  // 例外的なパターンの排除
  if (y1 == y3) {
    return;
  }
  if (x1 == x2 && x2 == x3) {
    return;
  }

  if (y1 == y2) {
    draw_flatTriangle(x3, y3, x1, y1, x2);
  } else if (y2 == y3) {
    draw_flatTriangle(x1, y1, x2, y2, x3);
  } else {
    int xa = x3 * (y2 - y1) / (y3 - y1) + x1 * (y2 - y3) / (y1 - y3);
    draw_flatTriangle(x1, y1, xa, y2, x2);
    draw_flatTriangle(x3, y3, xa, y2, x2);

	// 描画処理上無意味なこの行を抜くと何故が表示がバグる時がある。気持ち悪いけど後回し
/*
	c.dvi0[0] = x1;
	c.dvi0[1] = x2;
	c.dvi0[2] = x3;
	c.dvi1[0] = y1;
	c.dvi1[1] = y2;
	c.dvi1[2] = y3;
*/
  }
}

/*
// この関数は CPU で計算する
void Canvas::draw_triangle2( int x1, int y1, int x2, int y2, int x3, int y3 ) {
  // y1 < y2 < y3 となるように並べ直す
  if (y1 > y2) {
    swap(int, x1, x2);
    swap(int, y1, y2);
  }
  if (y1 > y3) {
    swap(int, x1, x3);
    swap(int, y1, y3);
  }
  if (y2 > y3) {
    swap(int, x2, x3);
    swap(int, y2, y3);
  }

  // 例外的なパターンの排除
  if (y1 == y3) {
    return;
  }
  if (x1 == x2 && x2 == x3) {
    return;
  }

  if (y1 == y2) {
    draw_flatTriangle(x3, y3, x1, y1, x2);
  } else if (y2 == y3) {
    draw_flatTriangle(x1, y1, x2, y2, x3);
  } else {
    int xa = x3 * (y2 - y1) / (y3 - y1) + x1 * (y2 - y3) / (y1 - y3);
    draw_flatTriangle(x1, y1, xa, y2, x2);
    draw_flatTriangle(x3, y3, xa, y2, x2);


  }
}
*/


void Canvas::setZval( float fz ) {
	float zd = (1.0f - fz) *0x7fff;
	int z = (int)zd;
	if ( z < 0 ) z = 0;
	if ( z > 0xffff ) z = 0xffff;
	zval_L = (z & 0xfc);
	zval_H = (z >> 8);
}

void Canvas::setDitherNo( int no ) {
	no += m_DitherAdd;
	if ( no < 0 ) {
		no = 0;
	}
	if ( no > 16 ) {
		no = 15;
	}
	DitherNo = (uint8_t)no;
}	


Canvas c;

