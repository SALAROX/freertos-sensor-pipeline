/*
 * Comms.c
 *
 *  Created on: Mar 13, 2013
 *      Author: B34981
 *      
 *  SCI Communications Driver for MagniV Bootloader 
 *  IP Versoin: SCIV5
 *  Channel: 1 (SCI1)
 *  BaudRate: 19200
 *  
 */

/**********************************************************************************************
 * Includes
 **********************************************************************************************/
#include "UDS_Comms.h"
#include "PE_Types.h"
#include "Queue.h"
#include "config.h"
#include "CAN1.h"
#include "IEE1.h"
#include "CAN_drv.h"
#include "CAN_cfg.h"
#include "PFlash.h"
/**********************************************************************************************
 * External objects
 **********************************************************************************************/

/**********************************************************************************************
 * Local function prototypes
 *********************************************************************************************/

/**********************************************************************************************
 * Global variables
 **********************************************************************************************/
static UINT8 u8LIN_Byte_Counter = 0;
static UINT8 u8phrase_counter = 0;
static CAN_FRAME t_CAN_frame;
static CAN_FRAME t_CAN_Tx_frame;
extern UINT8 u8BootPhraseRcvd;
extern DIAG_STATUS_t Diag_State;
extern DIAG_REQUEST_STATUS_t Request_Status;
extern volatile MEMORY_STATUS_t Memory_Status;
extern uint32_t sysTickTimer;

extern UINT8 Boot_FLag;
extern volatile BL_SM_t blState;
extern uint8_t q[QSIZE];
extern uint8_t head, tail;
extern uint32_t LIN_timeoutTimer;
extern volatile COMM_MODE_t Comm_Mode;
extern uint8_t failure_PDU_SID;

/* START Bootloader handler Variables */
extern byte byteBuf;
extern uint32_t ackTimer;
extern uint32_t LIN_timeoutTimer;
PFlash_TAddress flashAddress;

volatile uint32_t sRecByteCounter;
volatile uint16_t sRecDataIndex;
volatile uint8_t sRecDataIndexNibble;
volatile bool sRecScan;
volatile bool sRecDataStart;
volatile uint8_t sRecType;
volatile uint8_t sRecSize;
volatile uint32_t sRecAddr;
volatile uint32_t Prev_sRecAddr;
uint8_t sRecData[S_REC_BUF_SIZE];
uint32_t appVector;
uint8_t silentBusCounter;

extern BL_Repository_t BL_Repository;
AppEntry_t appEntry;

bool BL_PROG_Enable = FALSE;

/*Checksum variables*/
uint8_t checksum_SRecSize = 0;
uint8_t checksum_SRecAddr[2];
uint8_t checksum_SRecChecksum;

/* END Bootloader Handler Variables*/
/*const Byte	Boot_Version = BL_VERSION;*/
/**********************************************************************************************
 * Constants and macros
 **********************************************************************************************/
#define BAUD_RATE_VALUE 25000000 / BAUD_RATE

/**********************************************************************************************
 * Local types
 **********************************************************************************************/

/**********************************************************************************************
 * Local variables
 **********************************************************************************************/
UINT8 NodeAddress;
UINT8 current_PDU_SID;
UINT8 last_PDU_SID;
bool udsReqTimeOut_Flag = FALSE;

extern CAN_Frame_t CAN_Frames[MAX_NUM_FRAMES];
/**********************************************************************************************
 * Local functions
 **********************************************************************************************/

uint8_t lin_checksum(uint8_t pid, uint8_t lenght, const uint8_t *data_ptr);

/*Calculates checksum for each line of the SRec File*/
uint8_t can_checksum(uint8_t lenght, const uint8_t *address, const uint8_t *data_ptr);
/**********************************************************************************************
 * Global functions
 **********************************************************************************************/

/***********************************************************************************************
 *
 * @brief    Comms_Init - Initialize the serial communications driver. Designed for SCI0 - LIN phy
 * @param    none
 * @return   none
 *
 ************************************************************************************************/
void UDS_Comms_Init()
{
}

/***********************************************************************************************
 *
 * @brief    UDS_Diagnostics_Handler - It handles UDS Requests and Responses
 * @param    none
 * @return   none
 *
 ************************************************************************************************/
void UDS_Diagnostics_Handler(void)
{
	switch (Diag_State)
	{
	case DIAG_IDLE:
		udsReqTimeOut_Flag = FALSE;
		UDS_Comms_CAN();
		break;

	case DIAG_REQ_RECEIVED:
		Request_Status = IN_PROGRESS;
		UDS_process_request();
		Diag_State = DIAG_REQ_PROCESSING;
		break;
	case DIAG_REQ_PROCESSING:
		if (Request_Status == READY)
		{
			UDS_process_response();
		}
		break;
	case DIAG_RESPONSE_READY:
		UDS_Comms_CAN();
		break;

	default:
		break;
	}
}
/***********************************************************************************************
 *
 * @brief    Comms_Rx_Data - Poll or Interrupt that receives LIN frame.
 * @param    none
 * @return   none (Data must fill the BP structure, when BP is fill, update u8BootPhraseRcvd)
 *
 ************************************************************************************************/
