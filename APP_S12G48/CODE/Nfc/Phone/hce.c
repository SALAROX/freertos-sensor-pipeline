/*
 * phone.c
 *
 *  Created on: 25 Jul 2019
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
#include "hce.h"
#include <string.h>

/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

#define HCE_ISO_CLA_DEFAULT												0x00
#define HCE_ISO_CLA_NATIVE												0xFF

#define HCE_ISO_CMD_SELECT												0xA4
#define HCE_ISO_CMD_FILE_CREATE											0xE0
#define HCE_ISO_CMD_FILE_DELETE											0xE4
#define HCE_ISO_CMD_BINARY_READ											0xB0
#define HCE_ISO_CMD_BINARY_UPDATE										0xD6


/*
 * *****************************************************************
 * LOCAL MACRO
 * *****************************************************************
 */

#define HCE_EXIT_ON_ERR(err, func)								(err) = (func);								\
																if( err != HCE_ERR_NONE )					\
																{											\
																	return (err);							\
																}

#define HCE_MEMCPY(dest,src,size)								memcpy(dest,src,size)


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
static HCE_RetCode_t HCE_Transceive(HCE_Device_t *dev, uint16_t TxLen, uint16_t *RxActSize, uint8_t *encode)
{

	ReturnCode 	err;
	uint8_t		*SW;

	err = rfal_IsoDepBlockingTxRx(dev->hIsoDevice, dev->APDU_Buf, TxLen, dev->APDU_Buf, HCE_GENERAL_BUFFER_SIZE, RxActSize);

	if( ( err != ST_ERR_NONE) || (*RxActSize < 2) )
		return HCE_ERR_ISODEP_TXRX;

	SW = &dev->APDU_Buf[(*RxActSize)-2];

	*RxActSize -= 2;

	/* Check SW1 */
	switch(SW[0])
	{
		case 0x90:
			if(SW[1] == 0x00)
				return HCE_ERR_NONE;
			break;

		case 0x61:
			if( encode != NULL ) { *encode = SW[1]; }
			return HCE_ERR_DATA_AVAILABLE;

		case 0x64: return HCE_ERR_MEMORY_EXECUTION_ERROR;
		case 0x65:
			if(SW[1] == 0x81) { return HCE_ERR_MEMORY_FAILURE; }
			break;

		case 0x66: return HCE_ERR_SECURITY_ISSUE;
		case 0x67: return HCE_ERR_WRONG_LENGTH;
		case 0x68: return HCE_ERR_CLA_FUNCTION_NOT_SUPPORTED;
		case 0x69: return HCE_ERR_CMD_NOT_ALLOWED;
		case 0x6A:
			if(SW[1] == 0x82) { return HCE_ERR_FILE_APPLICATION_NOT_FOUND; }
			if(SW[1] == 0x89) { return HCE_ERR_FILE_EXIST; }
			if(SW[1] == 0x8A) { return HCE_ERR_APPLICATION_EXIST; }

		case 0x6B: return HCE_ERR_WRONG_PARAM;
		case 0x6C:
			if( encode != NULL ) { *encode = SW[1]; }
			return HCE_ERR_WRONG_EXPECTED_LENGTH;

		case 0x6D: return HCE_ERR_INS_NOT_SUPPORTED;
		case 0x6E: return HCE_ERR_CLASS_NOT_SUPPORTED;
		case 0x6F: return HCE_ERR_FILE_APPLICATION_NOT_FOUND;


		default:{}
	}


	return HCE_ERR_UNKNOWN;
}
/*
 * *****************************************************************
 * FUNCTION DECLARATION
 * *****************************************************************
 */
