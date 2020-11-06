/*
 * Timer.c
 *
 * Created: 11/26/2014 10:53:44 AM
 *  Author: Butch
 */ 

#include "main.h"
#include "Timer.h"

#ifndef	true
#define true	1
#endif
#ifndef	false
#define false	0
#endif

static struct timer_register_type {
	uint32_t start_time;
	uint32_t delay_val;
	char start;
	char ended;
} timerarray[MAX_TIMER];

#define SYSCLK_SHIFT	4

uint32_t CPU_Count;
uint32_t TIMER_Hz = 1000l;

volatile uint32_t time_of_day;
volatile int32_t LED_tick;

void Calc_Time_of_Day(void);

void StartTimer(uint8_t timerno, uint16_t delay_ms)
{
	if	(!timerarray[timerno].start)
	{
		Calc_Time_of_Day();
		timerarray[timerno].start_time = time_of_day;
		timerarray[timerno].delay_val = delay_ms;
		timerarray[timerno].ended = false;
		timerarray[timerno].start = true;
	}
}

void ResetTimer(uint8_t timerno)
{
	timerarray[timerno].ended = false;
	timerarray[timerno].start = false;
}

char TimerOut(uint8_t timerno)
{
	if (timerarray[timerno].ended)
		return true;
	else
	if (timerarray[timerno].start)
	{
		Calc_Time_of_Day();
		uint32_t temp = time_of_day - timerarray[timerno].start_time;
		if (temp>=timerarray[timerno].delay_val)	// detect if delay has elapsed
		{
			timerarray[timerno].ended = true;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void InitTimer(void)
{
	int temp;

	for (temp=0; temp<MAX_TIMER; temp++) ResetTimer(temp);
}

void SetupTimer(void)
{
	CPU_Count = HAL_GetTick();
	LED_tick = TIMER_Hz/2;
	time_of_day = 0;
}

void Calc_Time_of_Day(void)
{
	uint32_t Old_CPU_Count = CPU_Count;
	CPU_Count = HAL_GetTick();
	int32_t delta_time = CPU_Count - Old_CPU_Count;
	time_of_day += delta_time;
	LED_tick -= delta_time;
	if (LED_tick<=0)
	{
		LED_tick += TIMER_Hz/2;
		//HAL_GPIO_TogglePin(LED_Out_GPIO_Port, LED_Out_Pin);
	}
	#ifdef _TimerTask
	_TimerTask;
	#endif
}

int32_t GetStopWatch(uint8_t timerno)
{
	if (timerarray[timerno].start)
	{
		Calc_Time_of_Day();
		int32_t temp = time_of_day - timerarray[timerno].start_time;
		return temp;
	}
	return 0;
}
