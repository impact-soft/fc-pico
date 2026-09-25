/*
    rp_system.h
 */




#include "Arduino.h"
#include "pio/fcppu.pio.h"
#include "rp_system.h"

#include "Canvas.h"


rp_system::rp_system(void) {
}


void rp_system::init(void) {
	Serial.printf("--- FC_PICO START ---\n" );

	uint offset;
	frame_draw = 0;
	m_APU_W_IDX = m_APU_R_IDX = 0;

	// PIO INIT
	for (uint i = PI_D0_BIT; i <= PI_D7_BIT; i++) {
		gpio_pull_up(i);
	}

	gpio_init_mask(PI_INP_PINS | PI_DATA_MASK );
	gpio_set_dir_in_masked(PI_INP_PINS);
	gpio_set_dir_out_masked(PI_DATA_MASK);

	// PIO reset
	pio_enable_sm_mask_in_sync(pio0, 0);
	pio_clear_instruction_memory (pio0);

	// out pins
	for (uint i = PI_D0_BIT; i <= PI_D7_BIT; i++) {
		pio_gpio_init(pio0, i);
	}

    // ID格納用のバッファ（終端文字含め17バイト以上が必要）
    char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    
    // 固有IDを16進数文字列として取得
    pico_get_unique_board_id_string(board_id, sizeof(board_id));

	Serial.printf("Unique ID: %s\n", board_id);


	initDMA();

	//---------------------------
	// init SM_RECV
	//---------------------------
	offset = pio_add_program(pio0, &fcppu_w_program);

//	Serial.printf("SM_RECV offset:%d\n", offset );

	pio_sm_config cw = fcppu_w_program_get_default_config(offset);

//	sm_config_set_out_pins(&cw, PI_DATA_SHIFT, 8);
	sm_config_set_in_pins(&cw, PI_DATA_SHIFT);
	sm_config_set_jmp_pin(&cw, PI_CS1_BIT);

	sm_config_set_fifo_join (&cw, PIO_FIFO_JOIN_RX);

    sm_config_set_in_shift (&cw,
                            true,       // shift right
                            true,       // enable autopush
                            32);        // autopush after 32 bits

    // プログラムをロードさせ、プログラムの実行位置を先頭(offsetの位置)に移動します。
   pio_sm_init(pio0, SM_RECV, offset, &cw);


	//---------------------------
	// init SM_TRAN
	//---------------------------
	offset = pio_add_program(pio0, &fcppu_r_program);

//	Serial.printf("SM_TRAN offset:%d\n", offset );
	
	pio_sm_config cr = fcppu_r_program_get_default_config(offset);

	sm_config_set_out_pins(&cr, PI_DATA_SHIFT, 8);
	sm_config_set_jmp_pin(&cr, PI_CS1_BIT);

	sm_config_set_fifo_join (&cr, PIO_FIFO_JOIN_TX);
//	sm_config_set_fifo_join (&cr, PIO_FIFO_JOIN_RX);

    sm_config_set_out_shift (&cr,
                            true,       // shift right
                            true,       // enable autopush
                            32);        // autopush after 32 bits

	pio_sm_init(pio0, SM_TRAN, offset, &cr);



	//---------------------------
	// init SM_BUSDIR
	//---------------------------
	offset = pio_add_program(pio0, &fcppu_dir_program);

//	Serial.printf("SM_BUSDIR offset:%d\n", offset );
	
	pio_sm_config cd = fcppu_dir_program_get_default_config(offset);
	sm_config_set_out_pins(&cd, PI_DATA_SHIFT, 8);
	sm_config_set_in_pins(&cd, PI_DATA_SHIFT);
	sm_config_set_jmp_pin(&cd, PI_RD_BIT);
	pio_sm_init(pio0, SM_BUSDIR, offset, &cd);


	//---------------------------
	// init SM_TRCNT
	//---------------------------
	offset = pio_add_program(pio0, &fcppu_rna_program);

//	Serial.printf("SM_TRCNT offset:%d\n", offset );
	
	pio_sm_config cn = fcppu_rna_program_get_default_config(offset);

	sm_config_set_jmp_pin(&cn, PI_CS1_BIT);

	sm_config_set_fifo_join (&cn, PIO_FIFO_JOIN_TX);
	sm_config_set_fifo_join (&cn, PIO_FIFO_JOIN_RX);

    sm_config_set_in_shift (&cn,
                            true,       // shift right
                            true,       // enable autopush
                            32);        // autopush after 32 bits

	pio_sm_init(pio0, SM_TRCNT, offset, &cn);

    // ステートマシンを走らせます。 (true:開始 false:停止)
	 pio_sm_set_enabled(pio0, SM_RECV, true);	// ステートマシン開始
	 pio_sm_set_enabled(pio0, SM_TRAN, true);	// ステートマシン開始
	 pio_sm_set_enabled(pio0, SM_BUSDIR, true);	// ステートマシン開始
	 pio_sm_set_enabled(pio0, SM_TRCNT, true);	// ステートマシン開始


	gpio_init(LED_PIN);
	gpio_init(LED_PIN2);
	gpio_set_dir(LED_PIN, GPIO_OUT);  
	gpio_set_dir(LED_PIN2, GPIO_OUT);  

	vram_dma.initSM_DMA_TX( PIO_NO_0, SM_TRAN, vram_buf,VRAM_BUF_SIZE );
	pio_sm_put_blocking ( pio0, SM_TRAN, 0x43462321 );
	ver_dma();

	// セーブデータ読み込み
	EEPROM.begin(SAVE_DATA_SIZE);
	for( int i=0; i<SAVE_DATA_SIZE ;i++ ){
		SaveData[i] = EEPROM.read(i);
	}

	initResData();

	init2();
	WDT_init();
}