void UDS_Comms_CAN()
{
	UINT8 u8counter;
	UINT8 D_count = 0;
	UINT8 checksum_buffer[8];

	if ((Diag_State == DIAG_IDLE) && (CAN_Frames[CompWLatchDiagReq].DLC != 0))
	{
		/* Move Data to UDS CAN Frame from CAN Driver Frame*/
		// t_CAN_frame.CAN_NAD = CAN_Frames[CompWLatchDiagReq].Data[0];
		t_CAN_frame.CAN_PCI = CAN_Frames[CompWLatchDiagReq].Data[0];

		if (1 /*(t_CAN_frame.CAN_NAD == 0x7F) || (t_CAN_frame.CAN_NAD == NodeAddress)*/)
		{
			if (t_CAN_frame.CAN_PCI < 0x08)
			{
				udsReqTimeOut_Flag = TRUE;

				t_CAN_frame.CAN_SID = CAN_Frames[CompWLatchDiagReq].Data[1];

				for (D_count = 0; D_count < t_CAN_frame.CAN_PCI; D_count++)
				{
					t_CAN_frame.CAN_data[D_count] = CAN_Frames[CompWLatchDiagReq].Data[D_count + 2];
				}
			}
			else if ((t_CAN_frame.CAN_PCI == 0x10) && (failure_PDU_SID == 0))
			{

				last_PDU_SID = 0;
				t_CAN_frame.CAN_TP_Byte_Len = (CAN_Frames[CompWLatchDiagReq].Data[1]); /* Discard Checksum*/
				t_CAN_frame.CAN_SID = CAN_Frames[CompWLatchDiagReq].Data[2];

				for (D_count = 0; D_count < 5; D_count++) /*TODO: Return to 5 once app is fixed*/
				{
					t_CAN_frame.CAN_data[D_count] = CAN_Frames[CompWLatchDiagReq].Data[D_count + 3];
				}

				/*Confirm Reception of Consecutive Frame*/

				/* Positive Response */
				// t_CAN_Tx_frame.CAN_SID = 0x40 + 0xB8;
				// t_CAN_Tx_frame.CAN_PCI = 0x01;

				CAN_Frames[CompWLatchDiagRes].Data[0] = 0x30; /* Flow Control Frame -> Allows ECU to continue sending frames*/
				CAN_Frames[CompWLatchDiagRes].Data[1] = 0x01; /* Block Size  -> Max 1 frame*/
				CAN_Frames[CompWLatchDiagRes].Data[2] = 0x01; /* Delay between frames (STmin)*/
				CAN_Frames[CompWLatchDiagRes].Data[3] = t_CAN_Tx_frame.CAN_data[1];
				CAN_Frames[CompWLatchDiagRes].Data[4] = t_CAN_Tx_frame.CAN_data[2];
				CAN_Frames[CompWLatchDiagRes].Data[5] = t_CAN_Tx_frame.CAN_data[3];
				CAN_Frames[CompWLatchDiagRes].Data[6] = t_CAN_Tx_frame.CAN_data[4];
				CAN_Frames[CompWLatchDiagRes].Data[7] = t_CAN_Tx_frame.CAN_data[5];

				/* Send CAN_Frame */
				CAN1_SendFrame(0, CAN_Frames[CompWLatchDiagRes].ID, DATA_FRAME, CAN_Frames[CompWLatchDiagRes].DLC, CAN_Frames[CompWLatchDiagRes].Data);

				/* Reset Frames */
				for (D_count = 0; D_count < CAN_Frames[CompWLatchDiagRes].DLC; D_count++)
				{
					CAN_Frames[CompWLatchDiagRes].Data[D_count] = 0xFF;
				}
				for (D_count = 0; D_count < t_CAN_Tx_frame.CAN_data[CAN_MAX_DATA_BYTES]; D_count++)
				{
					t_CAN_Tx_frame.CAN_data[D_count] = 0xFF;
				}
			}
			else if ((t_CAN_frame.CAN_PCI > 0x20) && (failure_PDU_SID == 0) && BL_PROG_Enable)
			{
				current_PDU_SID = t_CAN_frame.CAN_PCI;
				if ((last_PDU_SID == 0) || (current_PDU_SID == (last_PDU_SID + 1)))
				{
					t_CAN_frame.CAN_SID = CONSECUTIVE_FRAME;
					for (D_count = 0; D_count < CAN_MAX_DATA_BYTES; D_count++)
					{
						t_CAN_frame.CAN_data[D_count] = CAN_Frames[CompWLatchDiagReq].Data[D_count + 1];
					}
					last_PDU_SID = current_PDU_SID;

					/*According to ISO-15765*/
					if (last_PDU_SID == 0x2F)
					{
						last_PDU_SID = 0x20; /*Reset Loop*/
					}

					/*Flow Control Frame*/
					CAN_Frames[CompWLatchDiagRes].Data[0] = 0x30; /* Flow Control Frame -> Allows ECU to continue sending frames*/
					CAN_Frames[CompWLatchDiagRes].Data[1] = 0x01; /* Block Size  -> Max 1 frame*/
					CAN_Frames[CompWLatchDiagRes].Data[2] = 0x01; /* Delay between frames (STmin)*/
					CAN_Frames[CompWLatchDiagRes].Data[3] = t_CAN_Tx_frame.CAN_data[1];
					CAN_Frames[CompWLatchDiagRes].Data[4] = t_CAN_Tx_frame.CAN_data[2];
					CAN_Frames[CompWLatchDiagRes].Data[5] = t_CAN_Tx_frame.CAN_data[3];
					CAN_Frames[CompWLatchDiagRes].Data[6] = t_CAN_Tx_frame.CAN_data[4];
					CAN_Frames[CompWLatchDiagRes].Data[7] = t_CAN_Tx_frame.CAN_data[5];

					/* Send CAN_Frame */
					CAN1_SendFrame(0, CAN_Frames[CompWLatchDiagRes].ID, DATA_FRAME, CAN_Frames[CompWLatchDiagRes].DLC, CAN_Frames[CompWLatchDiagRes].Data);

					/* Reset Frames */
					for (D_count = 0; D_count < CAN_Frames[CompWLatchDiagRes].DLC; D_count++)
					{
						CAN_Frames[CompWLatchDiagRes].Data[D_count] = 0xFF;
					}
					for (D_count = 0; D_count < t_CAN_Tx_frame.CAN_data[CAN_MAX_DATA_BYTES]; D_count++)
					{
						t_CAN_Tx_frame.CAN_data[D_count] = 0xFF;
					}
				}
				else
				{
					failure_PDU_SID = 1;
					current_PDU_SID = 0;
					last_PDU_SID = 0;
					blState = BL_SM_IDLE;
					Diag_State = DIAG_IDLE;
					Memory_Status = CORRUPTED;
					if (Comm_Mode == COMM_CAN)
					{
						BL_PROG_Enable = FALSE;
					}
				}
			}
			else
			{
				/* Do Nothing */
			}

			Diag_State = DIAG_REQ_RECEIVED;
		}
		/* Reset Entire Rx Frame */
		for (D_count = 0; D_count < CAN_Frames[CompWLatchDiagReq].DLC; D_count++)
		{
			CAN_Frames[CompWLatchDiagReq].Data[D_count] = 0;
		}
		CAN_Frames[CompWLatchDiagReq].DLC = 0;
	}
	else if ((Diag_State == DIAG_RESPONSE_READY))
	{
		/* Move Data from UDS CAN_Frame to CAN driver Frame*/
		// CAN_Frames[CompWLatchDiagRes].Data[0] = 0x7F;
		CAN_Frames[CompWLatchDiagRes].Data[0] = t_CAN_Tx_frame.CAN_PCI;
		CAN_Frames[CompWLatchDiagRes].Data[1] = t_CAN_Tx_frame.CAN_SID;
		CAN_Frames[CompWLatchDiagRes].Data[2] = t_CAN_Tx_frame.CAN_data[0];
		CAN_Frames[CompWLatchDiagRes].Data[3] = t_CAN_Tx_frame.CAN_data[1];
		CAN_Frames[CompWLatchDiagRes].Data[4] = t_CAN_Tx_frame.CAN_data[2];
		CAN_Frames[CompWLatchDiagRes].Data[5] = t_CAN_Tx_frame.CAN_data[3];
		CAN_Frames[CompWLatchDiagRes].Data[6] = t_CAN_Tx_frame.CAN_data[4];
		CAN_Frames[CompWLatchDiagRes].Data[7] = t_CAN_Tx_frame.CAN_data[5];

		/* Send CAN_Frame */
		CAN1_SendFrame(0, CAN_Frames[CompWLatchDiagRes].ID, DATA_FRAME, CAN_Frames[CompWLatchDiagRes].DLC, CAN_Frames[CompWLatchDiagRes].Data);

		/* Reset Frames */
		for (D_count = 0; D_count < CAN_Frames[CompWLatchDiagRes].DLC; D_count++)
		{
			CAN_Frames[CompWLatchDiagRes].Data[D_count] = 0xFF;
		}
		for (D_count = 0; D_count < t_CAN_Tx_frame.CAN_data[CAN_MAX_DATA_BYTES]; D_count++)
		{
			t_CAN_Tx_frame.CAN_data[D_count] = 0xFF;
		}
		Diag_State = DIAG_IDLE;
	}
	else
	{
		/* Do Nothing */
	}
}

