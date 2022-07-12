/******************************************************************************
*                                                                             *
*   Copyright (C) 2009 Chevalier Technologies Ltd.                            *
*   All Rights Reserved                                                       *
*                                                                             *
*   The code is the property of Chevalier Technologies Ltd.                   *
*                                                                             *
*   The copyright notice above does not evidence any                          *
*   actual or intended publication of such source code.                       *
*                                                                             *
*   History:      Use the source control system to display                    *
*                 revision history information.                               *
*                                                                             *
******************************************************************************/

/**
 *  File documentation goes here.
 */

#ifndef MAIN_H
#define MAIN_H

/******************************************************************************
 *                           Include Files                                    *
 ******************************************************************************/

/* None */

/******************************************************************************
 *                              Macros                                        *
 ******************************************************************************/

/* LIN Bootloader flag address */
#define BL_LIN_FLAG_ADDRESS 	(0x04A0)
#define RESET_FLAG_ADDRESS 		(0x04B0)

#define RESET_FLAG_APP_TO_APP	(0x02)
#define RESET_FLAG_BL_TO_APP	(0x01)


#define EEPROM_API_TRIM_ADDRESS	(0x0420U)


#define EEPROM_FAULTS_NUMBER			(8U)
#define EEPROM_FAULTS_MEM_TOP			(0x0700U)
#define EEPROM_LONG_TRIGGER_EVENTS		(0x0700U)
#define EEPROM_OVER_TEMPERATURE_EVENTS	(0x0710U)
#define EEPROM_UNDER_VOLTAGE_EVENTS		(0x0720U)
#define EEPROM_OVER_VOLTAGE_EVENTS		(0x0730U)
#define EEPROM_CLINCH_OBSTRUCTED_EVENTS	(0x0740U)
#define EEPROM_POWER_CYCLE_EVENTS		(0x0750U)
#define EEPROM_ACTUATION_EVENTS			(0x0760U)
#define EEPROM_REGISTRATION_EVENTS		(0x0770U)
#define EEPROM_FAULTS_MEM_BOTTOM		(0x0770U)


/******************************************************************************
 *                           Type Definitions                                 *
 ******************************************************************************/

typedef enum 
{
    SYS_defaultSched,        // 0
    SYS_sleepSched,          // 1
    SYS_pkeSched,            // 2
} SYS_schedSelector_t;

typedef enum 
{
    SYS_running,             // 0
    SYS_readyToSleep,        // 1
    SYS_awakeFromSleep,      // 2
} SYS_sleepStatus_t;

typedef enum {
	APP_LOADED = 0x01,
	LIN_BOOT = 0x02
} BT_FLAGS;

/******************************************************************************
 *                          Function Prototypes                               *
 ******************************************************************************/

void SYS_SelectScheduler(uint8_t scheduler);
uint8_t SYS_GetScheduler(void);

/******************************************************************************
 *                          External References                               *
 ******************************************************************************/

extern uint8_t  SYS_schedTick;
extern uint8_t  SYS_sleepStatus;



#endif /* MAIN_H */


