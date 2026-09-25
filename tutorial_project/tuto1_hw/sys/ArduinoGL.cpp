/*
    ArduinoGL.h - OpenGL subset for Arduino.
    Created by Fabio de Albuquerque Dela Antonio
    fabio914 at gmail.com
 */

#include "ArduinoGL.h"
#define MAX_VERTICES 24
#define MAX_MATRICES 8

#include "Canvas.h"


#define DEG2RAD (M_PI/180.0)


Canvas * glCanvas = NULL;
GLDrawMode glDrawMode = GL_NONE;

GLVertex glVertices[MAX_VERTICES];
GLVertex glVertices2[MAX_VERTICES];
unsigned glVerticesCount = 0;

float light_vector[3];
float vlootAt[3];
float normal_vector[3];	// 法線ベクトル

float dv0[3];
float dv1[3];
int dvi0[3];
int dvi1[3];

GLMatrixMode glmatrixMode = GL_PROJECTION;
float glMatrices[2][16];
float glMatrixStack[MAX_MATRICES][16];
unsigned glMatrixStackTop = 0;

unsigned glPointLength = 1;

/* Aux functions */
void copyMatrix(float * dest, float * src) {

	if( src == NULL ) {
	 	src = glMatrices[glmatrixMode];
	}
    
    for(int i = 0; i < 16; i++)
        dest[i] = src[i];
}

void multMatrix(float * dest, float * src1, float * src2) {
    
    int i, j, k;
    float m[16];
    
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++) {
            
            m[i + j * 4] = 0.0;
            
            for(k = 0; k < 4; k++)
                m[i + j * 4] += src1[i + k * 4] * src2[k + j * 4];
        }
    
    for(i = 0; i < 16; i++)
        dest[i] = m[i];
}

void pushMatrix(float * m) {
    
    if(glMatrixStackTop < MAX_MATRICES) {
        
        copyMatrix(glMatrixStack[glMatrixStackTop], m);
        glMatrixStackTop++;
    }
}

void popMatrix(void) {
    
    if(glMatrixStackTop > 0) {
        
        glMatrixStackTop--;
    }
}

GLVertex multVertex(float * m, GLVertex v) {
    
    GLVertex ret;
    
    ret.x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w;
    ret.y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w;
    ret.z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w;
    ret.w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w;
    
    return ret;
}

void copyVector3(float * dest, float * src) {
	for(int i = 0; i < 3; i++) {
		dest[i] = src[i];
	}
}


//------------------------------
//		ベクトル正規化
//------------------------------
void normVector3(float * dest, float * src) {
    
    float norm;
//    register int i;
    int i;
    
    norm = sqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
    
    for(i = 0; i < 3; i++)
        dest[i] = src[i]/norm;
}

//------------------------------
//		ベクトルの外積（クロス積）
//------------------------------
void crossVector3(float * dest, float * src1, float * src2) {

    copyVector3( dv0, src1);
	copyVector3( dv1, src2);

    dest[0] = src1[1] * src2[2] - src1[2] * src2[1];
    dest[1] = src1[2] * src2[0] - src1[0] * src2[2];
    dest[2] = src1[0] * src2[1] - src1[1] * src2[0];
}



//------------------------------
//ベクトル内積
//------------------------------
float dotVector3( float *vl, float *vr) {
    return vl[0] * vr[0] + vl[1] * vr[1] + vl[2] * vr[2];
}


//ベクトル内積
float dot_product( const GLVertex& vl, const GLVertex vr) {
    return vl.x * vr.x + vl.y * vr.y + vl.z * vr.z;
}

//ベクトルの長さを計算する
float get_vector_length( float *v ) {
	return sqrt( ( v[0] * v[0] ) + ( v[1] * v[1] ) + ( v[2] * v[2] ) );
}


//２つのベクトルABのなす角度θを求める
// ベクトル A B は正規化してセットする
float AngleOf2Vector( float *A, float *B ) {
	//※ベクトルの長さが0だと答えが出ませんので注意してください。

//	copyVector3( c.dv0, A);
//	copyVector3( c.dv1, B);

	//内積とベクトル長さを使ってcosθを求める
	float cos_sita = dotVector3(A,B);

	//cosθからθを求める
	float sita = acosf( cos_sita );	


	//ラジアンでなく0～180の角度でほしい場合はコメント外す
	//sita = sita * 180.0 / PI;

	return sita;
}

