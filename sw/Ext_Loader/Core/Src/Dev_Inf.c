/*
 * Dev_Inf.c
 *
 *  Created on: May 7, 2026
 *      Author: DWI
 */

#include "Dev_Inf.h"

#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo = {
#else
struct StorageInfo const StorageInfo = {
#endif
	"W25N01GV_Bat-Source",  // Device Name + version number
	NAND_FLASH,             // Device Type
	0x90000000,             // Device Start Address
	0x08000000,             // Device Size in Bytes (128MBytes/1Gbits)
	0x00020000,             // Programming Page Size 2KBytes
	0xFF,                   // Initial Content of Erased Memory
	// Specify Size and Address of Sectors (view example below)
	{
		{
			0x00000400, 0x00020000 // Sector Num : 1024, Sector Size: 128KBytes
		},
		{
			0x00000000, 0x00000000
		}
	}
};
