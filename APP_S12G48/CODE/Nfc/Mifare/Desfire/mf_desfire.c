/*
 * mf_desfire.c
 *
 *  Created on: Jun 28, 2019
 *      Author: Amir Rabbani
 *      Modifier: Cristian Castro
 */


/*
 * APDU frame based on ISO/IEC 7816-4
 * 
 *\\\\\\\\Command Message
 * *****************************************************************************************************************************************************************************
 * | CLA (1 uint8_t) | CMD (1 uint8_t) | Param One (1 uint8_t) | Param Two (1 uint8_t) | Lc (1 uint8_t Command Data Length) | Command Data (0~255 uint8_t) | Le(1 uint8_t Expected Response Length) |
 * *****************************************************************************************************************************************************************************
 * 
 * \\\\\\\Response Message
 * *********************************
 * | Response Data | SW (2 Bytes) |
 * *********************************
 */

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */

#include "mf_desfire.h"
#include "rfal_user.h"
#include <stdlib.h>
#include <stdio.h>
#include "platform.h"
#include "rfal_crc.h"


/*
 * *****************************************************************
 * DEFINE
 * *****************************************************************
 */

/* ISODep Mifare wrapping */

/* Mifare wrapping APDU header */
#define MFDF_APDU_WRAP_CLA									0x90U
#define MFDF_APDU_WRAP_PARAM_ONE							0x00U
#define MFDF_APDU_WRAP_PARAM_TWO							0x00U

/* Mifare status word (SW) */
#define MFDF_APDU_WRAP_RESP_SW_ONE							0x91U

#define MFDF_RESP_OPER_OK									0x00U
#define MFDF_RESP_NO_CHANGES								0x0CU
#define MFDF_RESP_OUTOFMEMORY_ERR							0x0EU
#define MFDF_RESP_ILLEGAL_CMD								0x1CU
#define MFDF_RESP_INTEGRITY_ERR								0x1EU
#define MFDF_RESP_NO_SUCH_KEY								0x40U
#define MFDF_RESP_LENGTH_ERR								0x7EU
#define MFDF_RESP_PERMIT_DENIED								0x9DU
#define MFDF_RESP_PARAM_ERR									0x9EU
#define MFDF_RESP_APP_NOT_FOUND								0xA0U
#define MFDF_RESP_APP_INTGERITY_ERR							0xA1U
#define MFDF_RESP_AUTHEN_ERR								0xAEU
#define MFDF_RESP_ADDITIONAL_FRAME							0xAFU
#define MFDF_RESP_BOUNDARY_ERR								0xBEU
#define MFDF_RESP_PICC_INTEGRITY_ERR						0xC1U
#define MFDF_RESP_CMD_ABORT									0xCAU
#define MFDF_RESP_PICC_DISABLED_ERR							0xCDU
#define MFDF_RESP_COUNT_ERR									0xCEU
#define MFDF_RESP_DUPLICATE_ERR								0xDEU
#define MFDF_RESP_MEMORY_ERR								0xEEU
#define MFDF_RESP_FILE_NOT_FOUND							0xF0U
#define MFDF_RESP_FILE_INTEGRITY_ERR						0xF1U

/* Additional frame request command */
#define MFDF_CMD_NEXT_FRAME									MFDF_RESP_ADDITIONAL_FRAME

/* Mifare security level command */
#define MFDF_CMD_SEC_AUTHENICATE							0x0AU
#define MFDF_CMD_SEC_AUTHENICATE_ISO						0x1AU
#define MFDF_CMD_SEC_AUTHENICATE_AES						0xAAU
#define MFDF_CMD_SEC_GET_KEY_VERSION						0x64U
#define MFDF_CMD_SEC_GET_KEY_SETTING						0x45U
#define MFDF_CMD_SEC_CHANGE_KEY_SETTING						0x54U
#define MFDF_CMD_SEC_CHANGE_KEY								0xC4U

/* Mifare PICC level command */
#define MFDF_CMD_PIC_CREATE_APP								0xCAU
#define MFDF_CMD_PIC_DELETE_APP								0xDAU
#define MFDF_CMD_PIC_GET_APP_ID								0x6AU
#define MFDF_CMD_PIC_GET_DF_NAME							0x6DU
#define MFDF_CMD_PIC_SELECT_APP								0x5AU
#define MFDF_CMD_PIC_FORMAT_PICC							0xFCU
#define MFDF_CMD_PIC_GET_VERSION							0x60U
#define MFDF_CMD_PIC_FREE_MEM								0x6EU
#define MFDF_CMD_PIC_SET_CONFIGURATION						0x5CU
#define MFDF_CMD_PIC_GET_CARD_UID							0x51U

/* Mifare application level command */
#define MFDF_CMD_APP_GET_FILE_ID							0x6FU
#define MFDF_CMD_APP_GET_ISO_FILE_ID						0x61U
#define MFDF_CMD_APP_GET_FILE_SETTING						0xF5U
#define MFDF_CMD_APP_CHANGE_FILE_SETTING					0x5FU
#define MFDF_CMD_APP_CREATE_STD_DAT_FILE					0xCDU
#define MFDF_CMD_APP_CREATE_BKUP_DAT_FILE					0xCBU
#define MFDF_CMD_APP_CREATE_VAL_FILE						0xCCU
#define MFDF_CMD_APP_CREATE_LINEAR_REC_FILE					0xC1U
#define MFDF_CMD_APP_CREATE_CYCLIC_REC_FILE					0xC0U
#define MFDF_CMD_APP_DELETE_FILE							0xDFU

/* Mifare data manipulation level command */
#define MFDF_CMD_DAT_READ_DATA								0xBDU
#define MFDF_CMD_DAT_WRITE_DATA								0x3DU
#define MFDF_CMD_DAT_GET_VALUE								0x6CU
#define MFDF_CMD_DAT_CREDIT									0x0CU
#define MFDF_CMD_DAT_LIMITED_CREDIT							0x1CU
#define MFDF_CMD_DAT_DEBIT									0xDCU
#define MFDF_CMD_DAT_WRITE_RECORD							0x3BU
#define MFDF_CMD_DAT_READ_RECORD							0xBBU
#define MFDF_CMD_DAT_CLEAR_RECORD							0xEBU
#define MFDF_CMD_DAT_COMMIT_TRANSACTION						0xC7U
#define MFDF_CMD_DAT_ABORT_TRANSACTION						0xA7U


#define MFDF_KEY_NUMBER_MASK								0x0FU
#define MFDF_MAX_APPLICATION_ID								28U
#define MFDF_APPLICATION_ID_MASK							0xFFFFFFU

#define MFDF_PICC_MK_SETTING								0x0FU
#define MFDF_APP_MK_SETTING									0xFFU

#define MFDF_MAX_FILE_ID									15U
#define MFDF_MAX_FILE_SIZE									0xFFFFFFU

#define MFDF_FILE_OFFSET_MASK								0xFFFFFFU
#define MFDF_FILE_LENGTH_MASK								MFDF_FILE_OFFSET_MASK
#define MFDF_FILE_READ_MAX_DATA								59U
#define MFDF_FILE_WRITE_MAX_DATA							59U