void UDS_process_request(void)
{

	uint8_t i;
	uint8_t copy_size;
	uint8_t byte_cmd;

	switch (t_CAN_frame.CAN_SID)
	{
	case FIRST_FRAME:

		if (blState == BL_SM_IDLE) /* for the start of S19, first first frame */
		{
			byte_cmd = BL_CMD_PROGRAM;
			push(byte_cmd);
			LIN_timeoutTimer = sysTickTimer;
		}

		t_CAN_frame.CAN_TP_Byte_Cnt = t_CAN_frame.CAN_TP_Byte_Len;

		if (t_CAN_frame.CAN_TP_Byte_Len >= 5) /*TODO: Return to 5 once app is fixed*/
		{
			copy_size = 5; /*TODO: Return to 5 once app is fixed*/
		}
		else
		{
			copy_size = t_CAN_frame.CAN_TP_Byte_Len;
		}

		for (i = 0; i < copy_size; i++)
		{
			if (push(t_CAN_frame.CAN_data[i]))
			{
			}
			t_CAN_frame.CAN_TP_Byte_Cnt--;
		}

		Request_Status = READY;

		break;
	case CONSECUTIVE_FRAME:
		LIN_timeoutTimer = sysTickTimer;

		if (t_CAN_frame.CAN_TP_Byte_Cnt >= 7)
		{
			copy_size = 7;
		}
		else
		{
			copy_size = t_CAN_frame.CAN_TP_Byte_Cnt;
		}

		for (i = 0; i < copy_size; i++)
		{
			if (!push(t_CAN_frame.CAN_data[i]))
			{
			}

			t_CAN_frame.CAN_TP_Byte_Cnt--;
		}

		if (t_CAN_frame.CAN_TP_Byte_Cnt <= 1)
		{
		}

		Request_Status = READY;

		break;
	case ERASE_MEMORY_REQUEST:
		byte_cmd = BL_CMD_ERASE_FULL;
		push(byte_cmd);
		break;
	case WRITE_BY_ID:
		if ((t_CAN_frame.CAN_data[0] == 0x00) && (t_CAN_frame.CAN_data[1] == 0xAA))
		{ //SET
			IEE1_SetByte(CAN_FLAG_ADDRESS, 0x01);
		}
		else if ((t_CAN_frame.CAN_data[0] == 0x00) && (t_CAN_frame.CAN_data[1] != 0xAA) && ((Memory_Status == UNTOUCHED) || (Memory_Status == PROGRAMMED)))
		{ //CLEAR
			IEE1_SetByte(CAN_FLAG_ADDRESS, 0x00);
		}
		while (IEE1_Busy())
			;
		Request_Status = READY;
		break;
	case READ_MEMORY_STATUS:
	case RESET_REQUEST:
	case READ_BY_ID:
		Request_Status = READY;
		break;

	case READ_BOOTLOADER_VERSION:
		Request_Status = READY;
		break;

	case CLEAR_BOOT_FLAG:
		if ((Memory_Status == UNTOUCHED) || (Memory_Status == PROGRAMMED))
		{
			IEE1_SetByte(CAN_FLAG_ADDRESS, 0x00);
			while (IEE1_Busy())
				;
		}
		else
		{
			/*Do Nothing*/
		}
		Request_Status = READY;
		break;

	case SID_START_APPLICATION:
		if ((Memory_Status == UNTOUCHED) || (Memory_Status == PROGRAMMED))
		{
			blState = BL_SM_RUN_APP;
		}
		else
		{
			Request_Status = READY;
		}
		break;
	default:
		Request_Status = READY;
		break;
	}
}