//２つのベクトルABのなす角度θを求める
// ベクトル A B は正規化してセットする
int getLightData( float *A, float *B ) {
	//※ベクトルの長さが0だと答えが出ませんので注意してください。
	float dt = 0;

	copyVector3( dv0, A);
	copyVector3( dv1, B);

	//内積とベクトル長さを使ってcosθを求める
	float cos_sita = dotVector3(A,B);
	
	if ( cos_sita <= 0 ) {
		dt = 10 + (cos_sita * 10);
	} else {
		dt = 10 + (cos_sita * 5);
	}
	return (int)dt;
}


// ベクトルvに対してポリゴンが表裏どちらを向くかを求める
// 戻り値    0:表    1:裏    -1:エラー
int polygon_side_chk( GLVertex& A, GLVertex& B, GLVertex& C, float*v ) {

    //ABCが三角形かどうか。ベクトルvが0でないかの判定は省略します

    //AB BCベクトル
    float AB[3];
    float BC[3];

    AB[0] = B.x - A.x;
    AB[1] = B.y - A.y;
    AB[2] = B.z - A.z;

    BC[0] = C.x - A.x;
    BC[1] = C.y - A.y;
    BC[2] = C.z - A.z;

    //AB BCの外積
    crossVector3( normal_vector, AB ,BC );

    //ベクトルvと内積。順、逆方向かどうか調べる
    float d = dotVector3( v, normal_vector );

    if ( d < 0.0 ) {
        return 0;    //ポリゴンはベクトルvから見て表側
    }else 
    if ( d > 0.0 ) {
        return 1;    //ポリゴンはベクトルvから見て裏側
    }

    // d==0 ポリゴンは真横を向いている。表裏不明
    return -1;
}

// ポリゴンの光源処理
void polygon_light( GLVertex& A, GLVertex& B, GLVertex& C ) {

    //ABCが三角形かどうか。ベクトルvが0でないかの判定は省略します

    //AB BCベクトル
    float AB[3];
    float BC[3];

    AB[0] = B.x - A.x;
    AB[1] = B.y - A.y;
    AB[2] = B.z - A.z;

    BC[0] = C.x - A.x;
    BC[1] = C.y - A.y;
    BC[2] = C.z - A.z;

    //AB BCの外積
    crossVector3( normal_vector, AB ,BC );
	normVector3( normal_vector,normal_vector );

   	c.setDitherNo( getLightData( light_vector , normal_vector) );
}


/* Matrices */

void glMatrixMode(GLMatrixMode mode) {
    if(mode == GL_MODELVIEW || mode == GL_PROJECTION)
        glmatrixMode = mode;
}

void glMultMatrixf(float * m) {
    multMatrix(glMatrices[glmatrixMode], glMatrices[glmatrixMode], m);
}

void glLoadMatrixf(float * m) {
    copyMatrix(glMatrices[glmatrixMode], m);
}

void glLoadIdentity(void) {
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        if(i % 5 == 0)
            m[i] = 1.0;
        else m[i] = 0.0;
    
    glLoadMatrixf(m);
}

void glPushMatrix(void) {
    pushMatrix(glMatrices[glmatrixMode]);
}

void glPopMatrix(void) {
    popMatrix();
}

void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
    
    float tx = -(right + left)/(right - left);
    float ty = -(top + bottom)/(top - bottom);
    float tz = -(zFar + zNear)/(zFar - zNear);
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = 2.0/(right - left);
    m[5] = 2.0/(top - bottom);
    m[10] = -2.0/(zFar - zNear);
    m[12] = tx;
    m[13] = ty;
    m[14] = tz;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void gluOrtho2D(float left, float right, float bottom, float top) {
    glOrtho(left, right, bottom, top, -1.0, 1.0);
}

void glFrustum(float left, float right, float bottom, float top, float zNear, float zFar) {
    
    float A = (right + left)/(right - left);
    float B = (top + bottom)/(top - bottom);
    float C = -(zFar + zNear)/(zFar - zNear);
    float D = -(2.0 * zFar * zNear)/(zFar - zNear);
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = (2.0 * zNear)/(right - left);
    m[5] = (2.0 * zNear)/(top - bottom);
    m[8] = A;
    m[9] = B;
    m[10] = C;
    m[11] = -1.0;
    m[14] = D;
    
    glMultMatrixf(m);
}

