#ifndef _MRC_FREE_CURRENCY_H_
#define _MRC_FREE_CURRENCY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mrc_base.h"

typedef void (*PluginLoadCB)(int32 result);
typedef void (*ChargeResultNotify)(int32 result);

// 初始化计费插件，如果计费插件不存在会去自动下载，
//ui控制是否显示插件版本检查界面，
//needCloseNet控制插件完成网络交互后是否关闭网络，
//回调函数loadCB通知了加载插件的结果
int32 mrc_freecurrency_init(int8 ui, int8 needCloseNet, 
							PluginLoadCB loadCB);

// 检查本地付费信息，如果regPoint的计费点已经付费返回MR_SUCCESS，否则返回MR_FAILED。
//对于payID为0的情况，由于
// 会启动浏览器，所以要设置回调函数，通知浏览器调用结果，
//回调函数擦描述为MR_SUCCESS表示浏览器调用成功。
int32 mrc_freecurrency_reg_check(int32 regPoint, 
								 ChargeResultNotify notifyCB);

// 为计费点regPoint进行支付。
// amount是支付虚拟货币的数量 
// brief和detail分别是计费的简述和详述，简述不超过40汉字，
// 详述不超过一屏幕，notifyCB通知回调返回MR_SUCCESS表示支付成功，其余表示失败。
int32 mrc_freecurrency_reg_pay(int32 regPoint, int32 amount, char *brief, char *detail, 
							   ChargeResultNotify notifyCB);

// 为计费点gamePoint进行支付。
// amount是支付虚拟货币的数量 
// brief和detail分别是计费的简述和详述，简述不超过40汉字，
// 详述不超过一屏幕，notifyCB通知回调返回MR_SUCCESS表示支付成功，其余表示失败
int32 mrc_freecurrency_prop_pay(int32 gamePoint, int32 amount, char *brief, char *detail, 
								ChargeResultNotify notifyCB);

// 卸载支付插件
int32 mrc_freecurrency_unload(void);

#ifdef __cplusplus
}
#endif
#endif