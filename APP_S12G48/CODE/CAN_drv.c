/*
 * CAN_drv.c
 *
 *  Created on: May 18, 2020
 *      Author: Abdelrahman Fahmy
 */

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */
#include "CAN_drv.h"

#ifndef UNIT_TEST
#include "CAN1.h"
// #include "COM_task.h"
//#include "system.h"
#include <string.h>
#else
/*#include "../../System/system.h"*/
#endif

/*
 * *****************************************************************
 * DEFINE
 * *****************************************************************
 */
#define CAN_TX_MBUFFERS 0x03U /* Number of TX buffers */

#define MATH_ABS(a) (((a) >= 0) ? ((uint16_t)a) : ((uint16_t)-a))
/*
 * *****************************************************************
 * GLOBAL
 * *****************************************************************
*/
extern CAN_Signal_t CAN_Signals[MAX_NUM_SIGNALS];
extern CAN_Frame_t CAN_Frames[MAX_NUM_FRAMES];
uint16_t CAN_SleepTimer;
extern volatile BusOFF;
/*
 * *****************************************************************
 * LOCAL
 * *****************************************************************
*/
uint16_t power(uint16_t x, uint16_t y);
void process_tx_signals(void);
void process_tx_frames(void);
void process_rx_signals(void);
void process_rx_frames(void);

/*
** ===================================================================
**     Method      :  power
**     Description :
**				The function raises x to the power y
**				
**     Parameters  :
**         NAME            - DESCRIPTION
**       	x					base
**       	y					exponent		       
**                         
**     Returns     :
**         ---             
**			res			the result of the x to the power y operation
** ===================================================================
*/
uint16_t power(uint16_t x, uint16_t y)
{
	uint16_t res = 1; /*Initialize result*/

	while (y > 0)
	{
		/* If y is odd, multiply x with result */
		if (y & 1)
			res = res * x;

		/* n must be even now */
		y = y >> 1; /* y = y/2 */
		x = x * x;	/* Change x to x^2 */
	}
	return res;
}

/*
** ===================================================================
**     Method      :  process_tx_signals
**     Description :
**				The function inserts the signals variables values
**				in the payload buffer as per the configuration:
**				
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void process_tx_signals(void)
{
	uint8_t signal_idx;
	int32_t temp = 0;
	int32_t LogicalValue = 0;
	Byte byte_1_mask = 0;
	Byte byte_2_mask = 0;
	uint8_t temp_signal_idx;

	/*Reset Entire Tx Frames before updating the signals -- TODO: Move to a subfunction*/
	for (temp_signal_idx = 0; temp_signal_idx < CAN_Frames[CompWLatchDiagRes].DLC; temp_signal_idx++)
	{
		CAN_Frames[CompWLatchDiagRes].Data[temp_signal_idx] = 0;
	}

	for (signal_idx = 0; signal_idx < MAX_NUM_SIGNALS; signal_idx++)
	{
		if (CAN_Frames[CAN_Signals[signal_idx].FrameID].Direction == CAN_TX)
		{
			/*offset*/
			temp = CAN_Signals[signal_idx].PhysicalValue + MATH_ABS(CAN_Signals[signal_idx].Offset);

			/*//factor*/
			LogicalValue = temp / CAN_Signals[signal_idx].Factor;

			/* Boundary check */
			if (LogicalValue >= CAN_Signals[signal_idx].Max)
			{
				LogicalValue = CAN_Signals[signal_idx].Max;
			}
			else if (LogicalValue <= CAN_Signals[signal_idx].Min)
			{
				LogicalValue = CAN_Signals[signal_idx].Min;
			}
			else
			{
				/* Do Nothing */
			}

			/*********************Insertion************************************/
			/*//little-endian, Intel; 0 = big-endian, Motorola*/
			/*//2's comp = 256 - value*/

			/* the mask to extract the first byte of data */
			byte_1_mask = (power(2, CAN_Signals[signal_idx].bits_in_byte_1)) - 1;

			/*			if(signal_idx == NCWLatchClawStatus)
				CAN_Frames[CompWLatchDiagRes].Data[1] = byte_1_mask;*/
			//3, ok

			/*extract the bits of interest to payload 1st byte*/
			temp = LogicalValue & byte_1_mask;

			/*if(signal_idx == NCWLatchClawStatus)
				CAN_Frames[CompWLatchDiagRes].Data[1] = temp;*/
			//2, ok

			/* TODO: This can be moved to initialization if space allows */
			/* prepare the data in the payload bit position */
			temp = temp << (CAN_Signals[signal_idx].StartBit % 8U); /* shift by the start bit mod 8 */

			/*if(signal_idx == NCWLatchClawStatus)
				CAN_Frames[CompWLatchDiagRes].Data[1] = temp;*/
			//32, ok

			/* clear the old value */
			CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[CAN_Signals[signal_idx].start_byte] &= ~(byte_1_mask << (CAN_Signals[signal_idx].StartBit % 8U));

			//if(signal_idx == NCWLatchClawStatus)
			//CAN_Frames[CompWLatchDiagRes].Data[2] = CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[CAN_Signals[signal_idx].start_byte];

			/* set the new value */
			CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[CAN_Signals[signal_idx].start_byte] |= temp;

			/* TODO: might need future adjustment for signals larger than 2 bytes */
			if (CAN_Signals[signal_idx].bits_in_byte_2 > 0U)
			{
				/* the mask to extract the second byte of data */
				byte_2_mask = power(2, CAN_Signals[signal_idx].bits_in_byte_2) - 1U;
				//byte_2_mask = byte_2_mask << CAN_Signals[signal_idx].bits_in_byte_1;

				/*extract the bits of interest to payload 2nd byte*/
				//temp = (LogicalValue & byte_2_mask) >> (CAN_Signals[signal_idx].bits_in_byte_1);
				temp = (LogicalValue >> CAN_Signals[signal_idx].bits_in_byte_1) & (byte_2_mask);

				/* clear the old value */
				CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[(CAN_Signals[signal_idx].start_byte) + 1U] &= ~(byte_2_mask);

				/* set the new value */
				CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[(CAN_Signals[signal_idx].start_byte) + 1U] |= temp;
			}
		}
	}
}

