/*
 * platform.c
 *
 *  Created on: Jun 24, 2019
 *      Author: Amir Rabbani
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "platform.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include <stddef.h>

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static uint32_t gTick = 0;

/******************************************************************************/
uint32_t platformGetSysTick()
{
	return gTick;
}
/******************************************************************************/
void tickInc(void)
{
	gTick++;
}
/******************************************************************************/
void platformSpiTxRx(uint8_t *txBuf, uint8_t *rxBuf, uint16_t len)
{
	uint8_t Temp;	

	while(len--)
	{
		if( txBuf != NULL )
			Temp = *txBuf++;
		
		while( SM1_SendChar( (SM1_TComData)(Temp) ) != ERR_OK ){}		
		while( SM1_RecvChar( (SM1_TComData*)(&Temp) ) != ERR_OK ){}
		
		if( rxBuf != NULL )
			*rxBuf++ = Temp;			
		
	}
	
}