void gluPerspective(float fovy, float aspect, float zNear, float zFar) {
    
    float aux = tan((fovy/2.0) * DEG2RAD);
    float top = zNear * aux;
    float bottom = -top;
    float right = zNear * aspect * aux;
    float left = -right;
    
    glFrustum(left, right, bottom, top, zNear, zFar);
}

void glRotatef(float angle, float x, float y, float z) {
    
    float c = cos(DEG2RAD * angle), s = sin(DEG2RAD * angle);
    float nx, ny, nz, norm;
    
    norm = sqrt(x*x + y*y + z*z);
    
    if(norm == 0)
        return;
    
    nx = x/norm;
    ny = y/norm;
    nz = z/norm;
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = nx*nx*(1.0 - c) + c;
    m[1] = ny*nx*(1.0 - c) + nz*s;
    m[2] = nx*nz*(1.0 - c) - ny*s;
    m[4] = nx*ny*(1.0 - c) - nz*s;
    m[5] = ny*ny*(1.0 - c) + c;
    m[6] = ny*nz*(1.0 - c) + nx*s;
    m[8] = nx*nz*(1.0 - c) + ny*s;
    m[9] = ny*nz*(1.0 - c) - nx*s;
    m[10] = nz*nz*(1.0 - c) + c;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void glTranslatef(float x, float y, float z) {
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = 1.0;
    m[5] = 1.0;
    m[10] = 1.0;
    m[12] = x;
    m[13] = y;
    m[14] = z;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void glScalef(float x, float y, float z) {
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = x;
    m[5] = y;
    m[10] = z;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void gluLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
    
    float dir[3], up[3];
    
    dir[0] = centerX - eyeX;
    dir[1] = centerY - eyeY;
    dir[2] = centerZ - eyeZ;
    
    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;
    
    float n[3], u[3], v[3];
    
    normVector3(n, dir);
	copyVector3( vlootAt, n );

    crossVector3(u, n, up);
    normVector3(u, u);
    
    crossVector3(v, u, n);
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = u[0];
    m[1] = v[0];
    m[2] = -n[0];
    
    m[4] = u[1];
    m[5] = v[1];
    m[6] = -n[1];
    
    m[8] = u[2];
    m[9] = v[2];
    m[10] = -n[2];
    
    m[15] = 1.0;
    
    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

void setLight(float x, float y, float z) {
	light_vector[0] = x;
	light_vector[1] = y;
	light_vector[2] = z;
	normVector3( light_vector,light_vector );
}

/* Vertices */

void glVertex4fv(float * v) {
    glVertex4f(v[0], v[1], v[2], v[3]);
}

void glVertex4f(float x, float y, float z, float w) {
    
    GLVertex v;
    
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    
    if(glVerticesCount < MAX_VERTICES) {
        
        glVertices[glVerticesCount] = v;
        glVerticesCount++;
    }
}

void glVertex3fv(float * v) {
    glVertex3f(v[0], v[1], v[2]);
}

void glVertex3f(float x, float y, float z) {
    glVertex4f(x, y, z, 1.0);
}

/* OpenGL */

void glUseCanvas(Canvas * c) {
    glCanvas = c;
}

void glClear(int mask) {
    
    if(mask & GL_COLOR_BUFFER_BIT) {
        
        if(glCanvas != NULL)
            glCanvas->clear();
    }
}

void glPointSize(unsigned size) {
    glPointLength = size;
}

void glBegin(GLDrawMode mode) {
    glDrawMode = mode;
    glVerticesCount = 0;
}

bool isDispArea( GLVertex *glVertices ) {
	if( glVertices->x >= -1.0 && glVertices->x <= 1.0)  return true;
	if( glVertices->y >= -1.0 && glVertices->y <= 1.0)  return true;
	if( glVertices->z >= -1.0 && glVertices->z <= 1.0)  return true;
	return false;
}

void glEnd(void) {
    
    if(glCanvas == NULL || glDrawMode == GL_NONE)
        return;
    
    float modelviewProjection[16];
    multMatrix(modelviewProjection, glMatrices[GL_PROJECTION], glMatrices[GL_MODELVIEW]);
    
    int frameWidth = glCanvas->width();
    int frameHeight = glCanvas->height();


    
    for(int i = 0; i < glVerticesCount; i++) {

        GLVertex aux = multVertex(modelviewProjection, glVertices[i]);
		if ( aux.w == 0 ) {
//			Serial.printf("glEnd div 0\n" );
//	        return;
		}
 
		glVertices2[i] = aux;
		aux.x = aux.x/aux.w;
		aux.y = aux.y/aux.w;
		aux.z = aux.z/aux.w;
		glVertices[i] = aux;
    }
    
    if(glDrawMode == GL_POINTS) {
        for(int i = 0; i < glVerticesCount; i++) {
            
            GLVertex * aux = &(glVertices[i]);

			if( !isDispArea( aux ) ) continue;
            
            int px = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            int py = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
			c.setZval( glVertices[0].z );

			glCanvas->setPixel(px, py);
			if ( glVertices[i].z < 0.995f ) {
				glCanvas->setPixel(px+1, py);
				glCanvas->setPixel(px, py+1);
				glCanvas->setPixel(px+1, py+1);
			}

        }
    
    } else if(glDrawMode == GL_SPR16) {
		GLVertex * aux = &(glVertices[0]);
		if( isDispArea( aux ) ) {
            int px = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            int py = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
			glCanvas->setZval( glVertices[0].z );

			glCanvas->setDitherNo( 0 );
			glCanvas->setSprZoomW( 32.0f / glVertices[0].w );
//			glCanvas->setPixel(px, py);
//			glCanvas->drawSquare(px-7, py-7,px+8, py+8);
			glCanvas->drawSPR16( px, py);
        }

    } else if(glDrawMode == GL_SPR8) {
		GLVertex * aux = &(glVertices[0]);
		if( isDispArea( aux ) ) {
            int px = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            int py = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
			glCanvas->setDitherNo( 0 );
			glCanvas->setSprZoomW( 32.0f / glVertices[0].w );
			glCanvas->setZval( glVertices[0].z );
//			glCanvas->setSprZoom( 1.0f,1.0f );
			glCanvas->drawSPR8( px, py);

        }

    
    } else if(glDrawMode == GL_POLYGON) {

        /* TODO Improve! */
        if(glVerticesCount < 2)
            return;

		if (polygon_side_chk( glVertices[0], glVertices[1], glVertices[2], vlootAt ) == 1 ) {
	        return;
		}

    	// 光源処理
    	polygon_light( glVertices2[0], glVertices2[1], glVertices2[2] );

        int px[MAX_VERTICES], py[MAX_VERTICES];
            
        for(int i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
		c.setZval( (glVertices[0].z + glVertices[1].z + glVertices[2].z)/3 );
//		c.setZval( glVertices[0].z );

        if(glVerticesCount == 3 ) {
			glCanvas->draw_triangle( px[0], py[0], px[1], py[1], px[2],py[2]);
        } else if(glVerticesCount == 4 ) {
			glCanvas->draw_triangle( px[0], py[0], px[1], py[1], px[2],py[2]);
			glCanvas->draw_triangle( px[0], py[0], px[2], py[2], px[3],py[3]);
        } else {
	        for(int i = 0; i < glVerticesCount; i++) {
	            
	            int next = (i + 1 == glVerticesCount) ? 0:(i + 1);
	            glCanvas->drawLine(px[i], py[i], px[next], py[next]);
	        }
		}
    }
    else if(glDrawMode == GL_LINE_LOOP) {

        /* TODO Improve! */
        if(glVerticesCount < 2)
            return;

		if (polygon_side_chk( glVertices[0], glVertices[1], glVertices[2], vlootAt ) == 1 ) {
	        return;
		}

        
        int px[MAX_VERTICES], py[MAX_VERTICES];
            
        for(int i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }

        for(int i = 0; i < glVerticesCount; i++) {
            int next = (i + 1 == glVerticesCount) ? 0:(i + 1);
            glCanvas->drawLine(px[i], py[i], px[next], py[next]);
		}
    }
    
    else if(glDrawMode == GL_TRIANGLE_STRIP) {
        
        /* TODO Improve! */
        if(glVerticesCount < 3)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
        
        for(int i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
        
        for(int i = 0; i < glVerticesCount - 2; i++) {
            
            glCanvas->drawLine(px[i], py[i], px[i + 1], py[i + 1]);
            glCanvas->drawLine(px[i], py[i], px[i + 2], py[i + 2]);
            glCanvas->drawLine(px[i + 1], py[i + 1], px[i + 2], py[i + 2]);
            
        }
    }
}