/*
** ===================================================================
**     Method      :  process_tx_frames
**     Description :
**				The function sends due cyclic frames as per the configuration.
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void process_tx_frames(void)
{
	uint8_t frame_idx = 0;
	static uint32_t sysTick = 0;
	uint32_t sysTickDiff = 0;
	Byte BufferNum = 0;

	/* Get system time tick */
	sysTick += CAN_TICK_MS;
	

	for (frame_idx = 0; frame_idx < MAX_NUM_FRAMES; frame_idx++)
	{
		if (CAN_Frames[frame_idx].Direction == CAN_TX)
		{
			if (CAN_Frames[frame_idx].Tx_Type == CYCLIC)
			{
				/* is it time to send ? */
				if ((sysTick % CAN_Frames[frame_idx].Cycle_ms) == 0)
				{
					// PTT_PTT3 ^= 1;
					CAN_Frames[frame_idx].LastSystick = sysTick;

					//if(!BusOFF)
					CAN1_SendFrame(BufferNum, CAN_Frames[frame_idx].ID, DATA_FRAME, CAN_Frames[frame_idx].DLC, CAN_Frames[frame_idx].Data);

					BufferNum++;
				}
			}
			else if (CAN_Frames[frame_idx].Tx_Type == TRIGGERED)
			{
				/* if the data has changed since last transmission, re-send the frame  */
				if (memcmp(CAN_Frames[frame_idx].Data, CAN_Frames[frame_idx].Prev_Data, sizeof(CAN_Frames[frame_idx].DLC)) != 0)
				{
					CAN1_SendFrame(BufferNum, CAN_Frames[frame_idx].ID, DATA_FRAME, CAN_Frames[frame_idx].DLC, CAN_Frames[frame_idx].Data);

					BufferNum++;

					/* Current data became previous data */
					memcpy(CAN_Frames[frame_idx].Prev_Data, CAN_Frames[frame_idx].Data, sizeof(CAN_Frames[frame_idx].DLC));
				}
			}
			else
			{
				/* NONE */
			}
			/* reset tx buffer index */
			if (BufferNum > (CAN_TX_MBUFFERS - 1U))
			{
				BufferNum = 0;
			}
		}
	}
}
/*
** ===================================================================
**     Method      :  process_rx_frames
**     Description :
**				The function process the rx frames received on the interrupt.
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void process_rx_frames(void)
{
	uint8_t frame_idx = 0;
	unsigned long RxMessageID = 0;
	Byte RxFrameType = 0;
	Byte RxFrameFormat = 0;
	Byte RxLength = 0;
	Byte RxData[8];
	Bool frame_found = FALSE;
	uint8_t i = 0;

	/****** Process Rx Frames *******/
	if (CAN1_GetStateRX() == TRUE) /* not empty */
	{
		//		PTT_PTT3 ^= 1;
		CAN1_ReadFrame(&RxMessageID, &RxFrameType, &RxFrameFormat, &RxLength, RxData);
		CAN_SleepTimer = 0;

		if (RxMessageID != CAN_Frames[CompWLatchDiagReq].ID)
		{
			return;
		}
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		return;
	}

	frame_idx = 0;
	while ((frame_idx < MAX_NUM_FRAMES) && (frame_found == FALSE))
	{ /* lookup the frame that matches the received ID */
		if (CAN_Frames[frame_idx].ID == RxMessageID)
		{

			frame_found = TRUE;
			CAN_Frames[frame_idx].DLC = RxLength;

			for (i = 0; i < RxLength; i++)
			{
				CAN_Frames[frame_idx].Data[i] = RxData[i];
			}
			//memcpy((uint8_t*)CAN_Frames[frame_idx].Data, (uint8_t*)RxData, (RxLength));
		}
		frame_idx++;
	}
}

