#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f10x.h"
#include "debug.h"

extern volatile uint32_t tick_count;
extern volatile uint32_t sys_int_times;
extern RCC_ClocksTypeDef sys_clk_def;
	
uint8_t init_sys_timer(void);
void busy_delay_ms(uint32_t time_ms);
void busy_delay_100us(uint32_t time_us);
uint32_t get_sys_tick(void);

#define SYS_TIMER_FREQ			10000

#ifdef __cplusplus
}
#endif 

#endif
