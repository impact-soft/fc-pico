/*
 BPE デコード
 */

#include "Arduino.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "rp_bpe.h"

#define MAXPASS 255

#define BUF_SIZE2 0x100

//1フレームデコード
ulong frame_decode2( int len, uint8_t *pdic, uint8_t pass, uint8_t *wbuf ) {
	uint8_t  dt_buf[BUF_SIZE2];
	uint8_t  ix_buf[BUF_SIZE2];
	int i, n,m,ix,data_ix,vram_ix=0;
	uint8_t dt,y;
	uint8_t *pdic_h = &pdic[0];
	uint8_t *pdic_l = &pdic[pass];
	uint8_t *pdic_o = &pdic[pass*2];
	uint8_t *pdata  = &pdic[pass*3];

	for( data_ix = 0; data_ix < len ; data_ix++ ) {
		ix = 0;
		i=pass;
		dt = pdata[ data_ix ];

		for(;;){
			while (i--){
				y = (uchar)i;
				if ( dt == pdic_o[y] ) {
					dt = pdic_h[y];
					dt_buf[ix] = pdic_l[y];
					ix_buf[ix] = y;
					ix++;
				}
			} 
			wbuf[ vram_ix++ ] = dt;
//			Serial.printf( "bep:%x\n", dt );

			
			if ( ix == 0 ) break;

			ix--;
			i = ix_buf[ix];
			dt =  dt_buf[ix];
		}
	
	}

	return vram_ix;
}


//デコード
int bpe_decode( uint8_t *buf, uint8_t *wbuf ) {

	uint8_t *bpe_io;
	int compsize_sum = 0, decompsize_sum = 0;
	int dp=0;
	int wp=0;
	int ret;

	while (1) {
		//ヘッダ読み込み
		uint8_t pass = buf[dp++];		// カウント
		if (pass == 0) {
			return decompsize_sum;
		}

		unsigned short decompsize = (buf[dp+1] << 8) + buf[dp];	// 伸張サイズ
		dp +=2;

		unsigned  short compsize = (buf[dp+1] << 8) + buf[dp];	// 圧縮サイズ
		dp +=2;

		//printf("pass:%d decompsize:%d compsize:%d\n", pass, decompsize, compsize);

		bpe_io = &buf[dp];
		dp += pass*3;
		dp += compsize;
		
		ret = frame_decode2( compsize, bpe_io, pass,  &wbuf[ decompsize_sum ] );
		if (decompsize != ret) {
			//printf("\n展開エラー(orig=%d %d)\n", decompsize, ret);
			return -1;
		}
		decompsize_sum += decompsize;
		compsize_sum += compsize;
//		printf("[%5.1f%%]%8dKB->%8dKB\r", (double)ftell(ifp)/fsiz*100.0, compsize_sum, decompsize_sum);

	}

}
/*

//デコード
ulong bpe_dec(uchar *infile, uchar *outfile)
{
	
	uchar *bpe_io;

	FILE *ifp, *ofp;
	uchar *data, head[4];
	ushort decompsize, compsize, ret;
	uchar pass;
	ulong framesize = DEFAULTFRAMESIZE;
	ulong crc32 = 0, crc32_orig = 0,j;
	ulong compsize_sum = 0, decompsize_sum = 0;
	ulong before = 0, after = 0, fsiz, dp=0;
	
	printf("in:%s\nout:%s\n", infile, outfile);

	ifp = fopen(infile, "rb");
	if (ifp == NULL) {
		puts("入力ファイルオープンエラー");
		return -1;
	}

	fsiz = fread(buf,  1, BUF_SIZE, ifp);
	if( fsiz  == 0) {
		puts("\nデータが読み込めない");
		return -1;
	}
	

	
	ofp = fopen(outfile, "wb");
	if (ofp == NULL) {
		puts("出力ファイルオープンエラー");
		return -1;
	}

	
	data = malloc(framesize);

	while (1) {
		//ヘッダ読み込み
		pass = buf[dp++];		// カウント
		if (pass == 0) {
			break;
		}

		decompsize = (buf[dp+1] << 8) + buf[dp];	// 伸張サイズ
		dp +=2;

		compsize = (buf[dp+1] << 8) + buf[dp];	// 圧縮サイズ
		dp +=2;

		printf("pass:%d decompsize:%d compsize:%d\n", pass, decompsize, compsize);

		bpe_io = &buf[dp];
		dp += pass*3;
		dp += compsize;
		
		ret = frame_decode2(data, compsize, bpe_io, pass );
			if (decompsize != ret) {
			printf("\n展開エラー(orig=%d %d)\n", decompsize, ret);
			break;
		}
		decompsize_sum += decompsize;
		compsize_sum += compsize;
//		printf("[%5.1f%%]%8dKB->%8dKB\r", (double)ftell(ifp)/fsiz*100.0, compsize_sum, decompsize_sum);

		fwrite(vram_buf, 1, decompsize, ofp);
	}
	
//	crc32_finish(&crc32);
	
	before = fsiz;
	after = ftell(ofp);
	
//	printf("伸張 : %d->%d(%6.2f%%) crc=%x(%s)\n",
//		before, after, ((double)after/before)*100.0,
//		crc32, crc32 == crc32_orig ? "TRUE" : "FALSE"
//		);
	printf("伸張 : %d->%d(%6.2f%%) \n",
		before, after, ((double)after/before)*100.0
		);

	fclose(ofp);
	free(data);

	return 0;
}


void usage(void) {
	puts("bpe BPEエンコーダ");
	puts("エンコード [-e] infile [outfile] [-bフレームサイズ]");
	puts("デコード -d infile [outfile]");
	return ;
}

int main(int argc, char **argv)
{
	ulong mode, framesize, n;
	uchar in[300], out[300];
	if (argc == 1) {
		usage();
		return 0;
	}
	
	//初期化
	in[0] = out[0] = 0;
	mode = ENCODE;
	framesize = DEFAULTFRAMESIZE;
	
	for (n=1; n<argc; n++) {
		if (argv[n][0] == '-' || argv[n][0] == '/') {
			if (argv[n][1] == 'd') mode = DECODE;
			
			if (argv[n][1] == 'b') framesize = atoi(&argv[n][2]);
		} else {
			
			//ファイル名
			if (in[0] != 0 && out[0] == 0) strcpy(out, argv[n]);
			if (in[0] == 0) strcpy(in, argv[n]);
		}
	}
	
	
	if (in[0] == 0) {
		usage();
		return -1;
	}
	
	if (out[0] == 0) {
		if (mode == ENCODE) sprintf(out, "%s.bpe", in);
		if (mode == DECODE) {
			uchar drv[4], dir[300], fnam[300];
			_splitpath(in, drv, dir, fnam, 0);
			sprintf(out, "%s%s%s", drv, dir, fnam);
		}
	}
	
	if (framesize <= 0 || framesize > 65535) framesize = DEFAULTFRAMESIZE;
	framesize = (int)(framesize/12) * 12;//アラインメント
	
	if (mode == ENCODE) {
//		bpe_enc(in, out, framesize);
	} else {
		bpe_dec(in, out);
	}
	
	return 0;
}

*/
