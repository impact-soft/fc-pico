/*
    rp_system.h
 */

#ifndef rp_system_h
#define rp_system_h

#include "Arduino.h"

#include "pico/stdlib.h"
#include "pico/unique_id.h" // 固有ID用ヘッダー
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include <hardware/watchdog.h>
#include <EEPROM.h>

#include "ArduinoGL.h"
#include "Canvas.h"
#include "rp_dma.h"

#include "pio/fcppu.pio.h"

#include "rp_debug.h"

#include "rp_fcemu.h"
#include "rp_nsfplayer.h"
#include "rp_sound.h"
#include "rp_bpe.h"

#include "Obj3d.h"



#define LOOP_MS 1

#define TIME_INTERVAL -500  // タイマー割り込みの間隔


//---------------------------------------
// GPIO
//---------------------------------------

#define LED_COUNT 1           // LEDの連結数
#define BRIGHTNESS 32        // 輝度

#define	LED_PIN 25
#define	LED_PIN2 24
#define	USRKEY_PIN 24
//#define DIN_PIN 23            // NeoPixel　の出力ピン番号はGP23


#define	PI_WR		(1<<PI_WR_BIT)
#define	PI_RD		(1<<PI_RD_BIT)
//#define	PI_PA13		(1<<PI_PA13_BIT)
//#define	PI_PA12		(1<<PI_PA12_BIT)
#define	PI_CS1		(1<<PI_CS1_BIT)
//#define	PI_OD_DIR	(1<<PI_OD_DIR_BIT)
#define	PI_DATA_SHIFT	PI_D0_BIT
#define	PI_DATA_MASK	(0x00FF<<PI_DATA_SHIFT)

//#define	PI_INP_PINS		(PI_WR|PI_RD|PI_PA13|PI_PA12|PI_CS1|PI_OD_DIR)
#define	PI_INP_PINS		(PI_WR|PI_RD|PI_CS1)
#define	PI_BIDIR_PINS	(PI_DATA_MASK)


//---------------------------------------
// ステートマシン関連
//---------------------------------------
#define PIO_NO_0	0
#define PIO_NO_1	1
#define PIO_NO_2	2

#define	SM_RECV	0
#define	SM_TRAN	1
#define	SM_BUSDIR	2
#define	SM_TRCNT	3


//---------------------------------------
// PICO->FC command
//---------------------------------------
#define  PF_COM_NONE  0		// コマンドなし
#define  PF_COM_DMOD  1		// 表示OFFにしてデータ転送モードへ
#define  PF_COM_FDIN  2		// フェードイン	処理終了　FP_COM_ACK
#define  PF_COM_FDOT  3		// フェードアウト	処理終了　FP_COM_ACK

#define  PF_COM_SE    0x80		// SEセット:0x80 + SE_NO
#define  PF_COM_BGM   0xA0		// BGMセット:BGM_NO
#define  PF_COM_VRAM  0xC0		// VRAM 書き換え:adrH,ardL,dt


	// データモードコマンド
#define  PF_DAT_VRAM  0x80		//  VRAM 書き換え:adrH,ardL,size,data....
								//  --> size = 0 は256バイト 256バイト以上送りたい場合は分割して送る
#define  PF_DAT_RAM   0x81 		//  VRAM 書き換え:adrH,ardL,size,data....

#define  PF_DAT_STEP  0x82 		//  データモードを抜けてファミコンの指定ステップへ

#define  PF_MAGIC_NO  0xFC		// 受け取ったコマンドの可否チェックコード

//---------------------------------------
// FC->PICO command
//---------------------------------------
enum{
//	FP_COM_ACK	= 0x0F,		// FCからのコマンド正常終了応答
//	FP_COM_NAK	= 0x1F,		// FCからのコマンド失敗終了応答
	FP_COM_VER	= 0x2F,		// BIOS-ROM romvarsion
	FP_COM_ROM	= 0x3F,		// BIOS-ROM romdeta load

	FP_COM_LOG	= 0xBF,		// debug log
	FP_COM_DRQ	= 0xCF,		// data request
	FP_COM_DLD	= 0xDF,		// data load
	FP_COM_RST	= 0xEF,		// PICO RESTART
	FP_COM_INI	= 0xFF,		// PIC INIT
};



//---------------------------------------
// FCキー入力定義
//---------------------------------------
#define KEY_A		0x80
#define KEY_B		0x40
#define KEY_SEL		0x20
#define KEY_RUN		0x10
#define KEY_UP		0x08
#define KEY_DOWN	0x04
#define KEY_LEFT	0x02
#define KEY_RIGHT	0x01


