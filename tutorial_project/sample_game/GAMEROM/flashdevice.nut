//bit is masking MSB
function mask_get(bit)
{
	local t = 1 << (bit + 1);
	return t - 1;
}
local mega = 0x20000;
local MASK_A14 = mask_get(14);
local MASK_A10 = mask_get(10);
device <- {
	["dummy"] = {
		capacity = 16 * mega, pagesize = 1,
		erase_wait = 0, erase_require = false,
		retry = false,
		id_manufacurer = 0xf1, id_device = 0xf1,
		command_mask = 0
	},
	["W29C020"] = {
		capacity = 2 * mega, pagesize = 0x80,
		erase_wait = 50, erase_require = false,
		retry = false,
		id_manufacurer = 0xda, id_device = 0x45,
		command_mask = MASK_A14
	},
	["W29C040"] = {
		capacity = 4 * mega, pagesize = 0x100,
		erase_wait = 50, erase_require = false,
		retry = true,
		id_manufacurer = 0xda, id_device = 0x46,
		command_mask = MASK_A14
	},
	["W49F002"] = {
		capacity = 2 * mega, pagesize = 1,
		erase_wait = 100, erase_require = true,
		retry = false,
		id_manufacurer = 0xda, id_device = 0xae,
		command_mask = MASK_A14
	},
	["AT49F002"] = {
		capacity = 2 * mega, pagesize = 1,
		erase_wait = 100, erase_require = true,
		retry = true,
		id_manufacurer = 0x1f, id_device = 0x08,
		command_mask = MASK_A14
	},
	["EN29F002T"] = {
		capacity = 2 * mega, pagesize = 1,
		erase_wait = 2000, erase_require = true,
		retry = false,
		id_manufacurer = 0x1c, id_device = 0x92,
		command_mask = MASK_A14
	},
	["AM29F040B"] = {
		capacity = 4 * mega, pagesize = 1,
		erase_wait = 8000, erase_require = true,
		retry = false,
		id_manufacurer = 0x01, id_device = 0xa4,
		command_mask = MASK_A10
	},
	//command mask is not written in datasheet!
	["PM29F002T"] = {
		capacity = 2 * mega, pagesize = 1,
		erase_wait = 500, erase_require = true,
		retry = false,
		id_manufacurer = 0x9d, id_device = 0x1d,
		command_mask = MASK_A10 //maybe A10-A0
	},
	//chip erase time is not written in datasheet!!
	["MBM29F080A"] = {
		capacity = 8 * mega, pagesize = 1,
		erase_wait = 8000, erase_require = true,
		retry = false,
		id_manufacurer = 0x04, id_device = 0xd5,
		command_mask = MASK_A10
	},
	["SST39SF040"] = {
		capacity = 4 * mega, pagesize = 1,
		erase_wait = 100, erase_require = true,
		retry = false,
		id_manufacurer = 0xbf, id_device = 0xb7,
		command_mask = MASK_A14
	}
};
function flash_device_get(name)
{
	return device[name];
}

//listup for GUI
//device table -> device name list array
{
	device_list<-[];
	foreach(v, i in device){
		device_list.append(v);
	}
	device_list.sort();
}

function flash_device_number_get()
{
	return device_list.len();
}
function flash_device_name_get(i)
{
	return device_list[i];
}
