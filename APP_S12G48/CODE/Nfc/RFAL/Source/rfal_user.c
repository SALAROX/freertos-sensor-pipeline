
/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "rfal_user.h"
#include "rfal_rf.h"

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/

/* Definition of possible states the demo state machine could have */
#define RFAL_ST_FIELD_OFF			        0
#define RFAL_ST_POLL_ACTIVE_TECH      		1
#define RFAL_ST_POLL_PASSIV_TECH      		2
#define RFAL_ST_WAIT_WAKEUP	          		3

#define RFAL_BUF_LEN                  		255
#define RFAL_NFCV_BLOCK_LEN           		4


/*
 ******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************
 */
rfalIsoDepApduTxRxParam  isoDepTxRx;
  
/*
 ******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************
 */
  
/*! Transmit buffers union, only one interface is used at a time                                                            */
static union{
    rfalIsoDepApduBufFormat  	isoDepTxBuf;                                /* ISO-DEP Tx buffer format (with header/prologue) */
    rfalNfcDepBufFormat      	nfcDepTxBuf;                                /* NFC-DEP Tx buffer format (with header/prologue) */
    uint8_t                		txBuf[RFAL_BUF_LEN];                        /* Generic buffer abstraction                      */    
}gTxBuf;


/*! Receive buffers union, only one interface is used at a time                                                             */
static union {
    rfalIsoDepApduBufFormat  	isoDepRxBuf;                                /* ISO-DEP Rx buffer format (with header/prologue) */
    rfalNfcDepBufFormat      	nfcDepRxBuf;                                /* NFC-DEP Rx buffer format (with header/prologue) */
    uint8_t                		rxBuf[RFAL_BUF_LEN];                        /* Generic buffer abstraction                      */
}gRxBuf;

static rfalIsoDepBufFormat   	tmpBuf;          /* Temporary buffer required for ISO-DEP APDU interface, I-Block interface does not */

  

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/


