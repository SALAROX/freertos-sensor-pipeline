/*
 * nfc_main.h
 *
 *  Created on: Feb 15, 2022
 *      Author: Cristian Castro
 */

#ifndef NFC_MAIN_H_
#define NFC_MAIN_H_

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */
#include <stddef.h>
#include "Cpu.h"
#include "Events.h"
#include "SM1.h"
#include "SPI_SS.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "rfal_analogConfig.h"
#include "st25r3911_com.h"
#include "st25r3911.h"
#include "rfal_user.h"
#include "platform.h"
#include "mf_desfire.h"
#include "hce.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

#define IMPLEMENT_NFC
/* NFC Scanner intervals */
#define NFC_SCAN_TIME_ON									50	//Default: 200     For testing: 500
#define NFC_SCAN_TIME_OFF									500	//Default: 2000    For testing: 1000

/* Encoding Values */
#define NFC_TAG_NODETECTED	0
#define NFC_TAG_PASSIVE		1
#define NFC_TAG_ACTIVE		2

#define NFC_AUTHENTICATION_NOSESSION		0
#define NFC_AUTHENTICATION_SUCCEED			1
#define NFC_AUTHENTICATION_FAILED			2
#define NFC_AUTHENTICATION_TAMPER_ATTEMPT	3

/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */



/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */


/*
 * *****************************************************************
 * GLOBAL VARIABLE
 * *****************************************************************
 */

/*
 * *****************************************************************
 * GLOBAL VARIABLE
 * *****************************************************************
 */

 /* RFAL Variable */
extern rfalNfcaListenDevice 	nfcaDev[1];
extern uint8_t					nfcaDevCnt;
extern rfalIsoDepDevice		IsoDevice;
extern Bool					NfcIsScanning;
extern uint32_t				NfcScannerTimer;
extern Bool					NfcCardDetected;

/* MFDesfire Variable */
extern MFDF_Device_t			MfdfDevice;
extern MFDF_Key_t				Mfdf_DES_Key;
//MFDF_Key_t				Mfdf_DES_NewKey;
extern MFDF_DevInfo_t			Mfdf_DevInfo;

 /* HCE Variable */
extern HCE_Device_t			HCEDevice;
extern HCE_File_ID				HCEAndroidAppID;
extern HCE_File_ID				HCEUserFileID;
extern uint8_t					DataBuf[50];
extern uint8_t					DataBufID[16];
extern uint16_t				DataLen;

/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */
void NFC_init(void);
void NFC_Task(void);

#endif /* NFC_MAIN_H_ */