void UDS_process_response(void) /*ISO 14229-1-2013 Page 20*/
{
	UINT8 i;
	IEE1_TAddress tempAddress;

	switch (t_CAN_frame.CAN_SID)
	{
	case FIRST_FRAME:
	case CONSECUTIVE_FRAME:
		/* no response expected, get ready to receive next frame  */
		Diag_State = DIAG_IDLE;

		break;
	case ERASE_MEMORY_REQUEST:
		if (Memory_Status == ERASED)
		{
			/* Positive Response */
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;
			t_CAN_Tx_frame.CAN_PCI = 0x01;
			t_CAN_Tx_frame.CAN_data[0] = 0x00;
			t_CAN_Tx_frame.CAN_data[1] = 0x00;
		}
		else
		{
			/* Negative Response */
			t_CAN_Tx_frame.CAN_SID = 0x7F;
			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_data[0] = ERASE_MEMORY_REQUEST;
			t_CAN_Tx_frame.CAN_data[1] = 0x23; /*Routine Not Complete*/
		}
		Diag_State = DIAG_RESPONSE_READY;
		break;
	case READ_MEMORY_STATUS:
		t_CAN_Tx_frame.CAN_PCI = 0x02;
		t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;
		t_CAN_Tx_frame.CAN_data[0] = Memory_Status;

		Diag_State = DIAG_RESPONSE_READY;
		break;
	case RESET_REQUEST:
		if ((Memory_Status == ERASING) || (Memory_Status == PROGRAMMING))
		{ /* NRC */
			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_SID = 0x7F;
			t_CAN_Tx_frame.CAN_data[0] = RESET_REQUEST;
			t_CAN_Tx_frame.CAN_data[1] = 0x22; /*Neg Resp - Conditions not Correct */
		}
		else
		{
			t_CAN_Tx_frame.CAN_PCI = 0x02;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;
			t_CAN_Tx_frame.CAN_data[0] = 0x01;

			/* reset MCU by WDT */
			CPMUCOP = 0x01;	   // enable COP
			CPMUARMCOP = 0x00; // reset MCU immediately by means of COP
		}
		Diag_State = DIAG_RESPONSE_READY;
		break;

	case WRITE_BY_ID:
		IEE1_GetByte(CAN_FLAG_ADDRESS, &Boot_FLag);
		while (IEE1_Busy())
			;
		if ((Boot_FLag == 0x00) || (Boot_FLag == 0x01))
		{
			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;
			t_CAN_Tx_frame.CAN_data[0] = 0x00;

			t_CAN_Tx_frame.CAN_data[1] = Boot_FLag;
		}
		else
		{
			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_SID = 0x7F;
			t_CAN_Tx_frame.CAN_data[0] = WRITE_BY_ID;

			t_CAN_Tx_frame.CAN_data[1] = Boot_FLag;
		}
		Diag_State = DIAG_RESPONSE_READY;
		break;

	case READ_BY_ID:
		switch (t_CAN_frame.CAN_data[0])
		{
		case READ_BY_ID_CAN_P_ID:

			t_CAN_Tx_frame.CAN_PCI = 0x06;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;

			t_CAN_Tx_frame.CAN_data[0] = 0x42;
			t_CAN_Tx_frame.CAN_data[1] = 0x45;

			tempAddress = FUNCTION_ID_ADDRESS_L;
			for (i = 2; i < (t_CAN_frame.CAN_PCI - 1); i++)
			{
				IEE1_GetByte(tempAddress, &t_CAN_Tx_frame.CAN_data[i]);
				while (IEE1_Busy())
					;
				tempAddress++;
			}

			Diag_State = DIAG_RESPONSE_READY;

			break;

		case READ_BY_ID_SERIAL:
			t_CAN_Tx_frame.CAN_PCI = 0x05;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;

			tempAddress = SERIAL_NUM_ADDRESS_MSB;

			for (i = 0; i < (t_CAN_frame.CAN_PCI - 1); i++)
			{
				IEE1_GetByte(tempAddress, &t_CAN_Tx_frame.CAN_data[i]);
				while (IEE1_Busy())
					;
				tempAddress++;
			}

			Diag_State = DIAG_RESPONSE_READY;
			break;
		case READ_BY_ID_BOOT_FLAG:
			t_CAN_Tx_frame.CAN_PCI = 0x02;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;

			IEE1_GetByte(CAN_FLAG_ADDRESS, &Boot_FLag);
			while (IEE1_Busy())
				;

			t_CAN_Tx_frame.CAN_data[0] = Boot_FLag;
			Diag_State = DIAG_RESPONSE_READY;
			break;
		case READ_BY_ID_BOOT_VER:
			/*			IEE1_GetByte(BOOT_VERSION_ADDRESS, &t_LIN_frame.LIN_data[0]);
			while(IEE1_Busy());*/

			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;
			t_CAN_Tx_frame.CAN_data[0] = (uint8_t)(SW_VERSION & 0xFF);
			t_CAN_Tx_frame.CAN_data[1] = (uint8_t)((SW_VERSION >> 8) & 0xFF);
			Diag_State = DIAG_RESPONSE_READY;
			break;
		case READ_BY_ID_APP_VER:
			/* TODO: Check this, it seems to be pointing to BL addresses*/
			IEE1_GetByte(BOOT_VERSION_ADDRESS, &t_CAN_Tx_frame.CAN_data[0]);
			while (IEE1_Busy())
				;
			IEE1_GetByte(SERIAL_NUM_ADDRESS_BYTE_3, &t_CAN_Tx_frame.CAN_data[1]);
			while (IEE1_Busy())
				;
			Diag_State = DIAG_RESPONSE_READY;
			break;
			case READ_BY_ID_DID_HALLSENSORS:
			t_CAN_Tx_frame.CAN_PCI = 0x06;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;
			t_CAN_Tx_frame.CAN_data[0] = (PT01AD) & ((uint8_t)0x0080); //External Lever Bit7
			t_CAN_Tx_frame.CAN_data[1] = (PT01AD) & ((uint8_t)0x0040); //Internal Lever Bit6
			t_CAN_Tx_frame.CAN_data[2] = (PT01AD) & ((uint8_t)0x0010); //Claw A North
			t_CAN_Tx_frame.CAN_data[3] = (PT01AD) & ((uint8_t)0x0020); //Claw A South
			t_CAN_Tx_frame.CAN_data[4] = (PTS) & ((uint8_t)0x0010); //Claw B North
			t_CAN_Tx_frame.CAN_data[5] = (PTS) & ((uint8_t)0x0020); //Claw B South
			Diag_State = DIAG_RESPONSE_READY;
			break;
		default: /* Negative Response */
			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_SID = 0x7F;
			t_CAN_Tx_frame.CAN_data[0] = t_CAN_frame.CAN_SID;
			t_CAN_Tx_frame.CAN_data[1] = 0x12; /*Neg Res - Sub function not supported*/

			Diag_State = DIAG_RESPONSE_READY;
			break;
		}
		break;

	case CLEAR_BOOT_FLAG:
		IEE1_GetByte(CAN_FLAG_ADDRESS, &Boot_FLag);
		while (IEE1_Busy())
			;
		if ((Boot_FLag == 0x00))
		{
			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_SID = 0x40 + t_CAN_frame.CAN_SID;
			t_CAN_Tx_frame.CAN_data[0] = 0x00;

			t_CAN_Tx_frame.CAN_data[1] = Boot_FLag;
		}
		else
		{
			t_CAN_Tx_frame.CAN_PCI = 0x03;
			t_CAN_Tx_frame.CAN_SID = 0x7F;
			t_CAN_Tx_frame.CAN_data[0] = CLEAR_BOOT_FLAG;
			t_CAN_Tx_frame.CAN_data[1] = Boot_FLag;
		}
		Diag_State = DIAG_RESPONSE_READY;
		break;

	default: /* Negative Response Wrong SID */
		t_CAN_Tx_frame.CAN_PCI = 0x03;
		t_CAN_Tx_frame.CAN_SID = 0x7F;
		t_CAN_Tx_frame.CAN_data[0] = t_CAN_frame.CAN_SID; /*Wrong SID*/
		t_CAN_Tx_frame.CAN_data[1] = 0x11;				  /*Neg Res - SID Not Supported*/
		Diag_State = DIAG_RESPONSE_READY;
		break;
	}
}

