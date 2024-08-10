#ifndef _TS_MAIN_H_
#define _TS_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mrc_base.h"
#include "mrc_base_i.h"


//打印log功能
#define ZHU_LOG mrc_printf


//--------所有EXTID---------------------
//系统extid
#define ZHU_EXTID_CFUNCTION		1			//cfunction.ext 的 EXTID

	typedef enum {
		TSMODULE_ID_BASE = 1000,

		ZHU_EXTID_MAIN,			//用户开发的主ext的 EXTID

		ZHU_EXTID_GRAPHICS,					//TS 绘图 模块
		ZHU_EXTID_XML,						//TS XML 模块
		ZHU_EXTID_FSCANER,					//TS 文件浏览 模块
		ZHU_EXTID_GB2UNI,					//TS GB2312 转 UNICODE 模块
		ZHU_EXTID_GZIP,						//TS GZIP 模块
		ZHU_EXTID_SGL,						//TS SGL 模块

		TSMODULE_ID_MAX
	}TS_MODULE_ID;

//#define TSMODULE_ID_BASE		1000
//#define ZHU_EXTID_LOADER		2			//下载模块
//#define ZHU_EXTID_BROWSER		4			//浏览器模块
//#define ZHU_EXTID_GRAPHICS		5			//TS 绘图 模块
//#define ZHU_EXTID_XML			6			//TS XML 模块
//#define ZHU_EXTID_FSCANER		7			//TS 文件浏览 模块
//#define ZHU_EXTID_GB2UNI		8			//TS GB2312 转 UNICODE 模块
//#define ZHU_EXTID_GZIP			9			//TS GZIP 模块
//#define ZHU_EXTID_SGL			10			//TS SGL 模块

//#define ZHU_EXTID_MAIN		2000	//用户开发的主ext的 EXTID


//开发者的extid 必须从下面这个值开始
#define ZHU_EXTID_USEREXTID_START 0x1001	//自定义EXTID基ID


//--------主模块功能函数ID---------------------
//前缀：ZHU_FUNID_
typedef enum {
	ZHU_FUNID_BASE = 1000,		//起始功能ID

	ZHU_FUNID_EXIT,	//通知主模块整个应用将退出

	ZHU_FUNID_GET_SCNSIZE,	//获取屏幕尺寸
	ZHU_FUNID_REGFUN,		//注册EXT功能函数
	ZHU_FUNID_LOADEXT,		//加载EXT
	ZHU_FUNID_REFSCN,		//刷新屏幕
	ZHU_FUNID_REFSCN_RECT,
	ZHU_FUNID_REFSCN_FULL,
	ZHU_FUNID_GET_EXTLOADMOD,	//获取插件加载模式
	ZHU_FUNID_BRINGEXTTOTOP,	//将EXT置顶
	ZHU_FUNID_UNLOADEXT,		//卸载ext
	ZHU_FUNID_EXTPAUSE,		//EXT暂停
	ZHU_FUNID_EXTRESUME,	//EXT恢复
	ZHU_FUNID_SAVEVM,	//保存VM状态
	ZHU_FUNID_LOADVM,	//恢复VM状态
	ZHU_FUNID_SETMOD,
	ZHU_FUNID_SETEXTALWAYSHANDELEVENTS,
	ZHU_FUNID_BLOCKEXT,
	ZHU_FUNID_UNBLOCKEXT,
	ZHU_FUNID_GETHANDEL,
	ZHU_FUNID_GETID,
	ZHU_FUNID_GETMRPINFO,
	ZHU_FUNID_RETRIEVEFILEFROMMRP,	//释放MRP包内文件
	//ZHU_FUNID_GET_EXT_VERSION,
	ZHU_FUNID_RUNMRP,
	ZHU_FUNID_SWITCH_CDR,
	ZHU_FUNID_RESTORE_CDR,
	ZHU_FUNID_TRANS_KEY_EVENT,
	//ZHU_FUNID_,
	//ZHU_FUNID_,
	//ZHU_FUNID_,
	//ZHU_FUNID_,
	//ZHU_FUNID_,
	//ZHU_FUNID_,
	//ZHU_FUNID_,

	ZHU_FUNID_MAX
}ZHU_FUNID;

//--------------------------------------------------------------------------------
//子模块入口函数声明，因为模拟器环境下不能存在同名函数，
//所以模拟器环境下必须采用这套入口函数作为你的主EXT入口函数
//--------------------------------------------------------------------------------
#ifdef SDK_MOD
extern int32 mrc_mainextInit(void);
extern int32 mrc_mainextEvent(int32 code, int32 p0, int32 p1);
extern int32 mrc_mainextPause(void);
extern int32 mrc_mainextResume(void);
extern int32 mrc_mainextExit(void);
#else
#define mrc_mainextInit mrc_init
#define mrc_mainextEvent mrc_event
#define mrc_mainextPause mrc_pause
#define mrc_mainextResume mrc_resume
#define mrc_mainextExit mrc_exitApp
#endif