#define MFDF_KEY_TYPE_MASK									0xC0
#define MFDF_KEY_TYPE_16TDES								0x00
#define MFDF_KEY_TYPE_24TDES								0x40
#define MFDF_KEY_TYPE_AES									0x80

#define MFDF_CRC_INITIAL									((uint16_t)0x6363)


/*
 * *****************************************************************
 * LOCAL MACRO
 * *****************************************************************
 */

#define MFDF_EXIT_ON_ERR(err, func)								(err) = (func);																	\
																if( !(((err) == MFDF_ERR_NONE) || ((err) == MFDF_ERR_NONE_MULTIFRAME)) )	\
																{																				\
																	return (err);																\
																}																				\

#define MFDF_MAKE_ACCESS_RIGHT(word, rd, wr, rw, ch)			word  = (((uint16_t)rd) << 12);			\
																word |= (((uint16_t)wr) << 8);			\
																word |= (((uint16_t)rw) << 4);			\
																word |= (((uint16_t)ch) << 0);

#define MFDF_SET_KEY_TYPE(key, type)							((key & ~MFDF_KEY_TYPE_MASK) | type)
#define MFDF_GET_KEY_TYPE(key)									(key & MFDF_KEY_TYPE_MASK)
#define MFDF_IS_KEY_16TDES(key)									((MFDF_GET_KEY_TYPE(key) ^ MFDF_KEY_TYPE_16TDES) ? FALSE : TRUE)
#define MFDF_IS_KEY_24TDES(key)									((MFDF_GET_KEY_TYPE(key) ^ MFDF_KEY_TYPE_24TDES) ? FALSE : TRUE)
#define MFDF_IS_KEY_AES(key)									((MFDF_GET_KEY_TYPE(key) ^ MFDF_KEY_TYPE_AES) 	 ? FALSE : TRUE)

#define MFDF_MEMCPY(dst,src,len)								memcpy(dst,src,len)
#define MFDF_MEMCMP(dst,src,len)								memcmp(dst,src,len)
#define MFDF_MEMSET(dst,src,len)								memset(dst,src,len)

#define MFDF_CRC(buf,len)										rfalCrcCalculateCcitt(MFDF_CRC_INITIAL,buf,len)

/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */


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
static MFDF_RetCode_t MFDF_Transceive(MFDF_Device_t *dev, uint8_t Cmd, uint8_t *TxBuf, uint16_t TxLen, uint8_t *RxBuf, uint16_t RxBufSize, uint16_t *RxActSize)
{
	ReturnCode	err;
	uint16_t	BufLen;
	uint16_t 	RxLen;
	uint8_t		Resp;
	
#if MFDF_ISO_WRAP_MODE

	BufLen = 0;
	dev->APDU_Buf[BufLen++] = MFDF_APDU_WRAP_CLA;
	dev->APDU_Buf[BufLen++] = Cmd;
	dev->APDU_Buf[BufLen++] = MFDF_APDU_WRAP_PARAM_ONE;
	dev->APDU_Buf[BufLen++] = MFDF_APDU_WRAP_PARAM_TWO;
	
	if( (TxLen > 0) && (TxBuf != NULL) )
	{
		dev->APDU_Buf[BufLen++] = TxLen;	//APDU Lc
		ST_MEMCPY(&dev->APDU_Buf[BufLen], TxBuf, TxLen); //Copy APDU payload
		BufLen += TxLen;
	}
	
	dev->APDU_Buf[BufLen++] = 0; //APDU Le=0 : Any Rx length is allowed
	
	err = rfal_IsoDepBlockingTxRx(dev->hIsoDevice, dev->APDU_Buf, BufLen, dev->APDU_Buf, sizeof(dev->APDU_Buf), &RxLen);

	if( ( err != ST_ERR_NONE) || (RxLen < 2) || (dev->APDU_Buf[RxLen-2] != MFDF_APDU_WRAP_RESP_SW_ONE) )
	{
		return MFDF_ERR_ISODEP_TXRX;
	}
	
	if( (RxLen > 2) && (RxBuf != NULL) )
	{
		ST_MEMCPY(RxBuf, dev->APDU_Buf, MIN(RxBufSize, RxLen-2));
	}
	
	if(RxActSize != NULL)
	{
		*RxActSize = RxLen-2;
	}
	
	Resp = dev->APDU_Buf[RxLen-1];

#else

	BufLen = 0;
	dev->APDU_Buf[BufLen++] = Cmd;

	if( (TxLen > 0) && (TxBuf != NULL) )
	{
		ST_MEMCPY(&dev->APDU_Buf[BufLen], TxBuf, TxLen);
		BufLen += TxLen;
	}

	err = rfal_IsoDepBlockingTxRx(dev->hIsoDevice, dev->APDU_Buf, BufLen, dev->APDU_Buf, MFDF_GENERAL_BUFFER_SIZE, &RxLen);

	if( (err != ST_ERR_NONE) || (RxLen < 1) )
	{
		return MFDF_ERR_ISODEP_TXRX;
	}

	if( (RxLen > 1) && (RxBuf != NULL) )
	{
		ST_MEMCPY(RxBuf, dev->APDU_Buf+1, MIN(RxBufSize, RxLen-1));
	}

	if(RxActSize != NULL)
	{
		*RxActSize = RxLen-1;
	}

	Resp = dev->APDU_Buf[0];

#endif

	switch(Resp)
	{
		case MFDF_RESP_OPER_OK: {return MFDF_ERR_NONE;}
		case MFDF_RESP_ADDITIONAL_FRAME: {return MFDF_ERR_NONE_MULTIFRAME;}
		case MFDF_RESP_NO_CHANGES: {return MFDF_ERR_NO_CHANGE;}
		case MFDF_RESP_OUTOFMEMORY_ERR: {return MFDF_ERR_OUT_OF_MEMORY;}
		case MFDF_RESP_ILLEGAL_CMD: {return MFDF_ERR_ILEGAL_CMD;}
		case MFDF_RESP_INTEGRITY_ERR: {return MFDF_ERR_INTEGRITY;}
		case MFDF_RESP_NO_SUCH_KEY: {return MFDF_ERR_NO_SUCH_KEY;}
		case MFDF_RESP_LENGTH_ERR: {return MFDF_ERR_LENGTH;}
		case MFDF_RESP_PERMIT_DENIED: {return MFDF_ERR_PERMIT_DENIED;}
		case MFDF_RESP_PARAM_ERR: {return MFDF_ERR_PARAM_ERR;}
		case MFDF_RESP_APP_NOT_FOUND: {return MFDF_ERR_APP_NOT_FOUND;}
		case MFDF_RESP_APP_INTGERITY_ERR: {return MFDF_ERR_APP_INTGERITY;}
		case MFDF_RESP_AUTHEN_ERR: {return MFDF_ERR_AUTHENTICATION;}
		case MFDF_RESP_PICC_INTEGRITY_ERR: {return MFDF_ERR_PICC_INTEGRITY;}
		case MFDF_RESP_CMD_ABORT: {return MFDF_ERR_CMD_ABORT;}
		case MFDF_RESP_PICC_DISABLED_ERR: {return MFDF_ERR_PICC_DISABLED;}
		case MFDF_RESP_MEMORY_ERR: {return MFDF_ERR_MEMORY_FAIL;}
		case MFDF_RESP_FILE_NOT_FOUND: {return MFDF_ERR_FILE_NOT_FOUND;}
		case MFDF_RESP_FILE_INTEGRITY_ERR: {return MFDF_ERR_FILE_INTEGRITY;}
		case MFDF_RESP_DUPLICATE_ERR: { return MFDF_ERR_DUPLICATION; }
		case MFDF_RESP_BOUNDARY_ERR: { return MFDF_ERR_BOUNDRY; }
		default:{}
	}
		
	return MFDF_ERR_UNKNOWN;
}
/*******************************************************************/
static void MFDF_PRNG(uint8_t *num, uint8_t size)
{
	uint16_t i;
	
	/*
	 * X(n+1) = (aX(n)+b) % m
	 */

	/* Change random seed using systick() to have better normalisation */
	srand((unsigned int)MFDF_GET_SYSTICK());

	/* Dummy PRN to normalise final PRN */
	for(i=0; i< 377; i++)
	{
		rand();
	}

	for(i=0; i< size; i++)
	{
		num[i] = (uint8_t)rand();
	}
	
}
/*******************************************************************/

