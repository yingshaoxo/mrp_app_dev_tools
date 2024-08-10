#ifndef _PLAT_H
#define _PLAT_H

#include "mrc_base.h"
#include "mrc_base_i.h"


/**
* 通知移植层本应用需要后台运行
* 数据结构：
*/
typedef struct
{
	int32	appid;             //app id
	uint8	describe[20];     //应用标志符 - "ipqq"
	uint8*  param;           //预留扩展用
}mr_backstage_st;


/**
* 函数1.后台运行程序
* 函数名BackRunApp
*/
/* 后台运行返回后平台收到的事件
* mrc_even(int32 code, int32 param0, int32 param1)
* code = MR_LOCALUI_EVENT
* param0 = 2
*/
int32 BackRunApp(int32 appid);

/**
 * 函数2.恢复后台运行程序
 * 函数名BackRunApp
 */
int32 AwakeApp(int32 appid);

#endif