void rp_system::commitSaveData() {
	for( int i=0; i<SAVE_DATA_SIZE ;i++ ){
		EEPROM.write( i, SaveData[i] );
	}
    if (EEPROM.commit()) {
      Serial.println("EEPROM successfully committed");
    } else {
      Serial.println("ERROR! EEPROM commit failed");
    }
}


void rp_system::init2() {
	rp2040.fifo.push( C1_RESET );
	initVram();
	initFC_COM_BUF();

	m_fade_wait = 0;

	clearAtrData();
	m_ATR_CHG = 0;
	m_PAL_CHG = 0;
	m_FC_STEP = 0;
	m_key_imp = 0;
	m_key_new = 0;
	m_key_trg = 0;
	m_key_old = 0;
	m_waitFP_COM_DRQ = 0;
	ap.setStep( ST_INIT );
}


void rp_system::soft_reset() {
	init();
}


void rp_system::initVram() {
	vram = (uint8_t *)vram_buf;
	uint32_t dp = 0;
	uint32_t dt = 0;
	c.setDitherNo( 0 );
	c.setDefCol( 3 );

	for (int i = 0; i < VRAM_BUF_SIZE; i++) {
		vram_buf0[i] = 0;
		vram_buf1[i] = 0;
	}

	vram_buf = vram_buf0;
	vram_bufDraw = vram_buf1;
}


void rp_system::initFC_COM_BUF() {
	memset(FC_COM_BUF, 0x0, FC_COM_BUF_SIZE );
	FC_COM_BUF[1] = PF_MAGIC_NO;
	m_FC_COM_IDX = 2;
}


bool rp_system::setPF_COM( uint8_t com ) {
//	Serial.printf("setPF_COM :%d\n", com );
	if ( m_FC_COM_IDX >= FC_COM_BUF_SIZE16 ) {
//@		Serial.printf("over flow:setPF_COM :%02x\n", com );
		return true;
	}
	FC_COM_BUF[m_FC_COM_IDX++] = com;
	return false;
}

bool rp_system::setPF_VRAM( uint16_t vadr, uint8_t dt ) {
	if ( m_FC_COM_IDX >= (FC_COM_BUF_SIZE16-2) ) {
		//Serial.printf("over flow:setPF_VRAM :%04x,%02x\n", vadr, dt );
		return true;
	}
	//Serial.printf("setPF_VRAM :%04x,%02x\n", vadr, dt );

	vadr &= 0x3FFF;

	FC_COM_BUF[m_FC_COM_IDX++] = PF_COM_VRAM | ((vadr >> 8) & 0xff);
	FC_COM_BUF[m_FC_COM_IDX++] = (vadr & 0xff);
	FC_COM_BUF[m_FC_COM_IDX++] = dt;
	return false;
}



//=================================================
//		フレームバッファをPPUデータに変換
//=================================================
void rp_system::convVram() {
	uint16_t *vram_w = (uint16_t*)vram_bufDraw;
	const uint16_t conv_tbl[4] = {
		0x0000,
		0x0001,
		0x0100,
		0x0101,
	};
	int fidx = 0;
	int vidx = 31;
	uint8_t* frame_buff = c.bitmap();

	for (int y = 0; y < 240; y++) {
		for (int x = 0; x < 34; x++) {
			uint16_t dt = 0;
			for (int f = 0; f < 8; f++) {
				dt <<= 1;
				dt |= conv_tbl[ frame_buff[ fidx++ ] & 3 ];
				}
			vram_w[vidx++] = dt;
		}
	}


	// swap buffer
	if ( vram_buf == vram_buf0 ) {
		vram_buf = vram_buf1;
		vram_bufDraw = vram_buf0;
	} else {
		vram_buf = vram_buf0;
		vram_bufDraw = vram_buf1;
	}

}

