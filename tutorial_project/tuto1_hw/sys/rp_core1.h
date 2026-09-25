


void setup1() {
	snd.init();
}


void loop1() {
	WDT_check();


	//--------------------------------------
	if (rp2040.fifo.available() == 0){
		snd.jobMP3();
		sleep_ms(LOOP_MS);
		return;
	}
	uint32_t buffer_Number = rp2040.fifo.pop();
	uint32_t com_sel = 0xFF000000 & buffer_Number;
	if ( com_sel == C1_SND_MP3PLAY ) {
		setMP3data( buffer_Number & 0xff );
	} else {
		switch ( buffer_Number ) {
		case C1_RESET:
			setup1();
			break;
		case C1_SNDJOB:
			snd.jobSound();
			sys.setPF_APU();
			break;

		}
	}

}

