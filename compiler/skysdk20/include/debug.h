/*******************************************************************************
 * CopyRight (c) 2005-2008 SKY-MOBI Ltd. All rights reserved.
 * Filename:  debug.h
 * Creator:   wangjian
 * Version:   0.0
 * Date:      
 * Description:
 *******************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mrc_base.h"

#ifdef DEBUG
#ifdef SDK_MOD
#define DBG_PRINTF(a)	\
	do {		\
		mrc_printf a;	\
	}while(0)
#else
#define DBG_PRINTF(a) \
		do {		\
			mrc_printf a;	\
		}while(0)
#endif

#ifdef SDK_MOD
	#define	QUICK_DBG	DBG_PRINTF(("enter FILE:%s @LINE: %d", __FILE__,__LINE__));
#else
	#define	QUICK_DBG	DBG_PRINTF(("FILE:%s @LINE: %d", __FILE__,__LINE__));//DBG_PRINTF(("enter %s, FILE:%s @LINE: %d", __func__, __FILE__,__LINE__));
#endif
#else
#define DBG_PRINTF(a)
#define	QUICK_DBG
#endif

#ifdef MAIN_TRACE_ENABLE
#define MAIN_TRACE(a)	\
	do {		\
		mrc_printf ("MAIN_TRACE ");	\
		mrc_printf a;	\
	}while(0)
#else
#define MAIN_TRACE(a)
#endif



//////////////////////////
#define SLEEP_DEBUG(a)	\
	do {		\
		mrc_printf a;	\
		mrc_sleep(200);	\
	}while(0)

#ifdef SDK_MOD
#define	SLEEP_QUICK_DBG	SLEEP_DEBUG(("enter FILE:%s @LINE: %d", __FILE__,__LINE__));
#else
#define	SLEEP_QUICK_DBG	SLEEP_DEBUG(("FILE:%s @LINE: %d", __FILE__,__LINE__));//DBG_PRINTF(("enter %s, FILE:%s @LINE: %d", __func__, __FILE__,__LINE__));
#endif

#ifdef SDK_MOD
#define FPRINTF(...)
#define __FPRINTF__(...)
#define	QUICK_FPRINTF
#else

#define FPRINTF(...) \
	do{\
		int32 h31415926len;\
		char msgBuffer[256] = {0};\
		int32 h31415926 = mrc_open("debug.txt", MR_FILE_CREATE|MR_FILE_WRONLY);\
		if (h31415926)\
		{\
			char *p = msgBuffer; \
			mr_datetime t; \
			mrc_getDatetime(&t); \
			h31415926len = sprintf(msgBuffer, "[%d/%02d/%02d %02d:%02d]\r\n", t.year, t.month, t.day, t.hour, t.minute); \
			mrc_seek(h31415926, 0, MR_SEEK_END);\
			p = msgBuffer + h31415926len; \
			h31415926len += sprintf(p, __VA_ARGS__);\
			mrc_write(h31415926, msgBuffer, h31415926len);\
			mrc_write(h31415926, "\r\n\r\n", 4);\
			mrc_close(h31415926);\
		}\
	}while(0)

#define __FPRINTF__(_fname_, ...) \
	do{\
		int32 h31415926len;\
		char *msgBuffer;\
		int32 h31415926 = mrc_open(_fname_, MR_FILE_WRONLY);\
		if(h31415926)	\
		{\
			msgBuffer = malloc(1024);\
			if (!msgBuffer)\
			{\
				mrc_close(h31415926);\
				break;\
			}\
			mrc_seek(h31415926,0,MR_SEEK_END);\
			memset(msgBuffer, 0, 1024);\
			h31415926len = sprintf(msgBuffer, __VA_ARGS__);\
			mrc_write(h31415926, msgBuffer, h31415926len);\
			mrc_close(h31415926);\
			free(msgBuffer);\
		}\
	}while(0)

#define	QUICK_FPRINTF	FPRINTF("enter FILE:%s @LINE: %d", __FILE__,__LINE__);
#endif	


	
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEBUG_H__ */
