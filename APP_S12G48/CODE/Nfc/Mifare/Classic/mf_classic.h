/*
 * mf_classic.h
 *
 *  Created on: 12 Jul 2019
 *      Author: Amir Rabbanis
 */

#ifndef MF_CLASSIC_H_
#define MF_CLASSIC_H_

/*
 * *****************************************************************
 * INCLUDE
 * *****************************************************************
 */
#include "PE_Types.h"
#include "rfal_nfca.h"
#include "mf_crypto1.h"

/*
 * *****************************************************************
 * DEFINITION
 * *****************************************************************
 */

#define MFCL_BLOCK_SIZE												16
#define MFCL_MAX_BLOCK_NUMBER										255

typedef enum
{
	MFCL_ERR_NONE													,
	MFCL_ERR_BUSY													,
	MFCL_ERR_CRC_PARITY												,
	MFCL_ERR_INVAL_OPER												,
	MFCL_ERR_FRAMING												,
	MFCL_ERR_IO														,
	MFCL_ERR_COLLISION												,
	MFCL_ERR_PARAM													,
	MFCL_ERR_FRAME_LENGTH											,
	MFCL_ERR_TIMEOUT												,
	MFCL_ERR_PROT													,
	MFCL_ERR_INVAL_TYPE												,
	MFCL_ERR_CIPHER													,
	MFCL_ERR_UNKNOWN

}MFCLRetCode_t;

typedef enum
{
	MFCL_MODULATION_NORM										= 0	,
	MFCL_MODULATION_STRONG
}MFCLModType_t;

typedef enum
{
	MFCL_UIDF_0												= 0x00	,
	MFCL_UIDF_1												= 0x40	,
	MFCL_UIDF_2												= 0x20	,
	MFCL_UIDF_3												= 0x60
}MFCL_UIDF_t;

typedef enum
{
	MFCL_AUTHENTICATE_KEY_A											,
	MFCL_AUTHENTICATE_KEY_B
}MFCL_Authen_t;

/*
 * *****************************************************************
 * DATA TYPE
 * *****************************************************************
 */


/*
 * *****************************************************************
 * FUNCTION PROTOTYPE
 * *****************************************************************
 */

/*
 * *****************************************************************
 *  \brief
 *
 *  \param
 *
 *  \return
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_Authenticate(rfalNfcaListenDevice *dev, MFCL_Crypto1_t *crypt, MFCL_Authen_t keyType, uint8_t blockAddr, uint8_t *key);

/*
 * *****************************************************************
 *  \brief	Personalise UID with different options
 *
 *  		MFCL_UIDF_0: 	Anti-Collision, Selection,
 *  							Double size UID
 *
 *  		MFCL_UIDF_1: 	Anti-Collision, Selection shortcut,
 *  							Double size UID
 *
 *  		MFCL_UIDF_2:	Anti-Collision, Selection,
 *  							Single size random UID

 *  		MFCL_UIDF_3:	Anti-Collision, Selection,
 *  							Single size NUID
 *
 *
 *  \param[in]	uidf					: <See MFCL_UIDF_t>
 *
 *  \return
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_UIDUsage(MFCL_UIDF_t uidf);

/*
 * *****************************************************************
 *  \brief	Set the strength of modulation
 *
 *  \Param[in]	Mod						: Modulation strength
 *
 *  \return MFCL_ERR_NONE			: Operation OK
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_SetModulation(MFCLModType_t Mod);

/*
 * *****************************************************************
 *  \brief	Read one block of memory
 *
 *  \param[in]	blockAddr				: Address of block
 *  \param[out]	Buf						: Buffer of read data
 *  \param[in]	BufLen					: Length of buffer
 *
 *  \return MFCL_ERR_NONE			: Operation OK
 *  \return MFCL_ERR_PARAM		: Input parameter error
 *  \return MFCL_ERR_FRAME_LENGTH	: Length of received data not correct
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_Read(uint8_t blockAddr, uint8_t *Buf, uint8_t BufLen);

/*
 * *****************************************************************
 *  \brief	Write one block of memory
 *  		Note: Size of block is different in
 *  			  Mifare Classic Ultra-light
 *
 *  \param[in] 	blockAddr				: Address of block
 *  \param[in]	Buf						: Buffer of write data
 *
 *  \return <See Error Definition>
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_Write(uint8_t blockAddr, uint8_t *Buf);

/*
 * *****************************************************************
 *  \brief	Deduct value of "data" from the value of block stored in
 *  		blockAddr. The result is stored in transfer buffer.
 *  		You need to call Restore function to save permanently.
 *
 *  \param[in]	blockAddr				: Address of block
 *  \param[in]	data					: value of operand
 *
 *  \return <See Error Definition>
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_Decrement(uint8_t blockAddr, int32_t data);

/*
 * *****************************************************************
 *  \brief	Add value of "data" to the value of block stored in
 *			blockAddr. The result is stored in transfer buffer.
 *			You need to call Restore function to save permanently.
 *
 *	\param[in]	blockAddr				: Address of block
 *  \param[in]	data					: value of operand
 *
 *  \return <See Error Definition>
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_Increment(uint8_t blockAddr, int32_t data);

/*
 * *****************************************************************
 *  \brief	Save the values of internal buffer
 *  		in addressed block
 *
 *  \param[in]	blockAddr				: Address of block
 *
 *  \return <See Error Definition>
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_Restore(uint8_t blockAddr);

/*
 * *****************************************************************
 *  \brief	Transfer the values from the addressed block
 *  		to the internal buffer
 *
 *  \param[in]	blockAddr				: Address of block
 *
 *  \return <See Error Definition>
 *  *****************************************************************
 */
MFCLRetCode_t MFCL_Transfer(uint8_t blockAddr);

#endif /* MF_CLASSIC_H_ */
