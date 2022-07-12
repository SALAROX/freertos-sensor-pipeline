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
A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE OR ADVISE SUPPLIED TO THE PROJECT
BY FREESCALE, AND OR NAY PRODUCT RESULTING FROM FREESCALE SERVICES. IN NO EVENT
SHALL FREESCALE BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
THIS AGREEMENT.

CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or 
actions by anyone on account of any damage, or injury, whether commercial, 
contractual, or tortuous, rising directly or indirectly as a result of an advise
or assistance supplied CUSTOMER in connection with product, services or goods 
supplied under this Agreement.
********************************************************************************
* File      PFlash.c
* Owner     b01802
* Version   1.0   
* Date      Dec-02-2010
* Classification   General Business Information
* Brief     Program and Erase functions
********************************************************************************
Revision History:
Version   Date	        Author		Description of Changes
1.0       Dec-02-2010   b01802    Initial version
*******************************************************************************/
#include "Cpu.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PFlash.h"
#pragma CODE_SEG RAM_CODE

/*lint -save  -e923 -e926 -e927 -e928 -e929 Disable MISRA rule (11.3,11.4) checking. */
/* Types definitions */

typedef struct
{
  uint8_t code[15]; /* Structure required to copy code to ram memory */
  /* Size of this structure needs to be at least (but best) the size of the FnCmdInRam_ */
} FnCmdInRamStruct;

typedef void (*pFnCmdInRam)(void);

/* Global variables */

#pragma CODE_SEG __NEAR_SEG NON_BANKED

static void FnCmdInRam_(void)
{
  FSTAT = 0x80U; /* Clear flag command buffer empty */
  while (FSTAT_CCIF == 0U)
  {
  } /* Wait to command complete */
  return;
}

/*lint -save  -e740 -e931 Disable MISRA rule (1.2) checking. */
static void CallFnCmdInRam()
{
  FnCmdInRamStruct FnCmdInRam = *(FnCmdInRamStruct *)(FnCmdInRam_); /* Create a copy of Wait in RAM routine on stack */

  ((pFnCmdInRam)&FnCmdInRam)(); /* Call code in RAM */
  return;
}
/*lint -restore Enable MISRA rule (1.2) checking. */

#pragma CODE_SEG RAM_CODE

#define FLASH_SECTOR_SIZE 0x200

/******************************************************************************/
#pragma CODE_SEG RAM_CODE
uint8_t PFlash_Program(uint32_t address, const uint16_t *ptr)
{
  uint8_t i;
  uint8_t flashArray_Counter = 0;

  address |= 0x0030000;  /* Full Address Required for Flashing*/

  EnterCritical();

  while ((FSTAT & FSTAT_CCIF_MASK) == 0)
    ;           //wait if command in progress
  FSTAT = 0x30; //clear ACCERR and PVIOL

  FCCOBIX = 0x00;
  FCCOB = 0x0600 | ((address & 0x00030000) >> 16);

  FCCOBIX = 0x01;
  FCCOB = (address & 0x0000FFFF);

  for (i = 2; i < 6; i++) //fill data (4 words) to FCCOB register
  {
    FCCOBIX = i;
    FCCOB = *ptr;
    ptr++;
  }

#if 0
  FSTAT = 0x80; //launch command
  while ((FSTAT & FSTAT_CCIF_MASK) == 0)
    ; //wait for done
#else
  CallFnCmdInRam();
#endif

  if ((FSTAT & (FSTAT_ACCERR_MASK | FSTAT_FPVIOL_MASK)) != 0)
  {
    ExitCritical();
    return FlashProgramError;
  }
  else
  {
    ExitCritical();
    return noErr;
  }
}
#pragma CODE_SEG DEFAULT

/******************************************************************************/
#pragma CODE_SEG RAM_CODE
uint8_t PFlash_EraseSector(uint32_t address)
{
  EnterCritical();
  while ((FSTAT & FSTAT_CCIF_MASK) == 0)
    ;           //wait if command in progress
  FSTAT = 0x30; //clear ACCERR and PVIOL

  FCCOBIX = 0x00;
  FCCOB = 0x0A00 | ((address & 0x00030000) >> 16);

  FCCOBIX = 0x01;
  FCCOB = (address & 0x0000FFF8);

#if 0
  FSTAT = 0x80; //launch command
  while ((FSTAT & FSTAT_CCIF_MASK) == 0)
    ; //wait for done
#else
  CallFnCmdInRam();
#endif

  if ((FSTAT & (FSTAT_ACCERR_MASK | FSTAT_FPVIOL_MASK)) != 0)
  {
    ExitCritical();
    return FlashEraseError;
  }
  else
  {
    ExitCritical();
    return noErr;
  }
}
#pragma CODE_SEG DEFAULT

