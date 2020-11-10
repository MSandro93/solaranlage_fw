/*
 * _7seg.h
 *
 * Created: 09.11.2020 10:25:57
 *  Author: stoff
 */ 

void SevenSeg_init(void);

//seg: 1=dach; 0=kessel
void SevenSeg_set_val(uint8_t seg, uint16_t val);

void SevenSeg_on(void);
void SevenSeg_off(void);
void SevenSeg_toggle(void);
uint8_t SevenSeg_get_state(void);