/*******************************************************************************/
ReturnCode rfal_PollAP2P( void )
{
  ReturnCode       err;
  Bool             try106 = FALSE;
  
  
  while (!try106)
  {
    /*******************************************************************************/
    /* NFC_ACTIVE_POLL_MODE                                                        */
    /*******************************************************************************/
    /* Initialize RFAL as AP2P Initiator NFC BR 424 */
    err = rfalSetMode(RFAL_MODE_POLL_ACTIVE_P2P, ((try106) ? RFAL_BR_106 : RFAL_BR_424), ((try106) ? RFAL_BR_106 : RFAL_BR_424));

    if (err != ST_ERR_NONE)
    {
      return err;
    }
    
    rfalSetErrorHandling(RFAL_ERRORHANDLING_NFC);
    rfalSetFDTListen(RFAL_FDT_LISTEN_AP2P_POLLER);
    rfalSetFDTPoll(RFAL_TIMING_NONE);
    
    rfalSetGT( RFAL_GT_AP2P_ADJUSTED );
    err = rfalFieldOnAndStartGT();
    
    /* AP2P at 424kb/s didn't found any device, try at 106kb/s */
    try106 = TRUE;
    rfalFieldOff();
  }

  return err;
}
/*******************************************************************************/
ReturnCode rfal_PollNFCA( rfalNfcaListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt )
{
  ReturnCode        err;
  rfalNfcaSensRes   sensRes;
  
  rfalNfcaPollerInitialize();   /* Initialise for NFC-A */
  rfalFieldOnAndStartGT();      /* Turns the Field On if not already and start GT timer */
 
 /*CC: Error Inside following method*/
  if( (err = rfalNfcaPollerTechnologyDetection( RFAL_COMPLIANCE_MODE_NFC, &sensRes )) != ST_ERR_NONE )
	  return err;
  
  err = rfalNfcaPollerFullCollisionResolution( RFAL_COMPLIANCE_MODE_NFC, DevLimit, DevList, DevCnt);
  
  return err;
}
/*******************************************************************************/
ReturnCode rfal_PollNFCB( rfalNfcbListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt )
{
  ReturnCode err;
  
  /*******************************************************************************/
  /* ISO14443B/NFC_B_PASSIVE_POLL_MODE                                           */
  /*******************************************************************************/

  rfalNfcbPollerInitialize();   /* Initialise for NFC-B */
  rfalFieldOnAndStartGT();      /* Turns the Field On if not already and start GT timer */

  
  err = rfalNfcbPollerCollisionResolution( RFAL_COMPLIANCE_MODE_NFC, DevLimit, DevList, DevCnt );
  
  return err;
}
/*******************************************************************************/
ReturnCode rfal_PollST25TB( rfalSt25tbListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt )
{
  ReturnCode	err;

  /*******************************************************************************/
  /* ST25TB_PASSIVE_POLL_MODE                                                    */
  /*******************************************************************************/  

  rfalSt25tbPollerInitialize();
  rfalFieldOnAndStartGT();

  if( (err = rfalSt25tbPollerCheckPresence(NULL)) != ST_ERR_NONE )
		return err;
		

  err = rfalSt25tbPollerCollisionResolution(DevLimit, DevList, DevCnt); 
  
  return err;
}
/*******************************************************************************/
ReturnCode rfal_PollNFCF( rfalNfcfListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt )
{
  ReturnCode            err;

  /*******************************************************************************/
  /* Felica/NFC_F_PASSIVE_POLL_MODE                                              */
  /*******************************************************************************/

  rfalNfcfPollerInitialize( RFAL_BR_212 ); /* Initialise for NFC-F */
  rfalFieldOnAndStartGT();                 /* Turns the Field On if not already and start GT timer */

  
  if( (err = rfalNfcfPollerCheckPresence()) != ST_ERR_NONE )
	  return err;
  
  err = rfalNfcfPollerCollisionResolution( RFAL_COMPLIANCE_MODE_NFC, DevLimit, DevList, DevCnt );

  return err;
}
/*******************************************************************************/
ReturnCode rfal_PollNFCV( rfalNfcvListenDevice *DevList, uint8_t DevLimit, uint8_t *DevCnt )
{
  ReturnCode            err;
  

  /*******************************************************************************/
  /* ISO15693/NFC_V_PASSIVE_POLL_MODE                                            */
  /*******************************************************************************/

  rfalNfcvPollerInitialize();           /* Initialise for NFC-V */
  rfalFieldOnAndStartGT();              /* Turns the Field On if not already and start GT timer */

  err = rfalNfcvPollerCollisionResolution(DevLimit, DevList, DevCnt);
  
  return err;
}
/*******************************************************************************/
ReturnCode rfal_ActivateP2P( uint8_t* nfcid, uint8_t nfidLen, Bool isActive, rfalNfcDepDevice *nfcDepDev, uint8_t *Buff, uint8_t BuffLen)
{
  rfalNfcDepAtrParam nfcDepParams;

  nfcDepParams.nfcid     = nfcid;
  nfcDepParams.nfcidLen  = nfidLen;
  nfcDepParams.BS        = RFAL_NFCDEP_Bx_NO_HIGH_BR;
  nfcDepParams.BR        = RFAL_NFCDEP_Bx_NO_HIGH_BR;
  nfcDepParams.LR        = RFAL_NFCDEP_LR_254;
  nfcDepParams.DID       = RFAL_NFCDEP_DID_NO;
  nfcDepParams.NAD       = RFAL_NFCDEP_NAD_NO;
  nfcDepParams.GBLen     = BuffLen;
  nfcDepParams.GB        = Buff;
  nfcDepParams.commMode  = ((isActive) ? RFAL_NFCDEP_COMM_ACTIVE : RFAL_NFCDEP_COMM_PASSIVE);
  nfcDepParams.operParam = (RFAL_NFCDEP_OPER_FULL_MI_EN | RFAL_NFCDEP_OPER_EMPTY_DEP_DIS | RFAL_NFCDEP_OPER_ATN_EN | RFAL_NFCDEP_OPER_RTOX_REQ_EN);
  
  /* Initialize NFC-DEP protocol layer */
  rfalNfcDepInitialize();
  
  /* Handle NFC-DEP Activation (ATR_REQ and PSL_REQ if applicable) */
  return rfalNfcDepInitiatorHandleActivation( &nfcDepParams, RFAL_BR_424, nfcDepDev );
}
/*******************************************************************************/
ReturnCode rfal_IsoDepBlockingTxRx( rfalIsoDepDevice *isoDepDev, const uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxBuf, uint16_t rxBufSize, uint16_t *rxActLen )
{
	ReturnCode               err;
//	rfalIsoDepApduTxRxParam  isoDepTxRx;
	
	/* Initialize the ISO-DEP protocol transceive context */
	isoDepTxRx.txBuf        = &gTxBuf.isoDepTxBuf;
	isoDepTxRx.txBufLen     = txBufSize;
	isoDepTxRx.DID          = isoDepDev->info.DID;
	isoDepTxRx.FWT          = isoDepDev->info.FWT;
	isoDepTxRx.dFWT         = isoDepDev->info.dFWT;
	isoDepTxRx.FSx          = isoDepDev->info.FSx;
	isoDepTxRx.ourFSx       = RFAL_ISODEP_FSX_KEEP;
	isoDepTxRx.rxBuf        = &gRxBuf.isoDepRxBuf;
	isoDepTxRx.rxLen        = rxActLen;
	isoDepTxRx.tmpBuf       = &tmpBuf;
	
	
	/* Copy data to send */
	ST_MEMMOVE( gTxBuf.isoDepTxBuf.apdu, txBuf, MIN( txBufSize, RFAL_ISODEP_APDU_MAX_LEN ) );
	
	/* Perform the ISO-DEP Transceive in a blocking way */
	rfalIsoDepStartApduTransceive( isoDepTxRx );
	do {
	rfalWorker();
	err = rfalIsoDepGetApduTransceiveStatus();
	} while(err == ST_ERR_BUSY);
	
	if( err != ST_ERR_NONE )
	{
	return err;
	}
	
	/* Copy received data */
	ST_MEMMOVE( rxBuf, isoDepTxRx.rxBuf->apdu, MIN(*rxActLen, rxBufSize) );
	return ST_ERR_NONE;
}
/*******************************************************************************/
ReturnCode rfal_NfcDepBlockingTxRx( rfalNfcDepDevice *nfcDepDev, const uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxBuf, uint16_t rxBufSize, uint16_t *rxActLen )
{
  ReturnCode             err;
  Bool                   isChaining;
  rfalNfcDepTxRxParam    rfalNfcDepTxRx;


  /* Initialize the NFC-DEP protocol transceive context */
  rfalNfcDepTxRx.txBuf        = &gTxBuf.nfcDepTxBuf;
  rfalNfcDepTxRx.txBufLen     = txBufSize;
  rfalNfcDepTxRx.rxBuf        = &gRxBuf.nfcDepRxBuf;
  rfalNfcDepTxRx.rxLen        = rxActLen;
  rfalNfcDepTxRx.DID          = RFAL_NFCDEP_DID_NO;
  rfalNfcDepTxRx.FSx          = rfalNfcDepLR2FS( rfalNfcDepPP2LR( nfcDepDev->activation.Target.ATR_RES.PPt ) );
  rfalNfcDepTxRx.FWT          = nfcDepDev->info.FWT;
  rfalNfcDepTxRx.dFWT         = nfcDepDev->info.dFWT;
  rfalNfcDepTxRx.isRxChaining = &isChaining;
  rfalNfcDepTxRx.isTxChaining = FALSE;
  
  /* Copy data to send */
  ST_MEMMOVE( gTxBuf.nfcDepTxBuf.inf, txBuf, MIN( txBufSize, RFAL_NFCDEP_FRAME_SIZE_MAX_LEN ) );

  /* Perform the NFC-DEP Transceive in a blocking way */
  rfalNfcDepStartTransceive( &rfalNfcDepTxRx );
  
  do {
    rfalWorker();
    err = rfalNfcDepGetTransceiveStatus();
  } while(err == ST_ERR_BUSY);
  
  if( err != ST_ERR_NONE )
  {
    return err;
  }
  
  /* Copy received data */
  ST_MEMMOVE( rxBuf, gRxBuf.nfcDepRxBuf.inf, MIN(*rxActLen, rxBufSize) );
  return ST_ERR_NONE;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
