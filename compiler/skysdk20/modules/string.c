#include "string.h"

#include "mrc_exb.h"

#ifndef WIN32
PSTR tolower(PSTR str)
{
	int i = 0;

	for(; str[i]; i++)
	{
		if(str[i] >= 'A' && str[i] <= 'Z') 
			str[i] += 'a' - 'A';
	}

	return str;
}


PSTR toupper(PSTR str)
{
	int i = 0;

	for(; str[i]; i++)
	{
		if(str[i] >= 'a' && str[i] <= 'z') 
			str[i] += 'A' - 'a';
	}

	return str;
}
#endif

PWSTR asc2uni( PSTR asc )
{
	int32 err, len;

	if(!asc) return NULL;
	return (uint8*)mrc_c2u(asc, &err, &len);
}

//使用VM编码表转换GB字串
PWSTR asc2uniEx(PSTR asc, PT_UNICODE pUni)
{
	int32 err;

	memset(pUni, 0, sizeof(T_UNICODE));
	if(!pUni) return NULL;
	pUni->uni_str = (PWSTR)mrc_c2uVM(asc, &err, &pUni->uni_len);

	return pUni->uni_str;
}

//是否由asc2uniEx转换unicode得到的内存
VOID FreeUniStr(PT_UNICODE pUni)
{
	if(!pUni) return;

	if(pUni->uni_str)
		mrc_freeFileData(pUni->uni_str, pUni->uni_len);
	memset(pUni, 0, sizeof(T_UNICODE));
}

PSTR uni2asc( PWSTR uni )
{
	PSTR asc;
	int32 uni_len, asc_len;

	asc_len = uni_len = wstrlen((PCWSTR)uni);
	asc = (PSTR)mrc_malloc(asc_len);
	mrc_memset(asc, 0, asc_len);
	mrc_unicodeToGb2312(uni, uni_len, (uint8**)&asc, &asc_len);

	return asc;
}

void unicode2asc(char * input, char * outPut,int32 outLen)
{
	int len,i=0;

	memset(outPut, 0, outLen);	
	if(NULL == input)
		return;
	
	len = mrc_wstrlen(input);
	if (!len)
	{
		memset(outPut, 0, outLen);	
	}else
	{
		memset(outPut, 0, outLen);
		while(len&&outLen)
		{
			outLen--;
			outPut[i] = input[i*2+1];
			i++;
			len -= 2;
		}
	}
}

void asc2unicode(char * input, char * outPut, int32 outLen)
{
	int len, i =0;

	len = mrc_strlen(input);
	if(!len)
		memset(outPut, 0, outLen);
	else
	{
		memset(outPut, 0, outLen);
		while(len && outLen)
		{
			outLen-=2;
			outPut[i*2+1] = input[i];
			i++;
			len--;
		}
	}
}

//创建字符串副本
PSTR copystr(PSTR str){
	int l;
	PSTR p;

	if(!str)
		return NULL;

	l = strlen(str) + 1;
	p = malloc(l);
	memcpy(p, str, l);

	return p;
}

//字符串插入
PSTR strinsert(PSTR src, int pos, PSTR s){
	int l, l2;

	if(!src || !s) return NULL;

	l = strlen(src);

	if(pos<0 || pos>l) return NULL;

	l2 = strlen(s);

	//void *memmove(void *dest, const void *src, unsigned int count);
	if(pos == 0){ //头插
		memmove(src+l2, src, l+1);
		memmove(src, s, l2);
	}else if (pos == l)	//尾插
	{
		memmove(src+l, s, l2+1);
	}else{
		memmove(src+pos+l2, src+pos, l-pos+1);
		memmove(src+pos, s, l2);
	}

	return src;
}


//不区分大小写比较字符串
Sint32 stricmp(PCSTR dst, PCSTR src)
{
    CHAR f, l;
    
    do{
        if (((f = *(dst++)) >= 'A') && (f <= 'Z'))
            f -= ('A' - 'a');
        
        if (((l = *(src++)) >= 'A') && (l <= 'Z'))
            l -= ('A' - 'a');
    } while (f && (f == l));
    
    return (f - l);
}

