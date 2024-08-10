#ifndef _MRC_ADVBAR_H_
#define _MRC_ADVBAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mrc_base.h"

// 定义广告条的高度，小屏幕手机为30，大屏幕手机为40
#ifdef SCREEN_176_220
#define ADV_BAR_HEIGHT	30
#else
#define ADV_BAR_HEIGHT	40
#endif

typedef enum {
	NOTIFY_TYPE_DOWNLOAD,
	NOTIFY_TYPE_BROWSER
}NOTIFY_TYPE;


typedef void (*AdvBarPause)();
typedef void (*AdvBarResume)();
typedef void (*AdvBarLoadCB)(int32 result);
typedef void (*AdvActivityCB)(int32 result);
typedef void (*AdvDoneNotify)(NOTIFY_TYPE type);


// 广告插件初始化方法，调用一次
// advChunkID	广告位ID，该字段由SKY广告运营分配
// x			广告条的起始x坐标
// y			广告条的起始y坐标
// w			广告条的宽度
// smallFont	是否使用SKY的小字体插件，TRUE表示使用SKY字库，FALSE表示使用手机自身字库
// pauseCB		广告插件通知应用挂起的回调函数
// resumeCB		广告插件通知应用恢复的回调函数
// loadCB		广告插件通知应用是否加载成功的回调函数
int32 mrc_advInit(uint32 advChunkID, 
	uint16 x, uint16 y, uint16 w, 
	int8 smallFont, 
	AdvBarPause pauseCB, AdvBarResume resumeCB, AdvBarLoadCB loadCB);


// 初始化广告插件之后，如果要更换广告位，调用此方法进行更换
// advChunkID	广告位ID，该字段由SKY广告运营分配
int32 mrc_advChange(uint32 advChunkID);


// 设置广告执行完成通知方法
// notifyCB		应用开发者传入的通知回调函数，当广告插件中的广告被开发者点击并执行完毕后，
// 广告插件通过次方法通知应用。type == 1表示下载完成、type == 2表示浏览器访问完毕
int32 mrc_advSetNotify(AdvDoneNotify notifyCB);


// 广告插件开始绘图
int32 mrc_advStartDraw(void);


// 广告插件停止绘图
int32 mrc_advStopDraw(void);


// 强制刷新一次界面，如果参数force为TRUE的话，插件自己强制调用mrc_refreshScreen刷新广告区域。
int32 mrc_advRefresh(int32 force);


// 广告插件成为焦点，此时按SELECT按键将会触发广告应用
int32 mrc_advFocus(void);


// 广告插件失去焦点
int32 mrc_advUnFocus(void);


// 设置激活广告内容的按键键值，默认为SELECT按键。
int32 mrc_setActiveKey(int32 key);



#ifdef __cplusplus
}
#endif
#endif