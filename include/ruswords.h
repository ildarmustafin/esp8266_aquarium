#pragma once
#ifndef RUSWORDS_H
#define RUSWORDS_H
#include <Arduino.h>

extern uint8_t z_gradus[8];
extern uint8_t z_empty[8];
extern uint8_t z_temp[8]; 
extern uint8_t z_led[8]; 
extern uint8_t z_wifi_sta[8]; 
extern uint8_t z_wifi_ap[8];  
extern uint8_t z_wifi_tc[8]; 

extern uint8_t z_fan[8]; 
extern uint8_t z_ten[8];
extern uint8_t z_ws[8]; 
extern uint8_t z_rel[4][8];
extern uint8_t right_empty[8];
extern uint8_t left_empty[8];
extern uint8_t center_empty[8];
extern uint8_t left_full[8]; 
extern uint8_t right_full[8]; 
extern uint8_t center_full[8];

extern uint8_t z_P[8]; 
extern uint8_t z_Ch[8]; 
extern uint8_t z_B[8];
#endif