/*
 * Timer.h
 *
 * Created: 11/26/2014 10:55:09 AM
 *  Author: Butch
 */ 

#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define TC_CHANNEL 	0	//use channel 0 of timer/counters 0-2
#define TIMER_FREQ	1000	//1000Hz
#ifndef MAX_TIMER
#define MAX_TIMER	256
#endif

#define	DEBUG_TIMER	(MAX_TIMER-1)

void SetupTimer(void);

void StartTimer(uint8_t timerno, uint16_t delayvalue);

void ResetTimer(uint8_t timerno);

char TimerOut(uint8_t timerno);

void InitTimer(void);

int32_t GetStopWatch(uint8_t timerno);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
