/*
 * mf_desfire.h
 *
 *  Created on: Jun 28, 2019
 *      Author: Amir Rabbani
 *      Modifier: Cristian Castro
 */

#ifndef MF_DESFIRE_H_
#define MF_DESFIRE_H_

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */

#include "PE_Types.h"
#include "rfal_user.h"
#include "des.h"
#include "aes.h"

/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

#define MFDF_ISO_WRAP_MODE											FALSE
#define DISABLE_AES
#define MFDF_GENERAL_BUFFER_SIZE									50U //100U
#define MFDF_PHILIPS_VENDOR_ID										0x04
#define MFDF_SN_LEN													7U
#define MFDF_BATCH_NUM_LEN											5U
#define MFDF_APP_ID_LEN												3U
#define MFDF_KEY_LENGTH												24U
#define MFDF_MAX_KEY_PER_APP										14U

#define MFDF_MK_SETTING_DEFAULT										(MFDF_KeySetting_t)(MFDF_KS_MASTER_KEY_CHANGE 			|	\
																						MFDF_KS_FREE_DIRECTORY_ACCESS		|	\
																						MFDF_KS_CREATE_DELETE_NOMASTER		|	\
																						MFDF_KS_KEY_CONFIG_CHANGE )

typedef enum
{
	MFDF_ERR_NONE													,
	MFDF_ERR_NONE_MULTIFRAME										,
	MFDF_ERR_INVAL_PARAM											,
	MFDF_ERR_NONE_MIFARE											,
	MFDF_ERR_DEV_INCOMP_DEVICE										,
	MFDF_ERR_ISODEP_TXRX											,
	MFDF_ERR_FRAME_FAIL												,
	MFDF_ERR_DES_KEY_PARITY											,
	MFDF_ERR_DES_INTERNAL											,
	MFDF_ERR_NO_CHANGE												,
	MFDF_ERR_OUT_OF_MEMORY											,
	MFDF_ERR_ILEGAL_CMD                                      		,
	MFDF_ERR_INTEGRITY                                       		,
	MFDF_ERR_NO_SUCH_KEY                                     		,
	MFDF_ERR_LENGTH                                          		,
	MFDF_ERR_PERMIT_DENIED                                   		,
	MFDF_ERR_PARAM_ERR                                       		,
	MFDF_ERR_APP_NOT_FOUND                                   		,
	MFDF_ERR_APP_INTGERITY                                   		,
	MFDF_ERR_AUTHENTICATION                                  		,
	MFDF_ERR_BOUNDARY                                        		,
	MFDF_ERR_PICC_INTEGRITY                                 		,
	MFDF_ERR_CMD_ABORT                                       		,
	MFDF_ERR_PICC_DISABLED                                   		,
	MFDF_ERR_MEMORY_FAIL											,
	MFDF_ERR_FILE_NOT_FOUND                                  		,
	MFDF_ERR_FILE_INTEGRITY											,
	MFDF_ERR_CIPHER													,	
	MFDF_ERR_PARAM_RANGE											,
	MFDF_ERR_DUPLICATION											,
	MFDF_ERR_FILEID_RANGE											,
	MFDF_ERR_FILE_SIZE_RANGE										,
	MFDF_ERR_BOUNDRY												,
	MFDF_ERR_CARDREGISTRATION										,
	MFDF_ERR_CARD_ID_FULL											,
	MFDF_ERR_UNKNOWN
}MFDF_RetCode_t;

typedef enum
{
	MFDF_FILE_TYPE_STANDARD_DATA							= 0x00 	,
	MFDF_FILE_TYPE_BACKUP_DATA								= 0x01	,
	MFDF_FILE_TYPE_VALUE_FILE_WITH_BACKUP					= 0x02	,
	MFDF_FILE_TYPE_LINEAR_RECORD_FILE_WITH_BACKUP			= 0x03	,
	MFDF_FILE_TYPE_CYCLIC_RECORD_FILE_WITH_BACKUP			= 0x04
}MFDF_FileType_t;

