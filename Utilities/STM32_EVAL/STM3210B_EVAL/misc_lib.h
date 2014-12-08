#ifndef MISC_LIB_H
#define MISC_LIB_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f10x.h"
#include "sys_timer.h"
#include "debug.h"

#ifndef NULL
#define NULL	0
#endif
	 
char *DECToASCII(uint32_t dat,char *str);
char *HexToASCII(uint32_t dat,char *str);
void Memcpy(void *destination,const void *source,int32_t c);
void MemSet(void *des, uint8_t c,int32_t num);
char *StrCat(char *destion, const char *source);
int32_t StrLen(const char *string);
int32_t StrNLen(const char *string, int precision);
int MemCmp(const char *str1, const char *str2, int len);


#ifdef __cplusplus
 }
#endif 

#endif
