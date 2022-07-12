/******************************************************************************
*                                                                             *
*   Copyright (C) 2009 Chevalier Technologies Ltd.                            *
*   All Rights Reserved                                                       *
*                                                                             *
*   The code is the property of Chevalier Technologies Ltd.                   *
*                                                                             *
*   The copyright notice above does not evidence any                          *
*   actual or intended publication of such source code.                       *
*                                                                             *
*   History:      Use the source control system to display                    *
*                 revision history information.                               *
*                                                                             *
******************************************************************************/

/**
 *  Compilation switches for product configuration.
 */

#ifndef CONFIG_H
#define CONFIG_H


/******************************************************************************
 *                      Compilation switches                                  *
 ******************************************************************************/

/* Bootloader software version */
#define SW_VERSION (0x0101) /*BL Version*/

#define ROM_4000

#define ENCODER_TMR_CHANNEL	0

/* SYSTEM CONFIG */
#define CAN_BOOTLOADER
#ifndef CAN_BOOTLOADER
#define LIN_BOOTLOADER
#endif

/* BL Product Variant */
#ifdef CAN_BOOTLOADER
#define RA_CAN_BL
#else
/*Define LIN Variants*/
#endif

/* Product Hardware Variant */
#define ENABLE_PCL

/* Bootloader Config */
#define BL_CFG_APPLICATION_RUN_TIMEOUT						30 // ms
#define BL_CFG_APPLICATION_LIN_RUN_TIMEOUT					20000 // ms
#define BL_CFG_FLASHING_TIMEOUT								2000 // ms

#define BL_CFG_APPLICATION_ADDRESS_START					0x034000UL
#define BL_CFG_APPLICATION_SIZE								0x008000UL /*36KB 90000*/
#define BL_CFG_DATA_REPOSITORY_ADDRESS_START				0x03DE80UL  /* Entry Point used to jump from BL to App*/
#define BL_CFG_BOOTLOADER_ADDRESS							0x03E000UL
#define BL_CFG_BOOTLOADER_SIZE								0x1F00UL /* 7KB */
#define BL_CFG_RESET_VECTOR_ADDRESS							0x03FFFEUL //0x03FFFAUL


#define BL_CFG_APPLICATION_ADDRESS_START_SREG					0x004000UL
#define BL_CFG_DATA_REPOSITORY_ADDRESS_START_SREG				0x00C000UL /*Start of Non Banked*/
#define BL_CFG_BOOTLOADER_ADDRESS_SREG							0x00E000UL
#define BL_CFG_RESET_VECTOR_ADDRESS_SREG							0x00FFFEUL
#define APP_INTERRUPTRESETVECTORSTART							0x00DE80UL

/*Bootloader Buffer Size*/
#define S_REC_BUF_SIZE			66U /*517U*/ //Data + CheckSum

/* DEBUG CONFIG */
//#define DATALOG
//#define DEBUG

 
/* PROTECTION & CORE CONFIG */
#define OVER_TEMPERATURE_THRESHOLD	90


/* CYCLONE CONFIG */
//#define CYCLONE_BUILD_PRODUCTION	/* app code to be flashed by CYCLONE SAP image!!! app + bootloader - vectors relocated */
									/* remove CYCLONE_BUILD_PRODUCTION if flashed via bootloader -  relocation done in bootloader */
//#define CYCLONE_BUILD           	/* this affects LIN configuration and other static/constant variables */

// #define DEV_COM
//#define LIN_COM

// #define CAN_PCL_FRONT_LEFT		1
// #define CAN_PCL_FRONT_RIGHT		1
// #define CAN_PCL_REAR_LEFT		1
#define CAN_PCL_REAR_RIGHT		1

#endif /* CONFIG_H */


