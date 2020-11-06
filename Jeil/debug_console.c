/*----------------------------------------------------------*/
/*  Filename :  DEBUG_CONSOLE.C     Created    : 2014-12-23 */
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
/*    1. Debug console for the AVR32 mini board.            */
/*                                                          */
/*----------------------------------------------------------*/

#include "main.h"
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debug_console.h"
#include "Timer.h"
#include "usbd_cdc_if.h"
//#include "stm32746g_qspi.h"

#ifndef EOF
#define EOF (-1)            /* End of file indicator */
#endif

#ifndef BS
#define	BS       	0x08
#endif
#ifndef LF
#define	LF       	0x0a
#endif
#ifndef CR
#define	CR       	0x0d
#endif

#ifndef	true
#define	true	1
#endif

#ifndef	false
#define	false	0
#endif

#define	SendDebugPrompt	DebugSend("\r\n->\0")
#define	SendError		DebugSend("\r\n Error???\0")

typedef uint8_t 	BYTE;
typedef uint16_t	WORD;

uint32_t _debug_dump_beg = 0;
uint32_t _old_debug_dump_beg = 0;
uint32_t _debug_dump_end = 0;
uint8_t *debug_port = (unsigned char *)0x80000000;

char bTest = 0;

#define	DBG_BUFFER_SIZE	1024

char dbg_rx_buffer[DBG_BUFFER_SIZE];
int16_t dbg_rx_head=0, dbg_rx_tail=0;

char dbg_tx_buffer[DBG_BUFFER_SIZE];
int16_t dbg_tx_head=0, dbg_tx_tail=0;

void debug_idle(void);
void debug_parse(char *cmd_line);
void debug_rcv(char ch);
unsigned int do_dump(void);

void DebugPutChar(char ch);
void DebugSend(char *message);


uint8_t USB_Receive(uint8_t* Buf, uint16_t length)
{
	for (uint16_t _i = 0; _i<length; _i++) {
		dbg_rx_buffer[dbg_rx_head++] = Buf[_i];
		if (dbg_rx_head >= DBG_BUFFER_SIZE) dbg_rx_head = 0;
		if (dbg_rx_head==dbg_rx_tail) {
			dbg_rx_tail++;
			if (dbg_rx_tail >= DBG_BUFFER_SIZE) dbg_rx_tail = 0;
		}
	}
	return USBD_OK;
}

inline char debug_rxchar_rdy(void)
{
	return (dbg_rx_head!=dbg_rx_tail);
}

char get_debug_rxchar(void)
{
	char debug_rxchar = dbg_rx_buffer[dbg_rx_tail++];
	if (dbg_rx_tail >= DBG_BUFFER_SIZE) dbg_rx_tail = 0;
	return debug_rxchar;
}

void DebugInit(void)
{
	dbg_rx_head = dbg_rx_tail = 0;
	dbg_tx_head = dbg_tx_tail = 0;
#if 0
	for (int i1=0;i1<8;i1++)
	{
		DebugPrint("\r\n %d. Welcome to Debug Console ver STM1.0!", i1+1);
		DebugPrint("\r\n  Compile Date: %s, Time: %s",__DATE__,__TIME__);
	}
	SendDebugPrompt;
#endif
}

unsigned int do_dump(void)
{
	int i1;
	unsigned char *pByte_Val, byte_val;
	char ascii_result[18];
	_old_debug_dump_beg = _debug_dump_beg;

	DebugPrint("\r\n %08x - ",_debug_dump_beg & 0xfffffff0);
	strcpy(ascii_result,"................");
	for (i1=0;i1<(_debug_dump_beg & 0x000f);i1++) DebugSend("   ");
	while (_debug_dump_beg <= _debug_dump_end)
	{
		pByte_Val = (unsigned char *)_debug_dump_beg++;
		byte_val = *pByte_Val;
		DebugPrint("%02x ",byte_val);
		if (!iscntrl(byte_val&0x7f)) ascii_result[i1] = byte_val;
		i1++;
		if (!(_debug_dump_beg & 0x000f))
		{
			DebugSend("  ");
			DebugSend(ascii_result);
			i1 = 0;
			strcpy(ascii_result,"................");
			if (_debug_dump_beg==0) break;
			if (_debug_dump_beg <= _debug_dump_end)
			{
				DebugPrint("\r\n %08x - ",_debug_dump_beg & 0xfffffff0);
			}
		}
	}
	return (_debug_dump_beg);
}

