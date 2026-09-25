/*
    rp_dma.h
 */

#ifndef rp_dma_h
#define rp_dma_h

#include "Arduino.h"
#include "hardware/pio.h"
#include "hardware/dma.h"


class rp_dma {
    
public:
    rp_dma();
	int initSM_DMA_RX( int pio_no, int sm, uint32_t *buf, int size );
    int initSM_DMA_TX( int pio_no, int sm, uint32_t *buf, int size );
	void TransSM_DMA( uint32_t *buf, int buf_size );
	void StopDMA();
	int memcpyDMA(void *DstBuf, const void *SrcBuf, size_t n);
	int memcpyDMA32(void *DstBuf, const void *SrcBuf, size_t n);
private:
	int dma_chan;
};

void initDMA(void);

#endif

