
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFAL_H
#define __RFAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "st_errno.h"
#include "utils.h"
#include "rfal_nfca.h"
#include "rfal_nfcb.h"
#include "rfal_nfcf.h"
#include "rfal_nfcv.h"
#include "rfal_st25tb.h"
#include "rfal_nfcDep.h"
#include "rfal_isoDep.h"
#include "PE_Types.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/*!
 *****************************************************************************
 * \brief Poll NFC-AP2P
 *
 * Configures the RFAL to AP2P communication and polls for a nearby 
 * device. If a device is found turns On a LED and logs its UID.
 * If the Device supports NFC-DEP protocol (P2P) it will activate 
 * the device and try to send an URI record.
 *
 * This method first tries to establish communication at 424kb/s and if
 * failed, tries also at 106kb/s
 * 
 * 
 *  \return ST_ERR_NONE    	: AP2P device found
 *  \return ST Error code   : No device found
 * 
 *****************************************************************************
 */
ReturnCode rfal_PollAP2P( void );


/*!
 *****************************************************************************
 * \brief Poll NFC-A
 *
 * Configures the RFAL to NFC-A (ISO14443A) communication and polls for a nearby 
 * NFC-A device. 
 * If a device is found turns On a LED and logs its UID.
 *
 * Additionally, if the Device supports NFC-DEP protocol (P2P) it will activate 
 * the device and try to send an URI record.
 * If the device supports ISO-DEP protocol (ISO144443-4) it will 
 * activate the device and try exchange some APDUs with PICC.
 * 
 * 
 *  \return TRUE    : NFC-A device found
 *  \return FALSE   : No device found
 * 
 *****************************************************************************
 */
ReturnCode rfal_PollNFCA( rfalNfcaListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt);

/*!
 *****************************************************************************
 * \brief Poll NFC-B
 *
 * Configures the RFAL to NFC-B (ISO14443B) communication and polls for a nearby 
 * NFC-B device. 
 * If a device is found turns On a LED and logs its UID.
 * Additionally, if the Device supports ISO-DEP protocol (ISO144443-4) it will 
 * activate the device and try exchange some APDUs with PICC
 * 
 *  \return TRUE    : NFC-B device found
 *  \return FALSE   : No device found
 * 
 *****************************************************************************
 */
ReturnCode rfal_PollNFCB( rfalNfcbListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt );
/*!
 *****************************************************************************
 * \brief Poll ST25TB
 *
 * Configures the RFAL and polls for a nearby ST25TB device. 
 * If a device is found turns On a LED and logs its UID.
 * 
 *  \return TRUE    : ST25TB device found
 *  \return FALSE   : No device found
 * 
 *****************************************************************************
 */
ReturnCode rfal_PollST25TB( rfalSt25tbListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt );

/*!
 *****************************************************************************
 * \brief Poll NFC-F
 *
 * Configures the RFAL to NFC-F (FeliCa) communication and polls for a nearby 
 * NFC-F device. 
 * If a device is found turns On a LED and logs its UID.
 * Additionally, if the Device supports NFC-DEP protocol (P2P) it will 
 * activate the device and try to send an URI record
 * 
 *  \return TRUE    : NFC-F device found
 *  \return FALSE   : No device found
 * 
 *****************************************************************************
 */
ReturnCode rfal_PollNFCF( rfalNfcfListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt );

/*!
 *****************************************************************************
 * \brief Poll NFC-V
 *
 * Configures the RFAL to NFC-V (ISO15693) communication, polls for a nearby 
 * NFC-V device. If a device is found turns On a LED and logs its UID 
 *  
 * 
 *  \return TRUE    : NFC-V device found
 *  \return FALSE   : No device found
 * 
 *****************************************************************************
 */
ReturnCode rfal_PollNFCV( rfalNfcvListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt );

/*!
 *****************************************************************************
 * \brief Activate P2P
 *
 * Configures NFC-DEP layer and executes the NFC-DEP/P2P activation (ATR_REQ 
 * and PSL_REQ if applicable)
 *  
 * \param[in] nfcid      : nfcid to be used
 * \param[in] nfcidLen   : length of nfcid
 * \param[in] isActive   : Active or Passive communiccation
 * \param[out] nfcDepDev : If activation successful, device's Info
 * 
 *  \return ERR_PARAM    : Invalid parameters
 *  \return ERR_TIMEOUT  : Timeout error
 *  \return ERR_FRAMING  : Framing error detected
 *  \return ERR_PROTO    : Protocol error detected
 *  \return ERR_NONE     : No error, activation successful
 * 
 *****************************************************************************
 */
ReturnCode rfal_ActivateP2P( uint8_t* nfcid, uint8_t nfidLen, Bool isActive, rfalNfcDepDevice *nfcDepDev, uint8_t *Buff, uint8_t BuffLen);

/*!
 *****************************************************************************
 * \brief ISO-DEP Blocking Transceive 
 *
 * Helper function to send data in a blocking manner via the rfalIsoDep module 
 *  
 * \warning A protocol transceive handles long timeouts (several seconds), 
 * transmission errors and retransmissions which may lead to a long period of 
 * time where the MCU/CPU is blocked in this method.
 * This is a demo implementation, for a non-blocking usage example please 
 * refer to the Examples available with RFAL
 *
 *
 * \param[in]  isoDepDev  : device details retrived during activation
 * \param[in]  txBuf      : data to be transmitted
 * \param[in]  txBufSize  : size of the data to be transmited
 * \param[out] rxBuf      : buffer to place receive data
 * \param[in]  rxBufSize  : size of the reception buffer
 * \param[out] rxActLen   : number of data bytes received

 * 
 *  \return ERR_PARAM     : Invalid parameters
 *  \return ERR_TIMEOUT   : Timeout error
 *  \return ERR_FRAMING   : Framing error detected
 *  \return ERR_PROTO     : Protocol error detected
 *  \return ERR_NONE      : No error, activation successful
 * 
 *****************************************************************************
 */
ReturnCode rfal_IsoDepBlockingTxRx( rfalIsoDepDevice *isoDepDev, const uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxBuf, uint16_t rxBufSize, uint16_t *rxActLen );

/*!
 *****************************************************************************
 * \brief NFC-DEP Blocking Transceive 
 *
 * Helper function to send data in a blocking manner via the rfalNfcDep module 
 *  
 * \warning A protocol transceive handles long timeouts (several seconds), 
 * transmission errors and retransmissions which may lead to a long period of 
 * time where the MCU/CPU is blocked in this method.
 * This is a demo implementation, for a non-blocking usage example please 
 * refer to the Examples available with RFAL
 *
 * \param[in]  nfcDepDev  : device details retrived during activation
 * \param[in]  txBuf      : data to be transmitted
 * \param[in]  txBufSize  : size of the data to be transmited
 * \param[out] rxBuf      : buffer to place receive data
 * \param[in]  rxBufSize  : size of the reception buffer
 * \param[out] rxActLen   : number of data bytes received

 * 
 *  \return ERR_PARAM     : Invalid parameters
 *  \return ERR_TIMEOUT   : Timeout error
 *  \return ERR_FRAMING   : Framing error detected
 *  \return ERR_PROTO     : Protocol error detected
 *  \return ERR_NONE      : No error, activation successful
 * 
 *****************************************************************************
 */
ReturnCode rfal_NfcDepBlockingTxRx( rfalNfcDepDevice *nfcDepDev, const uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxBuf, uint16_t rxBufSize, uint16_t *rxActLen );


#ifdef __cplusplus
}
#endif

#endif /* __DEMO_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
