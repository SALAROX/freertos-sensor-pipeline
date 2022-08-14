/**
  ******************************************************************************
  /*License         : MIT License. Copyright (c) 2022 IsaacIbm.
  * @Last author    : IsaacIbm
  * @Email/Hangouts : Isk.ibm@gmail.com
  * @role           : Senior Automotive software engineer
  *                 : For Custom Embedded software please dont hesitate to
  *                 : contact me isk.ibm@gmail.com
  *                 : at https://www.freelancer.com/u/IsaacIbm
  ******************************************************************************
**/


/*******************************************************************************
    Include Files
*******************************************************************************/
#include "Com_Cfg.h"

/*******************************************************************************
    Type declaration
*******************************************************************************/

#define REMAINDER_MULTIPLY 100

/*******************************************************************************
    Global Varaibles
*******************************************************************************/

/*Msgs structs*/
struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x205_sec_data_04_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x205_sec_data_04;
struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x204_sec_data_03_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x204_sec_data_03;
struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x203_sec_data_02_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x203_sec_data_02;
struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x202_sec_data_01_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x202_sec_data_01;
struct fm29_nfc_readers_can_v1_0_interior_scu_0x24_sec_data_04_t fm29_nfc_readers_can_v1_0_interior_scu_0x24_sec_data_04;
struct fm29_nfc_readers_can_v1_0_interior_scu_0x23_sec_data_03_t fm29_nfc_readers_can_v1_0_interior_scu_0x23_sec_data_03;
struct fm29_nfc_readers_can_v1_0_interior_scu_0x21_sec_data_01_t fm29_nfc_readers_can_v1_0_interior_scu_0x21_sec_data_01;
struct fm29_nfc_readers_can_v1_0_interior_scu_0x22_sec_data_02_t fm29_nfc_readers_can_v1_0_interior_scu_0x22_sec_data_02;
struct fm29_nfc_readers_can_v1_0_interior_scu_0x20_command_t fm29_nfc_readers_can_v1_0_interior_scu_0x20_command;
struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x201_extended_sts_frame_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x201_extended_sts_frame;
struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame;


/*******************************************************************************
    Function  Definition
*******************************************************************************/

/*******************************************************************************
    Function  Definition - external API
*******************************************************************************/ 
/**
 * Com_MainFunction_Rx - Process the reception of all configured Com PDUs 
 *
 * Parameters:
 * const uint8_t *buffer, const uint32_t msgID, const uint_t msgSize 
 * returns:
 * void
 */
void Com_MainFunction_Rx(const uint8_t *buffer, const uint32_t msgID, const uint8_t msgSize)
{
   if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_SCU_0X24_SEC_DATA_04_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_scu_0x24_sec_data_04_unpack(&fm29_nfc_readers_can_v1_0_interior_scu_0x24_sec_data_04,
      buffer, msgSize);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_SCU_0X23_SEC_DATA_03_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_scu_0x23_sec_data_03_unpack(&fm29_nfc_readers_can_v1_0_interior_scu_0x23_sec_data_03,
      buffer, msgSize);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_SCU_0X21_SEC_DATA_01_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_scu_0x21_sec_data_01_unpack(&fm29_nfc_readers_can_v1_0_interior_scu_0x21_sec_data_01,
      buffer, msgSize);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_SCU_0X22_SEC_DATA_02_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_scu_0x22_sec_data_02_unpack(&fm29_nfc_readers_can_v1_0_interior_scu_0x22_sec_data_02,
      buffer, msgSize);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_SCU_0X20_COMMAND_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_scu_0x20_command_unpack(&fm29_nfc_readers_can_v1_0_interior_scu_0x20_command,
      buffer, msgSize);
   }
}
 
 
/**
 * Com_MainFunction_Tx_Cyclic - Process the Transmition of all configured periodic Com PDUs 
 *
 * Parameters:
 * uint32_t Tick_1msCount
 * returns:
 * void
 */
