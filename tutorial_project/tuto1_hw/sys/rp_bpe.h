#pragma once

#define BPE_ERROR 65536
#define DEFAULTFRAMESIZE 0x8000
#define ENCODE 1
#define DECODE 2

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

#pragma pack(push,1)

typedef struct {
	uchar sign[4];//"BPE2"
	ulong crc32;
	ushort maxframesize;
} BPEHEADR;

typedef struct {
	uchar pass;
	ushort decomp;
	ushort comp;
} FRAMEHEADR;


//変換辞書
typedef struct {
	ushort twobyte;
	uchar onebyte;
} BPE_DIC;
#pragma pack(pop)



extern int bpe_decode(uint8_t *buf, uint8_t *wbuf );


//BPEファイル
/*

[フレームヘッダ]
パス(ushort)
元のサイズ(ushort)
圧縮サイズ(ushort)
辞書(パス数 * 3byte)

[圧縮データ]
バイナリ(圧縮サイズ)

以下[フレームヘッダ][圧縮データ]...が続く

*/