typedef enum
{
	MFDF_ACCESS_RIGHT_KEY_0										= 0	,
	MFDF_ACCESS_RIGHT_KEY_1				 							,
	MFDF_ACCESS_RIGHT_KEY_2					 						,
	MFDF_ACCESS_RIGHT_KEY_3                       					,
	MFDF_ACCESS_RIGHT_KEY_4                       					,
	MFDF_ACCESS_RIGHT_KEY_5                       					,
	MFDF_ACCESS_RIGHT_KEY_6                       					,
	MFDF_ACCESS_RIGHT_KEY_7                       					,
	MFDF_ACCESS_RIGHT_KEY_8                       					,
	MFDF_ACCESS_RIGHT_KEY_9                       					,
	MFDF_ACCESS_RIGHT_KEY_10                      					,
	MFDF_ACCESS_RIGHT_KEY_11                      					,
	MFDF_ACCESS_RIGHT_KEY_12                      					,
	MFDF_ACCESS_RIGHT_KEY_13                      					,
	MFDF_ACCESS_RIGHT_FREE                        					,
	MFDF_ACCESS_RIGHT_DENY
}MFDF_Access_t;

typedef enum
{
	/* PICC Master Key Setting */
	MFDF_KS_MASTER_KEY_CHANGE								= 0x01	,
	MFDF_KS_FREE_DIRECTORY_ACCESS							= 0x02	,
	MFDF_KS_CREATE_DELETE_NOMASTER							= 0x04	,
	MFDF_KS_KEY_CONFIG_CHANGE								= 0x08	,
		
	/* Application Master Key Setting */		
	MFDF_KS_ACCESS_RIGHT_KEY_0								= 0x00	,
	MFDF_KS_ACCESS_RIGHT_KEY_1								= 0x10	,
	MFDF_KS_ACCESS_RIGHT_KEY_2								= 0x20	,
	MFDF_KS_ACCESS_RIGHT_KEY_3                    			= 0x30  ,
	MFDF_KS_ACCESS_RIGHT_KEY_4                    			= 0x40  ,
	MFDF_KS_ACCESS_RIGHT_KEY_5                    			= 0x50  ,
	MFDF_KS_ACCESS_RIGHT_KEY_6                    			= 0x60  ,
	MFDF_KS_ACCESS_RIGHT_KEY_7                    			= 0x70  ,
	MFDF_KS_ACCESS_RIGHT_KEY_8                    			= 0x80  ,
	MFDF_KS_ACCESS_RIGHT_KEY_9                    			= 0x90  ,
	MFDF_KS_ACCESS_RIGHT_KEY_10                   			= 0xA0  ,
	MFDF_KS_ACCESS_RIGHT_KEY_11                   			= 0xB0  ,
	MFDF_KS_ACCESS_RIGHT_KEY_12                   			= 0xC0  ,
	MFDF_KS_ACCESS_RIGHT_KEY_13                   			= 0xD0  ,
	MFDF_KS_ACCESS_RIGHT_SAME_KEY                 			= 0xE0  ,
	MFDF_KS_ACCESS_RIGHT_FREEZE								= 0xF0
}MFDF_KeySetting_t;

typedef enum
{
	MFDF_COMMUNICATION_PLAIN								= 0x00	,
	MFDF_COMMUNICATION_PLAIN_DES_SECURED					= 0x01	,
	MFDF_COMMUNICATION_FULL_DES								= 0x03
}MFDF_CommSetting_t;

typedef enum
{
	MFDF_AUTHENTICATION_DES_NATIVE									,
	MFDF_AUTHENTICATION_DES_STANDARD								,
	MFDF_AUTHENTICATION_AES
}MFDF_AuthMode_t;

typedef enum
{
	MFDF_KEY_DES_1K3DES												,
	MFDF_KEY_DES_2K3DES												,
	MFDF_KEY_DES_3K3DES												,
	MFDF_KEY_AES	
}MFDF_KeyType_t;

/*
 * *****************************************************************
 * MACRO
 * *****************************************************************
 */