/*
 * *****************************************************************
 * FUNCTION DECLARATION
 * *****************************************************************
 */
char* MFDF_StringReturnCode(MFDF_RetCode_t retCode)
{
	
	switch(retCode)
	{
		case MFDF_ERR_NONE				: 	return "MFDF_ERR_NONE";				
		case MFDF_ERR_NONE_MULTIFRAME	:	return "MFDF_ERR_NONE_MULTIFRAME";  
		case MFDF_ERR_INVAL_PARAM		:   return "MFDF_ERR_INVAL_PARAM";      
		case MFDF_ERR_NONE_MIFARE		:   return "MFDF_ERR_NONE_MIFARE";       
		case MFDF_ERR_DEV_INCOMP_DEVICE	:   return "MFDF_ERR_DEV_INCOMP_DEVICE";
		case MFDF_ERR_ISODEP_TXRX		:   return "MFDF_ERR_ISODEP_TXRX";      
		case MFDF_ERR_FRAME_FAIL		:   return "MFDF_ERR_FRAME_FAIL";       
		case MFDF_ERR_DES_KEY_PARITY	:   return "MFDF_ERR_DES_KEY_PARITY";   
		case MFDF_ERR_DES_INTERNAL		:   return "MFDF_ERR_DES_INTERNAL";     
		case MFDF_ERR_NO_CHANGE			:   return "MFDF_ERR_NO_CHANGE";        
		case MFDF_ERR_OUT_OF_MEMORY		:   return "MFDF_ERR_OUT_OF_MEMORY";    
		case MFDF_ERR_ILEGAL_CMD        :   return "MFDF_ERR_ILEGAL_CMD";                 
		case MFDF_ERR_INTEGRITY         :   return "MFDF_ERR_INTEGRITY";                 
		case MFDF_ERR_NO_SUCH_KEY       :   return "MFDF_ERR_NO_SUCH_KEY";                 
		case MFDF_ERR_LENGTH            :   return "MFDF_ERR_LENGTH";                 
		case MFDF_ERR_PERMIT_DENIED     :   return "MFDF_ERR_PERMIT_DENIED";                 
		case MFDF_ERR_PARAM_ERR         :   return "MFDF_ERR_PARAM_ERR";                 
		case MFDF_ERR_APP_NOT_FOUND     :   return "MFDF_ERR_APP_NOT_FOUND";                 
		case MFDF_ERR_APP_INTGERITY     :   return "MFDF_ERR_APP_INTGERITY";                 
		case MFDF_ERR_AUTHENTICATION    :   return "MFDF_ERR_AUTHENTICATION";                 
		case MFDF_ERR_BOUNDARY          :   return "MFDF_ERR_BOUNDARY";                 
		case MFDF_ERR_PICC_INTEGRITY    :   return "MFDF_ERR_PICC_INTEGRITY";                
		case MFDF_ERR_CMD_ABORT         :   return "MFDF_ERR_CMD_ABORT";                 
		case MFDF_ERR_PICC_DISABLED     :   return "MFDF_ERR_PICC_DISABLED";                 
		case MFDF_ERR_MEMORY_FAIL		:   return "MFDF_ERR_MEMORY_FAIL";      
		case MFDF_ERR_FILE_NOT_FOUND    :   return "MFDF_ERR_FILE_NOT_FOUND";                 
		case MFDF_ERR_FILE_INTEGRITY	:   return "MFDF_ERR_FILE_INTEGRITY";   
		case MFDF_ERR_CIPHER			:	return "MFDF_ERR_CIPHER";           
		case MFDF_ERR_PARAM_RANGE		:   return "MFDF_ERR_PARAM_RANGE";      
		case MFDF_ERR_DUPLICATION		:   return "MFDF_ERR_DUPLICATION";      
		case MFDF_ERR_FILEID_RANGE		:   return "MFDF_ERR_FILEID_RANGE";     
		case MFDF_ERR_FILE_SIZE_RANGE	:   return "MFDF_ERR_FILE_SIZE_RANGE";  
		case MFDF_ERR_BOUNDRY			:   return "MFDF_ERR_BOUNDRY";          
		case MFDF_ERR_UNKNOWN           :   return "MFDF_ERR_UNKNOWN";          
		default:{}
	}
	
	return NULL;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_DeviceInit(MFDF_Device_t *dev, rfalNfcaListenDevice *NfcaDev, rfalIsoDepDevice *hIsoDev)
{
	if( (dev == NULL) || (NfcaDev == NULL) || (hIsoDev == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}

	/* Check device is Mifare by vendor ID: first byte of UID */
	else if( NfcaDev->nfcId1[0] != MFDF_PHILIPS_VENDOR_ID )
	{
		return MFDF_ERR_NONE_MIFARE;
	}
	
	else
	{
		/* Do nothing */
	}

	dev->NfcaDev 		= NfcaDev;
	dev->hIsoDevice 	= hIsoDev;

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_Authenticate(MFDF_Device_t *dev, MFDF_AuthMode_t AuthMode, uint8_t KeyNumber, MFDF_Key_t *Key, MFDF_KeyType_t keyType)
{
	MFDF_RetCode_t 		err;
	uint8_t				Buf[32];
	uint8_t				CipherOut[16];
	uint8_t				TxLen;
	uint16_t			RxLen;
	uint8_t				RandA[16];
	uint8_t				RandB[16];
	uint8_t				keySize;


	if( (dev == NULL) || (Key == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}

	else if( KeyNumber > MFDF_MAX_KEY_PER_APP )
	{
		return MFDF_ERR_PARAM_RANGE;
	}
	
	else
	{
		/* Do nothing */
	}
	
	/* Check supported key type */
	switch(keyType)
	{
		case MFDF_KEY_DES_1K3DES: keySize = 8;  break;
		case MFDF_KEY_DES_2K3DES: keySize = 16; break;
		case MFDF_KEY_DES_3K3DES: keySize = 24; break;
		case MFDF_KEY_AES: 		  keySize = 16; break;
		default: return MFDF_ERR_INVAL_PARAM;
	}
	
	/* Save last authenticated key number */
	dev->LastKeyNumber = KeyNumber;
	
	/* DES Native Authentication Mode */
	if( (AuthMode == MFDF_AUTHENTICATION_DES_NATIVE) || (AuthMode == MFDF_AUTHENTICATION_DES_STANDARD) )
	{		
		/* Initialise DES Cipher engine */
		if( DES_Init(&dev->Cipher.DES_Cipher) != DES_OK )
		{
				return MFDF_ERR_CIPHER;
		}
		
		/* DES Set key for Cipher and Decipher */
		if( DES_SetKeyData(&dev->Cipher.DES_Cipher, (uint8_t*)Key, keySize, dev->KeyVersion) != DES_OK )
		{
			return MFDF_ERR_CIPHER;
		}

		//Key index
		TxLen = 0;
		Buf[TxLen++] = KeyNumber;

		//Send command and get the first frame
		if(AuthMode == MFDF_AUTHENTICATION_DES_NATIVE)
		{
			MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_SEC_AUTHENICATE, Buf, TxLen, Buf, sizeof(Buf), &RxLen) );
		}
		
		if(AuthMode == MFDF_AUTHENTICATION_DES_STANDARD)
		{
			MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_SEC_AUTHENICATE_ISO, Buf, TxLen, Buf, sizeof(Buf), &RxLen) );
		}

		if( RxLen != DES_BLOCK_SIZE )
		{
			return MFDF_ERR_FRAME_FAIL;
		}

		/* DES Decrypt RandB in CBC_RECEIVE mode, it's common for both native and standard DES */		
		DES_ClearIV(&dev->Cipher.DES_Cipher);
		if( DES_CryptDataCBC(&dev->Cipher.DES_Cipher, DES_CBC_RECEIVE, DES_KEY_DECIPHER, RxLen, Buf, RandB) != DES_OK )
		{
			return MFDF_ERR_CIPHER;
		}
			
		/* Generate RandA */
		MFDF_MEMSET(RandA, 0, 16);
		MFDF_PRNG(RandA, DES_BLOCK_SIZE);

		/* Rotate 8 bits random B to left, make RndA+RanB' */
		MFDF_MEMCPY(Buf, RandA, DES_BLOCK_SIZE);
		MFDF_MEMCPY(Buf+DES_BLOCK_SIZE, RandB+1, DES_BLOCK_SIZE-1);
		Buf[(2*DES_BLOCK_SIZE)-1] = RandB[0];

			
		if( AuthMode == MFDF_AUTHENTICATION_DES_NATIVE )
		{
			/* DES Decrypt of RandA+RandB' in CBC_SEND mode*/
			if( DES_ClearIV(&dev->Cipher.DES_Cipher) != DES_OK )
			{
				return MFDF_ERR_CIPHER;
			}
			
			else if( DES_CryptDataCBC(&dev->Cipher.DES_Cipher, DES_CBC_SEND, DES_KEY_DECIPHER, 2*DES_BLOCK_SIZE, Buf, CipherOut) != DES_OK )
			{
				return MFDF_ERR_CIPHER;
			}
			
			else
			{
				/* Do nothing */
			}
		}
		else
		{
			/* DES Encrypt of RandA+RandB' */			
			if( DES_CryptDataCBC(&dev->Cipher.DES_Cipher, DES_CBC_SEND, DES_KEY_DECIPHER, 2*DES_BLOCK_SIZE, Buf, CipherOut) != DES_OK )
			{
				return MFDF_ERR_CIPHER;
			}
		}
			
		/* Send dec(RandA+RandB') */
		MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_NEXT_FRAME, CipherOut, 2*DES_BLOCK_SIZE, Buf, sizeof(Buf), &RxLen));

		/* DES Decrypt of RandA' in CBC_RECEIVE mode*/
		if( AuthMode == MFDF_AUTHENTICATION_DES_NATIVE )
		{
			DES_ClearIV(&dev->Cipher.DES_Cipher);
		}
			
		if( DES_CryptDataCBC(&dev->Cipher.DES_Cipher, DES_CBC_RECEIVE, DES_KEY_DECIPHER, DES_BLOCK_SIZE, Buf, CipherOut) != DES_OK )
		{
			return MFDF_ERR_CIPHER;
		}

		else if( RxLen != DES_BLOCK_SIZE )
		{
			return MFDF_ERR_FRAME_FAIL;
		}
		
		else
		{
			/* Do nothing */
		}
			
		/* Rotate 8 bits random A to right, make RndA */
		Buf[0] = CipherOut[DES_BLOCK_SIZE-1];
		MFDF_MEMCPY(Buf+1, CipherOut, DES_BLOCK_SIZE-1);
						
		if( MFDF_MEMCMP(RandA, Buf, DES_BLOCK_SIZE) != 0)
		{
			return MFDF_ERR_AUTHENTICATION;
		}
		
		/* Clear IV for next session */
		DES_ClearIV(&dev->Cipher.DES_Cipher);
		
		/* Generate Session Key */
		MFDF_MEMSET(dev->SessionKey, 0, MFDF_KEY_LENGTH);
		
		dev->SessionKeySize = 0;
		
		switch(dev->Cipher.DES_Cipher.ms32_KeySize)
		{
			case 8: /* Single DES Session Key */
		
				MFDF_MEMCPY(dev->SessionKey,    RandA, 4);
				MFDF_MEMCPY(dev->SessionKey+4,  RandB, 4);
				dev->SessionKeySize = 8;
				return MFDF_ERR_NONE;
				
			case 16: /* Single 2KTDES Session Key */
					
				MFDF_MEMCPY(dev->SessionKey,    RandA,   4);
				MFDF_MEMCPY(dev->SessionKey+4,  RandB,   4);
				MFDF_MEMCPY(dev->SessionKey+8,  RandA+4, 4);
				MFDF_MEMCPY(dev->SessionKey+12, RandB+4, 4);
				dev->SessionKeySize = 16;
				return MFDF_ERR_NONE;
				
			case 24: /* Single 3KTDES Session Key */
					
				MFDF_MEMCPY(dev->SessionKey,    RandA,    4);
				MFDF_MEMCPY(dev->SessionKey+4,  RandB, 	  4);
				MFDF_MEMCPY(dev->SessionKey+8,  RandA+6,  4);
				MFDF_MEMCPY(dev->SessionKey+12, RandB+6,  4);
				MFDF_MEMCPY(dev->SessionKey+16, RandA+12, 4);
				MFDF_MEMCPY(dev->SessionKey+20, RandB+12, 4);
				dev->SessionKeySize = 24;
				return MFDF_ERR_NONE;
			
			default:
				return MFDF_ERR_AUTHENTICATION;
		}
	}
	
	/* AES Authentication Mode */
	else if( AuthMode == MFDF_AUTHENTICATION_AES )
	{
		/* Not implemented yet */
		
		/* Do nothing */
#ifndef DISABLE_AES		
		/* Initialise AES cipher engine */
		if(AES_Init(&dev->Cipher.AES_Cipher) != AES_ERR_OK)
		{
			return MFDF_ERR_CIPHER;
		}
		
		/* Set AES key value */
		if(AES_SetKeyData(&dev->Cipher.AES_Cipher, (uint8_t*)Key, keySize) != AES_ERR_OK)
		{
			return MFDF_ERR_CIPHER;
		}
		
		//Key index
		TxLen = 0;
		Buf[TxLen++] = KeyNumber;

		/* Send command and get the first frame */
		MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_SEC_AUTHENICATE_AES, Buf, TxLen, Buf, sizeof(Buf), &RxLen) );
		
		if( RxLen != AES_BLOCK_SIZE )
		{
			return MFDF_ERR_FRAME_FAIL;
		}
		
		/* Decrypt RanB*/
		if(AES_CryptDataCBC(&dev->Cipher.AES_Cipher, AES_KEY_DECIPHER, RxLen, Buf, RandB) != AES_ERR_OK)
		{
			return MFDF_ERR_CIPHER;
		}
		
		/* Generate RandA */
		MFDF_PRNG(RandA, AES_BLOCK_SIZE);
		
		/* Copy random A to buffer, concatenate to RanB' */	
		MFDF_MEMCPY(Buf, RandA, AES_BLOCK_SIZE);
		Buf[AES_BLOCK_SIZE] = RandB[1];
		MFDF_MEMCPY(Buf+AES_BLOCK_SIZE+1, RandB, AES_BLOCK_SIZE-1);
		
		/* Encrypt RanA+RanB' */
		if(AES_CryptDataCBC(&dev->Cipher.AES_Cipher, AES_KEY_ENCIPHER, 2*AES_BLOCK_SIZE, Buf, CipherOut) != AES_ERR_OK)
		{
			return MFDF_ERR_CIPHER;
		}
		
		/* Send dec(RandA+RandB') */
		MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_NEXT_FRAME, CipherOut, 2*AES_BLOCK_SIZE, Buf, sizeof(Buf), &RxLen));
		
		if( RxLen != AES_BLOCK_SIZE )
		{
			return MFDF_ERR_FRAME_FAIL;
		}
		
		/* Decrypt RanA */
		if(AES_CryptDataCBC(&dev->Cipher.AES_Cipher, AES_KEY_DECIPHER, AES_BLOCK_SIZE, Buf, CipherOut) != AES_ERR_OK)
		{
			return MFDF_ERR_CIPHER;
		}
		
		/* Rotate 8 bits random A to right, make RndA */
		Buf[0] = CipherOut[AES_BLOCK_SIZE-1];
		MFDF_MEMCPY(Buf+1, CipherOut, AES_BLOCK_SIZE-1);
						
		if( MFDF_MEMCMP(RandA, Buf, AES_BLOCK_SIZE) != 0)
		{
			return MFDF_ERR_AUTHENTICATION;
		}
