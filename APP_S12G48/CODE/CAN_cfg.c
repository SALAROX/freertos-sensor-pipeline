/*
 * CAN_cfg.c
 *
 *  Created on: May 19, 2020
 *      Author: Abdelrahman Fahmy
 */

#include "CAN_cfg.h"
#include "CAN_drv.h"
#include "config.h"

#ifndef UNIT_TEST
//#include <PE_Types.h>
//#include <stddef.h>
//#include <stdtypes.h>
#else
/*#include "../../Generated_Code/PE_Types.h"*/
#include "C:/Unity/src/unity.h"
/*#include "C:/Freescale/CW MCU v11.0/MCU/S12lisa_Support/s12lisac/include/stddef.h"*/
#include "C:/Freescale/CW MCU v11.0/MCU/S12lisa_Support/s12lisac/include/stdtypes.h"
#ifndef TPE_Float
typedef float TPE_Float;
#endif
#endif


CAN_Signal_t CAN_Signals[MAX_NUM_SIGNALS];
CAN_Frame_t CAN_Frames[MAX_NUM_FRAMES];

void init_frames(void);
void init_signals(void);

/*
** ===================================================================
**     Method      :  init_frames
**     Description :
**         Initialise the frames parameters as per the dbc
**          
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void init_frames(void)
{
#ifdef RA_CAN_BL
#ifdef DEV_COM
	CAN_Frames[CompWLatchExtStatus].ID = 0x38;
	CAN_Frames[CompWLatchExtStatus].DLC = 8;
	CAN_Frames[CompWLatchExtStatus].Direction = CAN_TX;
	CAN_Frames[CompWLatchExtStatus].Cycle_ms = CYCLIC_FRAME_10MS /*CYCLIC_FRAME_40MS*/;
	CAN_Frames[CompWLatchExtStatus].Tx_Type = CYCLIC;
#endif
#if CAN_PCL_FRONT_LEFT
	/*CAN PCL Front Left Latch*/
	CAN_Frames[CompWLatchDiagRes].ID = 0x7F8; /*Diag Response based on ISO15765-4*/
	CAN_Frames[CompWLatchDiagReq].ID = 0x7F0; /*Diag Request based on ISO15765-4*/
#elif CAN_PCL_FRONT_RIGHT
	/*CAN PCL Front Right Latch*/
	CAN_Frames[CompWLatchDiagRes].ID = 0x7F9; /*Diag Response based on ISO15765-4*/
	CAN_Frames[CompWLatchDiagReq].ID = 0x7F1; /*Diag Request based on ISO15765-4*/
#elif CAN_PCL_REAR_LEFT
/*CAN PCL Rear Left Latch*/
	CAN_Frames[CompWLatchDiagRes].ID = 0x7FA; /*Diag Response based on ISO15765-4*/
	CAN_Frames[CompWLatchDiagReq].ID = 0x7F2; /*Diag Request based on ISO15765-4*/
#elif CAN_PCL_REAR_RIGHT
/*CAN PCL Rear Left Latch*/
	CAN_Frames[CompWLatchDiagRes].ID = 0x7FB; /*Diag Response based on ISO15765-4*/
	CAN_Frames[CompWLatchDiagReq].ID = 0x7F3; /*Diag Request based on ISO15765-4*/
#endif
	CAN_Frames[CompWLatchDiagRes].DLC = 8;
	CAN_Frames[CompWLatchDiagRes].Direction = CAN_TX;
	CAN_Frames[CompWLatchDiagRes].Cycle_ms = CYCLIC_FRAME_10MS /*CYCLIC_FRAME_20MS*/;
	CAN_Frames[CompWLatchDiagRes].Tx_Type = CYCLIC;

	CAN_Frames[CompWLatchDiagReq].DLC = 0;
	CAN_Frames[CompWLatchDiagReq].Direction = CAN_RX;
#else
/* TODO: Define other variants */
#endif
}

