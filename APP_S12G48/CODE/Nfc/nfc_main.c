/*
 * nfc_main.c
 *
 *  Created on: Feb 15, 2022
 *      Author: Cristian Castro
 */

#include "nfc_main.h"
#include "UDS_Comms.h"
#include "PE_Types.h"
#include "config.h"
#include "CAN1.h"
#include "CAN_drv.h"
#include "CAN_cfg.h"


/*
 * *****************************************************************
 * GLOBAL VARIABLE
 * *****************************************************************
 */

/* RFAL Variable */
rfalNfcaListenDevice 	nfcaDev[1];
uint8_t					nfcaDevCnt;
rfalIsoDepDevice		IsoDevice;
Bool					NfcIsScanning = FALSE;
uint32_t				NfcScannerTimer = 0;
Bool					NfcCardDetected = FALSE;

/* MFDesfire Variable */
MFDF_Device_t			MfdfDevice;
MFDF_Key_t				Mfdf_DES_Key;
//MFDF_Key_t				Mfdf_DES_NewKey;
MFDF_DevInfo_t			Mfdf_DevInfo;

/* HCE Variable */
HCE_Device_t			HCEDevice;
HCE_File_ID				HCEAndroidAppID={0xF0, 0x01, 0x02, 0x03, 0x04, 0x05};
HCE_File_ID				HCEUserFileID={0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t					DataBuf[50];
uint8_t					DataBufID[16];
uint16_t				DataLen;

/*
 * *****************************************************************
 * LOCAL MACRO
 * *****************************************************************
 */




/*
 * *****************************************************************
 * LOCAL VARIABLE
 * *****************************************************************
 */
/* NFC debug Variables*/
static uint8_t debug_var;
static uint8_t nfcScanning_debug;
static uint8_t checkchip_debug;
static uint8_t testreading;
/*
 * *****************************************************************
 * EXTERNAL VARIABLE
 * *****************************************************************
 */
extern CAN_Signal_t CAN_Signals[MAX_NUM_SIGNALS];
extern CAN_Frame_t CAN_Frames[MAX_NUM_FRAMES];
/*
 * *****************************************************************
 * LOCAL FUNCTION
 * *****************************************************************
 */

/******************************************************************/
void NFC_init(void)
{
#ifdef IMPLEMENT_NFC
debug_var = 10;
	/* Wait for full system power-up */
	platformDelay( 250U );
	debug_var = 11;

	/* Initialize RFAL */
	rfalAnalogConfigInitialize();
	debug_var = 12;

	if(rfalInitialize() != ST_ERR_NONE )
	{
		debug_var = 13;
		/*NFC Trcv failure*/
		// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_failure = 4; /*RF Failure*/
	}
	
debug_var = 14;
	/*
	 * Antenna Calibration
	 */
#ifdef IMPLEMENT_ANTENNA_CALIBRATION	

	/* Turn field on */
	rfalFieldOnAndStartGT();

	/* Reset COP, use as a timeout */
	WDog_Clear();

	do{
		/* Send command to calibrate Antenna */
		st25r3911CalibrateAntenna(&NFC_AntCalibValue);
		platformDelay(100);

		/*
		 * MSB nibble of the result shows the active capacitor switches
		 */

	}while( NFC_AntCalibValue & ST25R3911_REG_ANT_CAL_RESULT_tri_err ); // Check for error during calibration

	/* Send command to measure phase between RFO and RFI */
	st25r3911MeasurePhase((uint8_t*)&NFC_AntPhaseValue);

	/* Turn field off */
	rfalFieldOff();
#endif

#else
		SM1_Disable( );
#endif
}
/******************************************************************/
void NFC_Task(void)
{
#ifdef IMPLEMENT_NFC
	if(NfcIsScanning == TRUE)
	{
		/* Check for scanner interval */
		if((platformGetSysTick()-NfcScannerTimer) > NFC_SCAN_TIME_ON)
		{
			NfcIsScanning = FALSE;
			NfcScannerTimer = platformGetSysTick();
			nfcScanning_debug = 10;
		}

		/* Polling for NFC-A Tag */	  
		if( rfal_PollNFCA(nfcaDev, 1, &nfcaDevCnt) == ST_ERR_NONE )
		{debug_var = 15;
			/* Initialise ISODep */
			rfalIsoDepInitialize();

			// /* IsoDep Activation  */
			if( rfalIsoDepPollAHandleActivation((rfalIsoDepFSxI)RFAL_ISODEP_FSDI_DEFAULT, RFAL_ISODEP_NO_DID, RFAL_BR_424, &IsoDevice) == ST_ERR_NONE )
			{debug_var = 16;
				/*
				 * =================================================================================================
				 * Check for DESFire tag
				 * =================================================================================================
				 */
				if( MFDF_DeviceInit(&MfdfDevice, &nfcaDev[0], &IsoDevice) == MFDF_ERR_NONE )
				{
					/*Desfire Tag Detected*/
					// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_nfc_tag_status = NFC_TAG_PASSIVE;
					/* Send CAN_Frame */
				CAN1_SendFrame(0, CAN_Frames[CompWLatchDiagRes].ID, DATA_FRAME, CAN_Frames[CompWLatchDiagRes].DLC, CAN_Frames[CompWLatchDiagRes].Data);
					debug_var = 3;

					/* Select Application */
					if( MFDF_AppSelect(&MfdfDevice, 1) == MFDF_ERR_NONE )
					{debug_var = 4;
						/* Authenticate with the application key */
						memset(Mfdf_DES_Key, 0x00, sizeof(Mfdf_DES_Key));

						if( MFDF_Authenticate(&MfdfDevice, MFDF_AUTHENTICATION_DES_NATIVE, 0, &Mfdf_DES_Key, MFDF_KEY_DES_2K3DES) == MFDF_ERR_NONE ) 
						{
							/* Read Data from File */
							memset(DataBuf, 0, sizeof(DataBuf));

							if( MFDF_ReadData(&MfdfDevice, 0, 0, 0, DataBuf, sizeof(DataBuf), &DataLen) == MFDF_ERR_NONE )
							{	debug_var = 5;						  
								if( strcmp((char*)DataBuf, "4DOOR1") == 0 )
								{
									debug_var = 4;
									/*Update Auth Status*/
									// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_nfc_tag_auth_status = NFC_AUTHENTICATION_SUCCEED;
									// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_key_id = 1;
								}
								else if( strcmp((char*)DataBuf, "4DOOR2") == 0 )
								{
									debug_var = 4;
									/*Update Auth Status*/
									// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_nfc_tag_auth_status = NFC_AUTHENTICATION_SUCCEED;
									// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_key_id = 2;
								}
								else
								{
									/*Tamper Attempt*/
									// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_nfc_tag_auth_status = NFC_AUTHENTICATION_TAMPER_ATTEMPT;
								}
							}
							else
							{
								/*Authentication Failed*/
								// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_nfc_tag_auth_status = NFC_AUTHENTICATION_FAILED;
							}
						}
						else
						{
							/*Authentication Failed*/
							// fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame.nfc_int_scu_sts_nfc_tag_auth_status = NFC_AUTHENTICATION_FAILED;
						}
					}
					else
					{
						/*Do Nothing*/
					}
					/*Send Status Frame*/
					// ComSend_interior_nfc_int_0x200_status_frame_frame();
				}
				/*
				 * =================================================================================================
				 * Check for HCE tag
				 * =================================================================================================
				 */

				if( HCE_DeviceInit(&HCEDevice, &nfcaDev[0], &IsoDevice) == HCE_ERR_NONE )
				{
					/* Select Android Application */
					if( HCE_SelectAndroidApp(&HCEDevice, &HCEAndroidAppID) == HCE_ERR_NONE )
					{
						/* Select User File */
						if( HCE_SelectFile(&HCEDevice, &HCEUserFileID) == HCE_ERR_NONE )
						{
							memset(DataBuf, 0, sizeof(DataBuf));

							/* Read User File */
							if( HCE_ReadFile(&HCEDevice, 0, sizeof(DataBuf), DataBuf, sizeof(DataBuf), (uint16_t*)&DataLen) == HCE_ERR_NONE )
							{
								if( (DataBuf[0] == '1') && (DataBuf[1] == 'L') )
								{
									/*Do Something*/			  
								}
								else
								{											  

								}
							}
						}	
					}
				}
			}
		}
		else
		{
			/*Error*/
			debug_var = 0;
		}
	}
	else
	{
		/* Check for scanner interval */
		if((platformGetSysTick()-NfcScannerTimer) > NFC_SCAN_TIME_OFF)
		{
			NfcIsScanning = TRUE;
			NfcScannerTimer = platformGetSysTick();
			nfcScanning_debug = 20;
		}

		/* Check if the Field is on, turn it off */
		if(st25r3911IsTxEnabled() == TRUE)
		{
			rfalFieldOff( );
		}
	}
#endif
}
/***************************************************************/
