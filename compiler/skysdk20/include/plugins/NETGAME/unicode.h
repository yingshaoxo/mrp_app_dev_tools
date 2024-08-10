#ifndef __UNICODE_H__
#define __UNICODE_H__
#include "mrc_base.h"

/*
 * unicode strcat
 * ����:
 * dest:			Ŀ���ַ���
 * src:				Դ�ַ���
 * ����ֵ:			��
 */
char *uniStrcat(char *dest, char *src);

/*
 * unocode strcpy
 */
void uniStrcpy(char * dest, char * src);

/*
 * unicode ascii to ascii
 * ����:
 * input:			unocode�ַ���(ֻ֧��ascii��ʽ��unicode, ��֧������)
 * output:			���ascii���ַ���buf
 * outLen:			�����buf����
 * ����ֵ:			��
 */
void utoc(char * input, char * outPut, int32 outLen);

/* 
 * ascii to unicode
 * ����:
 * dest:			�����Ӧascii��unocode��ʽ
 * src:				����ascii���ַ���
 * srcLen:			����ascii���ַ����ĳ���
 * ����ֵ:			��
 */
void ctou(char *dest, char *src, int32 srcLen);

/*
 * unicode strcmp, unicode�ַ����Ƚ�
 * ����:
 * s1:				unicode�ַ���1
 * s2:				unicode�ַ���2
 * ����ֵ:			0:	��ͬ,  ��0  ����ͬ
 */
int32 ucsCmp(char *s1,char *s2);

/* unicode strcmp, unicode�ַ����Ƚ�
 * ����:
 * s1:				unicode�ַ���1
 * s2:				unicode�ַ���2
 * n:				�Ƚϵ�unocode�ַ�����
 * ����ֵ:			0:	��ͬ,  ��0  ����ͬ
 */
int32 ucsNCmp(char * s1, char * s2, int32 n);

/* ����unocode�ַ�����ǰ��ո� */
char* uniStrtrim(char *str);

/* ����unocode�ַ��������пո�ͻ��з� */
char* uniStrtrimLF(char *str);


#endif /* __UNICODE_H__ */
