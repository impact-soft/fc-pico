function loopsize_get(t, trans, image_size, device_size)
{
	local trans_full = 3, trans_top = 1, trans_bottom = 2; //header.h enum transtype
	local loop = {start = 0, end = 0};
	switch(trans){
	case trans_full:{
		local size = device_size < t.size_max ? device_size : t.size_max;
		loop.end = size / t.banksize;
		}break;
	case trans_top:
		loop.end = image_size / t.banksize;
		break;
	case trans_bottom:
		loop.start = (t.size_max - image_size) / t.banksize;
		loop.end = t.size_max / t.banksize;
		break;
	default:
		loop.start = 0;
		loop.end = 0;
		break;
	}
	return loop;
}

function testrun(
	d, script, mapper, 
	cpu_trans, cpu_image_size, cpu_device_size,
	ppu_trans, ppu_image_size, ppu_device_size
)
{
	const mega = 0x20000;
	local trans_empty = 0;
	dofile(script);

	if((board.mappernum != mapper) && (mapper != 0)){
		print("mapper number are not connected\n");
		print("script:" + board.mappernum + " image:" + mapper + "\n");
	}
	local cpu_loop = loopsize_get(board.cpu_rom, cpu_trans, cpu_image_size, cpu_device_size);
	local ppu_loop = loopsize_get(board.ppu_rom, ppu_trans, ppu_image_size, ppu_device_size);
	if(board.vram_mirrorfind == true){
		vram_mirrorfind(d);
	}
	program_initalize(d, board.cpu_rom.banksize, board.ppu_rom.banksize);
	if(cpu_trans != trans_empty){
		cpu_transfer(d, cpu_loop.start, cpu_loop.end, board.cpu_rom.banksize);
	}
	if(ppu_trans != trans_empty){
		ppu_transfer(d, ppu_loop.start, ppu_loop.end, board.ppu_rom.banksize);
	}
}

function program(
	d, script, mapper, 
	cpu_trans, cpu_image_size, cpu_device_size,
	ppu_trans, ppu_image_size, ppu_device_size
)
{
	const mega = 0x20000;
	local trans_empty = 0;
	dofile(script);

	if((board.mappernum != mapper) && (mapper != 0)){
		return;
	}
	local cpu_loop = loopsize_get(board.cpu_rom, cpu_trans, cpu_image_size, cpu_device_size);
	local ppu_loop = loopsize_get(board.ppu_rom, ppu_trans, ppu_image_size, ppu_device_size);
	local co_cpu = newthread(cpu_transfer);
	local co_ppu = newthread(ppu_transfer);
	program_initalize(d, board.cpu_rom.banksize, board.ppu_rom.banksize);
	if(cpu_trans != trans_empty){
		cpu_erase(d);
	}
	if(ppu_trans != trans_empty){
		ppu_erase(d);
	}
	erase_wait(d);
	if(cpu_trans != trans_empty){
		co_cpu.call(d, cpu_loop.start, cpu_loop.end, board.cpu_rom.banksize);
	}
	if(ppu_trans != trans_empty){
		co_ppu.call(d, ppu_loop.start, ppu_loop.end, board.ppu_rom.banksize);
	}
	program_main(d, co_cpu, co_ppu);
}