#define MFDF_GET_SYSTICK()						platformGetSysTick()

/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */
typedef uint32_t 	MFDF_AID_t;
typedef uint8_t 	MFDF_FID_t;
typedef uint8_t		MFDF_Key_t[MFDF_KEY_LENGTH];

typedef struct
{
	MFDF_FileType_t 		FileType;
	MFDF_CommSetting_t		CommSetting;
	MFDF_Access_t			ReadAccess;
	MFDF_Access_t			WriteAccess;
	MFDF_Access_t			ReadWriteAccess;
	MFDF_Access_t			ChangeAccess;
	
	union
	{
		/* FileType: Standard */
		struct
		{
			uint32_t 	FileSize;
		}StandardFile;
		
		/* FileType: Credit */
		struct
		{
			uint32_t 	LowerLimit;
			uint32_t 	UpperLimit;
			uint32_t 	LimitedCreditValue;
			uint8_t		LimitedCreditEnabled;
		}ValueFile;
		
		/* FileType: Record */
		struct
		{
			uint32_t	RecordSize;
			uint32_t	MaxRecordNumber;
			uint32_t	CurrentRecordNumber;
		}RecordFile;
	};
	
}MFDF_FileSetting_t;


typedef struct
{
	/* Hardware information */
	union
	{
		struct
		{
			uint8_t VendorID;
			uint8_t Type;
			uint8_t SubType;
			uint8_t MajorVer;
			uint8_t MinorVer;
			uint8_t MemorySize;
			uint8_t ProtocolCode;
		}HW_Info;
		uint8_t FrameOne[7];
	};
	
	/* Software information */
	union
	{
		struct
		{
			uint8_t VendorID;
			uint8_t Type;
			uint8_t SubType;
			uint8_t MajorVer;
			uint8_t MinorVer;
			uint8_t MemorySize;
			uint8_t ProtocolCode;
		}SW_Info;
		uint8_t FrameTwo[7];
	};
	
	/* Production information */
	union
	{
		struct
		{
			uint8_t SerialNumber[MFDF_SN_LEN];
			uint8_t BatchNumber[MFDF_BATCH_NUM_LEN];
			uint8_t ProductionWeek;
			uint8_t ProductionYear;			
		}Product_Info;		
		uint8_t FrameThree[14];
	};
	
}MFDF_DevInfo_t;


typedef struct
{
	rfalNfcaListenDevice 		*NfcaDev;
	rfalIsoDepDevice 			*hIsoDevice;
	uint8_t						APDU_Buf[MFDF_GENERAL_BUFFER_SIZE];

	union
	{
		DES_Cipher_t			DES_Cipher;
#ifndef DISABLE_AES
		AES_Cipher_t			AES_Cipher;
#endif
	}Cipher;

	uint8_t						KeyVersion;
	uint8_t						LastKeyNumber;
	uint8_t						SessionKey[MFDF_KEY_LENGTH];
	uint8_t						SessionKeySize;

}MFDF_Device_t;

/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */

/*
 * *****************************************************************
 *	\brief	Print the return code string with its value
 *			to terminal
 *	
 *	
 *	\param[in]	retCode							: Value of return code
 *	\return										: Pointer to report string
 * *****************************************************************
 */
char* MFDF_StringReturnCode(MFDF_RetCode_t retCode);

/*
 * *****************************************************************
 *	\brief	Initialise DESFire device
 *			It takes the NFC handle and try to activate ISO-DEP
 *			protocol to start data exchange
 *	
 *	
 *	\param[in]	dev								: Pointer to device content handle
 *	\param[in] 	NfcaDev 						: Pointer NFCA device handle
 *	\param[in]	hIsoDev							: Pointer to ISO-DEP activation handle
 *	
 *	\return DESFIRE_ERR_NONE					: Operation OK
 *	\return DESFIRE_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return DESFIRE_ERR_INVALID_PARAM			: Invalid handle pointer
 *	\return DESFIRE_ERR_NON_MIFARE				: Non-Mifare device
 *	\return DESFIRE_ERR_DEV_INCOMP_DEVICE		: Device does not support ISO13334-4 layer (ISO-Dep)
 * *****************************************************************
 */
