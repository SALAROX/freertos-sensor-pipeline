/*
 *	des.h
 *
 *  Created on: 22 Jul 2019
 *      Author: Amir Rabbani
 *      Modifier: Cristian Castro
 */

#ifndef DES_DES_H_
#define DES_DES_H_

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */
#include "PE_Types.h"
#include <stdtypes.h>


/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

#define DES_BLOCK_SIZE									8U
#define DES_KEY_MAX_LENGTH								24U

/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */

typedef uint8_t	DES_cblock[DES_BLOCK_SIZE];

typedef enum
{
	DES_KEY_ENCIPHER = 0				,
    DES_KEY_DECIPHER = 1
}DES_CipherMode_t;

// Cipher Block Chaining mode
typedef enum
{
    DES_CBC_SEND						,
    DES_CBC_RECEIVE
}DES_CBCMode_t;

// These values must be OR-ed with the key number when executing command DF_INS_CHANGE_KEY
typedef enum
{	
	DES_KEY_2K3DES  					,
	DES_KEY_3K3DES  					,
	DES_KEY_INVALID 		
}DES_KeyType_t;


typedef struct
{
	union
	{
		DES_cblock 	cblock;
		uint32_t 	deslong[2];
	} ks[16];
}DES_key_schedule;

typedef struct
{
	DES_key_schedule 	mk_ks[3];
	uint8_t 			mu8_IV[DES_BLOCK_SIZE];
	uint8_t 			mu8_Key[DES_KEY_MAX_LENGTH];
	int8_t				ms32_KeySize;
	int8_t  			ms32_BlockSize;
	uint8_t 			mu8_Version;
	DES_KeyType_t 		me_KeyType;

	//uint8_t 			mu8_Cmac[2][16];

}DES_Cipher_t;

typedef enum
{
	DES_OK							,
	DES_NO_KEY						,
	DES_WRONG_PARAM					,
	DES_INVLD_PARAM					,
	DES_BLOCK_MISALIGNED
	
}DES_RetCode_t;

/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */

/*
 * *****************************************************************
 *	\brief	Initialise DES handle
 *	
 *	
 *	\param[in] phCipher 						: Pointer to DES content struct
 *	
 *	\return	DES_OK								: No error occurred
 *	\return	DES_INVLD_PARAM						: Invalid input parameter	
 * *****************************************************************
 */
DES_RetCode_t DES_Init(DES_Cipher_t *phCipher);

/*
 * *****************************************************************
 *	\brief	Clear Initial Vector (IV)
 *	
 *	
 *	\param[in] phCipher 						: Pointer to DES content struct
 *	
 *	\return	DES_OK								: No error occurred
 *	\return	DES_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
DES_RetCode_t DES_ClearIV(DES_Cipher_t *phCipher);

/*
 * *****************************************************************
 *	\brief	Set DES engine key
 *	
 *	
 *	\param[in] 	phCipher 						: Pointer o DES content struct
 *	\param[in]	u8_Key							: Pointer to key array
 *	\param[in]	s32_KeySize						: Key size to detect key types (Single DES, 2K3DES, 3K3DES)
 *	\param[in]	u8_Version						: One byte key version distributed in LSBs of 8-Byte key
 *	
 *	\return	DES_OK								: No error occurred
 *	\return	DES_INVLD_PARAM						: Invalid input parameter
 *	\return DES_WRONG_PARAM						: Wrong input value	
 * *****************************************************************
 */
DES_RetCode_t DES_SetKeyData(DES_Cipher_t *phCipher, const uint8_t* u8_Key, int32_t s32_KeySize, uint8_t u8_Version);

/*
 * *****************************************************************
 *	\brief		Main function to use DES cipher engine
 *	
 *	
 *	\param[in] 	phCipher 						: Pointer o DES content struct
 *	\param[in]	e_CBC							: CBC mode chosen from DES_CBCMode_t. Send or receive mode
 *	\param[in]	e_Cipher						: Cipher mode chosen from DES_CipherMode_ts, encrypt or decrypt
 *	\param[in]	s32_ByteCount					: Length of input array, should be multiple of DES block size
 *	\param[in]	u8_In							: Pointer to input array
 *	\param[out]	u8_Out							: Pointer to output array
 *	
 *	\return	DES_OK								: No error occurred
 *	\return	DES_INVLD_PARAM						: Invalid input parameter
 *	\return DES_WRONG_PARAM						: Wrong input value	
 *	\return DES_NO_KEY							: No key is set
 *	\return DES_BLOCK_MISALIGNED				: The input array length is not multiple of DES block, padding is required
 * *****************************************************************
 */
DES_RetCode_t DES_CryptDataCBC(DES_Cipher_t *phCipher, DES_CBCMode_t e_CBC, DES_CipherMode_t e_Cipher, int32_t s32_ByteCount, const uint8_t* u8_In, uint8_t* u8_Out);


#endif /* DES_DES_H_ */