void rp_system::update(void) {

	// PAL update
	for( int i=0 ; i<0x20 ; i++ ) {
		uint8_t at = m_PAL_W[i];
		if ( at != m_PAL_W_old[i] ) {
			if ( setPF_VRAM( 0x3F00 + i , at ) ) {
				break;
			}
			m_PAL_W_old[i] = at;
			//Serial.printf("setPF_VRAM_PAL %02x,%02x\n", i, at );
		}
	}

	// BG ATR update
	for( int i=0 ; i<0x40 ; i++ ) {
		uint8_t at = m_ATR_W[i];
		if ( at != m_ATR_W_old[i] ) {
			if ( setPF_VRAM( 0x23C0 + i , at ) ) {
				break;
			}
			m_ATR_W_old[i] = at;
		}
	}


	convVram();
}



void rp_system::ppu_dma(void) {

//	pio_sm_exec( pio0, SM_TRAN, 0x8000 );  //  push   noblock                    
//	pio_sm_exec( pio0, SM_TRAN, 0xa02b );  //  mov    x, !null                   
//	ppu_count = pio_sm_get(pio0, SM_TRCNT);

	pio_sm_clear_fifos (pio0, SM_TRAN);
	pio_sm_restart (pio0, SM_TRAN);

	// １フレームのPPUデータ出力回数カウント
	pio_sm_exec( pio0, SM_TRCNT, 0x8000 );  //  push   noblock                    
	pio_sm_exec( pio0, SM_TRCNT, 0xa02b );  //  mov    x, !null                   
	pio_sm_restart (pio0, SM_TRCNT);
	ppu_count = pio_sm_get(pio0, SM_TRCNT);

	if ( (ppu_count >= (PPU_COUNT_VAL -2) ) && (ppu_count <= PPU_COUNT_VAL+2 ) )  {
		vram_dma.TransSM_DMA( vram_buf, VRAM_BUF_SIZE );
		if ( ppu_count == (PPU_COUNT_VAL -2) ) {
			pio_sm_exec( pio0, SM_TRAN, 0x6008 );  // out    pins, 8                    
			pio_sm_exec( pio0, SM_TRAN, 0x6008 );  // out    pins, 8                    
		}
		if ( ppu_count == (PPU_COUNT_VAL -1) ) {
			pio_sm_exec( pio0, SM_TRAN, 0x6008 );  // out    pins, 8                    
		}
#if 1
		// フレームデータの最後にコマンドをセット
		uint8_t* pb = (uint8_t*)vram_buf;
		memcpy( pb + PPU_COUNT_VAL - FC_COM_BUF_SIZE, FC_COM_BUF, FC_COM_BUF_SIZE);
		initFC_COM_BUF();
		rp2040.fifo.push( C1_SNDJOB );
//		snd.jobSound();		// サウンドメイン処理
//		setPF_APU();
#endif
	} else {
		vram_dma.StopDMA();
	}


	if ( ppu_count != ppu_count_old ) {
//		Serial.printf("ppu_count:%d\n", ppu_count );
		ppu_count_old = ppu_count;
	}


}


//=================================================
// Key Trg Update
//=================================================
void rp_system::setKeyUpdate() {
	uint8_t a;
	m_key_old = m_key_new;
	m_key_new = m_key_imp;
	m_key_trg =  (m_key_new ^ m_key_old) & m_key_new;
//	if ( m_key_new != m_key_old ) {
//		Serial.printf("setKeyData %02x %02x %02x\n", m_key_new, m_key_old, m_key_trg );
//	}

	a = m_key_trg & (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT);
	if ( a ) {
		m_rep_key = a;
		m_rep_new = a;
		m_rep_cnt = REP_WAIT;
	} else {
		m_rep_new = 0;
		a = m_key_new & m_rep_key;
		if ( a ) {
			m_rep_cnt--;
			if ( m_rep_cnt == 0) {
				m_rep_new = a;
				m_rep_cnt = REP_INTERVAL;
			}
		}
	}

}


uint8_t rp_system::getRcvCom() {
	return (uint8_t)(pio_sm_get_blocking(pio0, SM_RECV) & 0xff);
}


