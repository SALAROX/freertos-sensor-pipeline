/*
 * mf_crypto1.h
 *
 *  Created on: 30 Jul 2019
 *      Author: Amir Rabbani
 */

#ifndef MIFARE_CLASSIC_MF_CRYPTO1_H_
#define MIFARE_CLASSIC_MF_CRYPTO1_H_

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
#define MFCL_CRYPTO_KEY_SIZE									6U
#define MFCL_CRYPTO_NFSR_SIZE									MFCL_CRYPTO_KEY_SIZE
#define MFCL_CRYPTO_IV_SIZE										4
#define MFCL_CRYPTO_SERIAL_SIZE									4
#define MFCL_CRYPTO_CHALLENGE_SIZE								4

/*
 * *****************************************************************
 * MACRO
 * *****************************************************************
 */

/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */

typedef struct
{
	uint8_t	NFSR_VALUE[MFCL_CRYPTO_NFSR_SIZE];
	uint8_t	IV_Reader[MFCL_CRYPTO_IV_SIZE];
	uint8_t	IV_Tag[MFCL_CRYPTO_IV_SIZE];
	uint8_t	Challenge[MFCL_CRYPTO_CHALLENGE_SIZE];

}MFCL_Crypto1_t;

/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */

Bool MFCL_Crypto(MFCL_Crypto1_t *ctx, uint8_t *input, uint8_t *output, uint8_t len);



#endif /* MIFARE_CLASSIC_MF_CRYPTO1_H_ */