MFDF_RetCode_t MFDF_DeviceInit(MFDF_Device_t *dev, rfalNfcaListenDevice *NfcaDev, rfalIsoDepDevice *hIsoDev);

/*
 * *****************************************************************
 * SECURITY LEVEL API
 * *****************************************************************
 */

/*
 * *****************************************************************
 *	\brief	Authenticate with PICC using stored key
 *			in the PICC.
 *			
 *			NOTE: Authentication by key type 3K3DES is 
 *				  just possible in DES standard mode.
 *	
 *	
 *	\param[in]	dev								: Pointer to device content handle
 *	\param[in]	AuthMode						: Type of possible authentication, Native DES, Standard DES, AES
 *	\param[in]	KeyNumber						: Index of stored key. Key number 0 is reserved as Master(PICC) key. Between 0~13. Max 14 keys.
 *	\param[in]	DES Key							: Pointer to key array
 *	\param[in]	keyType							: Type of key, 1K3DES, 2K3DES, 3K3DES and AES
 *	
 *	\return MFDF_ERR_NONE						: Operation OK
 *	\return MFDF_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return MFDF_ERR_AUTHEN_KEY_RANGE			: Authentication key index out of range
 *	\return MFDF_ERR_FRAME_FAIL					: Multi-frame response failed
 *	
 * *****************************************************************
 */
MFDF_RetCode_t MFDF_Authenticate(MFDF_Device_t *dev, MFDF_AuthMode_t AuthMode, uint8_t KeyNumber, MFDF_Key_t *Key, MFDF_KeyType_t keyType);

/*
 * *****************************************************************
 *  \brief	
 *
 *
 *	\param[in]	dev								: Pointer to device content handle
 * 
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_ChangeKeySetting(MFDF_Device_t *dev, MFDF_KeySetting_t keySet, MFDF_Key_t DES_Key);

/*
 * *****************************************************************
 *  \brief	Get the key setting and type from selected application
 *
 *
 *  \param[in]	dev								: Pointer to device content handle
 *  \param[out]	keySetting						: Pointer to key setting
 *  \param[out]	maxNumKey						: Pointer to max number of available key for application
 *  \param[out] keyType							: Pointer to key type
 *
 *  \return MFDF_ERR_NONE						: Operation OK
 *	\return MFDF_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return MFDF_ERR_INVAL_PARAM				: Invalid input or output parameter
 *	\return MFDF_ERR_LENGTH						: Wrong expected response length
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_GetKeySetting(MFDF_Device_t *dev, uint8_t *keySetting, uint8_t *maxNumKey, MFDF_KeyType_t *keyType);

/*
 * *****************************************************************
 *  \brief	Change the key of an application.
 *
 *  		NOTE 1: You need to authenticate before changing keys.
 *  		NOTE 2: The type of key can NOT be changed. The only value
 *  				of key is changed. If you need to change the type,
 *  				you have to delete the application and re-create it.
 *
 *	\param[in]	dev								: Pointer to device content handle
 *  \Param[in]	KeyType							: The type of changing key to calculate the length of key
 *  \param[in]	KeyNumber						: The number of key to change	
 *  \param[in]	Key								: Pointer to current key data array
 *  \param[in]	NewKey							: Pointer to new key data array
 *
 *  \return MFDF_ERR_NONE						: Operation OK
 *	\return MFDF_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return MFDF_ERR_PARAM_RANGE				: Range of input number key is wrong
 *	\return MFDF_ERR_INVAL_PARAM				: Invalid input or output parameter
 *	\return MFDF_ERR_CIPHER						: Cipher engine error occurred
 *	\return MFDF_ERR_INTEGRITY					: Data integrity error. Normally CRC failure
 *	\return MFDF_ERR_NO_SUCH_KEY				: Input key number is incorrect
 *	\return MFDF_ERR_AUTHENTICATION				: Authentication with current key is incorrect
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_ChangeKey(MFDF_Device_t *dev, MFDF_KeyType_t KeyType, uint8_t KeyNumber, MFDF_Key_t Key, MFDF_Key_t NewKey);

/*
 * *****************************************************************
 *  \brief	Get the version of key with the input number
 *
 *
 *  \param[in]	dev								: Pointer to device content handle
 *  \Param[in]	KeyNumber						: The number of key
 *  \param[out]	keyVer							: Pointer to write 1-Byte key version
 *
 *  \return MFDF_ERR_NONE						: Operation OK
 *	\return MFDF_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return MFDF_ERR_PARAM_RANGE				: Range of input number key is wrong
 *	\return MFDF_ERR_INVAL_PARAM				: Invalid input or output parameter
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_GetKeyVersion(MFDF_Device_t *dev, uint8_t KeyNumber, uint8_t *keyVer);


/*
 * *****************************************************************
 * PICC LEVEL API
 * *****************************************************************
 */