extern const unsigned char _dsc00962[];

void debug_parse(char *cmd_line)
{
	int params;
	uint32_t temp1,temp2,temp3,temp4;
	uint16_t tempword;
	uint8_t tempbyte;

	while (*cmd_line == ' ') cmd_line++;

	switch (toupper(*cmd_line++))
	{
	case '?':
		DebugPrint("\r\n Compile Date: %s, Time: %s",__DATE__,__TIME__);
		break;
	case 'A':  // assemble
		//SDRAM_Test();
		break;
	case 'B':
		//W25Q_QUADSPI_Test();
		break;
	case 'C':  // compare
		DebugSend("\r\n Entering Memory Mapped Mode.");
		HAL_Delay(10);
		//BSP_QSPI_MemoryMappedMode();
		//_debug_dump_beg = (uint32_t)&_dsc00962[0];
		//_debug_dump_end = _debug_dump_beg + 127;
		//do_dump();
		break;
	case 'D':  // dump
		params = sscanf(cmd_line,"%lx %lx",&temp2,&temp3);
		switch (params) {
		case 0:
		case EOF:
			_debug_dump_end = _debug_dump_beg + 127;
			do_dump();
			break;
		case 1:
			_debug_dump_beg = temp2;
			_debug_dump_end = _debug_dump_beg + 127;
			do_dump();
			break;
		case 2:
			if (temp3<temp2) temp3=temp2;
			_debug_dump_beg = temp2;
			_debug_dump_end = temp3;
			do_dump();
			break;
		}
		break;
	case 'E':  // read unsigned int
		if (sscanf(cmd_line,"%lx",&temp1)==1)
			debug_port = (unsigned char*)(temp1&0xFFFFFFFC);
		temp2 = *(unsigned int*)debug_port;
		DebugPrint("\r\n %08lx -> %08lx",(int)debug_port,temp2);
//		debug_port += sizeof(int);
		break;
	case 'F':  // fill
		params = sscanf(cmd_line,"%lx %lx %lx",&temp2,&temp3,&temp4);
		switch (params) {
		case 3:
			break;
		default:
			{
				//temp4 = (uint32_t)&_dsc00962[0];
				//DebugPrint("\r\n _dsc00962 address = %08lX", temp4);
			}
			break;
		}
		break;
	case 'G':  // go
		break;
	case 'H':  // hex
		//W25Q_QUADSPI_Erase();
		break;
	case 'I':  // read byte
		if (sscanf(cmd_line,"%lx",&temp1)==1)
		{
			debug_port = (unsigned char*)temp1;
			tempbyte = *(unsigned char*)debug_port;
			DebugPrint("\r\n %08x -> %02x",(int)debug_port,tempbyte);
			debug_port += sizeof(tempbyte);
		}
		break;
	case 'J':  // read word
		if (sscanf(cmd_line,"%lx",&temp1)==1)
		{
			debug_port = (unsigned char*)(temp1&0xFFFFFFFE);
		}
		tempword = *(unsigned short*)debug_port;
		DebugPrint("\r\n %08X -> %04X",(int)debug_port,tempword);
		break;
	case 'K':
		break;
	case 'L':  // load
		break;
	case 'M':  // write unsigned int
		if (sscanf(cmd_line,"%lX %lX",&temp1,&temp2)==2)
		{
			debug_port = (unsigned char*)(temp1&0xFFFFFFFC);
			DebugPrint("\r\n %08X <- %08X",(int)debug_port,temp2);
			*(unsigned int*)debug_port = temp2;
			debug_port += sizeof(int);
		}
		else
		{
			DebugSend(" ?? \r\n");
		}
		break;
	case 'N':  // name
		while ((*cmd_line==' ')&&(*cmd_line!=0)) cmd_line++;
		DebugSend("\r\n");
		while (*cmd_line!=0) {
			DebugPutChar(*cmd_line);
			cmd_line++;
		}
		break;
	case 'O':  // output byte
		if (sscanf(cmd_line,"%lx %lx",&temp1,&temp2)==2)
		{
			debug_port = (unsigned char*)temp1;
			tempbyte = temp2;
			DebugPrint("\r\n %08X <- %02X",(int)debug_port,tempbyte);
			*debug_port = tempbyte;
			debug_port += sizeof(tempbyte);
		}
		else
		{
			DebugSend(" ?? \r\n");
		}
		break;
	case 'P':  // proceed
		if (sscanf(cmd_line,"%lx",&temp1)==1)
		{
		}
		else
		{
		}
		break;
	case 'Q':  // quit
		break;
	case 'R':  // register
		break;
	case 'S':  // search
		if (sscanf(cmd_line,"%lx",&temp1)==1)
		{
		}
		break;
	case 'T':  // Test
		if (sscanf(cmd_line,"%ld",&temp1)==1)
		{
		}
		else
		{
			bTest = !bTest;
			if (bTest)
			{
				ResetTimer(DEBUG_TIMER);
				StartTimer(DEBUG_TIMER, 100);
				DebugSend("\r\n Test ON!");
			}
			else
			{
				DebugSend("\r\n Test OFF!");
			}
			
		}
		break;
	case 'U':
		if (sscanf(cmd_line,"%ld %ld",&temp1,&temp2)==2)
		{
		}
		else
		if (sscanf(cmd_line,"%ld",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:	//U1
				break;
			case 2:	//U2
				break;
			case 3:	//U3
				break;
			case 4:	//U4
				break;
			case 5:	//U5
				break;
			case 6:	//U6
				break;
			case 7:	//U7
				break;
			case 8:	//U8
				break;
			case 9:	//U9
				break;
			case 10:	//U10
				break;
			case 11:	//U11
				break;
			}
		}
		else
		{
		}
		break;
	case 'W':  // write word
		if (sscanf(cmd_line,"%lx %lx",&temp1,&temp2)==2)
		{
			debug_port = (unsigned char*)(temp1&0xFFFFFFFE);
			tempword = temp2;
			DebugPrint("\r\n %08X <- %04X",(int)debug_port,tempword);
			*(unsigned short*)debug_port = tempword;
			debug_port += sizeof(tempword);
		}
		else
		{
			DebugSend(" ?? \r\n");
		}
		break;
	case 'X':
		if (sscanf(cmd_line,"%lx",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	case 'Y':
		temp4 = sscanf(cmd_line,"%lx %lx",&temp1,&temp2);
		if (temp4==1)
		{
		}
		else
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		break;
	case 'Z':
		if (sscanf(cmd_line,"%li %li",&temp1,&temp2)==2)
		{
		}
		else
		if (sscanf(cmd_line,"%lx",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	default:
		;
	}
}

void debug_rcv(char ch)
{
	static int  lineptr = 0;
	static char linebuff[256];

	if ((ch=='\r') || (lineptr==255))
	{
		linebuff[lineptr] = 0;
		if (lineptr)
		{
			debug_parse(linebuff);
		}
		lineptr = 0;
		SendDebugPrompt;
	}
	else if (iscntrl(ch))
	{
		switch (ch)
		{
		case BS:
			if (lineptr)
			{
				DebugPutChar(ch);
				lineptr--;
			}
			break;
		}
	}
	else
	{
		linebuff[lineptr++] = ch;
		DebugPutChar(ch);
	}
}

void debug_idle(void)
{
	if (bTest) {
#ifdef TFTLCD_HPP_
		uint16_t dbg_TouchCoordinates[5][2];
		int touches = TFTLCD_GetTouchData(dbg_TouchCoordinates);
		DebugPrint("\r\n TFTLCD_GetTouchData(); %d", touches);
		for (int _i = 0; _i < touches; _i++) {
			DebugPrint(" x=%3d y=%3d ", dbg_TouchCoordinates[_i][0],
					dbg_TouchCoordinates[_i][1]);
		}
#endif
	}
}

#ifndef	DEBUGTIMER
#define	DEBUGTIMER	1
#endif

static char InDebug = 0;

#define	_USE_SOF_	0

void DebugTask(void)
{
	if (!InDebug) {
		InDebug = 1;	//prevent recursion
		if (dbg_rx_head != dbg_rx_tail) {
			do {
				char _rxchar = dbg_rx_buffer[dbg_rx_tail++];
				if (dbg_rx_tail >= DBG_BUFFER_SIZE)
					dbg_rx_tail = 0;
				debug_rcv(_rxchar);
			} while (dbg_rx_head != dbg_rx_tail);
		} else {
			StartTimer(DEBUGTIMER, 50);
			if (TimerOut(DEBUGTIMER)) {
				ResetTimer(DEBUGTIMER);
				debug_idle();
			}
		}
		InDebug = 0;
	}
#if (_USE_SOF_==0)
	DoDebugSerial();
#endif
}

volatile uint8_t USB_Lock = 0;

void DebugPutChar(char ch)
{
#if _USE_SOF_
	USB_Lock = 1;
#endif
	dbg_tx_buffer[dbg_tx_head++] = ch;
	if (dbg_tx_head>=DBG_BUFFER_SIZE) dbg_tx_head = 0;
	if (dbg_tx_head==dbg_tx_tail) {
		dbg_tx_tail++;	//discard oldest
		if (dbg_tx_tail>=DBG_BUFFER_SIZE) dbg_tx_tail = 0;
	}
#if _USE_SOF_
	USB_Lock = 0;
#endif
}

void DebugSend(char *message)
{
	char ch;

	while (*message != '\0')
	{
		ch = *message;
		message++;
		DebugPutChar(ch);
	}
}

void DebugPrint(const char *format, ...)
{
	char debug_result[82];
	va_list argptr;
    va_start(argptr, format);
    vsprintf(debug_result, format, argptr);
    va_end(argptr);
	DebugSend(debug_result);
}

extern uint8_t USB_Transmit(uint8_t* Buf, uint16_t Len);
uint8_t IT_tx_buffer[DBG_BUFFER_SIZE];
int16_t	IT_tx_buffer_len;

void DoDebugSerial(void)
{
#if _USE_SOF_
	if (USB_Lock) return;
#endif
	if (dbg_tx_head!=dbg_tx_tail)
	{
		IT_tx_buffer_len = dbg_tx_head - dbg_tx_tail;
		if (IT_tx_buffer_len<0) IT_tx_buffer_len += DBG_BUFFER_SIZE;
		for (uint16_t _i = 0; _i<IT_tx_buffer_len; _i++) {
			IT_tx_buffer[_i] = dbg_tx_buffer[dbg_tx_tail++];
			if (dbg_tx_tail >= DBG_BUFFER_SIZE) dbg_tx_tail = 0;
		}
		USB_Transmit(IT_tx_buffer, IT_tx_buffer_len);
	}
}

inline char IsTest(void)
{
	return bTest;
}

/* redirect printf to debug port */
int __io_putchar(int ch)
{
	DebugPutChar(ch);
	return ch;
}

volatile uint8_t SOF_Count = 0;

uint8_t USB_SOF(USBD_HandleTypeDef *pdev)
{
#if _USE_SOF_
	DoDebugSerial();
#endif
	if (SOF_Count++==0) {
		HAL_GPIO_TogglePin(LED3_Out_GPIO_Port, LED3_Out_Pin);
	}

	return USBD_OK;
}


