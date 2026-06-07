/*
 * Loader_Src.h
 *
 *  Created on: May 7, 2026
 *      Author: DWI
 */

#ifndef INC_LOADER_SRC_H_
#define INC_LOADER_SRC_H_

#include "main.h"

int Init(void);

int Write(uint32_t Address, uint32_t Size, uint8_t* buffer);

int SectorErase(uint32_t StartAddress, uint32_t EndAddress);

int Read(uint32_t Address, uint32_t Size, uint8_t* buffer);

//uint64_t Verify(uint32_t FlashAddr, uint32_t RAMBufferAddr, uint32_t Size);

int MassErase(void);

//int Checksum(); // Todo: Parameters not specified in documentation

#endif /* INC_LOADER_SRC_H_ */
