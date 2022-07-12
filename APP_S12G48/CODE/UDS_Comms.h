/*
 * Comms.h
 *
 *  Created on: Mar 13, 2013
 *      Author: B34981
 */

#ifndef COMMS_H_
#define COMMS_H_

/**********************************************************************************************
* Includes
**********************************************************************************************/
/*#include "derivative.h"*/
#include "IO_Map.h"
#include "config.h"
/**********************************************************************************************
* Constants
**********************************************************************************************/


/**********************************************************************************************
* Macros
**********************************************************************************************/

/* Configurable macros */

#define BAUD_RATE					 	19200
#define LIN_MAX_DATA_BYTES   		 	0x06
#define CAN_MAX_DATA_BYTES   		 	0x07

/* communication driver macros */

#define FIRST_FRAME				0xB8
#define CONSECUTIVE_FRAME		0x20
#define ERASE_MEMORY_REQUEST	0xB9
#define READ_MEMORY_STATUS		0xBA
#define RESET_REQUEST			0x11
#define READ_BY_ID				0xB2
#define READ_BOOTLOADER_VERSION	0xBB
#define CLEAR_BOOT_FLAG			0XBD
#define SET_BOOT_FLAG			0XBE
#define SID_START_APPLICATION	0x01

#define READ_BY_ID_LIN_P_ID		0x00
#define READ_BY_ID_CAN_P_ID		0x00
#define READ_BY_ID_SERIAL		0x01
#define READ_BY_ID_BOOT_FLAG	0x21
#define READ_BY_ID_BOOT_VER		0x22
#define READ_BY_ID_APP_VER		0x24

#define WRITE_BY_ID				0x2E
#define WRITE_BY_ID_SET_FLAG	0xAA

#ifndef CAN_BOOTLOADER
#define	LIN_FLAG_ADDRESS		IEE1_AREA_START
#define	FUNCTION_ID_ADDRESS_L	(LIN_FLAG_ADDRESS+1)
#else
#define	CAN_FLAG_ADDRESS		IEE1_AREA_START
#define	FUNCTION_ID_ADDRESS_L	(CAN_FLAG_ADDRESS+1)
#endif
#define	FUNCTION_ID_ADDRESS_H	(FUNCTION_ID_ADDRESS_L+1)

#define	VARIANT_ID_ADDRESS		(FUNCTION_ID_ADDRESS_H+1)

#define	SERIAL_NUM_ADDRESS_MSB		(VARIANT_ID_ADDRESS+1)
#define	SERIAL_NUM_ADDRESS_BYTE_3	(SERIAL_NUM_ADDRESS_MSB+1)
#define	SERIAL_NUM_ADDRESS_BYTE_2	(SERIAL_NUM_ADDRESS_BYTE_3+1)
#define	SERIAL_NUM_ADDRESS_LSB		(SERIAL_NUM_ADDRESS_BYTE_2+1)

#define	BOOT_VERSION_ADDRESS		(SERIAL_NUM_ADDRESS_LSB+1)

#define	APP_VERSION_ADDRESS_L		(BOOT_VERSION_ADDRESS+1)
#define	APP_VERSION_ADDRESS_H		(APP_VERSION_ADDRESS_L+1)

#ifndef CAN_BOOTLOADER
#define LIN_JUMP_BL_APP_ADDRESS		(IEE1_AREA_START_11)
#else
#define CAN_JUMP_BL_APP_ADDRESS		(IEE1_AREA_START_11)
#endif

/* Flasher command */
#define BL_CMD_ACK				0xFCU
#define BL_CMD_VERSION			'I'
#define BL_CMD_ERASE_FULL		'e'
#define BL_CMD_PROGRAM			'p'
#define BL_CMD_RUN_APP			'q'

#define ACTIVITY_LED_ENABLE 1
#define ACTIVITY_LED_DDR DDRT_DDRT0
#define ACTIVITY_LED PTT_PTT0
#define LED_ON 1

/* Latch Hardware Status DIDs TODO: Confirm DIDs*/
#ifdef ENABLE_PCL
#define READ_BY_ID_DID_MANREL_EXT	0x50
#define READ_BY_ID_DID_MANREL_INT	0x51
#define READ_BY_ID_DID_CLAWA_HALL	0x52
#define READ_BY_ID_DID_CLAWB_HALL	0x53
#define READ_BY_ID_DID_SUPVOLT		0x54
#define READ_BY_ID_DID_TEMP			0x55
#define READ_BY_ID_DID_CURRENT		0x56
#define READ_BY_ID_DID_HALLSENSORS  0x57
#endif