void rp_system::jobRcvCom() {
	uint8_t dt;

	dt = getRcvCom();

	switch( dt ) {

	case FP_COM_VER:	// BIOS-ROMのバージョン取得
		Serial.printf("FP_COM_VER\n" );
		WDT_update();
		ver_dma();
		break;

	case FP_COM_ROM:	// BIOS-ROMのROMデータ要求コマンド
		dt =  getRcvCom();
		Serial.printf("FP_COM_ROM:%02x\n", dt );
		WDT_update();
		rom_dma( dt );
		break;

	case FP_COM_LOG:	// ログ表示
		Serial.printf( "FP_COM_LOG:" );
		for( int j =1 ; j< 8; j++ ) { 
			if( pio_sm_is_rx_fifo_empty(pio0, SM_RECV) ) {
				break;
			}
			dt = getRcvCom();
			Serial.printf( "%02x ", dt );
		}
		Serial.println( "" );
		break;

	case FP_COM_DRQ:	// データリクエスト
//		Serial.println( "FP_COM_DRQ" );
		jobFP_COM_DRQ();
		break;

	case FP_COM_DLD:	// データロード
		dt =  getRcvCom();
//		Serial.printf("FP_COM_DLD:%02x\n", dt );
		jobFP_COM_DLD( dt );
		break;

	case FP_COM_RST:	// PICOリスタート
		Serial.println( "FP_COM_RST" );
		soft_reset();
		break;

	case FP_COM_INI:	//  PICO初期化
		dt = getRcvCom();
		Serial.printf( "FP_COM_INI %02x\n", dt );
		setWDT_mode( 1 );
		init2();
		break;

		
	default:
		setKeyData( dt );
		ppu_dma();
		sys.frame_draw = 0;
		setWDT_mode( 1 );
		break;
	}
}



void rp_system::FadeIn() {
	setPF_COM( PF_COM_FDIN );

}


void rp_system::FadeOut() {
	setPF_COM( PF_COM_FDOT );
}


void rp_system::SleepMS( int ms ) {
	for ( int i = 0; i < ms; i+= 10 ) {
		WDT_update();
		sleep_ms( 10 );
	}
}


//========================================================
//			データモード関連
//========================================================
void rp_system::startDataMode(void) {
//	Serial.printf("startDataMode\n" );
	m_waitFP_COM_DRQ = 1;
	for(int cnt = 1; cnt < 100; cnt++) {
		setPF_COM( PF_COM_DMOD );
		WDT_update();
		SleepMS( 50 );
		if ( m_waitFP_COM_DRQ == 0 ) return;
		//Serial.printf("waitFP_COM_DRQ retry %d\n", cnt );
	}
}

void rp_system::jobFP_COM_DRQ() {
	m_waitFP_COM_DRQ = 0;

	WDT_update();

	if ( m_PAL_CHG  ) {
		//Serial.printf("jobFP_COM_DRQ:PAL\n" );
		m_PAL_CHG = 0;
		m_pDRQ = m_PAL_W;
		drq_ret(PF_DAT_VRAM, 0x3F00, 0x20 );
		return;
	}

	if ( m_ATR_CHG  ) {
		m_ATR_CHG = 0;
		m_pDRQ = m_ATR_W;
		drq_ret(PF_DAT_VRAM, 0x23C0, 0x40 );
		return;
	}

	if( m_FC_STEP ) {
		drq_ret(PF_DAT_STEP, m_FC_STEP, 0 );
		Serial.printf("PF_DAT_STEP:%02x\n", m_FC_STEP );
		setWDT_mode( 0 );
		m_FC_STEP = 0;
		return;
	}
	// data end
	drq_ret(PF_COM_NONE, 0, 0 );
//	Serial.printf("endDataMode\n" );
}


//=================================================
// JOB FP_COM_DLD
//=================================================
void rp_system::jobFP_COM_DLD( uint8_t adrh ) {
	pio_sm_clear_fifos (pio0, SM_TRAN);
	pio_sm_restart (pio0, SM_TRAN);
	vram_dma.TransSM_DMA( (uint32_t *)&m_pDRQ[ adrh << 8 ] , 0x100  / sizeof( uint32_t ) );
}

void rp_system::rom_dma( uint8_t adrh ) {
	const unsigned char *_rom = getResData( NES_ROM ) + 0x10;
	pio_sm_clear_fifos (pio0, SM_TRAN);
//	pio_sm_put_blocking ( pio0, SM_TRAN, 0x21212121 );
//	pio_sm_put_blocking ( pio0, SM_TRAN, 0x43462321 );
	pio_sm_restart (pio0, SM_TRAN);
	vram_dma.TransSM_DMA( (uint32_t *)&_rom[ (adrh & 0x7f) << 8 ] , 0x7000 );
}

