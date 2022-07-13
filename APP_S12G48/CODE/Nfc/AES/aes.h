/*
 * aes.h
 *
 *  Created on: Nov 6, 2019
 *      Author: Kokke (https://github.com/kokke/tiny-AES-c/)
 *      
 *      Modifier: Amir Rabbani
 *      Modifier: Cristian Castro
 */

#ifndef AES_H_
#define AES_H_

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */
#include "PE_Types.h"
#include "stdtypes.h"


/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

/* Feature Enable/Disable */
#define AES_ECB_ENABLE											FALSE
#define AES_CBC_ENABLE											TRUE
#define AES_CTR_ENABLE											FALSE

#define AES_SIZE_128											TRUE
#define AES_SIZE_192											FALSE
#define AES_SIZE_256											FALSE

/* Internal parameter setting */

#if defined(AES_SIZE_256) && (AES_SIZE_256 == TRUE)
    #define AES_KEY_LENGTH 					32
    #define AES_KEY_EXPANSION_SIZE 			240
#elif defined(AES_SIZE_192) && (AES_SIZE_192 == TRUE)
    #define AES_KEY_LENGTH 					24
    #define AES_KEY_EXPANSION_SIZE 			208
#elif defined(AES_SIZE_128) && (AES_SIZE_128 == TRUE)
    #define AES_KEY_LENGTH 					16
    #define AES_KEY_EXPANSION_SIZE 			176
#else
#error "No AES size selected"
#endif

#define AES_BLOCK_SIZE										AES_KEY_LENGTH

/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */
typedef uint8_t AES_Block_t[AES_BLOCK_SIZE];

typedef struct
{
  uint8_t RoundKey[AES_KEY_EXPANSION_SIZE];
#if (defined(AES_CBC_ENABLE) && (AES_CBC_ENABLE == TRUE)) || (defined(AES_CTR_ENABLE) && (AES_CTR_ENABLE == TRUE))
  AES_Block_t IV;
#endif
  uint8_t SessionKey[AES_KEY_LENGTH];
  AES_Block_t IVSession;
}AES_Cipher_t;

typedef enum
{
	AES_ERR_OK							,
	AES_ERR_NO_KEY						,
	AES_ERR_WRONG_PARAM					,
	AES_ERR_INVLD_PARAM					,
	AES_ERR_BLOCK_MISALIGNED	
}AES_RetCode_t;

typedef enum
{
	AES_KEY_ENCIPHER = 0				,
    AES_KEY_DECIPHER = 1
}AES_CipherMode_t;

/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */

/*
 * *****************************************************************
 *	\brief	Initialise AES handle
 *	
 *	
 *	\param[in] ctx									: Pointer to AES handle
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
AES_RetCode_t AES_Init(AES_Cipher_t *ctx);

/*
 * *****************************************************************
 *	\brief	Preset Initial Vector (IV)
 *	
 *	
 *	\param[in] 	ctx 								: Pointer to AES handle
 *	\param[in]	iv									: Initial vector array
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
AES_RetCode_t AES_SetIV(AES_Cipher_t *ctx, AES_Block_t iv);

/*
 * *****************************************************************
 *	\brief	Clear Initial Vector (IV)
 *	
 *	
 *	\param[in]	ctx 								: Pointer to AES handle
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
AES_RetCode_t AES_ClearIV(AES_Cipher_t *ctx);

/*
 * *****************************************************************
 *	\brief	Set the AES key
 *	
 *	
 *	\param[in] 	ctx 								: Pointer to AES handle
 *	\param[in]	Key									: Pointer to AES key buffer
 *	\param[in]	KeySize								: Size of AES key buffer
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_WRONG_PARAM						: Wrong key length
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
AES_RetCode_t AES_SetKeyData(AES_Cipher_t *ctx, const uint8_t* Key, uint8_t KeySize);

/*
 * *****************************************************************
 *	\brief	Generates a Random AES key
 *	
 *	
 *	\param[in] 	ctx 								: Pointer to AES handle
 *	\param[in]	KeySize								: Size of AES key buffer
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_WRONG_PARAM						: Wrong key length
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
AES_RetCode_t AES_SetRandomKeyData(AES_Cipher_t* ctx, uint8_t keySize);
/*
 * *****************************************************************
 *	\brief	Electronic Codebook (ECB) mode cipher and decipher
 *	
 *			NOTE : 	Size of the buffer has to be multiple of AES block size.
 *				  	Padding is based on ISO/IEC 7816-4.
 *				  	Starting with 0x80 and the rest is 0x00
 *	
 *	
 *	\param[in]	ctx 								: Pointer to AES handle
 *	\param[in]	cipherMode							: Encipher or decipher mode
 *	\param[in]	size								: Size of input data buffer
 *	\param[in]	inData								: Input data buffer
 *	\param[in]	outData								: Output data buffer
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_WRONG_PARAM						: Wrong cipher mode
 *	\return AES_ERR_BLOCK_MISALIGNED				: Input buffer size not multiple of AES block size
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
#if AES_ECB_ENABLE
AES_RetCode_t AES_CryptDataECB(AES_Cipher_t *ctx, AES_CipherMode_t cipherMode, uint32_t size, const uint8_t* inData, uint8_t* outData);
#endif


/*
 * *****************************************************************
 *	\brief	Cipher Block Chaining (CBC) mode cipher and decipher
 *	
 *			NOTE 1: Size of the buffer has to be multiple of AES block size.
 *				  	Padding is based on ISO/IEC 7816-4.
 *				  	Starting with 0x80 and the rest is 0x00
 *			
 *			NOTE 2: Initial Vector is effective
 *	
 *	
 *	\param[in] 	ctx 								: Pointer to AES handle
 *	\param[in]	cipherMode							: Encipher or decipher mode
 *	\param[in]	size								: Size of input data buffer
 *	\param[in]	inData								: Input data buffer
 *	\param[in]	outData								: Output data buffer
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_WRONG_PARAM						: Wrong cipher mode
 *	\return AES_ERR_BLOCK_MISALIGNED				: Input buffer size not multiple of AES block size
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
#if AES_CBC_ENABLE
AES_RetCode_t AES_CryptDataCBC(AES_Cipher_t *ctx, AES_CipherMode_t cipherMode, uint32_t size, const uint8_t* inData, uint8_t* outData);
#endif

/*
 * *****************************************************************
 *	\brief	Counter mode (CTR) cipher and decipher
 *	
 *			NOTE 1: Size of buffer could be any value. Normally used in streaming.
 *			
 *			NOTE 2: Initial Vector is effective
 *	
 *	
 *	\param[in] 	ctx 								: Pointer to AES handle
 *	\param[in]	cipherMode							: Encipher or decipher mode
 *	\param[in]	size								: Size of input data buffer
 *	\param[in]	inData								: Input data buffer
 *	\param[in]	outData								: Output data buffer
 *	
 *	\return	AES_ERR_OK								: No error occurred
 *	\return	AES_ERR_WRONG_PARAM						: Wrong cipher mode
 *	\return AES_ERR_BLOCK_MISALIGNED				: Input buffer size not multiple of AES block size
 *	\return	AES_ERR_INVLD_PARAM						: Invalid input parameter		
 * *****************************************************************
 */
#if AES_CTR_ENABLE
AES_RetCode_t AES_CryptDataCTR(AES_Cipher_t *ctx, uint32_t size, const uint8_t* inData, uint8_t* outData);
#endif


#endif /* AES_H_ */