/*
 * *****************************************************************
 *  \brief	Create application (Directory) in PICC
 *  
 *  	NOTE 1: Only 28 applications can be created in PICC
 *  			AID = 0x000000 is reserved for PICC master root
 *  			
 *  	NOTE 2: 14 different keys (Same key type) can be assigned
 *  			to each application. Key number 0~13
 *  			
 *
 *  \param[in]	dev								: Pointer to device content handle
 *  \param[in]	AID								: 3-Byte Application ID
 *  \param[in]	keySetting						: Security setting of keys
 *  \param[in]	numOfKey						: Number of keys assigned to application
 *  \param[in]	keyType							: Type of assigned keys. One type is assigned to all the keys
 *
 *  \return MFDF_ERR_NONE						: Operation OK
 *	\return MFDF_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return MFDF_ERR_PARAM_RANGE				: Range of input number key is wrong
 *	\return MFDF_ERR_INVAL_PARAM				: Invalid input or output parameter
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_AppCreate(MFDF_Device_t *dev, MFDF_AID_t AID, MFDF_KeySetting_t keySetting, uint8_t numOfKey, MFDF_KeyType_t keyType);

/*
 * *****************************************************************
 *  \brief	Delete application.
 *  
 *  		NOTE: AID = 0x000000 is reserved for PICC master root and
 *  			  can NOT be deleted.
 *
 *  \param[in]	dev								: Pointer to device content handle
 *  \param[in]	AID								: 3-Byte Application ID
 *
 *  \return MFDF_ERR_NONE						: Operation OK
 *	\return MFDF_ERR_ISODEP_TXRX				: Error in ISO-Dep layer activation
 *	\return MFDF_ERR_PARAM_RANGE				: Range of input number key is wrong
 *	\return MFDF_ERR_INVAL_PARAM				: Invalid input or output parameter
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_AppDelete(MFDF_Device_t *dev, MFDF_AID_t AID);

/*
 * *****************************************************************
 * \brief	Get all application ID
 * 			Returns the all available application IDs from PICC
 * 	
 * 	\param[in]	dev								: Pointer to device content handle
 * 	\param[out] AppIDList						: Pointer to array of application IDs
 * 	\param[in]	ListSize						: Size of Application ID list
 * 	\param[out] AppCnt							: Pointer to number of available application IDs
 * 	
 * 	\return MFDF_ERR_NONE						: Operation OK
 * 	\return MFDF_ERR_ISODEP_TXRX				: ISODep failed to transmit or receive data
 * 	\return MFDF_ERR_PARAM_RANGE				: Range of input data
 * 	\return MFDF_ERR_INVAL_PARAM				: Invalid input or output parameter
 * *****************************************************************
 */
MFDF_RetCode_t MFDF_AppGetID(MFDF_Device_t *dev, MFDF_AID_t *AIDList, uint8_t ListSize, uint8_t *AppCnt);