/*
** ===================================================================
**     Method      :  process_rx_signals
**     Description :
**				The function extracts signal values from received payload
**				as per the configuration:
**				
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void process_rx_signals(void)
{
	uint8_t signal_idx;
	int32_t temp = 0;
	Byte byte_1_mask = 0;
	Byte byte_2_mask = 0;

	for (signal_idx = 0; signal_idx < MAX_NUM_SIGNALS; signal_idx++)
	{
		/* Check the signal's corresponding frame id is Rx */
		if (CAN_Frames[CAN_Signals[signal_idx].FrameID].Direction == CAN_RX) /* disable in loop-back mode testing */
		{
			/*********************Extraction************************************/
			/* the mask to extract the first byte of data */
			byte_1_mask = (power(2, CAN_Signals[signal_idx].bits_in_byte_1)) - 1;

			/*extract the bits of interest to payload 1st byte*/
			/* get the start byte of the signal within the received frame buffer */
			temp = CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[CAN_Signals[signal_idx].start_byte] &
				   ((byte_1_mask << (CAN_Signals[signal_idx].StartBit % 8U)));

			/* re-align the extracted value */
			temp = temp >> (CAN_Signals[signal_idx].StartBit % 8U);

			/* TODO: might need future adjustment for signals larger than 2 bytes */
			/* Check if further extraction is needed */
			if (CAN_Signals[signal_idx].bits_in_byte_2 > 0U)
			{
				/* the mask to extract the second byte of data */
				byte_2_mask = power(2, CAN_Signals[signal_idx].bits_in_byte_2) - 1U; /* different from Tx */

				/*extract the bits of interest to payload 2nd byte*/
				temp |= (CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[CAN_Signals[signal_idx].start_byte + 1U] & byte_2_mask) << (CAN_Signals[signal_idx].bits_in_byte_1);

				/* set the new value */
				CAN_Frames[CAN_Signals[signal_idx].FrameID].Data[(CAN_Signals[signal_idx].start_byte) + 1U] |= temp;
			}

			/*offset*/
			temp += CAN_Signals[signal_idx].Offset;
			/*factor*/
			temp *= CAN_Signals[signal_idx].Factor;

			CAN_Signals[signal_idx].PhysicalValue = temp;
		}
	}
}

/*
	** ===================================================================
	**     Method      :  CAN_drv_init
	**     Description :
	**         Initialise the frames and signals by calling the configuration
	**         function and calculates the signal insertion and extraction
	**         parameters.
	**
	**     Parameters  :
	**         NAME            - DESCRIPTION
	**       	void
	**
	**     Returns     :
	**         ---             void
	** ===================================================================
	*/
void CAN_drv_init(void)
{

	uint8_t signal_idx;

	/* Load Frame and Signal properties */
	CAN_cfg_init();

	CAN_SleepTimer = 0;

	/* Calculate signals' properties */
	for (signal_idx = 0; signal_idx < MAX_NUM_SIGNALS; signal_idx++)
	{
		CAN_Signals[signal_idx].start_byte = (uint8_t)(CAN_Signals[signal_idx].StartBit / 8U);

		/* if the signal size exceeding the start byte boundary */
		if ((((CAN_Signals[signal_idx].start_byte + 1U) * 8U) - 1U) <
			((CAN_Signals[signal_idx].StartBit + CAN_Signals[signal_idx].Length) - 1U))
		{
			/* Determine how many bits are in the second byte */
			CAN_Signals[signal_idx].bits_in_byte_2 = ((CAN_Signals[signal_idx].StartBit + CAN_Signals[signal_idx].Length) - 1U) -
													 (((CAN_Signals[signal_idx].start_byte + 1U) * 8U) - 1U);

			if (CAN_Signals[signal_idx].bits_in_byte_2 < 0)
			{
				CAN_Signals[signal_idx].bits_in_byte_2 = 0;
			}
			/* the rest is in the first byte */
			CAN_Signals[signal_idx].bits_in_byte_1 = CAN_Signals[signal_idx].Length - CAN_Signals[signal_idx].bits_in_byte_2;
		}
		else
		{
			CAN_Signals[signal_idx].bits_in_byte_2 = 0U;
			CAN_Signals[signal_idx].bits_in_byte_1 = CAN_Signals[signal_idx].Length;
		}
	}
}

/*
** ===================================================================
**     Method      :  CAN_drv_run
**     Description :
**				The function performs the following as per the configuration:
**				-inserts the signals variables values in the payload buffer.
**				-send due cyclic frames
**				-send updated triggered frames
**				-read received frames 
**				-extracts signal values from received payload
**          
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void CAN_drv_run(void)
{
	uint8_t frame_idx = 0;
	unsigned long RxMessageID = 0;
	Byte RxFrameType = 0;
	Byte RxFrameFormat = 0;
	Byte RxLength = 0;
	Byte RxData[8];
	Bool frame_found = FALSE;
	uint8_t i = 0;

	/****** Process Tx Signals *******/
	// process_tx_signals();
	/********************************/

	/****** Process Tx Frames *******/
	process_tx_frames();
	/********************************/

	/****** Process Rx Signals *******/
	//	process_rx_signals();
	/********************************/
}
