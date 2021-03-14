/*
 * _7seg.h
 *
 * Created: 09.11.2020 10:25:57
 *  Author: stoff
 */ 

void SevenSeg_init(void);

//seg: 1=dach; 0=kessel
void SevenSeg_set_val(uint8_t seg, uint16_t val);
void SevenSeg_set_val_f(uint8_t seg, float val);
void SevenSeg_display_mode(uint8_t seg, uint8_t m);

void SevenSeg_on(void);
void SevenSeg_off(void);
uint8_t SevenSeg_get_state(void);
