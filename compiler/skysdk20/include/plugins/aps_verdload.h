#ifndef _APS_VERDLOAD_H_
#define _APS_VERDLOAD_H_

#include "aps_main.h"
#ifdef __cplusplus
extern "C" {
#endif

#define VERDLOAD_MRP_NAME    NULL
#define VERDLOAD_EXT_NAME    "verdload.ext"
#define VERDLOAD_MRP_FOLDER  "plugins" 


#ifndef SDK_MOD
/* 装载下载模块 */
#define aps_verdload_loadExt()  \
	aps_main_loadExt(APS_EXT_ID_VERDLOAD,VERDLOAD_MRP_NAME,VERDLOAD_MRP_FOLDER,VERDLOAD_EXT_NAME)

#else
/* 模拟器装载下载模块 */
int32 aps_verdload_loadExt();

#endif

/*立即卸载下载模块*/	
#define	aps_verdload_unloadExt() \
	aps_main_unloadExt(APS_EXT_ID_VERDLOAD,0)

/*延迟卸载下载模块*/
#define aps_verdload_delay_unloadExt() \
	aps_main_unloadExt(APS_EXT_ID_VERDLOAD,1)

/*暂停下载模块*/	
#define aps_verdload_pauseExt() \
	aps_main_pauseExt(APS_EXT_ID_VERDLOAD)

/*继续下载模块*/	
#define aps_verdload_resumeExt() \
	aps_main_resumeExt(APS_EXT_ID_VERDLOAD)

/*得到下载版本号*/
#define aps_verdload_getExtVersion() \
	aps_main_getExtVersion(APS_EXT_ID_VERDLOAD)


#define VERDLOAD_ID_DL_SRC                1
#define VERDLOAD_ID_DL_CANCEL             2
#define VERDLOAD_ID_DL_PRECONFIG_NETWORK  3


/* 下载状态 */
typedef enum
{
	MR_DL_STATUS_DIALING = 0,              /* 正在拨号*/
    MR_DL_STATUS_CONNECTING,               /* 正在连接下载服务器 */
	MR_DL_STATUS_DOWNLOADING,              /* 正在下载数据 */
    MR_DL_STATUS_COMPLETE,                 /*下载完成     */

	MR_DL_STATUS_EXCEPTION                /*下载异常     */
}MR_DL_STATUS;

typedef enum
{
	MR_DL_ERROR_REASON_NONE = 0,          /* 正常    */ 
    MR_DL_ERROR_REASON_DIAL,              /*拨号错误 */
	MR_DL_ERROR_REASON_CONNECT,           /*连接下载服务器错误*/
	MR_DL_ERROR_REASON_NOT_FOUND,         /*服务器未找到需下载资源*/
	MR_DL_ERROR_REASON_TIMEOUT,           /*下载超时*/
	MR_DL_ERROR_REASON_FILE,              /*文件错误*/

	MR_DL_ERROR_REASON_CANCEL               /*用户中止下载*/                
}MR_DL_ERROR_REASON;


/*配置预先网络拨号信息，应用如果已经拨号,则不需要进行拨号*/
typedef enum
{
	DL_PRECONFIG_NETWORK_NONE = 0,             /*无预先网络配置*/
	DL_PRECONFIG_NETWORK_CMWAP,            /*预先网络CMWAP */
	DL_PRECONFIG_NETWORK_CMNET             /*预先网络CMNET */ 
}MR_DL_PRECONFIG_NETWORK;

/*下载过程回调定义
  
   status:    当前通知的状态
   reason:    错误原因,仅在 MR_DL_STATUS_EXCEPTION时有效，其它状态下为MR_DL_ERROR_REASON_NONE
   p1:        status == MR_DL_STATUS_DOWNLOADING 时为总下载百分比值,如20%,p1=20,
              其它status状态下为备用参数
   (p2-p3):   status == MR_DL_STATUS_DOWNLOADING 时p2 为当前已下载数据，p3为应下载全部数据长度
              其它status状态下目前作为备用参数
*/
typedef void (*MR_DL_CB)(MR_DL_STATUS status,MR_DL_ERROR_REASON reason,int32 p1,int32 p2,int32 p3);
/*
  开始下载资源文件
  srcid:     资源ID  
  pathname:  资源存放路径
  cb:        MR_DL_CB类型的回调函数，用来通知下载的过程。 
  ui:        VERDLOAD_FLAG_UIBYAPP:  由应用来显示UI,否则由下载模块来显示UI
             VERDLOAD_FLAG_HINT: 有下载提示
  
*/
#define MR_DL_FLAG_UIBYAPP   0x0001    /* 由client来绘画下载过程UI*/
#define MR_DL_FLAG_UIHINT      0x0002    /* 在由verdload来绘画下载过程时，是否需要下载提示界面 */

#define MR_DL_SRC_RES          0  /* 普通资源包 */   
#define MR_DL_SRC_PLUGIN       1  /* 插件       */
#define MR_DL_SRC_COOKIE_PROXY 2

typedef struct  
{
	int32 srcType; /*  MR_DL_SRC_RES 或者 MR_DL_SRC_PLUGIN */
	int32 srcid;   /*  所下载的资源ID */
	int32 srcver;  /*  需要更新到的版本号*/
	int32 reqver;  /*  需要更新到的最低版本号*/
}MR_DL_SrcInfo;

typedef struct 
{
	uint16 *title;  /*下载界面标题  unicode*/ 
	uint16 *hint;   /*下载此文件的详细说明  unicode*/
}MR_DL_UI_TIP;


/*预先设置拨号情况
  precfg_network:  预先的拨号情况,如果不为DL_PRECONFIG_NETWORK_NONE，则VREDLOAD模块将不进行拨号动作

  用来指示在下载过程中需不需要重新拨号
  输入:
	precfg_network:  预先的拨号环境,如果不为DL_PRECONFIG_NETWORK_NONE，则下载过程中将不进行重新拨号动作	
  输出:
	 MR_SUCCESS:  成功
	 MR_FAILED:   失败
*/
#define aps_verdload_preconfig_network(precfg_network)\
	mrc_extSendAppEventEx(APS_EXT_ID_VERDLOAD,VERDLOAD_ID_DL_PRECONFIG_NETWORK,(int32)(precfg_network),0,0,0,0)


/*
  下载文件

  srcinfo [MR_DL_SrcInfo*]:   文件资源信息 
  pathname[char*]:            文件存放的本地绝对路径
  cb[MR_DL_CB]:               用来通知下载的过程
  ui[int32]:                  VERDLOAD_FLAG_UIBYAPP或VERDLOAD_FLAG_HINT
  ui_tip[MR_DL_UI_TIP*]:      如果由verdload控制下载界面,需提供的相关文本说明                
*/

#define aps_verdload_dl(srcinfo,pathname,cb,ui,ui_tip)\
    mrc_extSendAppEventEx(APS_EXT_ID_VERDLOAD, VERDLOAD_ID_DL_SRC,(int32)(srcinfo),(int32)(pathname),(int32)(cb),(int32)ui,(int32)ui_tip)


/*
   取消当前下载任务
*/
#define aps_verdload_dl_cancel()\
	mrc_extSendAppEventEx(APS_EXT_ID_VERDLOAD,VERDLOAD_ID_DL_CANCEL,0,0,0,0,0)


#ifdef __cplusplus
}
#endif
#endif


