#ifndef _STRING_H
#define _STRING_H

#include "types.h"
#include "mrc_base.h"

//ת��unicode�Ľṹ
//ʹ�ú��� mrc_freeOrigin �ͷ��ڴ�
typedef struct {
	PWSTR uni_str;   //unicode�ַ���ָ��
	int32 uni_len;   //�ַ������峤��
}T_UNICODE, *PT_UNICODE;


//GB2312תunnicode����Ҫ�ͷ��ڴ棩
PWSTR asc2uni(PSTR asc);

//unicodeתGB2312����Ҫ�ͷ��ڴ棩
PSTR uni2asc(PWSTR uni);

//ʹ��VM�����ת��GB�ִ�
PWSTR asc2uniEx(PSTR asc, PT_UNICODE pUni);

//�Ƿ���asc2uniExת��unicode�õ����ڴ�
VOID FreeUniStr(PT_UNICODE pUni);

//asciiתunicode��ֻ֧��ascii�ַ���
void asc2unicode(char * input, char * outPut, int32 outLen);

//unicodeתascii��ֻ֧��ascii�ַ���
void unicode2asc(char * input, char * outPut,int32 outLen);

//�����ַ�������
PSTR copystr(PSTR str);

//�ַ�������
PSTR strinsert(PSTR src, int pos, PSTR s);

/**
 * \brief Convert a string to lower case.
 *
 * \param str the string to be translated
 * \return the lower case string
 */
PSTR tolower(PSTR str);

/**
 * \brief Convert a string to up case.
 *
 * \param str the string bo be translated
 * \return the up case string
 */
PSTR toupper(PSTR str);

/**
 * \brief Compare two string with case ignore.
 *
 * \param dst the first string
 * \param src the second string
 * \return
 *	<0 when dst < src
 *	=0 when dst = src
 *	>0 when dst > src
 */
Sint32 stricmp(PCSTR dst, PCSTR src);
int32 strincmp(PSTR src, PSTR dst, int32 len);

/**
 * \brief Trim left space chars here just ' '.
 *
 * \param str the string to be trimed
 * \return the trimed string
 */
PSTR trimleft(PSTR str);

/**
 * \brief Trim right space chars here just ' '.
 *
 * \param str the string to be trimed
 * \return the trimed string
 */
PSTR trimright(PSTR str);

/**
 * \brief Trim(����) left&right space chars.
 *
 * \param str the string to be trimed
 * \return the trimed string
 */
PSTR trim(PSTR str);

/**
 * \brief Split(����) the string with specific(����) char.
 * 
 * \param str the string to be splited
 * \param sep the seperator char
 * \param[out] values the splited string array
 * \param size the max size of values
 * \return real size of values array returned
 */
Sint32 split(PSTR str, CHAR sep, PSTR values[], Sint32 size);

/**
 * \brief Get length of the wstring in bytes.
 *
 * \param str the wstring
 * \return the length of the wstring in bytes
 */
Sint32 wstrlen(PCWSTR str);

/**
 * \brief Copy characters of one wstring to another.
 *
 * \param dst Destination string
 * \param src Source string
 * \return the destination string
 */
PWSTR wstrcpy(PWSTR dst, PCWSTR src);

/**
 * \brief Copy characters of one wstring to another.
 *
 * \param dst Destination string
 * \param src Source string
 * \param size of bytes to be copied
 * \return the destination string
 */
PWSTR wstrncpy(PWSTR dst, PCWSTR src, Sint32 size);

/**
 * \brief Compare strings.
 *
 * \param str1 the first string
 * \param str2 the second string
 * \return
 *	0 when str1 == str2
 *	<0 when str1 < str2
 *	>0 when str1 > str2
 */
Sint32 wstrcmp(PCWSTR str1, PCWSTR str2);

/**
 * \brief Find a wchar in wstring.
 * 
 * \param str the wstring
 * \param chr1 the first byte of the wchar
 * \param chr2 the second byte of the wchar
 * \return the first occurrence of c in string, NULL when not found
 */
PCWSTR wstrchr(PCWSTR str, UCHAR chr1, UCHAR chr2);

/**
 * \brief Trim left a wstring.
 *
 * \param str the wstring to be trimed
 * \return the trimed string
 */
PWSTR wtrimleft(PWSTR str);

/**
 * \brief Trim right a wstring.
 * 
 * \param str the wstring to be trimed
 * \return the trimed string
 */
PWSTR wtrimright(PWSTR str);

/**
 * \brief Trim a wstring.
 * 
 * \param str the wstring to be trimed
 * \return the trimed string
 */
PWSTR wtrim(PWSTR str);

/**
 * \brief Convert utf8 string to unicode string.
 *
 * \param utf8 the utf8 string to be transformed
 * \param unicode the unicode buffer for transformed
 * \param size the max size of unicode buffer
 * \return the transformed size in bytes
 */
Sint32 str2wstr(PCSTR utf8, PWSTR unicode, Sint32 size);

/**
 * \brief Convert unicode string to utf8 string.
 *
 * \param unicode the unicode string to be transformed
 * \param utf8 the utf8 buffer for transformed
 * \param size the max size of utf8 buffer
 * \return the transformed size in bytes
 */
Sint32 wstr2str(PCWSTR unicode, PSTR utf8, Sint32 size);

char *strDup(char *pcStr);
PWSTR wstrcat(PWSTR dst, PCWSTR src);
char *strChrEx(const char *src,int c);
PWSTR trimCRLF(PWSTR str);
char *itoa(int num,char *str,int radix);
PWSTR wstrdup(PCWSTR str);

#endif
