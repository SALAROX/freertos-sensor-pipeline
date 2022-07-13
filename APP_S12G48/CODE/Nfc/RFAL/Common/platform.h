
/******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/myliberty
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
******************************************************************************/
/*! \file
 *
 *  \author 
 *
 *  \brief Platform header file. Defining platform independent functionality.
 *
 */


/*
 *      PROJECT:   
 *      $Revision: $
 *      LANGUAGE:  ISO C99
 */

/*! \file platform.h
 *
 *  \author Gustavo Patricio
 *
 *  \brief Platform specific definition layer  
 *  
 *  This should contain all platform and hardware specifics such as 
 *  GPIO assignment, system resources, locks, IRQs, etc
 *  
 *  Each distinct platform/system/board must provide this definitions 
 *  for all SW layers to use
 *  
 */

#ifndef PLATFORM_H
#define PLATFORM_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "limits.h"
#include "timer.h"
//#include "log.h"
#include <stdtypes.h>
#include "ST25_IRQ.h"
#include "SPI_SS.h"

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/                                                

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/
#define platformProtectST25R391xComm()					__DI()
#define platformUnprotectST25R391xComm()				__EI()

#define platformIrqST25R3911SetCallback( cb )
#define platformIrqST25R3911PinInitialize()

#define platformProtectST25R391xIrqStatus()           	platformProtectST25R391xComm()                /*!< Protect unique access to IRQ status var - IRQ disable on single thread environment (MCU) ; Mutex lock on a multi thread environment */
#define platformUnprotectST25R391xIrqStatus()         	platformUnprotectST25R391xComm()              /*!< Unprotect the IRQ status var - IRQ enable on a single thread environment (MCU) ; Mutex unlock on a multi thread environment         */

#define platformTimerCreate( t )                      	timerCalculateTimer(t)                        /*!< Create a timer with the given time (ms)     */
#define platformTimerIsExpired( timer )               	timerIsExpired(timer)                         /*!< Checks if the given timer is expired        */
#define platformDelay( t )                            	timerDelay(t)                                 /*!< Performs a delay for the given time (ms)    */

#define PlatformST25IRQisHigh()							ST25IRQ_GetVal()
#define platformSpiSelect()								SPI_SS_ClrVal()
#define platformSpiDeselect()							SPI_SS_SetVal()

/*
******************************************************************************
* RFAL FEATURES CONFIGURATION
******************************************************************************
*/

#define RFAL_FEATURE_NFCA                      TRUE     /*!< Enable/Disable RFAL support for NFC-A (ISO14443A)                         */
#define RFAL_FEATURE_NFCB                      FALSE    /*!< Enable/Disable RFAL support for NFC-B (ISO14443B)                         */
#define RFAL_FEATURE_NFCF                      FALSE    /*!< Enable/Disable RFAL support for NFC-F (FeliCa)                            */
#define RFAL_FEATURE_NFCV                      FALSE    /*!< Enable/Disable RFAL support for NFC-V (ISO15693)                          */
#define RFAL_FEATURE_T1T                       FALSE    /*!< Enable/Disable RFAL support for T1T (Topaz)                               */
#define RFAL_FEATURE_T2T					   FALSE	/*!< Enable/Disable RFAL support for T2T		                               */
#define RFAL_FEATURE_ST25TB                    FALSE    /*!< Enable/Disable RFAL support for ST25TB                                    */
#define RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG     TRUE     /*!< Enable/Disable Analog Configs to be dynamically updated (RAM)             */
#define RFAL_FEATURE_DPO                       TRUE     /*!< Enable/Disable RFAL dynamic power support                                 */
#define RFAL_FEATURE_ISO_DEP                   TRUE     /*!< Enable/Disable RFAL support for ISO-DEP (ISO14443-4)                      */
#define RFAL_FEATURE_ISO_DEP_POLL              TRUE     /*!< Enable/Disable RFAL support for Poller mode (PCD) ISO-DEP (ISO14443-4)    */
#define RFAL_FEATURE_ISO_DEP_LISTEN            FALSE    /*!< Enable/Disable RFAL support for Listen mode (PICC) ISO-DEP (ISO14443-4)   */
#define RFAL_FEATURE_NFC_DEP                   FALSE    /*!< Enable/Disable RFAL support for NFC-DEP (NFCIP1/P2P)                      */

#define RFAL_FEATURE_ISO_DEP_IBLOCK_MAX_LEN    128      /*!< ISO-DEP I-Block max length. Please use values as defined by rfalIsoDepFSx */ /*Default: 256*/
#define RFAL_FEATURE_ISO_DEP_APDU_MAX_LEN      256     /*!< ISO-DEP APDU max length. Please use multiples of I-Block max length       */ /* Default 1024 */

// #define platformLog(...)                             		//logPrint(__VA_ARGS__)                          /*!< Log  method          */

// #define platformLogHEX(buf, len, colSize, head)				//LogPrintHex(buf, len, colSize, head)


uint32_t platformGetSysTick();
void tickInc(void);
void platformSpiTxRx(uint8_t *txBuf, uint8_t *rxBuf, uint16_t len);

#endif /* PLATFORM_H */