int32 strincmp(PSTR src, PSTR dst, int32 len)
{
	int i;
	CHAR f = 0, l = 0;

	for(i=0; i<len; ++i)
	{
		if (((f = *(dst++)) >= 'A') && (f <= 'Z'))
			f -= ('A' - 'a');

		if (((l = *(src++)) >= 'A') && (l <= 'Z'))
			l -= ('A' - 'a');

		if(f != l) break;
	}

	return (f - l);
}

PSTR trimleft(PSTR str)
{
	return str;
}


PSTR trimright(PSTR str)
{
	return str;
}


PSTR trim(PSTR str)
{
    int i;
    PSTR porg = str;
    int len = wstrlen((PCWSTR)str);

    //trim left
    for(;str[0] == 0 && str[1] == ' '; str += 2);

    //trim right
    for(i = len -1; porg[i -1] == 0 && porg[i] == ' '; i -= 2)
        porg[i] = 0;

    if(porg != str) 
		wstrcpy((PWSTR)porg, (PCWSTR)str);

    return porg;
}



Sint32 split(PSTR str, CHAR sep, PSTR values[], Sint32 size)
{
	int i;	
	for(i = 0; i < size; i++)
	{
		while(*str && *str == sep) str++; //test space
		if(!*str) break;
		values[i] = str;
		while(*str && *str != sep) str++;
		if(!*str) return i+1;
		*str++ = 0;
	}	

	return i;
}

//获得Unicode字符串长度 
Sint32 wstrlen(PCWSTR str)
{
	int i = 0;
	while(str[i] || str[i+1]) 
		i+=2;
	return i;
}

//复制Unicode字符
PWSTR wstrcpy(PWSTR dst, PCWSTR src)
{
	int i = 0;
	while(src[i] || src[i+1]) 
	{
		dst[i] = src[i];
		dst[i+1] = src[i+1];
		i+=2;
	}

	dst[i] = dst[i+1] = 0;
	return dst;
}

//拼接Unicode字符
PWSTR wstrcat(PWSTR dst, PCWSTR src)
{
	int len = wstrlen(dst);
	return wstrcpy(dst + len, src);
}

//复制size个Unicode到目标字串
PWSTR wstrncpy(PWSTR dst, PCWSTR src, Sint32 size)
{
	int i = 0;
	size -= 2;

	while(src[i] || src[i+1]) 
	{
		if(i >= size) break;

		dst[i] = src[i];
		dst[i+1] = src[i+1];
		i+=2;
	}

	dst[i] = dst[i+1] = 0;
	return dst;
}


PCWSTR wstrchr(PCWSTR str, UCHAR chr1, UCHAR chr2)
{
	while(str[0] || str[1]) 
	{	
		if(str[0] == chr1 && str[1] == chr2)
			return str;
		str += 2;
	}
	
	return NULL;
}

//比较两个字符串Unicode
Sint32 wstrcmp(PCWSTR str1, PCWSTR str2)
{
	while((str1[0] || str1[1]) && str1[0] == str2[0] && str1[1] == str2[1])
	{
		str1+=2; 
		str2+=2;
	}

	return str1[0]==str2[0]? str1[1]-str2[1] : str1[0]-str2[0];
}


PWSTR wtrimleft(PWSTR str)
{
	return str;
}


PWSTR wtrimright(PWSTR str)
{
	return str;
}


PWSTR wtrim(PWSTR str)
{
	int i;
	PWSTR porg = str;
	int len = wstrlen(str);

	//trim left
	for(;str[0] == 0 && str[1] == ' '; str += 2);

	//trim right
	for(i = len -1; porg[i -1] == 0 && porg[i] == ' '; i -= 2)
		porg[i] = 0;

	if(porg != str) wstrcpy(porg, str);

	return porg;
}

