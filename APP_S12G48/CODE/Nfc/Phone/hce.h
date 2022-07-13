/*
 * phone.h
 *
 *  Created on: 25 Jul 2019
 *      Author: Amir Rabbani
 *      Modifier: Cristian Castro
 */

#ifndef PHONE_HCE_H_
#define PHONE_HCE_H_

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */

#include "rfal_user.h"
#include "PE_Types.h"

/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

#define HCE_GENERAL_BUFFER_SIZE							50
#define HCE_APP_ID_LENGTH								6
#define HCE_FILE_ID_LENGTH								6

typedef enum
{
	HCE_ERR_NONE										,
	HCE_ERR_ISODEP_TXRX									,
	HCE_ERR_INVAL_PARAM									,
	HCE_ERR_DATA_AVAILABLE								,
	HCE_ERR_MEMORY_EXECUTION_ERROR						,
	HCE_ERR_MEMORY_FAILURE								,
	HCE_ERR_SECURITY_ISSUE								,
	HCE_ERR_WRONG_LENGTH								,
	HCE_ERR_CLA_FUNCTION_NOT_SUPPORTED					,
	HCE_ERR_FILE_APPLICATION_NOT_FOUND					,
	HCE_ERR_APPLICATION_EXIST							,
	HCE_ERR_FILE_EXIST									,
	HCE_ERR_WRONG_PARAM									,
	HCE_ERR_WRONG_EXPECTED_LENGTH						,
	HCE_ERR_INS_NOT_SUPPORTED							,
	HCE_ERR_CMD_NOT_ALLOWED								,
	HCE_ERR_CLASS_NOT_SUPPORTED							,
	HCE_ERR_UNKNOWN

}HCE_RetCode_t;


/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */

typedef struct
{
	rfalNfcaListenDevice 		*NfcaDev;
	rfalIsoDepDevice 			*hIsoDevice;

	uint8_t						APDU_Buf[HCE_GENERAL_BUFFER_SIZE];

}HCE_Device_t;

typedef uint8_t HCE_App_ID[HCE_APP_ID_LENGTH];
typedef uint8_t HCE_File_ID[HCE_FILE_ID_LENGTH];


/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */

/*
 * *****************************************************************
 *	\brief
 *
 *
 *	\param[in] NfcaDev 							: NFCA device handle pointer
 *
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 *
 * *****************************************************************
 */
HCE_RetCode_t HCE_DeviceInit(HCE_Device_t *dev, rfalNfcaListenDevice *NfcaDev, rfalIsoDepDevice *hIsoDev);

/*
 * *****************************************************************
 *	\brief
 *
 *
 *	\param[in] NfcaDev 							: NFCA device handle pointer
 *
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 *
 * *****************************************************************
 */
HCE_RetCode_t HCE_SelectAndroidApp(HCE_Device_t *dev, HCE_App_ID *appID);

/*
 * *****************************************************************
 *	\brief
 *
 *
 *	\param[in] NfcaDev 							: NFCA device handle pointer
 *
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 *
 * *****************************************************************
 */
HCE_RetCode_t HCE_SelectFile(HCE_Device_t *dev, HCE_File_ID *fileID);

/*
 * *****************************************************************
 *	\brief
 *
 *
 *	\param[in] NfcaDev 							: NFCA device handle pointer
 *
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 *
 * *****************************************************************
 */
HCE_RetCode_t HCE_CreateFile(HCE_Device_t *dev, HCE_File_ID *fileID);

/*
 * *****************************************************************
 *	\brief
 *
 *
 *	\param[in] NfcaDev 							: NFCA device handle pointer
 *
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 *
 * *****************************************************************
 */
HCE_RetCode_t HCE_DeleteFile(HCE_Device_t *dev, HCE_File_ID *fileID);

/*
 * *****************************************************************
 *	\brief
 *
 *
 *	\param[in] NfcaDev 							: NFCA device handle pointer
 *
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 *
 * *****************************************************************
 */
HCE_RetCode_t HCE_ReadFile(HCE_Device_t *dev, uint32_t offset, uint32_t length, uint8_t *buf, uint16_t bufSize, uint16_t *ActLen);

/*
 * *****************************************************************
 *	\brief
 *
 *
 *	\param[in] NfcaDev 							: NFCA device handle pointer
 *
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 *
 * *****************************************************************
 */
HCE_RetCode_t HCE_WriteFile(HCE_Device_t *dev, uint32_t offset, uint32_t length, uint8_t *buf, uint16_t bufLen);




#endif /* PHONE_HCE_H_ */