//---------------------------------------
// キーリピート設定
//---------------------------------------
#define REP_WAIT	24		// リピート開始までの時間 (フレーム数)
#define REP_INTERVAL 8		// リピート間隔 (フレーム数)


//---------------------------------------
// フェードイン、アウト
//---------------------------------------
#define FADE_WAIT	8


//---------------------------------------
// FC STEP
//---------------------------------------
#define FCST_OPTION  3

//---------------------------------------
//---------------------------------------
#define MICROS_1S  (1000*1000)
#define MICROS_1MS  (1000)


//#define FC_COM_BUF_SIZE	8
//#define FC_COM_BUF_SIZE	16
#define FC_COM_BUF_SIZE16	16
#define FC_COM_BUF_SIZE	64

#define PICO_APU_BUF_SIZE  0x40

#define PICO_SNDREG		0x10

#define PPU_COUNT_VAL	(15426 + FC_COM_BUF_SIZE)

#define SAVE_DATA_SIZE	256




//=================================================
//			仮想VRAM関連
//=================================================

#define VRAM_BUF_SIZE ((36 * 2 * 240 + FC_COM_BUF_SIZE) / sizeof(uint32_t))
//#define VRAM_BUF_SIZE ((32 * 2 * 240) / sizeof(uint32_t))


class rp_system {
    
public:
    rp_system();
    void init(void);
    void init2();
    void update(void);
	void soft_reset();
    void initVram();
    void convVram();
	void jobRcvCom();

    void ppu_dma(void);
    void ver_dma();

	void FadeIn();
	void FadeOut();
	void SleepMS( int ms );


	void setKeyData( uint8_t key ) { m_key_imp = key; }
	void setKeyUpdate();
	bool setPF_COM( uint8_t com );
	bool setPF_VRAM( uint16_t vadr, uint8_t dt );
    void startDataMode(void);

    uint8_t  getKeyNew(void) { return m_key_new; }
    uint8_t  getKeyTrg(void) { return m_key_trg; }
    uint8_t  getKeyRep(void) { return m_rep_new; }


	void setAPU( uint8_t reg, uint8_t data );
	void setPF_APU();

	void setPalData( const uint8_t *paldt );
	void setPal( uint8_t idx, uint8_t dt );
	void setAtrData( const uint8_t *atrdt );
	void clearAtrData( void );
	void setAtr( uint8_t lx, uint8_t ly, uint8_t dt );
	void setFcStep( uint8_t step ) { m_FC_STEP = step; }
	void commitSaveData();


	uint32_t ppu_count;
	uint8_t frame_draw;

	uint8_t SaveData[SAVE_DATA_SIZE];


private:
	void initFC_COM_BUF();
	void jobFP_COM_DRQ();
	void jobFP_COM_DLD( uint8_t adrh );
    void rom_dma( uint8_t adrh );
	void drq_ret( uint8_t com, uint16_t adr, uint16_t size );

	uint8_t getRcvCom();

	uint8_t m_waitFP_COM_DRQ;

	uint8_t m_key_imp;
	uint8_t m_key_new;
	uint8_t m_key_trg;
	uint8_t m_key_old;
	uint8_t m_rep_key;
	uint8_t m_rep_cnt;
	uint8_t m_rep_new;
	uint32_t ppu_count_old;
	uint8_t m_fade_wait;

	uint8_t *m_pDRQ;

	uint8_t m_PAL_W[0x20];
	uint8_t m_PAL_W_old[0x20];
	uint8_t m_PAL_CHG;

	uint8_t m_ATR_W[0x40];
	uint8_t m_ATR_W_old[0x40];
	uint8_t m_ATR_CHG;

	uint8_t FC_COM_BUF[ FC_COM_BUF_SIZE ];
	uint8_t m_FC_COM_IDX;
	uint32_t vram_buf0[VRAM_BUF_SIZE];
	uint32_t vram_buf1[VRAM_BUF_SIZE];	// バックバッファ

	uint32_t *vram_buf;
	uint32_t *vram_bufDraw;

	uint8_t *vram;

	uint8_t m_FC_STEP;

	uint8_t m_APU[ 0x18 ];

	uint8_t m_APU_REG[PICO_APU_BUF_SIZE];
	uint8_t m_APU_DAT[PICO_APU_BUF_SIZE];
	uint8_t m_APU_W_IDX;
	uint8_t m_APU_R_IDX;

	rp_dma vram_dma;

};

extern rp_system sys;

#endif

