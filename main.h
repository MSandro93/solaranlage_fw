/*
 * main.h
 *
 * Created: 09.11.2020 20:53:21
 *  Author: stoff
 */ 


#ifndef MAIN_H_
#define MAIN_H_

enum state
{
	INIT,
	DISPLAY_OFF,
	SHOW_TEMPS,
	MODIFY_DELTA1,
	MODIFY_DELTA2,
	MODIFY_K,
	SWITCH_MODES
};

enum display
{
	DISPLAY_KESSEL,
	DISPLAY_DACH
};

enum modes
{
	MODE_AUTO,
	MODE_ON,
	MODE_OFF
};

void setState(uint8_t s);
uint8_t getState(void);


#endif /* MAIN_H_ */