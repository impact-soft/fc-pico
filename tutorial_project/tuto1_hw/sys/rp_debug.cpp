/*
    rp_dma.h - DMA関連
 */


#define rp_debug_cpp


#include "Arduino.h"
#include "rp_system.h"
#include "rp_debug.h"

uint8_t WDT_mode;
unsigned long WDI_time;


const char *DTR_str[DTR_MAX] = {
		"DTR_ROOT",
		"DTR_MAIN",
		"DTR_TITLE",
		"DTR_DEMO",
};


uint8_t  DTR_log_idx;
uint8_t  DTR_log_kind[ DTR_LOG_SIZE ];
uint16_t DTR_log_line[ DTR_LOG_SIZE ];


//=========================================================
//
//			 DebugTrace system
//
//=========================================================
void setDebugTrace( uint8_t dtrno, uint16_t line ) {
	DTR_log_idx &= (DTR_LOG_SIZE -1);
	DTR_log_kind[ DTR_log_idx ] = dtrno;
	DTR_log_line[ DTR_log_idx ] = line;
	DTR_log_idx++;
}

String strDebugTrace(void) {
	char buffer[50];
	String msg;
	uint8_t idx = DTR_log_idx;

	for( int i = 0 ; i < DTR_LOG_SIZE; i++ ) {
		idx--;
		idx &= (DTR_LOG_SIZE -1);
		uint16_t line = DTR_log_line[ idx ];
		if ( line == 0 ) continue;
		snprintf(buffer, sizeof(buffer), "%s:%d\n",DTR_str[ DTR_log_kind[ idx ] ],line  );
		msg += buffer;
	}
	return msg;
}



void WDT_init() {
	watchdog_enable(5000, 1); // 5000ms後にリセット
	setWDT_mode( 0 );
	WDT_update();
	DTR_log_idx = 0;
}

void WDT_update() {
	WDI_time = micros();
	watchdog_update();
}

void setWDT_mode( uint8_t mode  ) {
	if ( WDT_mode != mode ) {
		//Serial.printf("setWDT_mode %d\n",mode );
		WDT_mode = mode;
	}
}

void WDT_check() {

	int timer = (int)((micros() - WDI_time) / MICROS_1MS);
//	Serial.printf("WDT_check %d\n",timer );
	if ( timer > 2000 ) {
		if ( WDT_mode == 0 ) {
			WDT_update();
		}
		WDI_time = micros();
#ifndef NDEBUG
/*
		char buffer[50]; // バッファサイズを適切な値に設定

		String msg;
		snprintf(buffer, sizeof(buffer), "Warning WDT_check %d:%d step %d:%d\n",timer, WDT_mode,ap.getStep(),ap.getStepSub() );
		msg += buffer;
		msg += strDebugTrace();

		Serial.print( msg );
*/

#endif
	}

}





