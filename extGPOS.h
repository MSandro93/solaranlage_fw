/*
 * extGPOS.h
 *
 * Created: 13.03.2021 22:38:00
 *  Author: mstof
 */ 


#ifndef EXTGPOS_H_
#define EXTGPOS_H_

typedef enum
{
	LED_TEMP_DACH,
	LED_TEMP_KELLER,
	LED_MODE,
	LED_K_FACTOR,
	LED_DELTA2,
	LED_DELTA1,
	BUZZER,
	LED_PWM
} extGPO;

typedef enum
{
	EXGPO_OFF,
	EXGPO_ON,	
} extGPO_state;


void extGPOs_init(void);
void extGPOS_clearAllLEDs(void);
uint8_t extGPO_switch(extGPO ego, extGPO_state s);
void extGPO_update(void);

#endif /* EXTGPOS_H_ */