/*
 * *****************************************************************
 *  \brief	Select application to work
 *
 *  \param[in]	dev								: Pointer to device content handle
 *  \param[in]	AID								: 3-Byte Application ID
 *
 *  \return MFDF_ERR_NONE						: Operation OK
 * 	\return MFDF_ERR_ISODEP_TXRX				: ISODep failed to transmit or receive data
 * 	\return MFDF_ERR_INVAL_PARAM				: Invalid input or output parameter
 * 	\return MFDF_ERR_APP_NOT_FOUND				: No such application ID to select
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_AppSelect(MFDF_Device_t *dev, MFDF_AID_t AID);

/*
 * *****************************************************************
 *  \brief	Format the whole PICC
 *  
 *  		NOTE: This erases all the data on the card.
 *
 *  \param[in]	dev								: Pointer to device content handle
 *
 *  \return MFDF_ERR_NONE						: Operation OK
 * 	\return MFDF_ERR_ISODEP_TXRX				: ISODep failed to transmit or receive data
 * 	\return MFDF_ERR_AUTHENTICATION				: Authentication with current key is incorrect or authentication is needed
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_AppFormat(MFDF_Device_t *dev);

/*
 * *****************************************************************
 * \brief 	Get the DESFire device info
 * 			Returns production information, hardware information and
 * 			software information
 * 			
 * 
 * \param[in]	dev								: Pointer to device content handle
 * \param[out]	DevInfo							: Device info content handle
 * 
 * \return MFDF_ERR_NONE						: Operation OK
 * \return MFDF_ERR_ISODEP_TXRX					: ISODep failed to transmit or receive data
 * \return MFDF_ERR_INVAL_PARAM					: Invalid device handle
 * \return MFDF_ERR_FRAME_FAIL					: Wrong expected frame length
 * *****************************************************************
 */
MFDF_RetCode_t MFDF_GetVersion(MFDF_Device_t *dev, MFDF_DevInfo_t *DevInfo);

/*
 * *****************************************************************
 * APPLICATION LEVEL API
 * *****************************************************************
 */

/*
 * *****************************************************************
 * \brief 	Get the file IDs
 * 			Returns the list of available file IDs
 * 			in the selected application
 * 
 * 
 * \param[in]	dev								: Pointer to device content handle
 * \param[out] 	FileIDList						: File IDs list
 * \param[in]	ListSize						: Size of list
 * \param[out]	FileCnt							: Number of available file IDs
 * 
 * \return MFDF_ERR_NONE						: Operation OK
 * \return MFDF_ERR_ISODEP_TXRX					: ISODep failed to transmit or receive data
 * \return MFDF_ERR_INVAL_PARAM					: Invalid device handle
 * \return MFDF_ERR_FRAME_FAIL					: Wrong expected frame length
 * \return MFDF_ERR_PARAM_RANGE					: Range of input data
 * *****************************************************************
 */
MFDF_RetCode_t MFDF_GetFileID(MFDF_Device_t *dev, MFDF_FID_t *FileIDList, uint8_t ListSize, uint8_t *FileCnt);

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_GetFileSetting(MFDF_Device_t *dev, MFDF_FID_t FileID, MFDF_FileSetting_t *FileSetting);

/*
 * *****************************************************************
 * \brief 	Create standard file
 * 			Use to create standard unformatted file
 * 			
 * 			NOTE: File ID should be between 0~15.
 * 				  Maximum 16 file in each application.
 *
 *
 * \param[in]	dev								: Pointer to device content handle
 * \param[in] 	FileID							: File index
 * \param[in]	Size							: Size of file in Byte
 * \param[in]	RdAcc							: Read Access Right. Use "MFDF_Access_t" enumeration
 * \param[in]	WrAcc							: Write Access Right. Use "MFDF_Access_t" enumeration
 * \param[in]	RWAcc							: Read/Write Access Right. Use "MFDF_Access_t" enumeration
 * \param[in]	ChgAcc							: Permission to change the Access Rights of file. Use "MFDF_Access_t" enumeration
 * \param[in]	CommSetting						: Communication setting to have plain or encryption. Use MFDF_ComSet_t enumeration
 *
 * \return MFDF_ERR_NONE						: Operation OK 
 * \return MFDF_ERR_ISODEP_TXRX					: ISODep failed to transmit or receive data
 * \return MFDF_ERR_INVAL_PARAM					: Invalid device handle
 * \return MFDF_ERR_FILEID_RANGE				: Wrong expected file ID
 * \return MFDF_ERR_PARAM_RANGE					: Range of input data
 * *****************************************************************
 */
