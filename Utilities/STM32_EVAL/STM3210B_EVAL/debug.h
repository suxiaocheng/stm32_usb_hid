#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f10x.h"
#include "misc_lib.h"
	 
uint8_t init_debug_fun(void);
void serial_put_binary (uint8_t *dat_bin, uint32_t len);
uint8_t stm_putc(uint8_t str);
char stm_puts(char *str);
char *HexToASCII(uint32_t dat,char *str);
int stm_printf(const char *fmt, ...);
void system_dump(char *str);

	 
#ifdef __cplusplus
 }
#endif
	 
#endif
