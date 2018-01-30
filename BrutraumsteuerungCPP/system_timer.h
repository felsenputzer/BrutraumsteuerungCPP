/*
 * System_Timer.h
 *
 * Created: 28.07.2015 21:29:04
 *  Author: Johannes
 */ 


#ifndef SYSTEM_TIMER_H_
#define SYSTEM_TIMER_H_

void sTimer_Init(void);
void sTimer_Event(void);
void sTimer_Add(t_sTimer_Event )

typedef void (*t_funktion)(void);

typedef struct t_sTimer_Event {
	t_sTimer_Event* next;
	uint32_t time;
	t_funktion funktion;
	};

#endif /* SYSTEM_TIMER_H_ */