/*
 * timeout.h
 *
 * Created: 09.11.2020 19:47:55
 *  Author: stoff
 */ 

//switches display back to temp if no action si taken.

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

void Timeout_init();
void start_timeout_timer();
void stop_timeout_timer();


#endif /* TIMEOUT_H_ */