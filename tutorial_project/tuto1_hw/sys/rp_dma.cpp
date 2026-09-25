/*
    rp_dma.h - DMA関連
 */




#include "Arduino.h"
#include "rp_dma.h"


void initDMA(void) {
	//---------------------------
	// INTT DMA
	//---------------------------
	for (uint i = 0; i < 16; i++) {
		if ( dma_channel_is_claimed ( i ) ) {
			dma_channel_cleanup(i);
			dma_channel_unclaim(i);
		}
	}
}


rp_dma::rp_dma(void) {
//	Serial.println("rp_dma::rp_dma");
//	dma_chan = -1;
}


//----------------------------------------------------------
//
//  ステートマシン DMA受信初期化（整備中）
//
//----------------------------------------------------------
int rp_dma::initSM_DMA_RX( int pio_no, int sm, uint32_t *buf, int size ) {
	PIO pio;
	volatile void *r_addr;
	
	if ( pio_no == 0 ) {
		pio = pio0;
		r_addr = (volatile void *)&pio0_hw->rxf[sm];
	} else if ( pio_no == 1 ) {
		pio = pio1;
		r_addr = (volatile void *)&pio1_hw->rxf[sm];
	}

	// DMA設定 未使用DAMチャンネル取得
	int dma_chan = dma_claim_unused_channel(true);
	//  取得したDMAチャンネルの構成情報を取得する
	dma_channel_config dma_chan_config = dma_channel_get_default_config(dma_chan);

//	channel_config_set_write_increment(&dma_chan_config, false);
    channel_config_set_read_increment(&dma_chan_config, false);

	/* default_config
Read Increment	true
Write Increment	false
DReq	DREQ_FORCE
Chain to	self
Data size	DMA_SIZE_32
Ring	write=false, size=0 (i.e. off)
Byte Swap	false
Quiet IRQs	false
High Priority	false
Channel Enable	true
Sniff Enable	false
*/
	//  SMのデータ要求によってDMA REQが発生するよう指定する
	channel_config_set_dreq(&dma_chan_config, pio_get_dreq(pio, sm, true));


	//  DMAチャンネル構成情報を設定する
	dma_channel_configure(
	  dma_chan,
	  &dma_chan_config,
	  buf, 		 // 出力先
	  r_addr,	 // 読み出し元
	  size,  	// データテーブルからの読み出しデータ数、これを読みだしたらDMA停止
	  true		// スタートタイミング（即時スタートの場合はtrue）
	);

	Serial.printf("initSM_DMA_RX:%d\n", dma_chan );

	return dma_chan;
}



//----------------------------------------------------------
//
//  ステートマシン DMA送信初期化
//
//----------------------------------------------------------
int rp_dma::initSM_DMA_TX( int pio_no, int sm, uint32_t *buf, int size ) {
	PIO pio;
	volatile void *w_addr;
	
	if ( pio_no == 0 ) {
		pio = pio0;
		w_addr = &pio0_hw->txf[sm];
	} else if ( pio_no == 1 ) {
		pio = pio1;
		w_addr = &pio1_hw->txf[sm];
	}

	// DMA設定 未使用DAMチャンネル取得
	dma_chan = dma_claim_unused_channel(true);
	//  取得したDMAチャンネルの構成情報を取得する
	dma_channel_config dma_chan_config = dma_channel_get_default_config(dma_chan);

	/* default_config
Read Increment	true
Write Increment	false
DReq	DREQ_FORCE
Chain to	self
Data size	DMA_SIZE_32
Ring	write=false, size=0 (i.e. off)
Byte Swap	false
Quiet IRQs	false
High Priority	false
Channel Enable	true
Sniff Enable	false
*/
	//  SMのデータ要求によってDMA REQが発生するよう指定する
	channel_config_set_dreq(&dma_chan_config, pio_get_dreq(pio, sm, true));
//	channel_config_set_dreq(&dma_chan_config, pio_get_dreq(pio0, sm, true));


	//  DMAチャンネル構成情報を設定する
	dma_channel_configure(
	  dma_chan,
	  &dma_chan_config,
//	  &pio0_hw->txf[sm],
	  w_addr,
	  buf,       // 読み出し元
	  size,  // データテーブルからの読み出しデータ数、これを読みだしたらDMA停止
	  false           // スタートタイミング（即時スタートの場合はtrue）
	);

//	Serial.printf("initSM_DMA_TX:%d\n", dma_chan );

	return dma_chan;
}