void Com_MainFunction_Tx_Cyclic(uint32_t Tick_1msCount)
{
   uint8_t buffer[8];
   
   if( (Tick_1msCount % FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X201_EXTENDED_STS_FRAME_CYCLE_TIME_MS) == 0 )
   {
	   ComSend_interior_nfc_int_0x201_extended_sts_frame_frame();
   }
   
   if( (Tick_1msCount % FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X200_STATUS_FRAME_CYCLE_TIME_MS) == 0 )
   {
	   ComSend_interior_nfc_int_0x200_status_frame_frame();
   }

}


/**
 * Com_SendPduByID - Send Pdu based on msgID
 *
 * Parameters:
 * const uint32_t msgID
 * returns:
 * void
 */
void Com_SendPduByID(const uint32_t msgID)
{
   uint8_t buffer[8];
      
   if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X205_SEC_DATA_04_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_nfc_int_0x205_sec_data_04_pack(buffer,
      &fm29_nfc_readers_can_v1_0_interior_nfc_int_0x205_sec_data_04,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X205_SEC_DATA_04_LENGTH);

      CAN1_SendFrame(0,FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X205_SEC_DATA_04_FRAME_ID,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X205_SEC_DATA_04_IS_EXTENDED,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X205_SEC_DATA_04_LENGTH, buffer);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X204_SEC_DATA_03_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_nfc_int_0x204_sec_data_03_pack(buffer,
      &fm29_nfc_readers_can_v1_0_interior_nfc_int_0x204_sec_data_03,
	  FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X204_SEC_DATA_03_LENGTH);

      CAN1_SendFrame(0,FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X204_SEC_DATA_03_FRAME_ID,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X204_SEC_DATA_03_IS_EXTENDED,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X204_SEC_DATA_03_LENGTH, buffer);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X203_SEC_DATA_02_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_nfc_int_0x203_sec_data_02_pack(buffer,
      &fm29_nfc_readers_can_v1_0_interior_nfc_int_0x203_sec_data_02,
	  FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X203_SEC_DATA_02_LENGTH);

      CAN1_SendFrame(0,FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X203_SEC_DATA_02_FRAME_ID,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X203_SEC_DATA_02_IS_EXTENDED,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X203_SEC_DATA_02_LENGTH, buffer);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X202_SEC_DATA_01_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_nfc_int_0x202_sec_data_01_pack(buffer,
      &fm29_nfc_readers_can_v1_0_interior_nfc_int_0x202_sec_data_01,
	  FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X202_SEC_DATA_01_LENGTH);

      CAN1_SendFrame(0,FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X202_SEC_DATA_01_FRAME_ID,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X202_SEC_DATA_01_IS_EXTENDED,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X202_SEC_DATA_01_LENGTH, buffer);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X201_EXTENDED_STS_FRAME_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_nfc_int_0x201_extended_sts_frame_pack(buffer,
      &fm29_nfc_readers_can_v1_0_interior_nfc_int_0x201_extended_sts_frame,
	  FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X201_EXTENDED_STS_FRAME_LENGTH);

      CAN1_SendFrame(0,FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X201_EXTENDED_STS_FRAME_FRAME_ID,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X201_EXTENDED_STS_FRAME_IS_EXTENDED,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X201_EXTENDED_STS_FRAME_LENGTH, buffer);
   }
   else if(msgID == FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X200_STATUS_FRAME_FRAME_ID)
   {
      fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame_pack(buffer,
      &fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame,
	  FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X200_STATUS_FRAME_LENGTH);

      CAN1_SendFrame(0,FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X200_STATUS_FRAME_FRAME_ID,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X200_STATUS_FRAME_IS_EXTENDED,
      FM29_NFC_READERS_CAN_V1_0_INTERIOR_NFC_INT_0X200_STATUS_FRAME_LENGTH, buffer);
   }
}


/****************EOF****************/