/******************************************************************************/
#pragma CODE_SEG RAM_CODE
uint8_t PFlash_EraseSectorBySector(uint32_t addr_l, uint32_t addr_h)
{
  uint32_t Address;
  uint8_t Error;

  for (Address = addr_l; Address < addr_h; Address += FLASH_SECTOR_SIZE)
  {
    Error = PFlash_EraseSector(Address);
    if (Error != noErr)
      return (Error);
  }
  return (noErr);
}
#pragma CODE_SEG DEFAULT
/******************************************************************************/

/*
** ===================================================================
**     Method      :  PFlash_GetWordFlash (component IntFLASH)
*/
/*!
**     @brief
**         Gets a word from an address in FLASH.
**     @param
**         Addr            - Address to FLASH.
**     @param
**         Data            - A pointer to the returned 16-bit data.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - Address is out of range 
**                           - ERR_BUSY - Device is busy
*/
/* ===================================================================*/
#pragma CODE_SEG RAM_CODE
byte PFlash_GetWordFlash(PFlash_TAddress Addr, word *Data)
{
  if (BlockOutOfRange(Addr, (Addr + 1U)))
  { /* Check range of address */
    return (ERR_RANGE);
  }
  if (Addr & 1U)
  { /* Aligned address ? */
    return ERR_NOTAVAIL;
  }
  if (FSTAT_CCIF == 0U)
  {                  /* Is previous command complete ? */
    return ERR_BUSY; /* If yes then error */
  }
  *Data = *(word *)Addr; /* Get word form Flash */
  return ERR_OK;
}
#pragma CODE_SEG DEFAULT

/*lint -restore Enable MISRA rule (16.7) checking. */
/*
** ===================================================================
**     Method      :  IFsh1_GetBlockFlash (component IntFLASH)
*/
/*!
**     @brief
**         Reads data from FLASH.
**     @param
**         Source          - Destination address in FLASH.
**     @param
**         Dest            - Source address of the data.
**     @param
**         Count           - Count of the data fields (in the
**                           smallest addressable units).
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - The address is out of range 
**                           - ERR_BUSY - Device is busy
*/
/* ===================================================================*/
#pragma CODE_SEG RAM_CODE
byte PFlash_GetBlockFlash(PFlash_TAddress Addr, PFlash_TDataAddress Dest, word Count)
{
  if (BlockOutOfRange(Addr, ((Addr + Count) - 1U)))
  { /* Check range of address */
    return (ERR_RANGE);
  }
  if (((Addr & 1U) != 0U) || ((Count & 1U) != 0U))
  { /* Word aligned address and count? */
    return ERR_NOTAVAIL;
  }
  if (FSTAT_CCIF == 0U)
  {                  /* Is previous command complete ? */
    return ERR_BUSY; /* If yes then error */
  }
  while (Count--)
  {
    *Dest = *(byte *)(Addr); /* Set virtual page element according to value of Flash given by the appropriate address */
    Addr++;
    Dest++;
  } /* while Count */
  return ERR_OK;
}
#pragma CODE_SEG DEFAULT

/* ===================================================================*/
#pragma CODE_SEG RAM_CODE
byte PFlash_SetGlobalProtection(bool ProtectAll)
{
  if (ProtectAll)
  {
    FPROT = 0x7FU; /* Set whole array as protect */
    if ((FPROT & 0xBFU) != 0x3FU)
    {
      return ERR_VALUE; /* Was protection register set corectly? */
    }
  }
  else
  {
    FPROT = 0xFFU; /* Set whole array as unprotect */
    if ((FPROT & 0xBFU) != 0xBFU)
    {
      return ERR_VALUE; /* Was protection register set corectly? */
    }
  }
  return ERR_OK;
}
#pragma CODE_SEG DEFAULT