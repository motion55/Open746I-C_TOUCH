/*----------------------------------------------------------*/
/*  Filename :  DEBUG_CONSOLE.H     Created    : 2014-12-23 */
/*  Released :                      Revision # :            */
/*  Author   :  Butch Tabios                                */
/*                                                          */
/*  Routine List:                                           */
/*                                                          */
/*  Update List:                                            */
/*                                                          */
/*  Files needed for linking:                               */
/*                                                          */
/*  Comments:                                               */
/*                                                          */
/*----------------------------------------------------------*/
#ifndef _JEIL_DEBUG_H_
#define _JEIL_DEBUG_H_

#include "usbd_def.h"

#ifdef __cplusplus
 extern "C" {
#endif

//void DebugRcvISR(unsigned char ch);

void DebugInit(void);

void DebugTask(void);

void DebugPutChar(char ch);

void DebugSend(char *message);

void DebugPrint(const char *format, ...);

void DoDebugSerial(void);

char IsTest(void);

uint8_t USB_SOF(USBD_HandleTypeDef *pdev);

#ifdef __cplusplus
}
#endif

#endif	//#ifndef _JEIL_DEBUG_H_