uint8_t can_checksum(uint8_t lenght, const uint8_t *address, const uint8_t *data_ptr)
{
	uint8_t i = 0;
	uint16_t result = 0;

	result += lenght;

	if ((lenght == 0) || lenght > 0x23)
	{
		return 0;
	}
	else
	{
		for (i = 0; i < sizeof(address); i++)
		{
			result += address[i];
		}
		for (i = 0; i < (lenght - 3); i++) //-3-> Remove Checksum(1Byte) and Address(2bytes)
		{
			result += data_ptr[i];
		}
		result &= 0x00FF;
		result = 0xFF - result;
		return (uint8_t)(result);
	}
}

void BL_Prog_Handler(void)
{
#if 1

	if (IO_readByte(&byteBuf) == ERR_OK)
	{
		if (byteBuf == 'S')
		{
			ackTimer = sysTickTimer;

			sRecByteCounter = 0;
			sRecScan = TRUE;
			sRecDataStart = FALSE;
		}

		else if (sRecScan == TRUE)
		{

			sRecByteCounter++;

			/* Parse type */
			if (sRecByteCounter == 1)
			{
				sRecType = bcdToBinary(byteBuf);

				/* Check record type validity */
				if (sRecType > 9)
				{
					sRecScan = FALSE;
					/*continue;*/
					if (Comm_Mode == COMM_SERIAL)
					{
						return;
					}
				}
				else if (sRecType == 0) /* Ignore Header  for now*/
				{
					sRecScan = FALSE;
					return;
				}
			}
			/* Parse size */
			else if (sRecByteCounter == 2)
			{
				sRecSize = bcdToBinary(byteBuf);
			}
			else if (sRecByteCounter == 3)
			{
				sRecSize <<= 4;
				sRecSize |= bcdToBinary(byteBuf);

				/* Check minimum record size */
				if (sRecSize < 3)
				{
					sRecScan = FALSE;
					/*continue;*/
					if (Comm_Mode == COMM_SERIAL)
					{
						return;
					}
				}
				else
				{
					/*SRec Line length for Checksum*/
					checksum_SRecSize = sRecSize;
				}
			}
			else
			{

				switch (sRecType)
				{
				/* 16-bit address */
				case 0:
				case 1:
				case 5:
				case 9:

					/* Parse address */
					if (sRecByteCounter == 4)
					{
						Prev_sRecAddr = sRecAddr;
						sRecAddr = bcdToBinary(byteBuf);
					}
					else if ((sRecByteCounter > 4) && (sRecByteCounter < 7))
					{
						sRecAddr <<= 4;
						sRecAddr |= bcdToBinary(byteBuf);
					}
					else if (sRecByteCounter == 7)
					{
						sRecAddr <<= 4;
						sRecAddr |= bcdToBinary(byteBuf);

						sRecSize -= 2; // 2 byte address
						sRecDataIndex = 0;
						sRecDataStart = TRUE;

						/* Prepare SRecAddress for checksum calculation */
						checksum_SRecAddr[0] = (uint8_t)((sRecAddr >> 8) & 0xFF);
						checksum_SRecAddr[1] = (uint8_t)((sRecAddr)&0xFF);

						if (sRecAddr == APP_INTERRUPTRESETVECTORSTART)
						{
							Prev_sRecAddr = 0;
						}

						/*continue;*/
						if (Comm_Mode == COMM_SERIAL)
						{
							return;
						}
					}

					break;

					/* 24-bit address */
				case 2:
				case 6:
				case 8:

					/* Parse address */
					if (sRecByteCounter == 4)
					{
						Prev_sRecAddr = sRecAddr;
						sRecAddr = bcdToBinary(byteBuf);
					}

					else if ((sRecByteCounter > 4) && (sRecByteCounter < 9))
					{
						sRecAddr <<= 4;
						sRecAddr |= bcdToBinary(byteBuf);
					}
					else if (sRecByteCounter == 9)
					{
						sRecAddr <<= 4;
						sRecAddr |= bcdToBinary(byteBuf);

						sRecSize -= 3; // 3 bytes address
						sRecDataIndex = 0;
						sRecDataStart = TRUE;

						/*continue;*/
						if (Comm_Mode == COMM_SERIAL)
						{
							return;
						}
					}

					break;

					/* 32-bit address */
				case 3:
				case 7:

					/* Parse address */
					if (sRecByteCounter == 4)
					{
						Prev_sRecAddr = sRecAddr;
						sRecAddr = bcdToBinary(byteBuf);
					}
					else if ((sRecByteCounter > 4) && (sRecByteCounter < 11))
					{
						sRecAddr <<= 4;
						sRecAddr |= bcdToBinary(byteBuf);
					}
					else if (sRecByteCounter == 11)
					{
						sRecAddr <<= 4;
						sRecAddr |= bcdToBinary(byteBuf);

						sRecSize -= 4; // 4 Bytes address
						sRecDataIndex = 0;
						sRecDataStart = TRUE;
						/*continue;*/
						if (Comm_Mode == COMM_SERIAL)
						{
							return;
						}
					}

					break;

				default:
					break;
				}
				/*(sRecByteCounter > 7) write this in a smarter way! to be Srec type dependat*/
				if (((sRecDataStart == TRUE) && (sRecByteCounter > 7)) || //7 for 16Bit addres //9 for 24
					((sRecDataStart == TRUE) && (sRecType == 9)))
				{

					if ((sRecDataIndex < sRecSize) && (sRecDataIndex < S_REC_BUF_SIZE))
					{

						if (sRecDataIndexNibble == 0)
						{

							sRecDataIndexNibble++;
							sRecData[sRecDataIndex] = bcdToBinary(byteBuf);
						}
						else
						{
							sRecData[sRecDataIndex] <<= 4;
							sRecData[sRecDataIndex] |= bcdToBinary(byteBuf);
							sRecDataIndexNibble = 0;

							sRecDataIndex++;
						}
					}

					/* Check end of record */
					if (sRecDataIndex == (sRecSize))
					{
						sRecScan = FALSE;

						checksum_SRecChecksum = sRecData[sRecDataIndex - 1];
						/*Calculate checksum*/
						if (checksum_SRecChecksum == can_checksum(checksum_SRecSize, &checksum_SRecAddr, &sRecData))
						{
							if (sRecType == 1) /* Data Rec*/
							{
								/* Decrement checksum */
								sRecSize--;

								if ((sRecAddr + sRecSize - 2) == BL_CFG_RESET_VECTOR_ADDRESS_SREG)
								{
									if (Memory_Status == CORRUPTED)
									{
										/*PTT_PTT2 = 1;*/
										Memory_Status = ERASED;
										Request_Status = READY;
										blState = BL_SM_IDLE;
										/*PTT_PTT2 = 0;*/
									}
									else
									{
										/* Store application entry point */
										appVector |= (uint32_t)sRecData[sRecSize - 2];
										appVector <<= 8;
										appVector |= (uint32_t)sRecData[sRecSize - 1];
										appVector |= 0x030000;

										BL_Repository.ApplicationInfo.entryPoint = (AppEntry_t)appVector;

										if (Comm_Mode == COMM_CAN) /* SBurner isn't sending the S9 line */
										{
											/* Protect all flash */
											while (PFlash_SetGlobalProtection(TRUE) != ERR_OK)
											{
											}

											/* Boot Flag Reseted on Application*/

											/*Flag to tell the app a new flash is there*/
											IEE1_SetByte(CAN_JUMP_BL_APP_ADDRESS, 0x01);
											while (IEE1_Busy())
												;

											push(BL_CMD_RUN_APP);
											blState = BL_SM_RUN_APP; /* Bypass IDEL and jump to APP */
											Memory_Status = PROGRAMMED;
										}
									}
								}

								/* Flash memory data */
								else if (((sRecAddr + sRecSize) < BL_CFG_BOOTLOADER_ADDRESS_SREG))
								{
									if (((sRecAddr - Prev_sRecAddr) != sRecSize) && (Prev_sRecAddr != 0) && (sRecAddr < (BL_CFG_APPLICATION_ADDRESS_START_SREG + BL_CFG_APPLICATION_SIZE)))
									{
										Memory_Status = CORRUPTED;
										blState = BL_SM_IDLE;
										Diag_State = DIAG_IDLE;
										BL_PROG_Enable = FALSE;
									}
									/* Check for Interrupt vector Area*/
									else if (((sRecAddr - Prev_sRecAddr) != sRecSize) && (Prev_sRecAddr != 0) && (sRecAddr > (APP_INTERRUPTRESETVECTORSTART)))
									{
										Memory_Status = CORRUPTED;
										blState = BL_SM_IDLE;
										Diag_State = DIAG_IDLE;
										BL_PROG_Enable = FALSE;
									}
									else
									{
										/* Force to word align */
										if (sRecSize & 1)
										{
											sRecData[sRecSize] = 0xFF;
											sRecSize++;
										}

										/* Program flash memory */
										while (PFlash_Program(sRecAddr, &sRecData) != ERR_OK) /* About <600us */
										{
										}
										while (PFlash_Program(sRecAddr + 8, &sRecData[8]) != ERR_OK) /* About <600us */
										{
										}
										while (PFlash_Program(sRecAddr + 16, &sRecData[16]) != ERR_OK) /* About <600us */
										{
										}
										while (PFlash_Program(sRecAddr + 24, &sRecData[24]) != ERR_OK) /* About <600us */
										{
										}
									}
								}
								else
								{
								}
							}
							else if (sRecType == 9) /*End of SRec*/
							{
								/* Store application info */
								// while (IFsh1_SetBlockFlash((IFsh1_TDataAddress)&BL_Repository, BL_CFG_DATA_REPOSITORY_ADDRESS_START, sizeof(BL_Repository_t)) != ERR_OK)
								// while (PFlash_Program((uint32_t)BL_CFG_DATA_REPOSITORY_ADDRESS_START, (PFlash_TDataAddress)&BL_Repository) != ERR_OK)
								// {
								// }

								/* Protect all flash */
								while (PFlash_SetGlobalProtection(TRUE) != ERR_OK)
								{
								}

								/* Send ACK to flasher */
								// IO_writeByte((AS1_TComData)BL_CMD_ACK);

								push(BL_CMD_RUN_APP);
								blState = BL_SM_IDLE;
								Memory_Status = PROGRAMMED;
							}
						}
						else
						{
							Memory_Status = CORRUPTED;
							Diag_State = DIAG_IDLE;
							blState = BL_SM_IDLE;
							BL_PROG_Enable = FALSE;
						}
					}
					else
					{
					}
				}
			}
		}
		else
		{
		}
	}
	else
	{
	}

#endif
}