HCE_RetCode_t HCE_DeviceInit(HCE_Device_t *dev, rfalNfcaListenDevice *NfcaDev, rfalIsoDepDevice *hIsoDev)
{
	if( (dev == NULL) || (NfcaDev == NULL) || (hIsoDev == NULL) )
		return HCE_ERR_INVAL_PARAM;

	dev->NfcaDev 	= NfcaDev;
	dev->hIsoDevice = hIsoDev;

	return HCE_ERR_NONE;
}
/*******************************************************************/
HCE_RetCode_t HCE_SelectAndroidApp(HCE_Device_t *dev, HCE_App_ID *appID)
{

	HCE_RetCode_t 	err;
	uint16_t		TxLen;
	uint16_t		RxLen;

	if( (dev == NULL) || (appID == NULL) )
		return HCE_ERR_INVAL_PARAM;

	TxLen = 0;
	dev->APDU_Buf[TxLen++] = HCE_ISO_CLA_DEFAULT; //CLA
	dev->APDU_Buf[TxLen++] = HCE_ISO_CMD_SELECT; //INS
	dev->APDU_Buf[TxLen++] = 0x04; //P1
	dev->APDU_Buf[TxLen++] = 0x00; //P2
	dev->APDU_Buf[TxLen++] = HCE_FILE_ID_LENGTH; //Lc

	HCE_MEMCPY(&dev->APDU_Buf[TxLen], (uint8_t*)appID, HCE_APP_ID_LENGTH);
	TxLen += HCE_FILE_ID_LENGTH;

	dev->APDU_Buf[TxLen++] = 0x00; //Le

	HCE_EXIT_ON_ERR(err, HCE_Transceive(dev, TxLen, &RxLen, NULL));


	return HCE_ERR_NONE;
}
/*******************************************************************/
HCE_RetCode_t HCE_SelectFile(HCE_Device_t *dev, HCE_File_ID *fileID)
{
	HCE_RetCode_t 	err;
	uint16_t		TxLen;
	uint16_t		RxLen;

	if( (dev == NULL) || (fileID == NULL) )
		return HCE_ERR_INVAL_PARAM;

	TxLen = 0;
	dev->APDU_Buf[TxLen++] = HCE_ISO_CLA_NATIVE; //CLA
	dev->APDU_Buf[TxLen++] = HCE_ISO_CMD_SELECT; //INS
	dev->APDU_Buf[TxLen++] = 0x00; //P1
	dev->APDU_Buf[TxLen++] = 0x00; //P2
	dev->APDU_Buf[TxLen++] = HCE_FILE_ID_LENGTH; //Lc

	HCE_MEMCPY(&dev->APDU_Buf[TxLen], (uint8_t*)fileID, HCE_FILE_ID_LENGTH);
	TxLen += HCE_FILE_ID_LENGTH;

	dev->APDU_Buf[TxLen++] = 0x00; //Le

	HCE_EXIT_ON_ERR(err, HCE_Transceive(dev, TxLen, &RxLen, NULL));


	return HCE_ERR_NONE;
}
/*******************************************************************/
HCE_RetCode_t HCE_CreateFile(HCE_Device_t *dev, HCE_File_ID *fileID)
{
	HCE_RetCode_t 	err;
	uint16_t		TxLen;
	uint16_t		RxLen;

	if( (dev == NULL) || (fileID == NULL) )
		return HCE_ERR_INVAL_PARAM;

	TxLen = 0;
	dev->APDU_Buf[TxLen++] = HCE_ISO_CLA_NATIVE; //CLA
	dev->APDU_Buf[TxLen++] = HCE_ISO_CMD_FILE_CREATE; //INS
	dev->APDU_Buf[TxLen++] = 0x00; //P1
	dev->APDU_Buf[TxLen++] = 0x00; //P2
	dev->APDU_Buf[TxLen++] = HCE_FILE_ID_LENGTH; //Lc

	HCE_MEMCPY(&dev->APDU_Buf[TxLen], (uint8_t*)fileID, HCE_FILE_ID_LENGTH);
	TxLen += HCE_FILE_ID_LENGTH;

	dev->APDU_Buf[TxLen++] = 0x00; //Le

	HCE_EXIT_ON_ERR(err, HCE_Transceive(dev, TxLen, &RxLen, NULL));

	return HCE_ERR_NONE;
}
/*******************************************************************/
HCE_RetCode_t HCE_DeleteFile(HCE_Device_t *dev, HCE_File_ID *fileID)
{
	HCE_RetCode_t 	err;
	uint16_t		TxLen;
	uint16_t		RxLen;

	if( (dev == NULL) || (fileID == NULL) )
		return HCE_ERR_INVAL_PARAM;

	TxLen = 0;
	dev->APDU_Buf[TxLen++] = HCE_ISO_CLA_NATIVE; //CLA
	dev->APDU_Buf[TxLen++] = HCE_ISO_CMD_FILE_DELETE; //INS
	dev->APDU_Buf[TxLen++] = 0x00; //P1
	dev->APDU_Buf[TxLen++] = 0x00; //P2
	dev->APDU_Buf[TxLen++] = HCE_FILE_ID_LENGTH; //Lc

	HCE_MEMCPY(&dev->APDU_Buf[TxLen], (uint8_t*)fileID, HCE_FILE_ID_LENGTH);
	TxLen += HCE_FILE_ID_LENGTH;

	dev->APDU_Buf[TxLen++] = 0x00; //Le

	HCE_EXIT_ON_ERR(err, HCE_Transceive(dev, TxLen, &RxLen, NULL));

	return HCE_ERR_NONE;

}
/*******************************************************************/
HCE_RetCode_t HCE_ReadFile(HCE_Device_t *dev, uint32_t offset, uint32_t length, uint8_t *buf, uint16_t bufSize, uint16_t *ActLen)
{
	HCE_RetCode_t 	err;
	uint16_t		TxLen;
	uint16_t		RxLen;
	uint16_t		RxDataLen;

	if( (dev == NULL) || (buf == NULL) )
		return HCE_ERR_INVAL_PARAM;

	TxLen = 0;
	dev->APDU_Buf[TxLen++] = HCE_ISO_CLA_NATIVE; //CLA
	dev->APDU_Buf[TxLen++] = HCE_ISO_CMD_BINARY_READ; //INS
	dev->APDU_Buf[TxLen++] = 0x00; //P1
	dev->APDU_Buf[TxLen++] = 0x00; //P2
	dev->APDU_Buf[TxLen++] = 8; //Lc
	
	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 0);
	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 8);
	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 16);
	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 24);
	
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 0);
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 8);
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 16);
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 24);
	

	dev->APDU_Buf[TxLen++] = 0x00; //Le

	HCE_EXIT_ON_ERR(err, HCE_Transceive(dev, TxLen, &RxLen, NULL));

	RxDataLen = MIN(bufSize, RxLen);

	HCE_MEMCPY(buf, dev->APDU_Buf, RxDataLen);
	
	if(ActLen != NULL)
		*ActLen = RxDataLen;

	return HCE_ERR_NONE;
}
/*******************************************************************/
HCE_RetCode_t HCE_WriteFile(HCE_Device_t *dev, uint32_t offset, uint32_t length, uint8_t *buf, uint16_t bufLen)
{
	HCE_RetCode_t 	err;
	uint16_t		TxLen;
	uint16_t		RxLen;

	if( (dev == NULL) || (buf == NULL) )
		return HCE_ERR_INVAL_PARAM;

	TxLen = 0;
	dev->APDU_Buf[TxLen++] = HCE_ISO_CLA_NATIVE; //CLA
	dev->APDU_Buf[TxLen++] = HCE_ISO_CMD_BINARY_UPDATE; //INS
	dev->APDU_Buf[TxLen++] = 0x00; //P1
	dev->APDU_Buf[TxLen++] = 0x00; //P2
	dev->APDU_Buf[TxLen++] = 8; //Lc

	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 0);
	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 8);
	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 16);
	dev->APDU_Buf[TxLen++] = (uint8_t)(offset >> 24);
	
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 0);
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 8);
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 16);
	dev->APDU_Buf[TxLen++] = (uint8_t)(length >> 24);


	HCE_MEMCPY(&dev->APDU_Buf[TxLen], buf, bufLen);
	TxLen += bufLen;

	dev->APDU_Buf[TxLen++] = 0x00; //Le

	HCE_EXIT_ON_ERR(err, HCE_Transceive(dev, TxLen, &RxLen, NULL));

	return HCE_ERR_NONE;

}
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