/**********************************************************************************************
* Types
**********************************************************************************************/
typedef unsigned char   UINT8;  /* unsigned 8 bit definition */
typedef unsigned short  UINT16; /* unsigned 16 bit definition */
typedef unsigned long   UINT32; /* unsigned 32 bit definition */
typedef signed char     INT8;   /* signed 8 bit definition */
typedef short           INT16;  /* signed 16 bit definition */
typedef long int        INT32;  /* signed 32 bit definition */
typedef void(*pt2Func)(void);	/* Pointer to Functions */

#define NULLFUNCPTR	0x00000000	/* Null Function Pointer */

typedef struct{
	UINT8 LIN_break_detect;
	UINT8 LIN_sync_field;
	UINT8 LIN_PID;
	UINT8 LIN_NAD;
	UINT8 LIN_PCI;
	UINT8 LIN_SID;
	UINT8 LIN_TP_Byte_Len;
	UINT8 LIN_TP_Byte_Cnt;
	UINT8 LIN_data[LIN_MAX_DATA_BYTES];
	UINT8 checksum;
}LIN_FRAME;

typedef struct{
	UINT8 CAN_NAD;
	UINT8 CAN_PCI;
	UINT8 CAN_SID;
	UINT8 CAN_TP_Byte_Len;
	UINT8 CAN_TP_Byte_Cnt;
	UINT8 CAN_data[CAN_MAX_DATA_BYTES];
	UINT8 checksum;
}CAN_FRAME;

typedef enum
{
	BL_SM_INIT			= 0	,
	BL_SM_ACK				,
	BL_SM_IDLE				,
	BL_SM_PROGRAM			,
	BL_SM_RUN_APP			,
	BL_SM_LIN_MODE			,
	BL_SM_CAN_MODE			,
	BL_SM_UNKNOWN
}BL_SM_t;

enum ACK_OPTS
{
	INIT = 0x00,
	/*DEFAULT = 0x01,*/
	OK = 0x41,				/* 'A' means data was received OK */
	CRC_Error = 0x45		/* 'E' means that the prhase has an error, expect retransmission */
};

typedef enum
{
	DIAG_IDLE			= 0	,
	DIAG_REQ_RECEIVED			,
	DIAG_REQ_PROCESSING			,
	DIAG_RESPONSE_READY		,
	DIAG_RESPONSE_SENT			
}DIAG_STATUS_t;

typedef enum
{
	READY			= 0	,
	IN_PROGRESS			
}DIAG_REQUEST_STATUS_t;

typedef enum
{
	COMM_SERIAL			= 0	,
	COMM_LIN			= 1 ,
	COMM_CAN
}COMM_MODE_t;

/*
0x01 Erasing memory (internal state, should not be present)
0x02 Erased
0x04 Programming in progress
0x08 Successfully programmed
0x10 Empty, bootloader started with already erased memory
0x20 New application code rejected due to compatibility issue
0x80 Corrupted, erasing or programming operation failed
0xF0 Untouched, application code is ready to run
*/

typedef enum
{
	ERASING			= 0x01	,
	ERASED			= 0x02	,
	PROGRAMMING		= 0x04	,
	PROGRAMMED		= 0x08	,
	EMPTY			= 0x10	,
	APP_REJECTED	= 0x20	,
	CORRUPTED		= 0x80	,
	UNTOUCHED		= 0xF0	
}MEMORY_STATUS_t;

typedef void(*AppEntry_t)(void);

typedef union
{
	uint8_t Repository[2/*BL_CFG_DATA_REPOSITORY_SIZE*/];
	
	struct
	{
		AppEntry_t entryPoint;
	}ApplicationInfo;
}BL_Repository_t;

/**********************************************************************************************
* Variables
**********************************************************************************************/


/**********************************************************************************************
* Global Variables
**********************************************************************************************/


/**********************************************************************************************
* Global Functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief    Comms_Init - Initialize the serial communications driver
* @param    none
* @return   none
*
************************************************************************************************/
void UDS_Comms_Init(void);

/***********************************************************************************************
*
* @brief    Comms_Rx_Data - Poll or Interrupt that receives serial data.
* @param    none
* @return   none (Data must fill the BP structure, when BP is fill, update u8BootPhraseRcvd)
*
************************************************************************************************/ 
void UDS_Comms_CAN(void);

/***********************************************************************************************
*
* @brief    Comms_Rx_Data - Poll or Interrupt that receives serial data.
* @param    none
* @return   none (Data must fill the BP structure, when BP is fill, update u8BootPhraseRcvd)
*
************************************************************************************************/ 
unsigned char UDS_Comms_Rx_Data_Polling(void);

void UDS_Diagnostics_Handler(void);

void UDS_process_request(void);

void UDS_process_response(void);

void BL_Prog_Handler(void);

void STM_Bootloader(void);

/* Auxiliary Methods */
byte IO_readByte(byte *Chr);
byte IO_writeByte(byte Chr);
uint8_t bcdToBinary(uint8_t bcd);

#endif /* COMMS_H_ */
