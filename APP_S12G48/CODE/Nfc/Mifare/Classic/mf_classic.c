/*
 * mf_classic.c
 *
 *  Created on: 12 Jul 2019
 *      Author: Amir Rabbani
 */


/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */
#include <string.h>
#include <stdlib.h>
#include "mf_classic.h"
#include "rfal_rf.h"
#include "rfal_t2t.h"



/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

/* Mifare Classic Command */
#define MFCL_CMD_AUTHENTICATE_KEY_A								0x60
#define MFCL_CMD_AUTHENTICATE_KEY_B								0x61
#define MFCL_CMD_UID_USAGE										0x40
#define MFCL_CMD_SET_MODULATION_TYPE							0x43
#define MFCL_CMD_READ											0x30
#define MFCL_CMD_WRITE											0xA0
#define MFCL_CMD_DECREMENT										0xC0
#define MFCL_CMD_INCREMENT										0xC1
#define MFCL_CMD_RESTORE										0xC2
#define MFCL_CMD_TRANSFER										0xB0

/* Transceiver Parameter */
#define MFCL_TXRX_FWT											(5*13560U)

#define MFCL_TXRX_FLAGS_DEFAULT                    				((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_AUTO 		|	\
																 (uint32_t)RFAL_TXRX_FLAGS_NFCIP1_OFF  		| 	\
																 (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_KEEP		|	\
																 (uint32_t)RFAL_TXRX_FLAGS_AGC_ON 			| 	\
																 (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_REMV 		| 	\
																 (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_AUTO 		| 	\
																 (uint32_t)RFAL_TXRX_FLAGS_NFCV_FLAG_AUTO)

#define MFCL_ACK                           						0x0A
#define MFCL_INVALID_OPERATION_0             					0x00
#define MFCL_PARITY_CRC_0                      					0x01
#define MFCL_INVALID_OPERATION_4             					0x04
#define MFCL_PARITY_CRC_5                           			0x05
#define MFCL_ACK_LEN                           					1
#define MFCL_ACK_MASK                      						0x0F



/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */


/*
 * *****************************************************************
 * MACRO
 * *****************************************************************
 */

#define MFCL_EXIT_ON_ERR(err, func)								(err) = (func);							\
																if( (err) != MFCL_ERR_NONE )			\
																	return err;							\

#define MFCL_MEM_CPY(dest, src, len)							memcpy(dest, src, len)

#define MFCL_MIN(a,b)											(((a) < (b)) ? (a) : (b))
#define MFCL_MAX(a,b)											(((a) > (b)) ? (a) : (b))

/*
 * *****************************************************************
 * LOCAL VARIABLE
 * *****************************************************************
 */

/*
 * *****************************************************************
 * LOCAL FUNCTION
 * *****************************************************************
 */

static MFCLRetCode_t MFCL_TransceiveTxRxBlocking(uint8_t *txBuf, uint16_t txBufLen, uint8_t *rxBuf, uint16_t rxBufLen, uint16_t *actLen)
{
	ReturnCode err;

	err = rfalTransceiveBlockingTxRx(txBuf, txBufLen, rxBuf, rxBufLen, actLen, MFCL_TXRX_FLAGS_DEFAULT, MFCL_TXRX_FWT);

	/* Mifare Classic 4-Bit acknowledgement handling */
	if( (err == ST_ERR_INCOMPLETE_BYTE) && (*actLen == MFCL_ACK_LEN) )
	{
		switch( (*rxBuf) & MFCL_ACK_MASK )
		{
			case MFCL_ACK: 							{ return MFCL_ERR_NONE; 		}
			case MFCL_INVALID_OPERATION_0:
			case MFCL_INVALID_OPERATION_4:			{ return MFCL_ERR_INVAL_OPER; }
			case MFCL_PARITY_CRC_0:
			case MFCL_PARITY_CRC_5:					{ return MFCL_ERR_CRC_PARITY;	}
			default:								{ return MFCL_ERR_UNKNOWN;	}
		}
	}

	switch( err )
	{
		case ST_ERR_NONE:
		case ST_ERR_CRC:				{ return MFCL_ERR_NONE; 		} //NOTE: Ignore CRC error, mifare classic does NOT send CRC in response.
		case ST_ERR_BUSY: 				{ return MFCL_ERR_BUSY; 		}
		case ST_ERR_TIMEOUT:			{ return MFCL_ERR_TIMEOUT;		}
		case ST_ERR_FRAMING: 			{ return MFCL_ERR_FRAMING; 		}
		case ST_ERR_IO: 				{ return MFCL_ERR_IO; 			}
		case ST_ERR_RF_COLLISION: 		{ return MFCL_ERR_COLLISION; 	}
		default:{}
	}

	return MFCL_ERR_UNKNOWN;
}
/*******************************************************************/
static void MFCL_RNG(uint8_t *buf, uint8_t size)
{
	uint8_t i;

	for(i=0; i<rand();i++)
		rand();

	for(i=0; i<size; i++)
		buf[i] = rand();
}

/*
 * *****************************************************************
 * FUNCTION DECLARATION
 * *****************************************************************
 */
MFCLRetCode_t MFCL_Authenticate(rfalNfcaListenDevice *dev, MFCL_Crypto1_t *crypt, MFCL_Authen_t keyType, uint8_t blockAddr, uint8_t *key)
{
	MFCLRetCode_t 		err;
	uint8_t 			Buf[8];
	uint16_t			RxLen;
	uint8_t				RandA[4];
	uint8_t				RandB[4];


	Buf[0] = (keyType == MFCL_AUTHENTICATE_KEY_A) ? MFCL_CMD_AUTHENTICATE_KEY_A : MFCL_CMD_AUTHENTICATE_KEY_B;
	Buf[1] = blockAddr;

	MFCL_EXIT_ON_ERR( err, MFCL_TransceiveTxRxBlocking(Buf, 2, RandB, sizeof(RandB), &RxLen) );

	if( RxLen != 4 )
		return MFCL_ERR_FRAME_LENGTH;

	MFCL_RNG(RandA, 4); /* Generate 4 Bytes RandA */
	MFCL_MEM_CPY(Buf, 	RandA, 4); /* Copy RandA */
	MFCL_MEM_CPY(Buf+4, RandB, 4); /* Copy RandB */

	if( MFCL_Crypto(crypt, Buf, Buf, 8) == FALSE )
		return MFCL_ERR_CIPHER;

	MFCL_EXIT_ON_ERR( err, MFCL_TransceiveTxRxBlocking(Buf, 8, Buf, sizeof(Buf), &RxLen) );

	if( RxLen != 4 )
		return MFCL_ERR_FRAME_LENGTH;

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_UIDUsage(MFCL_UIDF_t uidf)
{
	MFCLRetCode_t 		err;
	uint8_t 			TxBuf[2];
	uint16_t			RxLen;

	TxBuf[0] = MFCL_CMD_UID_USAGE;
	TxBuf[1] = (uint8_t)uidf;

	MFCL_EXIT_ON_ERR( err, MFCL_TransceiveTxRxBlocking(TxBuf, 2, NULL, 0, &RxLen) );

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_SetModulation(MFCLModType_t Mod)
{
	MFCLRetCode_t 		err;
	uint8_t 			TxBuf[2];
	uint16_t			RxLen;

	TxBuf[0] = MFCL_CMD_SET_MODULATION_TYPE;
	TxBuf[1] = (uint8_t)Mod;

	MFCL_EXIT_ON_ERR( err, MFCL_TransceiveTxRxBlocking(TxBuf, 2, NULL, 0, &RxLen) );

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_Read(uint8_t blockAddr, uint8_t *Buf, uint8_t BufLen)
{
	MFCLRetCode_t 		err;
	uint8_t 			TempBuf[18];
	uint16_t			RxLen;

	if( (Buf == NULL) || (BufLen == 0) )
		return MFCL_ERR_PARAM;

	TempBuf[0] = MFCL_CMD_READ;
	TempBuf[1] = blockAddr;

	/* Send read command and receive data */
	MFCL_EXIT_ON_ERR( err, MFCL_TransceiveTxRxBlocking(TempBuf, 2, TempBuf, sizeof(TempBuf), &RxLen) );

	if( (RxLen-2) != MFCL_BLOCK_SIZE )
		return MFCL_ERR_FRAME_LENGTH;

	MFCL_MEM_CPY(Buf, TempBuf, MFCL_MIN(MFCL_BLOCK_SIZE, BufLen));

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_Write(uint8_t blockAddr, uint8_t *Buf)
{
	MFCLRetCode_t 		err;
	uint8_t 			TxBuf[18];

	TxBuf[0] = MFCL_CMD_WRITE;
	TxBuf[1] = blockAddr;

	/* Send write command and block address*/
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking(TxBuf, 2, NULL, 0, NULL));

	/* Write data of one block*/
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking(Buf, MFCL_BLOCK_SIZE, NULL, 0, NULL));

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_Decrement(uint8_t blockAddr, int32_t data)
{
	MFCLRetCode_t 		err;
	uint8_t 			TxBuf[4];
	uint8_t				*DataTemp = (uint8_t*)data;


	TxBuf[0] = MFCL_CMD_DECREMENT;
	TxBuf[1] = blockAddr;

	/* Send decrement command and block address*/
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking(TxBuf, 2, NULL, 0, NULL));

	MFCL_MEM_CPY(TxBuf, DataTemp, 4);

	/* Write 4-Byte signed data */
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking((uint8_t*)&data, 4, NULL, 0, NULL));

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_Increment(uint8_t blockAddr, int32_t data)
{
	MFCLRetCode_t 		err;
	uint8_t 			TxBuf[4];
	uint8_t				*DataTemp = (uint8_t*)data;


	TxBuf[0] = MFCL_CMD_INCREMENT;
	TxBuf[1] = blockAddr;

	/* Send increment command and block address*/
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking(TxBuf, 2, NULL, 0, NULL));

	MFCL_MEM_CPY(TxBuf, DataTemp, 4);

	/* Write 4-Byte signed data */
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking((uint8_t*)&data, 4, NULL, 0, NULL));

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_Restore(uint8_t blockAddr)
{
	MFCLRetCode_t 		err;
	uint8_t 			TxBuf[4];


	TxBuf[0] = MFCL_CMD_RESTORE;
	TxBuf[1] = blockAddr;

	/* Send restore command and block address*/
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking(TxBuf, 2, NULL, 0, NULL));

	/* Write 4-Byte dummy */
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking(TxBuf, 4, NULL, 0, NULL));

	return MFCL_ERR_NONE;
}
/*******************************************************************/
MFCLRetCode_t MFCL_Transfer(uint8_t blockAddr)
{
	MFCLRetCode_t 		err;
	uint8_t 			TxBuf[4];


	TxBuf[0] = MFCL_CMD_TRANSFER;
	TxBuf[1] = blockAddr;

	/* Send transfer command and block address*/
	MFCL_EXIT_ON_ERR(err, MFCL_TransceiveTxRxBlocking(TxBuf, 2, NULL, 0, NULL));

	return MFCL_ERR_NONE;
}
