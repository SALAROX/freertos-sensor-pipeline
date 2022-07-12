/** ###################################################################
**     Filename  : Generic_Latch_Bootloader.c
**     Project   : Generic_Latch_Bootloader
**     Processor : MC9S12G48VLC
**     Version   : Driver 01.14
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 27/07/2021, 16:17
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE Generic_Latch_Bootloader */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "CAN1.h"
#include "TIM1.h"
#include "GPIO_Port_S.h"
#include "GPIO_Port_AD.h"
#include "ADC_Port_AD.h"
#include "GPIO_Port_T.h"
#include "GPIO_Port_P.h"
#include "PWM_Port_P.h"
#include "RTI1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "config.h"
#include "main.h"
#include "CAN_drv.h"
#include "UDS_Comms.h"
#include "PFlash.h"

/******************************************************************************
 *                              Macros                                        *
 ******************************************************************************/

/* None */

/******************************************************************************
 *                           Type Definitions                                 *
 ******************************************************************************/
/* None */

/******************************************************************************
 *                          Function Prototypes                               *
 ******************************************************************************/

static void SYS_DefaultScheduler(void);
static void SYS_IdcScheduler(void);
static void SYS_PkeScheduler(void);
static void SYS_SleepScheduler(void);

uint8_t bcdToBinary(uint8_t bcd);

/******************************************************************************
 *                          Variable Declarations                             *
 ******************************************************************************/

uint16_t SYS_backUpCounter;
uint16_t SYS_schedClock1ms;
uint8_t SYS_schedTick;
uint8_t SYS_schedSelector;
uint8_t SYS_sleepStatus;
uint8_t can_counter = 0;
int dummy;

/* START VARIABLES BOOTLOADER */
volatile uint32_t sysTickTimer = 0;

uint16_t linRxLen;
volatile BL_SM_t blState;
byte byteBuf;
uint32_t ackTimer;
uint32_t LIN_timeoutTimer;

BL_Repository_t BL_Repository;

DIAG_STATUS_t Diag_State;
DIAG_REQUEST_STATUS_t Request_Status;
volatile COMM_MODE_t Comm_Mode;
DIAG_REQUEST_STATUS_t Request_Status;
volatile MEMORY_STATUS_t Memory_Status;
UINT8 Boot_FLag;

uint8_t appEntryPoint[2];
uint32_t appVector_check = 0;

uint8_t failure_PDU_SID;

/* END VARIABLES BOOTLOADER*/

/******************************************************************************
 *                          External References                               *
 ******************************************************************************/
extern volatile uint16_t sRecDataIndex;
extern bool BL_PROG_Enable;
extern uint8_t silentBusCounter;
extern bool udsReqTimeOut_Flag;
/******************************************************************************
 *                               Main Task                                    *
 ******************************************************************************/

void main(void)
{
  /* Write your local variable definition here */

  __DI(); /* Disable interrupts */
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  CAN_drv_init();

  SYS_SelectScheduler((uint8_t)SYS_defaultSched);
  SYS_sleepStatus = SYS_running;
  __EI(); /* Enable interrupts */

  /* initialization of Bootloader Variables */
  failure_PDU_SID = 0;
  Diag_State = DIAG_IDLE;
  IEE1_GetByte(CAN_FLAG_ADDRESS, &Boot_FLag);
  while (IEE1_Busy())
    ;

  Comm_Mode = COMM_CAN;
  blState = BL_SM_INIT;
  Diag_State = DIAG_IDLE;
  Request_Status = READY;
  sRecDataIndex = 0;

  silentBusCounter = 0;

  /* Load application info */
  // while (PFlash_GetBlockFlash(BL_CFG_DATA_REPOSITORY_ADDRESS_START, (PFlash_TDataAddress)&BL_Repository, sizeof(BL_Repository_t)) != ERR_OK)
  while (PFlash_GetBlockFlash(BL_CFG_DATA_REPOSITORY_ADDRESS_START, (PFlash_TDataAddress)&appEntryPoint, sizeof(appEntryPoint)) != ERR_OK)
  {
  }

  appVector_check |= (uint32_t)appEntryPoint[0];
  appVector_check <<= 8;
  appVector_check |= (uint32_t)appEntryPoint[1];
  appVector_check |= 0x030000;

  BL_Repository.ApplicationInfo.entryPoint = (AppEntry_t)appVector_check;

  if (BL_Repository.ApplicationInfo.entryPoint == (AppEntry_t)0x03FFFF)
  {
    Memory_Status = EMPTY;
  }
  else
  {
    Memory_Status = UNTOUCHED;
  }

  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for (;;)
  {
    /* Runs as fast as possible */
    if (Comm_Mode == COMM_CAN)
    {
      UDS_Diagnostics_Handler();

      if (BL_PROG_Enable)
      {
        BL_Prog_Handler();
      }
      else
      {
        /* Do Nothing */
      }
    }

#if 1
    if (TRUE == SYS_schedTick)
    {
      SYS_schedTick = FALSE;

      switch (SYS_schedSelector)
      {
      case SYS_sleepSched:
        // SYS_SleepScheduler();
        break;

      case SYS_pkeSched:
        // SYS_PkeScheduler();
        break;

      default:
        SYS_DefaultScheduler();
        break;
      }

      SYS_schedClock1ms++;
      sysTickTimer++;
    }
    else
    {
      /* Do Nothing */
    }
#endif

    /*State Machine for Bootloader*/
    STM_Bootloader();
  }
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/******************************************************************************
 *                           Supporting Functions                             *
 ******************************************************************************/

#pragma INLINE
static void SYS_DefaultScheduler(void)
{
  //---------------------------
  switch (SYS_schedClock1ms % 4)
  {
  case 0:
    // PTT_PTT3 ^= 1;
    // UDS_Diagnostics_Handler();
    break;

  default:
    break;
  }

  switch (SYS_schedClock1ms % 10)
  {
  case 0:
    if (!udsReqTimeOut_Flag)
    {
      silentBusCounter++;
    }
    else
    {
      /*Reset Counter */
      silentBusCounter = 0;
    }

    // PTT_PTT3 ^= 1;
    break;

  default:
    break;
  }

#ifdef DATALOG
  switch (SYS_schedClock1ms % 100)
  {
  case 99:
    SCI_DataLogTask();
    break;
  }
#endif

#ifdef ENABLE_DAY_COUNTER
  if (0 == SYS_schedClock1ms)
  {
    HAL_StoreRecoveryData();
    SYS_backUpCounter++;
  }
#endif
}

void SYS_SelectScheduler(uint8_t scheduler)
{
  SYS_schedSelector = scheduler;
}

uint8_t SYS_GetScheduler(void)
{
  return (SYS_schedSelector);
}

#pragma INLINE
void ISR_SchedulerTick(void)
{
  SYS_schedTick = TRUE;
}

void Dummy_Task(void)
{
  dummy++;
}
#if 1
#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void ivVrti(void)
{
  /* clear the flag */
  CPMUFLG = 0x80U;

  ISR_SchedulerTick();
#ifdef LIN_COM
  lin_lld_timer_isr();
#endif

  if (can_counter >= 10)
  {
    can_counter = 0;
  }
  else
  {
    can_counter++;
  }
}
#pragma CODE_SEG DEFAULT
#endif

/* END Generic_Latch_Bootloader */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.05 [04.46]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