//将 UTF8 转为 Unicode
Sint32 str2wstr(PCSTR utf8, PWSTR unicode, Sint32 size)
{
	int i = 0, u = 0;
	
	mrc_memset(unicode, 0, size); 
	while(utf8[i] && u < size - 2)
	{
		if((utf8[i] & 0x80) == 0)
		{
			// one byte 0...
			unicode[u+1] = utf8[i++];
		}else if((utf8[i] & 0x20) == 0){
			// two bytes 110... 10...
			unicode[u] = (UCHAR)((utf8[i]) & 0x1f) >> 2;
			unicode[u+1] = ((UCHAR)((utf8[i]) & 0x1f) << 6) | (utf8[i+1] & 0x3f);
			i+=2;
		}else{
			// three bytes 1110... 10... 10...
			unicode[u] = ((utf8[i] & 0x0f) << 4) | ((utf8[i+1] & 0x3f) >> 2);
			unicode[u+1] = (((utf8[i+1] & 0x3f) << 6)) | (utf8[i+2] & 0x3f);
			i+=3;
		}

		u+=2;
	}

	return u;
}

//将Unicode转为UTF8
Sint32 wstr2str(PCWSTR unicode, PSTR utf8, Sint32 size)
{
	int u = 0, i = 0;

	mrc_memset(utf8, 0, size);
	while((unicode[u] || unicode[u+1]) && i < size-1)
	{
		if(unicode[u] == 0 && unicode[u+1] < 128)
		{
			// 0 - 7 bits
			utf8[i++] = unicode[u+1];
		}else if((unicode[u] & 0xf8) == 0){
			// 8 - 11 bits
			utf8[i++] = 0xc0 | (unicode[u] << 2) | (unicode[u+1] >> 6);
			utf8[i++] = 0x80 | (unicode[u+1] & 0x3f);
		}else{
			// 12 - 16 bits
			utf8[i++] = 0xe0 | (unicode[u] >> 4);
			utf8[i++] = 0x80 | ((unicode[u] & 0x0f) << 2) | (unicode[u+1] >> 6);
			utf8[i++] = 0x80 | (unicode[u+1] & 0x3f);
		}

		u+=2;
	}
	
	return i;
}


char *strDup(char *pcStr)
{
    char *pcTmp = NULL;
    int iSize = 0;
    
    if(NULL == pcStr)
    {
        return NULL;
    }
    
    iSize = strlen(pcStr) + 1;
    pcTmp = malloc(iSize);
    if(NULL == pcTmp)
    {
        return NULL;
    }
    memset(pcTmp, 0, iSize);
    strcpy(pcTmp, pcStr);

    return pcTmp;
}


char *strChrEx(const char *src,int c)
{
    char *pTmp = NULL;
    
    if(NULL == src)
    {
        return NULL;
    }

    while(*src != 0)
    {
        if(*src == c)
        {
            pTmp = (char *)src;
            break;
        }
        src++;
    }
    return pTmp;
}

/*
*    CR:0x00 0x0d
*    LF: 0x00 0x0a
*    remove all CRLF
*/
PWSTR trimCRLF(PWSTR str)
{
    int iSize = 0;
    int i = 0;
   // int j = 0;

    if(NULL == str)
    {
        return NULL;
    }
    iSize = wstrlen((PCWSTR) str);
    for(i = 0; i < iSize;)
    {
        if(((0x00 == str[i]) && (0x0d == str[i+1])) || ((0x00 == str[i]) && (0x0a == str[i+1])))
        {
            memcpy(str+i, str+i+2, (iSize - i));
            iSize -= 2;
        }
        else
        {
            i+=2;
        }
    }

    return str;
}


PWSTR wstrdup(PCWSTR str)
{
    int len = 0;
    PWSTR pDest = 0;
    
    if(0 == str)
    {
        return NULL;
    }
    len = wstrlen(str) + 2;
    
    pDest = malloc(len);
    if(NULL == pDest)
    {
        return NULL;
    }

    memcpy(pDest, str, len);
    return pDest;
}

