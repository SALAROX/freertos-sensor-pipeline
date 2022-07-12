/*******************************************************************************
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2005 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
********************************************************************************
Services performed by FREESCALE in this matter are performed AS IS and without 
any warranty. CUSTOMER retains the final decision relative to the total design 
and functionality of the end product. FREESCALE neither guarantees nor will be 
held liable by CUSTOMER for the success of this project.
FREESCALE DISCLAIMS ALL WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING, 
BUT NOT LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR 
A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE ORE ADVISE SUPPLIED TO THE PROJECT
BY FREESCALE, AND OR NAY PRODUCT RESULTING FROM FREESCALE SERVICES. IN NO EVENT
SHALL FREESCALE BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
THIS AGREEMENT.

CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or 
actions by anyone on account of any damage, or injury, whether commercial, 
contractual, or tortuous, rising directly or indirectly as a result of an advise
or assistance supplied CUSTOMER in connection with product, services or goods 
supplied under this Agreement.
********************************************************************************
* File      PFlash.h
* Owner     b01802
* Version   1.0 
* Date      Dec-02-2010
* Classification   General Business Information
* Brief     Program and Erase functions
********************************************************************************
* Detailed Description:
********************************************************************************
Revision History:
Version  Date          Author    Description of Changes
1.0      Feb-24-2010   b01802    Initial version
*******************************************************************************/
#ifndef _FLASH_H
#define _FLASH_H

#include "PE_Types.h"

#pragma CODE_SEG RAM_CODE

#define noErr           0
#define SRecRangeError  1    //S-Record Out Of Range
#define SRecOddError    2      //S-Record Size Must Be Even
#define FlashProgramError 3 //Flash Programming Error
#define FlashEraseError 4   //Flash Erase Error
#define BadHexData      5        //Bad Hex Data
#define SRecTooLong     6       //S-Record Too Long
#define CheckSumErr     7       //Checksum Error
#define UnknownPartID   8      //Unknown Part ID


/* Flash area sector size valid for all areas: minimal erasable unit (in bytes). */
#define PFLASH_AREA_SECTOR_SIZE          (0x0200U)
/* Size of programming phrase, i.e. number of bytes that must be programmed at once */
#define PFLASH_PROGRAMMING_PHRASE        (0x08U)

#ifndef __BWUserType_IFsh1_TAddress
#define __BWUserType_IFsh1_TAddress
  typedef uint32_t PFlash_TAddress;        /* component type for address to the FLASH. Address is 24-bit GLOBAL address*/
#endif
#ifndef __BWUserType_IFsh1_TDataAddress
#define __BWUserType_IFsh1_TDataAddress
  typedef byte *PFlash_TDataAddress;    /* component type for pointer to data. Data are either source, when writting a block to flash, or output, when reading a block from flash */
#endif

#pragma CODE_SEG RAM_CODE


uint8_t PFlash_Program(uint32_t address, uint16_t *ptr);
uint8_t PFlash_EraseSector(uint32_t address);
uint8_t PFlash_EraseSectorBySector(uint32_t addr_l, uint32_t addr_h);

byte PFlash_GetWordFlash(PFlash_TAddress Addr,word *Data);
byte PFlash_GetBlockFlash(PFlash_TAddress Addr, PFlash_TDataAddress Dest, word Count);

#define BlockOutOfRange(Addr1, Addr2) ((Addr1 > Addr2) || (Addr2 > 0x03FFFFUL) || (Addr1 < 0x034000UL))

byte PFlash_SetGlobalProtection(bool ProtectAll);
#pragma CODE_SEG DEFAULT

#endif
/******************************************************************************/