MFDF_RetCode_t MFDF_CreateStdDataFile(MFDF_Device_t *dev, MFDF_FID_t FileID, uint32_t Size, MFDF_Access_t ReadAccess, MFDF_Access_t WriteAccess, MFDF_Access_t ReadWriteAccess, MFDF_Access_t ChangeAccess, MFDF_CommSetting_t CommSetting);

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_CreateBkupDataFile();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_CreateValueFile();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_CreateLinearRecFile();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_CreateCyclicRecFile();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_DeleteFile();

/*
 * *****************************************************************
 * MIFARE DATA MANIPULATION LEVEL API
 * *****************************************************************
 */

/*
 * *****************************************************************
 *  \brief	Read the content of file
 *  
 *  		NOTE: File ID should be between 0~15.
 * 				  Maximum 16 file in each application.
 *
 * \param[in]	dev								: Pointer to device content handle
 * \param[in] 	FileID							: File index
 * \param[in]	Offset							: Offset to start reading from
 * \param[in]	Length							: Length of data
 * \param[in]	DataBuf							: Pointer to write data to
 * \param[in]	BufSize							: Size of dedicated buffer to write
 * \param[in]	ActLength						: Actual length of read data
 *
 * \return MFDF_ERR_NONE						: Operation OK 
 * \return MFDF_ERR_ISODEP_TXRX					: ISODep failed to transmit or receive data
 * \return MFDF_ERR_INVAL_PARAM					: Invalid device handle
 * \return MFDF_ERR_FILEID_RANGE				: Wrong expected file ID
 * \return MFDF_ERR_PARAM_RANGE					: Range of input data
 * \return MFDF_ERR_OUT_OF_MEMORY				: Size of buffer is NOT enough
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_ReadData(MFDF_Device_t *dev, MFDF_FID_t FileID, uint32_t Offset, uint32_t Length, uint8_t *DataBuf, uint16_t BufSize, uint16_t *ActLength);

/*
 * *****************************************************************
 *  \brief	Write to file
 *  
 *  		NOTE: File ID should be between 0~15.
 * 				  Maximum 16 file in each application.
 * 				  
 * 				  
 *
 * \param[in]	dev								: Pointer to device content handle
 * \param[in] 	FileID							: File index
 * \param[in]	Offset							: Offset to start reading from
 * \param[in]	Length							: Length of data
 * \param[in]	DataBuf							: Pointer to write data to
 * \param[in]	BufSize							: Size of dedicated buffer to write
 *
 * \return MFDF_ERR_NONE						: Operation OK 
 * \return MFDF_ERR_ISODEP_TXRX					: ISODep failed to transmit or receive data
 * \return MFDF_ERR_INVAL_PARAM					: Invalid device handle
 * \return MFDF_ERR_FILEID_RANGE				: Wrong expected file ID
 * \return MFDF_ERR_PARAM_RANGE					: Range of input data
 * \return MFDF_ERR_OUT_OF_MEMORY				: Size of buffer is NOT enough
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_WriteData(MFDF_Device_t *dev, MFDF_FID_t FileID, uint32_t Offset, uint32_t Length, uint8_t *DataBuf, uint16_t BufSize);

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_GetValue();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_Credit();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_Debit();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_LimitedCredit();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_WriteRecord();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_ReadRecord();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_ClearRecordFile();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_CommitTransaction();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_AbortTransaction();

/*
 * *****************************************************************
 *  \brief
 *
 *  \Param
 *
 *  \return
 *  *****************************************************************
 */
MFDF_RetCode_t MFDF_CardRegistration(uint8_t CardID);



#endif /* MF_DESFIRE_H_ */
