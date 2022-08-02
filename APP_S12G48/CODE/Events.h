/** ###################################################################
**     Filename  : Events.h
**     Project   : Generic_Latch_Bootloader
**     Processor : MC9S12G48VLC
**     Component : Events
**     Version   : Driver 01.04
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 27/07/2021, 16:17
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "CAN1.h"
#include "TIM1.h"
#include "GPIO_Port_S.h"
#include "GPIO_Port_T.h"
#include "RTI1.h"
#include "IEE1.h"
#include "SM1.h"
#include "SPI_SS.h"
#include "ADC.h"
#include "ST25_IRQ.h"

#pragma CODE_SEG DEFAULT


void ST25_IRQ_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  ST25_IRQ_Interrupt (module Events)
**
**     Component   :  ST25_IRQ [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void ADC_OnEnd(void);
/*
** ===================================================================
**     Event       :  ADC_OnEnd (module Events)
**
**     Component   :  ADC [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

// void ST25_IRQ_Interrupt(void);
/*
** ===================================================================
**     Event       :  ST25_IRQ_Interrupt (module Events)
**
**     Component   :  ST25_IRQ [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/* END Events */
#endif /* __Events_H*/

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.05 [04.46]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