void rp_system::ver_dma() {
	const unsigned char *_rom = getResData( NES_ROM ) + 0x10;
	pio_sm_clear_fifos (pio0, SM_TRAN);
	pio_sm_put_blocking ( pio0, SM_TRAN, 0x21212121 );
	pio_sm_put_blocking ( pio0, SM_TRAN, 0x43462321 );
	pio_sm_restart (pio0, SM_TRAN);
	vram_dma.TransSM_DMA( (uint32_t *)&_rom[ 0x6FF0 ] , 0x10 / sizeof( uint32_t ) );
}


//=================================================
// Data Request Response
//=================================================
void rp_system::drq_ret( uint8_t com, uint16_t adr, uint16_t size ) {
	pio_sm_clear_fifos (pio0, SM_TRAN);
	pio_sm_restart (pio0, SM_TRAN);
	vram_dma.StopDMA();

	uint32_t data = 0xFC00 + com + (adr << 16);
	pio_sm_put_blocking ( pio0, SM_TRAN, data);
	pio_sm_put_blocking ( pio0, SM_TRAN, size);
}

//=================================================
// FC PAL SET
//=================================================
void rp_system::setPalData( const uint8_t *paldt ) {
	memcpy( m_PAL_W, paldt, 0x20);
	memcpy( m_PAL_W_old, paldt, 0x20);
	m_PAL_CHG = 1;
}

void rp_system::setPal( uint8_t idx, uint8_t dt ) {
	if ( m_PAL_W[ idx ] != dt ) {
		Serial.printf("setPal %02x:%02x\n",idx,dt );
		m_PAL_W[ idx ] = dt;
		m_PAL_CHG = 1;
	}
}

//=================================================
// FC ATR SET
//=================================================
void rp_system::setAtrData( const uint8_t *atrdt ) {
	memcpy( m_ATR_W, atrdt, 0x40);
	memcpy( m_ATR_W_old, atrdt, 0x40);
	m_ATR_CHG = 1;
}

//=================================================
// FC ATR CLEAR
//=================================================
void rp_system::clearAtrData( void ) {
	memset(m_ATR_W, 0x0, 0x40 );
	memset(m_ATR_W_old, 0x0, 0x40 );
	m_ATR_CHG = 1;
}



void rp_system::setAtr( uint8_t lx, uint8_t ly, uint8_t dt ) {
	const uint8_t mask_tbl[4]= {
		0b11111100,
		0b11110011,
		0b11001111,
		0b00111111
	};
	const uint8_t set_tbl[4]= {
		0b00000001,
		0b00000100,
		0b00010000,
		0b01000000
	};

	uint8_t idx = (lx >> 1) + (( ly >> 1 ) << 3);
	idx &= 0x3F;
	uint8_t sel = (lx & 1) + (( ly & 1 ) << 1);

	m_ATR_W[ idx ] &= mask_tbl[sel];
	m_ATR_W[ idx ] |= set_tbl[sel] * dt;
	m_ATR_CHG = 1;
}


//=================================================
// APU REG SET
//  reg -> サウンドレジスターの下位8bit
//  data -> 書き込む値
//
// 補足
//  書き込む値をバッファリングして毎フレームファミコンに送信する
//=================================================
void rp_system::setAPU( uint8_t reg, uint8_t data ) {
//	m_APU[ reg ] = data;
	
//	FC_COM_BUF[ PICO_SNDREG + reg ] = data;	// 終了コード

	m_APU_REG[ m_APU_W_IDX ] = reg;
	m_APU_DAT[ m_APU_W_IDX ] = data;
	m_APU_W_IDX++;
	m_APU_W_IDX &= (PICO_APU_BUF_SIZE -1);
}

//=================================================
// APUバッファの値をFC_COM_BUFにセット
//=================================================
void rp_system::setPF_APU() {
//	memcpy( &FC_COM_BUF[ PICO_SNDREG ], m_APU, 0x18 );
	int com_buf_idx = PICO_SNDREG;
	for ( ; com_buf_idx < FC_COM_BUF_SIZE-2 ;  ) {
		if ( m_APU_W_IDX == m_APU_R_IDX ) break;
		FC_COM_BUF[ com_buf_idx++ ] = m_APU_REG[ m_APU_R_IDX ];
		FC_COM_BUF[ com_buf_idx++ ] = m_APU_DAT[ m_APU_R_IDX ];
		m_APU_R_IDX++;
		m_APU_R_IDX &= (PICO_APU_BUF_SIZE -1);
	}
	FC_COM_BUF[ com_buf_idx ] = 0xff;	// 終了コード
}


rp_system sys;

