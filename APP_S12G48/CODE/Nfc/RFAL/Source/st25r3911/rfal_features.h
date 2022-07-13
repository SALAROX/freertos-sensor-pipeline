
/******************************************************************************
  * \attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        www.st.com/myliberty
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


/*
 *      PROJECT:   ST25R391x firmware
 *      Revision:
 *      LANGUAGE:  ISO C99
 */

/*! \file
 *
 *  \author Gustavo Patricio 
 *
 *  \brief RFAL Features/Capabilities Definition for ST25R3911
 */


#ifndef RFAL_FEATURES_H
#define RFAL_FEATURES_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "platform.h"
#include "PE_Types.h"

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/

#define RFAL_SUPPORT_MODE_POLL_NFCA                TRUE          /*!< RFAL Poll NFCA mode support switch    */
#define RFAL_SUPPORT_MODE_POLL_NFCB                TRUE          /*!< RFAL Poll NFCB mode support switch    */    
#define RFAL_SUPPORT_MODE_POLL_NFCF                TRUE          /*!< RFAL Poll NFCF mode support switch    */
#define RFAL_SUPPORT_MODE_POLL_NFCV                TRUE          /*!< RFAL Poll NFCV mode support switch    */
#define RFAL_SUPPORT_MODE_POLL_ACTIVE_P2P          TRUE          /*!< RFAL Poll AP2P mode support switch    */
#define RFAL_SUPPORT_MODE_LISTEN_NFCA              FALSE         /*!< RFAL Listen NFCA mode support switch  */
#define RFAL_SUPPORT_MODE_LISTEN_NFCB              FALSE         /*!< RFAL Listen NFCB mode support switch  */
#define RFAL_SUPPORT_MODE_LISTEN_NFCF              FALSE         /*!< RFAL Listen NFCF mode support switch  */
#define RFAL_SUPPORT_MODE_LISTEN_ACTIVE_P2P        TRUE          /*!< RFAL Listen AP2P mode support switch  */

/*******************************************************************************/
/*! RFAL supported Card Emulation (CE)        */
#define RFAL_SUPPORT_CE                            ( RFAL_SUPPORT_MODE_LISTEN_NFCA || RFAL_SUPPORT_MODE_LISTEN_NFCB || RFAL_SUPPORT_MODE_LISTEN_NFCF )

/*! RFAL supported Reader/Writer (RW)         */
#define RFAL_SUPPORT_RW                            ( RFAL_SUPPORT_MODE_POLL_NFCA || RFAL_SUPPORT_MODE_POLL_NFCB || RFAL_SUPPORT_MODE_POLL_NFCF || RFAL_SUPPORT_MODE_POLL_NFCV )

/*! RFAL support for Active P2P (AP2P)        */
#define RFAL_SUPPORT_AP2P                          ( RFAL_SUPPORT_MODE_POLL_ACTIVE_P2P || RFAL_SUPPORT_MODE_POLL_ACTIVE_P2P )


/*******************************************************************************/
#define RFAL_SUPPORT_BR_RW_106                      TRUE         /*!< RFAL RW  106 Bit Rate support switch   */
#define RFAL_SUPPORT_BR_RW_212                      TRUE         /*!< RFAL RW  212 Bit Rate support switch   */
#define RFAL_SUPPORT_BR_RW_424                      TRUE         /*!< RFAL RW  424 Bit Rate support switch   */
#define RFAL_SUPPORT_BR_RW_848                      TRUE         /*!< RFAL RW  848 Bit Rate support switch   */
#define RFAL_SUPPORT_BR_RW_1695                     TRUE         /*!< RFAL RW 1695 Bit Rate support switch   */
#define RFAL_SUPPORT_BR_RW_3390                     TRUE         /*!< RFAL RW 3390 Bit Rate support switch   */
#define RFAL_SUPPORT_BR_RW_6780                     FALSE        /*!< RFAL RW 6780 Bit Rate support switch   */
#define RFAL_SUPPORT_BR_RW_13560                    FALSE        /*!< RFAL RW 6780 Bit Rate support switch   */


/*******************************************************************************/
#define RFAL_SUPPORT_BR_AP2P_106                    TRUE         /*!< RFAL AP2P  106 Bit Rate support switch */
#define RFAL_SUPPORT_BR_AP2P_212                    TRUE         /*!< RFAL AP2P  212 Bit Rate support switch */
#define RFAL_SUPPORT_BR_AP2P_424                    TRUE         /*!< RFAL AP2P  424 Bit Rate support switch */
#define RFAL_SUPPORT_BR_AP2P_848                    FALSE        /*!< RFAL AP2P  848 Bit Rate support switch */
#define RFAL_SUPPORT_BR_AP2P_1695                   FALSE        /*!< RFAL AP2P 1695 Bit Rate support switch */
#define RFAL_SUPPORT_BR_AP2P_3390                   FALSE        /*!< RFAL AP2P 3390 Bit Rate support switch */
#define RFAL_SUPPORT_BR_AP2P_6780                   FALSE        /*!< RFAL AP2P 6780 Bit Rate support switch */


/*******************************************************************************/
#define RFAL_SUPPORT_BR_CE_A_106                    FALSE        /*!< RFAL CE A 106 Bit Rate support switch  */
#define RFAL_SUPPORT_BR_CE_A_212                    FALSE        /*!< RFAL CE A 212 Bit Rate support switch  */
#define RFAL_SUPPORT_BR_CE_A_424                    FALSE        /*!< RFAL CE A 424 Bit Rate support switch  */
#define RFAL_SUPPORT_BR_CE_A_848                    FALSE        /*!< RFAL CE A 848 Bit Rate support switch  */


/*******************************************************************************/
#define RFAL_SUPPORT_BR_CE_B_106                    FALSE        /*!< RFAL CE B 106 Bit Rate support switch  */
#define RFAL_SUPPORT_BR_CE_B_212                    FALSE        /*!< RFAL CE B 212 Bit Rate support switch  */
#define RFAL_SUPPORT_BR_CE_B_424                    FALSE        /*!< RFAL CE B 424 Bit Rate support switch  */
#define RFAL_SUPPORT_BR_CE_B_848                    FALSE        /*!< RFAL CE B 848 Bit Rate support switch  */


/*******************************************************************************/
#define RFAL_SUPPORT_BR_CE_F_212                    FALSE        /*!< RFAL CE F 212 Bit Rate support switch  */
#define RFAL_SUPPORT_BR_CE_F_424                    FALSE        /*!< RFAL CE F 424 Bit Rate support switch  */


#endif /* RFAL_FEATURES_H */
