#include "misc_lib.h"

char *DECToASCII(uint32_t dat, char *str)
{
	char tmp[12], *stmp = tmp;

	*stmp++ = 0;

	if (dat == 0)
		*str++ = '0';

	while (dat) {
		*stmp++ = dat % 10 + '0';
		dat = dat / 10;
	}

	while (*(--stmp))
		*str++ = *stmp;
	*str = 0;
	return str;
}

char *HexToASCII(uint32_t dat, char *str)
{
	char tmp[12], *stmp = tmp;
	int i;

	*stmp++ = 0;
	*str++ = '0';
	*str++ = 'x';

	if (dat == 0)
		*str++ = '0';

	while (dat) {
		i = dat & 0x0f;
		if (i < 10)
			*stmp++ = '0' + i;
		else
			*stmp++ = 'a' + i - 0xa;
		dat >>= 4;
	}

	while (*(--stmp))
		*str++ = *stmp;
	*str = 0;
	return str;
}

void Memcpy(void *destination, const void *source, int32_t c)
{
	uint8_t *dst;
	const uint8_t *src;
	dst = destination;
	src = source;
	while ((c--) > 0) {
		*(dst++) = *(src++);
	}
}

void MemSet(void *des, uint8_t c, int32_t num)
{
	uint8_t *p = des;
	while ((num--) > 0) {
		*(p++) = c;
	}
}

char *StrCat(char *destion, const char *source)
{
	if ((destion == NULL) | (source == NULL))
		return NULL;
	while (*destion++ != NULL) ;
	destion--;
	while (*source != NULL) {
		*(destion++) = *(source++);
	}
	*destion = NULL;
	return destion;
}

int32_t StrLen(const char *string)
{
	int32_t i = 0;
	while (*(string++))
		i++;
	return i;
}

int32_t StrNLen(const char *string, int precision)
{
	int32_t i = 0;
	while (*(string++)) {
		if ((precision != -1) && (i >= precision))
			break;
		i++;
	}
	return i;
}

int MemCmp(const char *str1, const char *str2, int len)
{
	int i;
	const char *s1, *s2;
	s1 = str1;
	s2 = str2;
	for (i = 0; i < len; i++) {
		if ((*s1 - *s2)) {
			return (*s1 - *s2);
		}
		s1++;
		s2++;
	}
	return 0;
}
