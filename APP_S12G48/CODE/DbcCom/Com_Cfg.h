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


#ifndef	__COM_CFG_H_
#define	__COM_CFG_H_
/*******************************************************************************
    Include Files
*******************************************************************************/

#include "fm29_nfc_readers_can_v1_0_interior.h"
#include "CAN1.h"
/*******************************************************************************
    Type Definition
*******************************************************************************/
#if 0 /*Interior Config*/
#define ComSend_interior_nfc_int_0x205_sec_data_04_frame()         Com_SendPduByID(0x205)
#define ComSend_interior_nfc_int_0x204_sec_data_03_frame()         Com_SendPduByID(0x204)
#define ComSend_interior_nfc_int_0x203_sec_data_02_frame()         Com_SendPduByID(0x203)
#define ComSend_interior_nfc_int_0x202_sec_data_01_frame()         Com_SendPduByID(0x202)
#define ComSend_interior_nfc_int_0x201_extended_sts_frame_frame()  Com_SendPduByID(0x201)
#define ComSend_interior_nfc_int_0x200_status_frame_frame()        Com_SendPduByID(0x200)
#endif

#if 1 /*Exterior Config*/
#define ComSend_interior_nfc_int_0x205_sec_data_04_frame()         Com_SendPduByID(0x105)
#define ComSend_interior_nfc_int_0x204_sec_data_03_frame()         Com_SendPduByID(0x104)
#define ComSend_interior_nfc_int_0x203_sec_data_02_frame()         Com_SendPduByID(0x103)
#define ComSend_interior_nfc_int_0x202_sec_data_01_frame()         Com_SendPduByID(0x102)
#define ComSend_interior_nfc_int_0x201_extended_sts_frame_frame()  Com_SendPduByID(0x101)
#define ComSend_interior_nfc_int_0x200_status_frame_frame()        Com_SendPduByID(0x100)
#endif

/*******************************************************************************
    Global Varaibles Extern
*******************************************************************************/
extern struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x205_sec_data_04_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x205_sec_data_04;
extern struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x204_sec_data_03_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x204_sec_data_03;
extern struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x203_sec_data_02_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x203_sec_data_02;
extern struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x202_sec_data_01_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x202_sec_data_01;
extern struct fm29_nfc_readers_can_v1_0_interior_scu_0x24_sec_data_04_t fm29_nfc_readers_can_v1_0_interior_scu_0x24_sec_data_04;
extern struct fm29_nfc_readers_can_v1_0_interior_scu_0x23_sec_data_03_t fm29_nfc_readers_can_v1_0_interior_scu_0x23_sec_data_03;
extern struct fm29_nfc_readers_can_v1_0_interior_scu_0x21_sec_data_01_t fm29_nfc_readers_can_v1_0_interior_scu_0x21_sec_data_01;
extern struct fm29_nfc_readers_can_v1_0_interior_scu_0x22_sec_data_02_t fm29_nfc_readers_can_v1_0_interior_scu_0x22_sec_data_02;
extern struct fm29_nfc_readers_can_v1_0_interior_scu_0x20_command_t fm29_nfc_readers_can_v1_0_interior_scu_0x20_command;
extern struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x201_extended_sts_frame_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x201_extended_sts_frame;
extern struct fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame_t fm29_nfc_readers_can_v1_0_interior_nfc_int_0x200_status_frame;

/*******************************************************************************
    Function  Extern
*******************************************************************************/

/**
 * Com_MainFunction_Rx - Process the reception of all configured Com PDUs 
 *
 * Parameters:
 * const uint8_t *buffer, const uint32_t msgID, const uint_t msgSize 
 * returns:
 * void
 */
void Com_MainFunction_Rx(const uint8_t *buffer, const uint32_t msgID, const uint8_t msgSize);
 
 
/**
 * Com_MainFunction_Tx_Cyclic - Process the Transmition of all configured periodic Com PDUs 
 *
 * Parameters:
 * uint32_t Tick_1msCount
 * returns:
 * void
 */
void Com_MainFunction_Tx_Cyclic(uint32_t Tick_1msCount);

/**
 * Com_SendPduByID - Send Pdu based on msgID
 *
 * Parameters:
 * const uint32_t msgID
 * returns:
 * void
 */
void Com_SendPduByID(const uint32_t msgID);

#endif