//	転送開始
void rp_dma::TransSM_DMA( uint32_t *buf, int buf_size ) {
	StopDMA();

	dma_channel_transfer_from_buffer_now(
	  dma_chan,   // DMA channel no.
	  buf,        // read address (unit32_t*)
	  buf_size );  // number of transfers
}

//	転送開始
void rp_dma::StopDMA() {
	dma_channel_abort( dma_chan );
}



//----------------------------------------------------------
//
//  DMAを使ったメモリー転送（動作検証中）
//
//----------------------------------------------------------
int rp_dma::memcpyDMA(void *DstBuf, const void *SrcBuf, size_t n) {
	int dma_chan = dma_claim_unused_channel(true);
	//  取得したDMAチャンネルの構成情報を取得する
	dma_channel_config dma_chan_config = dma_channel_get_default_config(dma_chan);

	/* default_config
Read Increment	true
Write Increment	false
DReq	DREQ_FORCE
Chain to	self
Data size	DMA_SIZE_32
Ring	write=false, size=0 (i.e. off)
Byte Swap	false
Quiet IRQs	false
High Priority	false
Channel Enable	true
Sniff Enable	false
*/
	channel_config_set_transfer_data_size(&dma_chan_config, DMA_SIZE_8);
	channel_config_set_write_increment(&dma_chan_config, true);

	//  DMAチャンネル構成情報を設定する
	dma_channel_configure(
	  dma_chan,
	  &dma_chan_config,
	  DstBuf,
	  SrcBuf,  // 読み出し元
	  n,
	  true  // スタートタイミング（即時スタートの場合はtrue）
	);
	return dma_chan;
}


//----------------------------------------------------------
//
//  DMAを使ったメモリー転送（動作検証中）
//
//----------------------------------------------------------
int rp_dma::memcpyDMA32(void *DstBuf, const void *SrcBuf, size_t n) {
	int dma_chan = dma_claim_unused_channel(true);
	//  取得したDMAチャンネルの構成情報を取得する
	dma_channel_config dma_chan_config = dma_channel_get_default_config(dma_chan);

	/* default_config
Read Increment	true
Write Increment	false
DReq	DREQ_FORCE
Chain to	self
Data size	DMA_SIZE_32
Ring	write=false, size=0 (i.e. off)
Byte Swap	false
Quiet IRQs	false
High Priority	false
Channel Enable	true
Sniff Enable	false
*/
	channel_config_set_write_increment(&dma_chan_config, true);

	//  DMAチャンネル構成情報を設定する
	dma_channel_configure(
	  dma_chan,
	  &dma_chan_config,
	  DstBuf,
	  SrcBuf,  // 読み出し元
	  n,
	  true  // スタートタイミング（即時スタートの場合はtrue）
	);

	return dma_chan;
}



//=================================================
//			DMA関連
//=================================================

#if 0

	// DMA設定
	dma_chan = dma_claim_unused_channel(true);
	//  取得したDMAチャンネルの構成情報を取得する
	dma_channel_config dma_chan_config = dma_channel_get_default_config(dma_chan);
	//  DMA転送単位を32ビットに設定する
	channel_config_set_transfer_data_size(&dma_chan_config, DMA_SIZE_32);
	//  DMA読み出し元（データテーブル）アドレスを自動インクリメント設定する
	channel_config_set_read_increment(&dma_chan_config, true);

	//  DMA書き込み先（PWMレジスタ）アドレスは固定にする
	channel_config_set_write_increment(&dma_chan_config, false);
	//  SMのデータ要求によってDMA REQが発生するよう指定する
	channel_config_set_dreq(&dma_chan_config, pio_get_dreq(pio, SM_TRAN, true) );
	//  DMAチャンネル構成情報を設定する
	dma_channel_configure(
		dma_chan,
		&dma_chan_config,
		&pio0_hw->txf[SM_TRAN], 
		vram_buf,	// 読み出し元
		VRAM_BUF_SIZE, 		// データテーブルからの読み出しデータ数、これを読みだしたらDMA停止
		false				// スタートタイミング（即時スタートの場合はtrue）
	);


#endif


