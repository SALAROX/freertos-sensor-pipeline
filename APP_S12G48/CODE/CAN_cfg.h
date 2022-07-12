/*
 * CAN_cfg.h
 *
 *  Created on: May 19, 2020
 *      Author: Abdelrahman Fahmy
 */

#ifndef CAN_CFG_H_
#define CAN_CFG_H_
#include "config.h"

/******************************************************************************
 *                          Function Prototypes                               *
 ******************************************************************************/
void CAN_cfg_init(void);

#define OPTIONAL_SIGNALS 1
/*
 * *****************************************************************
 * Signals
 * *****************************************************************
 */
typedef enum
{
#ifdef RA_CAN_BL
#ifdef DEV_COM
	ExtStat_7,
	ExtStat_6,
	ExtStat_5,
	ExtStat_4,
	ExtStat_23,
	ExtStat_01,
#endif
	NCWLatchCounter,
	NCWLatchFailure,
	BCWLResponseError,
	NCWLatchClawStatus,
	NCWLatchNodeIdInput,
	NCWLatchTriggerInput,
	NCWLatchCommandCounter,
	NCWLatchOpenRequested,
	NCWLatchOpenInvRequested,
	NCWLatchVehicleMoving,
	NCWLatchVehicleLocked,
	MAX_NUM_SIGNALS
#else
/* TODO: Define other Variants*/
#endif
} CAN_Signal_Names_t;

/*
 * *****************************************************************
 * Frames
 * *****************************************************************
 */
typedef enum
{
	CompWLatchDiagRes,
	CompWLatchDiagReq,
#ifdef DEV_COM
	CompWLatchExtStatus,
#endif
	MAX_NUM_FRAMES
}CAN_Frame_Names_t;

typedef enum
{
	UNDEFINED_DIR = 0,
	CAN_TX,
	CAN_RX
} CAN_Frame_Direction_t;

typedef enum
{
	NONE = 0,
	CYCLIC,
	TRIGGERED
} CAN_Frame_TX_TYPE_t;

typedef enum
{
	UNSIGNED = 0,
	SIGNED
} CAN_SIGNAL_SIGN_t;

/*
 * *****************************************************************
 * Value Tables
 * *****************************************************************
 */
typedef enum
{
	NCWLatchFailure_DEFAULT = 15,
	NCWLatchFailure_NO_FAULT = 0
} NCWLatchFailure_t;

typedef enum
{
	BCWLResponseError_ERROR = 1,
	BCWLResponseError_NO_ERROR = 0
} BCWLResponseError_t;

typedef enum
{
	NCWLatchClawStatus_DEFAULT = 3,
	NCWLatchClawStatus_SECONDARY = 2,
	NCWLatchClawStatus_OPEN = 1,
	NCWLatchClawStatus_CLOSED = 0
} NCWLatchClawStatus_t;

typedef enum
{
	NCWLatchTriggerInput_DEFAULT = 3,
	NCWLatchTriggerInput_BATTERY = 2,
	NCWLatchTriggerInput_OPEN_CIRCUIT = 1,
	NCWLatchTriggerInput_GROUND = 0
} NCWLatchTriggerInput_t;

typedef enum
{
	NCWLatchOpenRequested_DEFAULT = 3,
	NCWLatchOpenRequested_OPEN = 1,
	NCWLatchOpenRequested_CLOSE = 0
} NCWLatchOpenRequested_t;

typedef enum
{
	NCWLatchOpenInvRequested_CLOSE = 3,
	NCWLatchOpenInvRequested_OPEN = 2,
	NCWLatchOpenInvRequested_DEFAULT = 0
} NCWLatchOpenInvRequested_t;

typedef enum
{
	NCWLatchVehicleMoving_IN_MOTION = 3,
	NCWLatchVehicleMoving_STATIC = 2,
	NCWLatchVehicleMoving_DEFAULT = 0
} NCWLatchVehicleMoving_t;

typedef enum
{
	NCWLatchVehicleLocked_LOCKED = 3,
	NCWLatchVehicleLocked_UNLOCKED = 2,
	NCWLatchVehicleLocked_DEFAULT = 0
} NCWLatchVehicleLocked_t;

#endif /* CAN_CFG_H_ */
