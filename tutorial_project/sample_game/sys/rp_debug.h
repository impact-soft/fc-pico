/*
    rp_debug.h
 */

#ifndef rp_debug_h
#define rp_debug_h

//#define NDEBUG	1


// デバッグトレース番号
enum {
	DTR_ROOT,
	DTR_MAIN,
	DTR_TITLE,
	DTR_DEMO,

	DTR_MAX,
};

#define DTR_LOG_SIZE  16



#ifdef NDEBUG
#define TRACE(dtrno) /* Nothing */
#define TRACE_END(dtrno) /* Nothing */
#else
#define TRACE(dtrno) setDebugTrace( dtrno, __LINE__);
#define TRACE_END(no) setDebugTrace( no, __LINE__);
#endif



//---------------------------------------
// ASSERT
//---------------------------------------
#define QUOTE_detail(arg) #arg
#define QUOTE(arg) QUOTE_detail(arg)

#ifdef NDEBUG
#define ASSERT(arg) /* Nothing */
#else

#define ASSERT(arg) \
    do \
    { \
        if ( false == static_cast<bool>(arg) ) \
        { \
            Serial.println("assert failed on line #" QUOTE(__FILE__) QUOTE(__LINE__) " : '" #arg "'" \
                           " -- now entering infinite loop"); \
            for (;;) yield(); \
        } \
    } while ( false )
#endif


#define TRS_BUFF_SIZE  16


void WDT_init();
void WDT_update();
void WDT_check();
void setWDT_mode( uint8_t mode  );
void setDebugTrace( uint8_t dtrno, uint16_t line );


#endif