#endif		
	}

	return MFDF_ERR_PARAM_RANGE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_ChangeKeySetting(MFDF_Device_t *dev, MFDF_KeySetting_t keySetting, MFDF_Key_t Key)
{
	if( dev == NULL)
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_GetKeySetting(MFDF_Device_t *dev, uint8_t *keySetting, uint8_t *maxNumKey, MFDF_KeyType_t *keyType)
{
	MFDF_RetCode_t 		err;
	uint8_t				Buf[32];
	uint16_t			RxLen;
	
	if(dev == NULL)
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_SEC_GET_KEY_SETTING, NULL, 0, Buf, sizeof(Buf), &RxLen));
	
	if( RxLen != 2 )
	{
		return MFDF_ERR_LENGTH;
	}
	
	if(keySetting != NULL)
	{
		*keySetting = Buf[0];
	}
	
	if(maxNumKey != NULL)
	{
		*maxNumKey = Buf[1] & MFDF_KEY_NUMBER_MASK;
	}
	
	if(keyType != NULL)
	{
		switch(MFDF_GET_KEY_TYPE(Buf[1]))
		{
			case MFDF_KEY_TYPE_16TDES: *keyType = MFDF_KEY_DES_2K3DES; break;
			case MFDF_KEY_TYPE_24TDES: *keyType = MFDF_KEY_DES_3K3DES; break;
			case MFDF_KEY_TYPE_AES:    *keyType = MFDF_KEY_AES; 	   break;
			
			default: return MFDF_ERR_INVAL_PARAM;
		}
	}

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_ChangeKey(MFDF_Device_t *dev, MFDF_KeyType_t KeyType, uint8_t KeyNumber, MFDF_Key_t Key, MFDF_Key_t NewKey)
{
	MFDF_RetCode_t 		err;
	uint8_t				Buf[42];
	uint8_t				CipherBuf[42];
	uint16_t			TxLen;
	uint16_t			crc;
	uint8_t				KeySize;
	uint8_t				i;
	
	if(dev == NULL)
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	else if(KeyNumber >= MFDF_MAX_KEY_PER_APP)
	{
		return MFDF_ERR_PARAM_RANGE;
	}
	
	else
	{
		/* Do nothing */
	}
	
	MFDF_MEMSET(Buf, 0, sizeof(Buf));
	MFDF_MEMSET(CipherBuf, 0, sizeof(CipherBuf));
	
	switch(KeyType)
	{
		case MFDF_KEY_DES_2K3DES:
		case MFDF_KEY_DES_3K3DES:
			
			/* Set proper key size */
			KeySize = (KeyType == MFDF_KEY_DES_2K3DES) ? 16 : 24;		
			
			/* Check if the new key number is the same as the authenticated key number */
			if( dev->LastKeyNumber == KeyNumber )
			{
				/* Key numbers are same (CASE 2) */
				
				MFDF_MEMCPY(Buf, NewKey, KeySize);
				TxLen = (uint16_t)KeySize;
				
				crc = MFDF_CRC(NewKey, KeySize);

				Buf[TxLen++] = (uint8_t)(crc >> 0);
				Buf[TxLen++] = (uint8_t)(crc >> 8);

				/* Calculate padding */
				if(TxLen % DES_BLOCK_SIZE)
					TxLen += DES_BLOCK_SIZE - (TxLen % DES_BLOCK_SIZE);
				
				/* Set DES key to last session key */
				if( DES_SetKeyData(&dev->Cipher.DES_Cipher, dev->SessionKey, dev->SessionKeySize, 0) != DES_OK )
				{
					return MFDF_ERR_CIPHER;
				}
							
				/* Cipher the new key + CRC */
				else if( DES_CryptDataCBC(&dev->Cipher.DES_Cipher, DES_CBC_SEND, DES_KEY_DECIPHER, TxLen, Buf, CipherBuf) != DES_OK )
				{
					return MFDF_ERR_CIPHER;
				}
				
				else
				{
					/* Do nothing */
				}
			}
			else
			{
				/* Key numbers are different (CASE 1) */
				
				/* Bitwise X-OR previous key and new key */
				for(i=0; i<KeySize; i++)
				{
					Buf[i] = Key[i] ^ NewKey[i];
				}
				
				TxLen = (uint16_t)KeySize;
				
				/* Calculate CRC of XORed keys */
				crc = MFDF_CRC(Buf, KeySize);

				Buf[TxLen++] = (uint8_t)(crc >> 0);
				Buf[TxLen++] = (uint8_t)(crc >> 8);

				
				/* Calculate CRC of new key */
				crc = MFDF_CRC(NewKey, KeySize);

				Buf[TxLen++] = (uint8_t)(crc >> 0);
				Buf[TxLen++] = (uint8_t)(crc >> 8);
				
				/* Calculate padding */
				if(TxLen % DES_BLOCK_SIZE)
					TxLen += DES_BLOCK_SIZE - (TxLen % DES_BLOCK_SIZE);
				
				/* Set DES key to last session key */
				if( DES_SetKeyData(&dev->Cipher.DES_Cipher, dev->SessionKey, dev->SessionKeySize, 0) != DES_OK )
				{
					return MFDF_ERR_CIPHER;
				}
							
				/* Cipher the new key + CRC */
				else if( DES_CryptDataCBC(&dev->Cipher.DES_Cipher, DES_CBC_SEND, DES_KEY_DECIPHER, TxLen, Buf, CipherBuf) != DES_OK )
				{
					return MFDF_ERR_CIPHER;
				}
				
				else
				{
					/* Do nothing */
				}
			}
			
			break;
			
		case MFDF_KEY_AES:
			
			/* Do nothing */
			
			return MFDF_ERR_INVAL_PARAM; /* AES not implemented yet */
			break;
		
		default: return MFDF_ERR_INVAL_PARAM;
	}
	
	/* Add key number to frame */
	Buf[0] = MFDF_SET_KEY_TYPE(KeyNumber, ((KeyType == MFDF_KEY_DES_2K3DES) ? MFDF_KEY_TYPE_16TDES : MFDF_KEY_TYPE_24TDES) );
	MFDF_MEMCPY(Buf+1, CipherBuf, TxLen);
	TxLen++;
	
	platformLog("TxLen: %d, Tx:\r\n", TxLen);
	platformLogHEX(Buf, TxLen, 16, TRUE);
	
	/* Send buffer to PICC */
	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_SEC_CHANGE_KEY, Buf, TxLen, NULL, sizeof(Buf), NULL));

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_GetKeyVersion(MFDF_Device_t *dev, uint8_t KeyNumber, uint8_t *keyVer)
{
	MFDF_RetCode_t 		err;
	uint8_t				Buf[32];
	uint16_t			RxLen;
	
	if( (dev == NULL) || (keyVer == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}

	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_SEC_GET_KEY_VERSION, &KeyNumber, 1, Buf, sizeof(Buf), &RxLen));

	if( RxLen != 1 )
	{
		return MFDF_ERR_LENGTH;
	}

	*keyVer = Buf[0];

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_AppCreate(MFDF_Device_t *dev, MFDF_AID_t AID, MFDF_KeySetting_t keySetting, uint8_t numOfKey, MFDF_KeyType_t keyType)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[5];

	if(dev == NULL )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	AID = AID & MFDF_APPLICATION_ID_MASK;

	if( (AID == 0) || (numOfKey > MFDF_MAX_KEY_PER_APP) )
	{
		return MFDF_ERR_PARAM_RANGE;
	}

	Buf[0] = (uint8_t)(AID >> 0);
	Buf[1] = (uint8_t)(AID >> 8);
	Buf[2] = (uint8_t)(AID >> 16);

	Buf[3] = keySetting & MFDF_PICC_MK_SETTING;
	
	switch(keyType)
	{
		case MFDF_KEY_DES_1K3DES:
		case MFDF_KEY_DES_2K3DES: numOfKey = MFDF_SET_KEY_TYPE(numOfKey, MFDF_KEY_TYPE_16TDES); break;
		case MFDF_KEY_DES_3K3DES: numOfKey = MFDF_SET_KEY_TYPE(numOfKey, MFDF_KEY_TYPE_24TDES); break;
		case MFDF_KEY_AES: 		  numOfKey = MFDF_SET_KEY_TYPE(numOfKey, MFDF_KEY_TYPE_AES);    break;
		
		default: return MFDF_ERR_INVAL_PARAM;
	}
	
	Buf[4] = numOfKey;

	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_PIC_CREATE_APP, Buf, 5, NULL, 0, NULL));

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_AppDelete(MFDF_Device_t *dev, MFDF_AID_t AID)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[3];
	
	if(dev == NULL )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	AID = AID & MFDF_APPLICATION_ID_MASK;

	if(AID == 0)
	{
		return MFDF_ERR_PARAM_RANGE;
	}

	Buf[0] = (uint8_t)(AID >> 0);
	Buf[1] = (uint8_t)(AID >> 8);
	Buf[2] = (uint8_t)(AID >> 16);

	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_PIC_DELETE_APP, Buf, 3, NULL, 0, NULL));

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_AppGetID(MFDF_Device_t *dev, MFDF_AID_t *AIDList, uint8_t ListSize, uint8_t *AppCnt)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[20];
	uint8_t				*Temp;
	uint16_t			RxLen;
	uint8_t				Cmd = MFDF_CMD_PIC_GET_APP_ID;
	Bool				LastFrame = FALSE;
	uint8_t 			i,j;
	
	if( (dev == NULL) || (AIDList == NULL) || (AppCnt == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	else if(ListSize == 0)
	{
		return MFDF_ERR_PARAM_RANGE;
	}
	
	else
	{
		/* Do nothing */
	}
	
	i = 0;
	
	do
	{
		MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, Cmd, NULL, 0, Buf, sizeof(Buf), &RxLen) );
			
		j = 0;
				
		while( (j < RxLen) && (i < ListSize) )
		{
			Temp = (uint8_t*)&AIDList[i];

			AIDList[i] = Buf[j++];
			AIDList[i] <<= 8;
			AIDList[i] |= Buf[j++];
			AIDList[i] <<= 8;
			AIDList[i] |= Buf[j++];			

			i++;
		}
	
		Cmd = MFDF_CMD_NEXT_FRAME;
		
		if( err == MFDF_ERR_NONE )
		{
			LastFrame = TRUE;
		}
			
	}while(LastFrame == FALSE);
	
	*AppCnt = i;
	
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_AppSelect(MFDF_Device_t *dev, MFDF_AID_t AID)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[3];

	if(dev == NULL )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	Buf[0] = (uint8_t)(AID >> 0);
	Buf[1] = (uint8_t)(AID >> 8);
	Buf[2] = (uint8_t)(AID >> 16);

	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_PIC_SELECT_APP, Buf, 3, NULL, 0, NULL));

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_AppFormat(MFDF_Device_t *dev)
{
	MFDF_RetCode_t	err;
	
	if(dev == NULL )
	{
		return MFDF_ERR_INVAL_PARAM;
	}

	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_PIC_FORMAT_PICC, NULL, 0, NULL, 0, NULL));

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_GetVersion(MFDF_Device_t *dev, MFDF_DevInfo_t *DevInfo)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[20];
	uint16_t			RxLen;
	uint8_t				FrameNum = 0;
	
	if( (dev == NULL) || (DevInfo == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	do
	{		
		switch(FrameNum)
		{
			/* First frame is HW Info */
			case 0:
			{
				MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_PIC_GET_VERSION, NULL, 0, Buf, sizeof(Buf), &RxLen ) );
						
				if( RxLen != sizeof(DevInfo->FrameOne))
				{
					return MFDF_ERR_FRAME_FAIL;
				}
			
				ST_MEMCPY(DevInfo->FrameOne, Buf, sizeof(DevInfo->FrameOne));
				
				FrameNum++;
				
				if(err != MFDF_ERR_NONE_MULTIFRAME)
				{
					return MFDF_ERR_FRAME_FAIL;
				}
				
				break;				
			}
			
			/* Second frame is SW Info */
			case 1:
			{
				MFDF_EXIT_ON_ERR( err, MFDF_Transceive(dev, MFDF_CMD_NEXT_FRAME, NULL, 0, Buf, sizeof(Buf), &RxLen) );
				
				if( RxLen != sizeof(DevInfo->FrameTwo) )
				{
					return MFDF_ERR_FRAME_FAIL;
				}
			
				ST_MEMCPY(DevInfo->FrameTwo, Buf, sizeof(DevInfo->FrameTwo));
								
				FrameNum++;
				
				if(err != MFDF_ERR_NONE_MULTIFRAME)
				{
					return MFDF_ERR_FRAME_FAIL;
				}
				
				break;
			}
			
			/* Production Info */
			case 2:
			{
				MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_NEXT_FRAME, NULL, 0, Buf, sizeof(Buf), &RxLen) );
								
				if( RxLen != sizeof(DevInfo->FrameThree) )
				{
					return err;
				}
			
				ST_MEMCPY(DevInfo->FrameThree, Buf, sizeof(DevInfo->FrameThree));
				
				if(err != MFDF_ERR_NONE)
				{
					return MFDF_ERR_FRAME_FAIL;
				}
				
				FrameNum++;
				
				break;
			}
		}
		
	}while(FrameNum < 3);
	
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_GetFileID(MFDF_Device_t *dev, MFDF_FID_t *FileIDList, uint8_t ListSize, uint8_t *FileCnt)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[16];
	uint16_t			RxLen;
	uint8_t				i,j;
	
	if( (dev == NULL) || (FileIDList == NULL) || (FileCnt == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	else if(ListSize == 0)
	{
		return MFDF_ERR_PARAM_RANGE;
	}
	
	else
	{
		/* Do nothing */
	}
	
	MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, MFDF_CMD_APP_GET_FILE_ID, NULL, 0, Buf, sizeof(Buf), &RxLen) );

	i = 0;
	j = 0;			
			
	while( (j < RxLen) && (i < ListSize) )
	{
		ST_MEMCPY(&FileIDList[i++], Buf+j, sizeof(MFDF_FID_t));
		
		j += sizeof(MFDF_FID_t);
	}
	
	*FileCnt = i;
	
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_GetFileSetting(MFDF_Device_t *dev, MFDF_FID_t FileID, MFDF_FileSetting_t *FileSetting)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[17];
	uint16_t			RxLen;
	
	if((dev == NULL) || (FileSetting == NULL))
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	else if(FileID > MFDF_MAX_FILE_ID)
	{
		return MFDF_ERR_FILEID_RANGE;
	}
	
	else
	{
		/* Do nothing */
	}
	
	/* Send command */
	Buf[0] = FileID;
	MFDF_EXIT_ON_ERR( err, MFDF_Transceive(dev, MFDF_CMD_APP_GET_FILE_SETTING, Buf, 1, Buf, sizeof(Buf), &RxLen) );
	
	if(RxLen == 0)
	{
		return MFDF_ERR_LENGTH;
	}
	
	/* File Type */
	FileSetting->FileType = (MFDF_FileType_t)Buf[0];
	
	/* Communication Setting */
	FileSetting->CommSetting = (MFDF_CommSetting_t)Buf[1];
	
	/* Access Rights */
	FileSetting->ReadAccess 		= (MFDF_Access_t)((Buf[2] >> 4) & 0x0F);
	FileSetting->WriteAccess 		= (MFDF_Access_t)((Buf[2] >> 0) & 0x0F);
	FileSetting->ReadWriteAccess 	= (MFDF_Access_t)((Buf[3] >> 4) & 0x0F);
	FileSetting->ChangeAccess 		= (MFDF_Access_t)((Buf[3] >> 0) & 0x0F);
	
	/* File type check */
	switch(FileSetting->FileType)
	{
		case MFDF_FILE_TYPE_STANDARD_DATA:
		case MFDF_FILE_TYPE_BACKUP_DATA:
			
			/* File size */
			FileSetting->StandardFile.FileSize = Buf[6];
			FileSetting->StandardFile.FileSize <<= 8;
			FileSetting->StandardFile.FileSize |= Buf[5];
			FileSetting->StandardFile.FileSize <<= 8;
			FileSetting->StandardFile.FileSize |= Buf[4];
			break;
			
		case MFDF_FILE_TYPE_VALUE_FILE_WITH_BACKUP:
			
			/* Lower limit value */
			FileSetting->ValueFile.LowerLimit = Buf[7];
			FileSetting->ValueFile.LowerLimit <<= 8;
			FileSetting->ValueFile.LowerLimit |= Buf[6];
			FileSetting->ValueFile.LowerLimit <<= 8;
			FileSetting->ValueFile.LowerLimit |= Buf[5];			
			FileSetting->ValueFile.LowerLimit <<= 8;
			FileSetting->ValueFile.LowerLimit |= Buf[4];
			
			/* Upper limit value */
			FileSetting->ValueFile.UpperLimit = Buf[11];
			FileSetting->ValueFile.UpperLimit <<= 8;
			FileSetting->ValueFile.UpperLimit |= Buf[10];
			FileSetting->ValueFile.UpperLimit <<= 8;
			FileSetting->ValueFile.UpperLimit |= Buf[9];			
			FileSetting->ValueFile.UpperLimit <<= 8;
			FileSetting->ValueFile.UpperLimit |= Buf[8];
			
			/* Limited credit value */
			FileSetting->ValueFile.LimitedCreditValue = Buf[15];
			FileSetting->ValueFile.LimitedCreditValue <<= 8;
			FileSetting->ValueFile.LimitedCreditValue |= Buf[14];
			FileSetting->ValueFile.LimitedCreditValue <<= 8;
			FileSetting->ValueFile.LimitedCreditValue |= Buf[13];			
			FileSetting->ValueFile.LimitedCreditValue <<= 8;
			FileSetting->ValueFile.LimitedCreditValue |= Buf[12];
			
			/* Limited credit enabled */
			FileSetting->ValueFile.LimitedCreditEnabled = Buf[16];			
			break;
			
		case MFDF_FILE_TYPE_LINEAR_RECORD_FILE_WITH_BACKUP:
		case MFDF_FILE_TYPE_CYCLIC_RECORD_FILE_WITH_BACKUP:
			
			/* Record size */
			FileSetting->RecordFile.RecordSize = Buf[6];
			FileSetting->RecordFile.RecordSize <<= 8;
			FileSetting->RecordFile.RecordSize |= Buf[5];
			FileSetting->RecordFile.RecordSize <<= 8;
			FileSetting->RecordFile.RecordSize |= Buf[4];
			
			/* Max number of records */
			FileSetting->RecordFile.RecordSize = Buf[9];
			FileSetting->RecordFile.RecordSize <<= 8;
			FileSetting->RecordFile.RecordSize |= Buf[8];
			FileSetting->RecordFile.RecordSize <<= 8;
			FileSetting->RecordFile.RecordSize |= Buf[7];
			
			/* Current number of records */
			FileSetting->RecordFile.RecordSize = Buf[12];
			FileSetting->RecordFile.RecordSize <<= 8;
			FileSetting->RecordFile.RecordSize |= Buf[11];
			FileSetting->RecordFile.RecordSize <<= 8;
			FileSetting->RecordFile.RecordSize |= Buf[10];		
			break;
			
		default:
			break;
	}
	
	
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_CreateStdDataFile(MFDF_Device_t *dev, MFDF_FID_t FileID, uint32_t Size, MFDF_Access_t ReadAccess, MFDF_Access_t WriteAccess, MFDF_Access_t ReadWriteAccess, MFDF_Access_t ChangeAccess, MFDF_CommSetting_t CommSetting)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[7];
	uint8_t				TxLen;
	uint16_t			AccessRight = 0;

	if(dev == NULL)
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	else if(FileID > MFDF_MAX_FILE_ID)
	{
		return MFDF_ERR_FILEID_RANGE;
	}
	
	else
	{
		/* Do nothing */
	}

	TxLen = 0;
	Buf[TxLen++] = FileID;

	switch( CommSetting )
	{
		case MFDF_COMMUNICATION_PLAIN: 				{ Buf[TxLen++] = 0; break; }
		case MFDF_COMMUNICATION_PLAIN_DES_SECURED: 	{ Buf[TxLen++] = 2; break; }
		case MFDF_COMMUNICATION_FULL_DES: 			{ Buf[TxLen++] = 3; break; }
		
		default: return MFDF_ERR_PARAM_RANGE;
	}


	/* Calculate access right nibbles */
	MFDF_MAKE_ACCESS_RIGHT(AccessRight, ReadAccess, WriteAccess, ReadWriteAccess, ChangeAccess);

	/* Set 2-Byte Access right, Little-Endian*/
	Buf[TxLen++] = (uint8_t)(AccessRight >> 0);
	Buf[TxLen++] = (uint8_t)(AccessRight >> 8);

	/* Set 3-Byte size of file, Little-Endian*/
	Buf[TxLen++] = (uint8_t)(Size >> 0);
	Buf[TxLen++] = (uint8_t)(Size >> 12);
	Buf[TxLen++] = (uint8_t)(Size >> 8);

	/* Send command */
	MFDF_EXIT_ON_ERR( err, MFDF_Transceive(dev, MFDF_CMD_APP_CREATE_STD_DAT_FILE, Buf, TxLen, NULL, 0, NULL) );

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_CreateBkupDataFile()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_CreateValueFile()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_CreateLinearRecFile()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_CreateCyclicRecFile()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_DeleteFile()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_ReadData(MFDF_Device_t *dev, MFDF_FID_t FileID, uint32_t Offset, uint32_t Length, uint8_t *DataBuf, uint16_t BufSize, uint16_t *ActLength)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[MFDF_FILE_READ_MAX_DATA];
	uint16_t			TxLen;
	uint16_t			RxLen;
	uint8_t				Cmd = MFDF_CMD_DAT_READ_DATA;
	Bool				LastFrame = FALSE;

	if( (dev == NULL) || (DataBuf == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
	
	else if( (FileID > MFDF_MAX_FILE_ID) || (BufSize == 0) )
	{
		return MFDF_ERR_FILEID_RANGE;
	}
	
	else
	{
		/* Do nothing */
	}

	Offset &= MFDF_FILE_OFFSET_MASK;
	Length &= MFDF_FILE_LENGTH_MASK;

	*ActLength = 0;

	TxLen = 0;
	Buf[TxLen++] = FileID;

	Buf[TxLen++] = (uint8_t)(Offset >> 0);
	Buf[TxLen++] = (uint8_t)(Offset >> 8);
	Buf[TxLen++] = (uint8_t)(Offset >> 16);

	Buf[TxLen++] = (uint8_t)(Length >> 0);
	Buf[TxLen++] = (uint8_t)(Length >> 8);
	Buf[TxLen++] = (uint8_t)(Length >> 16);

	do
	{
		MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, Cmd, Buf, TxLen, Buf, sizeof(Buf), &RxLen) );

		memcpy(DataBuf, Buf, MIN(RxLen, BufSize));

		if( BufSize < RxLen )
		{
			return MFDF_ERR_OUT_OF_MEMORY;
		}

		BufSize -= RxLen;
		DataBuf += RxLen;

		if(ActLength != NULL)
		{
			*ActLength += (uint32_t)RxLen;
		}
		
		if(err == MFDF_ERR_NONE)
		{
			return err;
		}

		TxLen = 0;
		Cmd = MFDF_CMD_NEXT_FRAME;

	}while(LastFrame == FALSE);


	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_WriteData(MFDF_Device_t *dev, MFDF_FID_t FileID, uint32_t Offset, uint32_t Length, uint8_t *DataBuf, uint16_t BufSize)
{
	MFDF_RetCode_t		err;
	uint8_t				Buf[MFDF_FILE_WRITE_MAX_DATA];
	uint16_t			TxLen;
	uint8_t				LenTemp;
	uint8_t				Cmd = MFDF_CMD_DAT_WRITE_DATA;
	Bool				LastFrame = FALSE;

	if( (dev == NULL) || (DataBuf == NULL) )
	{
		return MFDF_ERR_INVAL_PARAM;
	}
		
	else if(FileID > MFDF_MAX_FILE_ID)
	{
		return MFDF_ERR_FILEID_RANGE;
	}
	
	else if( BufSize < Length )
	{
		return MFDF_ERR_OUT_OF_MEMORY;
	}
	
	else
	{
		/* Do nothing */
	}

	Offset &= MFDF_FILE_OFFSET_MASK;
	Length &= MFDF_FILE_LENGTH_MASK;

	TxLen = 0;
	Buf[TxLen++] = FileID;

	Buf[TxLen++] = (uint8_t)(Offset >> 0);
	Buf[TxLen++] = (uint8_t)(Offset >> 8);
	Buf[TxLen++] = (uint8_t)(Offset >> 16);

	Buf[TxLen++] = (uint8_t)(Length >> 0);
	Buf[TxLen++] = (uint8_t)(Length >> 8);
	Buf[TxLen++] = (uint8_t)(Length >> 16);

	LenTemp = MIN(Length, sizeof(Buf)-TxLen);
	memcpy(&Buf[TxLen], DataBuf, LenTemp);
	Length -= LenTemp;
	DataBuf += LenTemp;
	TxLen += LenTemp;

	do
	{
		MFDF_EXIT_ON_ERR(err, MFDF_Transceive(dev, Cmd, Buf, TxLen, NULL, 0, NULL) );

		if( (err == MFDF_ERR_NONE) || (Length == 0) )
		{
			LastFrame = TRUE;
		}

		TxLen = MIN(sizeof(Buf), Length);
		memcpy(Buf, DataBuf, TxLen);

		Length -= TxLen;
		DataBuf += TxLen;


		Cmd = MFDF_CMD_NEXT_FRAME;

	}while(LastFrame == FALSE);

	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_GetValue()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_Credit()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_Debit()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_LimitedCredit()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_WriteRecord()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_ReadRecord()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_ClearRecordFile()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_CommitTransaction()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
MFDF_RetCode_t MFDF_AbortTransaction()
{
	return MFDF_ERR_NONE;
}
/*******************************************************************/
