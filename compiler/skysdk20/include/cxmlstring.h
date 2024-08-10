#ifndef __STRING_H__
#define __STRING_H__


typedef struct tTag_Rep
{
	int size, capacity;
	char str[1];
}Rep;

typedef struct tTag_microS
{
	Rep * rep;
}microS;


char* microS_start(microS * tmp);
char* microS_finish(microS * tmp);
void i_sInit(microS * tmp, int sz, int cap);
void i_sQuit(microS * tmp);
microS *microS_nullCreate(void);
microS * microS_create( const char * str, int len);
void microS_clear (microS *src);
void microS_assign(microS *src,const char* str, int len);
int micros_firstUse(microS * src, const char * str, int len);
void microS_sReserve (microS *src, int cap);
void microS_RepInit(void);
char * clearSpace(char *s, int len);
#endif