void STM_Bootloader(void)
{
	switch (blState)
	{
	case BL_SM_INIT:

		/* 50 ms delay to check if CAN Bus is busy */
		if (silentBusCounter > 5)
		{
			blState = BL_SM_RUN_APP;
		}
		/* Enter Bootloader if a valid diagnostic request is sent or if Bootflag is set*/
		else if (udsReqTimeOut_Flag || (Boot_FLag == 0x01) || ((Memory_Status != UNTOUCHED) && (Memory_Status != PROGRAMMED)))
		{
			blState = BL_SM_CAN_MODE;
			Comm_Mode = COMM_CAN;
			LIN_timeoutTimer = sysTickTimer;
		}
		else
		{
			/* Wait for 50ms */
		}
		break;

	case BL_SM_CAN_MODE:

		if ((sysTickTimer - LIN_timeoutTimer) > BL_CFG_APPLICATION_LIN_RUN_TIMEOUT)
		{
			blState = BL_SM_RUN_APP;
		}
		else if (Diag_State == DIAG_REQ_RECEIVED)
		{
			blState = BL_SM_IDLE; /* Bypass ACK */
		}
		break;

	case BL_SM_ACK:

		/* Check for bootloader timeout */
		if ((sysTickTimer - ackTimer) > BL_CFG_APPLICATION_RUN_TIMEOUT)
		{
			blState = BL_SM_RUN_APP;
		}
		else if (IO_readByte(&byteBuf) == ERR_OK)
		{
			if (byteBuf == BL_CMD_ACK)
			{
				blState = BL_SM_IDLE;
			}
		}
		break;

	case BL_SM_IDLE:

		if (IO_readByte(&byteBuf) == ERR_OK)
		{
			/*Reset TimeOut*/
			LIN_timeoutTimer = sysTickTimer;

			switch (byteBuf)
			{
			case BL_CMD_VERSION:

				break;

			case BL_CMD_ERASE_FULL:

				// __DI();
				Memory_Status = ERASING;
				/* UnProtect all flash */ // TODO: Carry over but Dangerous
				while (PFlash_SetGlobalProtection(FALSE) != ERR_OK)
				{
				}

				/* Erase all the block dedicated to Application region */
				flashAddress = (PFlash_TAddress)BL_CFG_APPLICATION_ADDRESS_START;

				/*About 1.6 Sec*/
				while (PFlash_EraseSectorBySector(BL_CFG_APPLICATION_ADDRESS_START, (PFlash_TAddress)BL_CFG_BOOTLOADER_ADDRESS) != ERR_OK)
				{
				}

				Memory_Status = ERASED;
				Request_Status = READY;
				failure_PDU_SID = 0;
				break;

			case BL_CMD_PROGRAM:
				if ((Memory_Status == ERASED) || (Memory_Status == EMPTY))
				{
					sRecScan = FALSE;
					blState = BL_SM_PROGRAM;
					Memory_Status = PROGRAMMING;
					ackTimer = sysTickTimer;

					if (Comm_Mode == COMM_CAN)
					{
						BL_PROG_Enable = TRUE;
						failure_PDU_SID = 0;
					}
				}
				else
				{
					/* Do nothing */
				}
				break;

			case BL_CMD_RUN_APP:
				blState = BL_SM_RUN_APP;
				break;

			default:
			{
			}
			}
		}
		else
		{
			if ((sysTickTimer - LIN_timeoutTimer) > BL_CFG_APPLICATION_LIN_RUN_TIMEOUT)
			{
				blState = BL_SM_RUN_APP;
			}
		}

		break;

	case BL_SM_PROGRAM:
		if ((sysTickTimer - ackTimer) > BL_CFG_FLASHING_TIMEOUT)
		{
			/*PTT_PTT2 ^= 1;*/
			blState = BL_SM_IDLE;
			Memory_Status = CORRUPTED;
			Diag_State = DIAG_IDLE;

			if (Comm_Mode == COMM_CAN)
			{
				BL_PROG_Enable = FALSE;
			}
		}
		else
		{
			/* handled by the timer */
		}

		break;

	case BL_SM_RUN_APP:

		if (Comm_Mode == COMM_CAN)
		{
			BL_PROG_Enable = FALSE;
		}
		if (BL_Repository.ApplicationInfo.entryPoint != (AppEntry_t)0x03FFFF)
		{
			appEntry = (AppEntry_t)(BL_Repository.ApplicationInfo.entryPoint);
			appEntry();
		}
		else
		{
			// Cpu_Delay100US(10000);
			blState = BL_SM_INIT;
		}
		break;

	default:
		break;
	}
}

/***********************************************
 * Auxiliary Functions
 * *********************************************/

byte IO_readByte(byte *Chr)
{
	byte err;

	if (Comm_Mode == COMM_SERIAL)
	{
		/* Do Nothing */
	}
	else
	{
		if ((get_size()) > 0)
		{
			pop(Chr);
			err = ERR_OK;
		}
		else
		{
			err = ERR_RXEMPTY;
		}
	}

	return err;
}

byte IO_writeByte(byte Chr)
{
	byte err;
	if (Comm_Mode == COMM_SERIAL)
	{
		/* Do Nothing */
	}
	else
	{
		//TODO: read BL feedback
		err = ERR_OK;
	}
	return err;
}
uint8_t bcdToBinary(uint8_t bcd)
{
	uint8_t bin = 0;

	if (bcd >= '0' && bcd <= '9')
	{
		bin = bcd - '0';
	}
	else if (bcd >= 'A' && bcd <= 'F')
	{
		bin = bcd - 'A' + 10;
	}

	return bin;
}
