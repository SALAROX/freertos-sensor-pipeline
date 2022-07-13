/*
 * mf_crypto1.c
 *
 *  Created on: 30 Jul 2019
 *      Author: Amir Rabbani
 */

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */
#include "mf_crypto1.h"


/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */
#define MFCL_FEEDBACK_BIT_SIZE									18

/*
 * *****************************************************************
 * MACRO
 * *****************************************************************
 */

/*
 * *****************************************************************
 * LOCAL FUNCTION
 * *****************************************************************
 */

static Bool fa(Bool a, Bool b, Bool c, Bool d)
{
	return FALSE;
}

static Bool fb(Bool a, Bool b, Bool c, Bool d)
{
	return FALSE;
}

static Bool fc(Bool a, Bool b, Bool c, Bool d, Bool e)
{
	return FALSE;
}

static Bool NFSR(MFCL_Crypto1_t *crypto)
{
	Bool FeedBack;
	Bool func;
	Bool out = FALSE;


	FeedBack = 	(crypto->NFSR_VALUE[0] & (1 << 0))	^ /* Byte 1 */
				(crypto->NFSR_VALUE[0] & (1 << 5))	^

				(crypto->NFSR_VALUE[1] & (1 << 1))	^ /* Byte 2 */
				(crypto->NFSR_VALUE[1] & (1 << 3))	^
				(crypto->NFSR_VALUE[1] & (1 << 4))	^
				(crypto->NFSR_VALUE[1] & (1 << 6))	^
				(crypto->NFSR_VALUE[1] & (1 << 7))	^

				(crypto->NFSR_VALUE[2] & (1 << 1))	^ /* Byte 3 */
				(crypto->NFSR_VALUE[2] & (1 << 3))	^

				(crypto->NFSR_VALUE[3] & (1 << 0))	^ /* Byte 4 */
				(crypto->NFSR_VALUE[3] & (1 << 1))	^
				(crypto->NFSR_VALUE[3] & (1 << 3))	^
				(crypto->NFSR_VALUE[3] & (1 << 5))	^

				(crypto->NFSR_VALUE[4] & (1 << 3))	^
				(crypto->NFSR_VALUE[4] & (1 << 7))	^

				(crypto->NFSR_VALUE[5] & (1 << 1))	^
				(crypto->NFSR_VALUE[5] & (1 << 2))	^
				(crypto->NFSR_VALUE[5] & (1 << 3));

	func = fc( 	fb( (crypto->NFSR_VALUE[1] & (1 << 1)), (crypto->NFSR_VALUE[1] & (1 << 3)), (crypto->NFSR_VALUE[1] & (1 << 5)), (crypto->NFSR_VALUE[1] & (1 << 7)) ) ,
				fa( (crypto->NFSR_VALUE[2] & (1 << 1)), (crypto->NFSR_VALUE[2] & (1 << 3)), (crypto->NFSR_VALUE[2] & (1 << 5)), (crypto->NFSR_VALUE[2] & (1 << 7)) ) ,
				fa( (crypto->NFSR_VALUE[3] & (1 << 1)), (crypto->NFSR_VALUE[3] & (1 << 3)), (crypto->NFSR_VALUE[3] & (1 << 5)), (crypto->NFSR_VALUE[3] & (1 << 7)) ) ,
				fb( (crypto->NFSR_VALUE[4] & (1 << 1)), (crypto->NFSR_VALUE[4] & (1 << 3)), (crypto->NFSR_VALUE[4] & (1 << 5)), (crypto->NFSR_VALUE[4] & (1 << 7)) ) ,
				fa( (crypto->NFSR_VALUE[5] & (1 << 1)), (crypto->NFSR_VALUE[5] & (1 << 3)), (crypto->NFSR_VALUE[5] & (1 << 5)), (crypto->NFSR_VALUE[5] & (1 << 7)) ) );


	return out;
}

/*
 * *****************************************************************
 * PROTOTYPE FUNCTION DECLARATION
 * *****************************************************************
 */
Bool MFCL_Crypto(MFCL_Crypto1_t *ctx, uint8_t *input, uint8_t *output, uint8_t len)
{
	return TRUE;
}

/*******************************************************************/
