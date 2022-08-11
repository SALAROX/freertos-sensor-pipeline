/** ###################################################################
**     Filename  : Events.c
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
/* MODULE Events */


#include "Cpu.h"
#include "Events.h"
#include "st25r3911_interrupt.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

#pragma CODE_SEG DEFAULT

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
void ST25_IRQ_OnInterrupt(void)
{
  /* place your ST25_IRQ interrupt procedure body here */
  st25r3911Isr();
}


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
void ADC_OnEnd(void)
{
  /* Write your code here ... */
}

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
// void ST25_IRQ_Interrupt(void)
// {
//   /* place your ST25_IRQ interrupt procedure body here */
// }


/*
** ===================================================================
**     Event       :  Timer_1ms_OnInterrupt (module Events)
**
**     Component   :  Timer_1ms [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Timer_1ms_OnInterrupt(void)
{
  ISR_SchedulerTick();
  tickInc();
  PT1AD_PT1AD1 ^= 1;
}

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
// void ST25_IRQ_Interrupt(void)
// {
//   /* place your ST25_IRQ interrupt procedure body here */
// }


/* END Events */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.05 [04.46]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