/*
** ===================================================================
**     Method      :  init_signals
**     Description :
**         Initialise the signals parameters as per the dbc
**          
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void init_signals(void)
{
#ifdef RA_CAN_BL
#ifdef DEV_COM
	CAN_Signals[ExtStat_7].FrameID = CompWLatchExtStatus;
	CAN_Signals[ExtStat_7].Factor = 1;
	CAN_Signals[ExtStat_7].Offset = 0;
	CAN_Signals[ExtStat_7].StartBit = 56;
	CAN_Signals[ExtStat_7].Length = 8;
	CAN_Signals[ExtStat_7].Min = 0;
	CAN_Signals[ExtStat_7].Max = 255;

	CAN_Signals[ExtStat_6].FrameID = CompWLatchExtStatus;
	CAN_Signals[ExtStat_6].Factor = 1;
	CAN_Signals[ExtStat_6].Offset = 0;
	CAN_Signals[ExtStat_6].StartBit = 48;
	CAN_Signals[ExtStat_6].Length = 8;
	CAN_Signals[ExtStat_6].Min = 0;
	CAN_Signals[ExtStat_6].Max = 255;

	CAN_Signals[ExtStat_5].FrameID = CompWLatchExtStatus;
	CAN_Signals[ExtStat_5].Factor = 1;
	CAN_Signals[ExtStat_5].Offset = 0;
	CAN_Signals[ExtStat_5].StartBit = 40;
	CAN_Signals[ExtStat_5].Length = 8;
	CAN_Signals[ExtStat_5].Min = 0;
	CAN_Signals[ExtStat_5].Max = 255;

	CAN_Signals[ExtStat_4].FrameID = CompWLatchExtStatus;
	CAN_Signals[ExtStat_4].Factor = 1;
	CAN_Signals[ExtStat_4].Offset = 0;
	CAN_Signals[ExtStat_4].StartBit = 32;
	CAN_Signals[ExtStat_4].Length = 8;
	CAN_Signals[ExtStat_4].Min = 0;
	CAN_Signals[ExtStat_4].Max = 255;

	CAN_Signals[ExtStat_23].FrameID = CompWLatchExtStatus;
	CAN_Signals[ExtStat_23].Factor = 1;
	CAN_Signals[ExtStat_23].Offset = 0;
	CAN_Signals[ExtStat_23].StartBit = 16;
	CAN_Signals[ExtStat_23].Length = 16;
	CAN_Signals[ExtStat_23].Min = 0;
	CAN_Signals[ExtStat_23].Max = 65535;

	CAN_Signals[ExtStat_01].FrameID = CompWLatchExtStatus;
	CAN_Signals[ExtStat_01].Factor = 1;
	CAN_Signals[ExtStat_01].Offset = 0;
	CAN_Signals[ExtStat_01].StartBit = 0;
	CAN_Signals[ExtStat_01].Length = 16;
	CAN_Signals[ExtStat_01].Min = 0;
	CAN_Signals[ExtStat_01].Max = 65535;
#endif
	CAN_Signals[NCWLatchCommandCounter].FrameID = CompWLatchDiagReq;
	CAN_Signals[NCWLatchCommandCounter].Factor = 1;
	CAN_Signals[NCWLatchCommandCounter].Offset = 0;
	CAN_Signals[NCWLatchCommandCounter].StartBit = 8;
	CAN_Signals[NCWLatchCommandCounter].Length = 8;
	CAN_Signals[NCWLatchCommandCounter].Min = 0;
	CAN_Signals[NCWLatchCommandCounter].Max = 255;

	CAN_Signals[NCWLatchOpenRequested].FrameID = CompWLatchDiagReq;
	CAN_Signals[NCWLatchOpenRequested].Factor = 1;
	CAN_Signals[NCWLatchOpenRequested].Offset = 0;
	CAN_Signals[NCWLatchOpenRequested].StartBit = 6;
	CAN_Signals[NCWLatchOpenRequested].Length = 2;
	CAN_Signals[NCWLatchOpenRequested].Min = 0;
	CAN_Signals[NCWLatchOpenRequested].Max = 3;

	CAN_Signals[NCWLatchOpenInvRequested].FrameID = CompWLatchDiagReq;
	CAN_Signals[NCWLatchOpenInvRequested].Factor = 1;
	CAN_Signals[NCWLatchOpenInvRequested].Offset = 0;
	CAN_Signals[NCWLatchOpenInvRequested].StartBit = 4;
	CAN_Signals[NCWLatchOpenInvRequested].Length = 2;
	CAN_Signals[NCWLatchOpenInvRequested].Min = 0;
	CAN_Signals[NCWLatchOpenInvRequested].Max = 3;

	CAN_Signals[NCWLatchVehicleMoving].FrameID = CompWLatchDiagReq;
	CAN_Signals[NCWLatchVehicleMoving].Factor = 1;
	CAN_Signals[NCWLatchVehicleMoving].Offset = 0;
	CAN_Signals[NCWLatchVehicleMoving].StartBit = 2;
	CAN_Signals[NCWLatchVehicleMoving].Length = 2;
	CAN_Signals[NCWLatchVehicleMoving].Min = 0;
	CAN_Signals[NCWLatchVehicleMoving].Max = 3;

	CAN_Signals[NCWLatchVehicleLocked].FrameID = CompWLatchDiagReq;
	CAN_Signals[NCWLatchVehicleLocked].Factor = 1;
	CAN_Signals[NCWLatchVehicleLocked].Offset = 0;
	CAN_Signals[NCWLatchVehicleLocked].StartBit = 0;
	CAN_Signals[NCWLatchVehicleLocked].Length = 2;
	CAN_Signals[NCWLatchVehicleLocked].Min = 0;
	CAN_Signals[NCWLatchVehicleLocked].Max = 3;

	CAN_Signals[NCWLatchCounter].FrameID = CompWLatchDiagRes;
	CAN_Signals[NCWLatchCounter].Factor = 1;
	CAN_Signals[NCWLatchCounter].Offset = 0;
	CAN_Signals[NCWLatchCounter].StartBit = 16;
	CAN_Signals[NCWLatchCounter].Length = 8;
	CAN_Signals[NCWLatchCounter].Min = 0;
	CAN_Signals[NCWLatchCounter].Max = 255;

	CAN_Signals[NCWLatchFailure].FrameID = CompWLatchDiagRes;
	CAN_Signals[NCWLatchFailure].Factor = 1;
	CAN_Signals[NCWLatchFailure].Offset = 0;
	CAN_Signals[NCWLatchFailure].StartBit = 12;
	CAN_Signals[NCWLatchFailure].Length = 4;
	CAN_Signals[NCWLatchFailure].Min = 0;
	CAN_Signals[NCWLatchFailure].Max = 15;

	CAN_Signals[BCWLResponseError].FrameID = CompWLatchDiagRes;
	CAN_Signals[BCWLResponseError].Factor = 1;
	CAN_Signals[BCWLResponseError].Offset = 0;
	CAN_Signals[BCWLResponseError].StartBit = 8;
	CAN_Signals[BCWLResponseError].Length = 1 /*4*/;
	CAN_Signals[BCWLResponseError].Min = 0;
	CAN_Signals[BCWLResponseError].Max = 1;

	CAN_Signals[NCWLatchClawStatus].FrameID = CompWLatchDiagRes;
	CAN_Signals[NCWLatchClawStatus].Factor = 1;
	CAN_Signals[NCWLatchClawStatus].Offset = 0;
	CAN_Signals[NCWLatchClawStatus].StartBit = 6 /*4*/;
	CAN_Signals[NCWLatchClawStatus].Length = 2;
	CAN_Signals[NCWLatchClawStatus].Min = 0;
	CAN_Signals[NCWLatchClawStatus].Max = 3;

	CAN_Signals[NCWLatchNodeIdInput].FrameID = CompWLatchDiagRes;
	CAN_Signals[NCWLatchNodeIdInput].Factor = 1;
	CAN_Signals[NCWLatchNodeIdInput].Offset = 0;
	CAN_Signals[NCWLatchNodeIdInput].StartBit = 2;
	CAN_Signals[NCWLatchNodeIdInput].Length = 2;
	CAN_Signals[NCWLatchNodeIdInput].Min = 0;
	CAN_Signals[NCWLatchNodeIdInput].Max = 3;

	CAN_Signals[NCWLatchTriggerInput].FrameID = CompWLatchDiagRes;
	CAN_Signals[NCWLatchTriggerInput].Factor = 1;
	CAN_Signals[NCWLatchTriggerInput].Offset = 0;
	CAN_Signals[NCWLatchTriggerInput].StartBit = 0;
	CAN_Signals[NCWLatchTriggerInput].Length = 2;
	CAN_Signals[NCWLatchTriggerInput].Min = 0;
	CAN_Signals[NCWLatchTriggerInput].Max = 3;
#else
	/* TODO: Define other Variants (PCL, APCL, etc) */
#endif
}

/*
** ===================================================================
**     Method      :  CAN_cfg_init
**     Description :
**         Initialise the frames and signals parameters as per the dbc
**          
**     Parameters  :
**         NAME            - DESCRIPTION
**       	void		       
**                         
**     Returns     :
**         ---             void
** ===================================================================
*/
void CAN_cfg_init(void)
{
	init_frames();
	init_signals();
}