//-------------------------------------------------------------------------------------
//插件的功能函数原型
//p0:  自定义消息编号  p1~p5: 参数
//功能函数指对插件 mrc_extSendAppEventEx 后接受消息并处理的函数
//注意：mrc_extRecvAppEventEx 函数无效
typedef int32 (*ExtFun)(int32 funid, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

//注册ext通信功能函数，当别的模块向该模块调用 mrc_extSendAppEventEx
//发送消息时，并不是 mrc_extRecvAppEventEx 接受到消息，而是 该入口函数
//注意：mrc_extRecvAppEventEx 函数无效
#define Zhu_regExtFun(id, fun) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REGFUN, id, (int32)fun, 0,0,0)
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//ext加载函数
//--------------------------------------------------------------------------------
//EXT加载模式
typedef enum {
	ZHU_EXTLOAD_NULL,	//空表示未加载
	ZHU_EXTLOAD_NORMAL,	//已正常加载
	ZHU_EXTLOAD_DUMP,	//Dump模式加载（复用Game Ext代码段）
	ZHU_EXTLOAD_VM,		//加载到VM内存
	ZHU_EXTLOAD_RESET	//通过reset方式加载
}EXT_LOADMOD;

//加载插件地址
typedef struct {
	void* addr;  /*预加载内存地址 */
	int32 len;   /*所提供的用于加载的内存尺寸, ==0 表示不指定长度,将使用插件实际大小内存*/  
}ZHU_MEMINFO, *PZHU_MEMINFO;

#ifdef SDK_MOD
typedef struct {
	mrc_init_t		mrc_init_f;
	mrc_event_t		mrc_event_f;
	mrc_pause_t		mrc_pause_f;
	mrc_resume_t	mrc_resume_f;
	mrc_exitApp_t	mrc_exitApp_f;
}mrcEnterFuncSt;

//加载本身MRP包内的EXT
#define Zhu_loadExtSelf(id, extname, entryfun) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, 0, 0, (int32)entryfun)
	//注：参数顺序：EXTID EXT名 MRP路径 欲加载内存 SDK入口函数

//从某个路径下的MRP内加载EXT
#define Zhu_loadExtPath(id, extname, mrppath, entryfun) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, (int32)mrppath, 0, (int32)entryfun)

#else

//加载本身MRP包内的EXT
#define Zhu_loadExtSelf(id, extname) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, 0,0,0)
	//注：参数顺序：EXTID EXT名 MRP路径 欲加载内存

//从某个路径下的MRP内加载EXT
#define Zhu_loadExtPath(id, extname, mrppath) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, (int32)mrppath, 0,0)

#define Zhu_loadExtAddr(id, extname, mrppath, addr) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, (int32)mrppath, (int32)addr, 0)

#endif


//获取插件加载模式，可以查询插件是否已加载
#define Zhu_getExtLoadMod(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_GET_EXTLOADMOD, id, 0,0,0,0)

//将EXT置顶，最顶上的ext优先接收按键事件，最后加载的ext在最顶上
#define Zhu_bringExtToTop(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_BRINGEXTTOTOP, id, 0,0,0,0)

//卸载EXT，若extid为加载，该ext还被其他模块引用，则返回MR_FAILED，
//成功卸载：返回MR_SUCCESS，该ext无效
//成功卸载时 会调用ext的 mrc_exitApp
#define Zhu_unloadExt(id, delay) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_UNLOADEXT, id, delay, 0,0,0)

#define Zhu_refScn(x, y, w, h) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REFSCN, x, y, w, h, 0)

#define Zhu_refScnRect(r) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REFSCN_RECT, (int32)r, 0,0,0,0)

#define Zhu_refScnFull() \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REFSCN_FULL, 0,0,0,0,0)

//获取屏幕尺寸
#define Zhu_getScnSize() \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_GET_SCNSIZE, 0,0,0,0,0)

#define Zhu_resumeExt(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_EXTRESUME, id, 0,0,0,0)

#define Zhu_pauseExt(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_EXTPAUSE, id, 0,0,0,0)

#define Zhu_saveVM() \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_SAVEVM, 0,0,0,0,0)

#define Zhu_loadVM() \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADVM, 0,0,0,0,0)

//其他插件通过调用次函数，来通知主模块退出虚拟机，
//mrc_exit无效
#define Zhu_exit() \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_EXIT, 0,0,0,0,0)

typedef enum {
	ZHU_EXTMOD_MAIN,	//主模块
	ZHU_EXTMOD_APP,		//普通插件
	ZHU_EXTMOD_PLUG		//应用插件
}ZHU_EXTMOD;

//设置ext模式
#define Zhu_setExtMod(id, mod) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_SETMOD, id, mod, 0,0,0)

//设置一个 EXT总是接受事件
//always为1时，该ext不管在不在顶层都会接受所有事件
#define Zhu_setExtAlwaysHandelEvents(id, always) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_SETEXTALWAYSHANDELEVENTS, id, always, 0,0,0)

//EXT挂起 会暂停游戏的定时器， 并且block按键事件.
#define Zhu_blockExt(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_BLOCKEXT, id, 0,0,0,0)

#define Zhu_unBlockExt(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_UNBLOCKEXT, id, 0,0,0,0)

//获取EXT句柄 返回-1表示无效 mrc_extHandel 代表本身
#define Zhu_getExtHandel(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_GETHANDEL, id, 0, 0,0,0)

//通过句柄获取EXTID 返回 -1 表示无效
#define Zhu_getExtId(handel) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_GETID, handel, 0, 0,0,0)


#ifdef __cplusplus
}
#endif

#endif