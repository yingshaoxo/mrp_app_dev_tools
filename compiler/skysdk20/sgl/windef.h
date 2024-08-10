#ifndef _WINDEF_H_
#define _WINDEF_H_

#include "types.h"


#ifdef __cplusplus
extern "C" {
#endif


//---------- 数据类型定义 ----------------------------------------------
/**
 * \brief A type definition for the first message paramter.
 */
typedef UINT            WPARAM;

/**
 * \brief A type definition for the second message paramter.
 */
typedef DWORD           LPARAM;

/**
 *\brief A type definition for for RESULT
 */
typedef DWORD		LRESULT;

/**
 * \brief Returns the low byte of the word \a w.
 */
#define LOBYTE(w)           ((BYTE)(w))

/**
 * \brief Returns the high byte of the word \a w.
 */
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

/**
 * \brief Makes a word from \a low byte and \a high byte.
 */
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))

/**
 * \brief Returns the low word of the double word \a l
 */
#define LOWORD(l)           ((WORD)(DWORD)(l))

/**
 * \brief Returns the high word of the double word \a l
 */
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))

/**
 * \brief Returns the low signed word of the double word \a l
 */
#define LOSWORD(l)          ((SWORD)(DWORD)(l))

/**
 * \brief Returns the high signed word of the double word \a l
 */
#define HISWORD(l)          ((SWORD)((((DWORD)(l)) >> 16) & 0xFFFF))

/**
 * \brief A type definition for GUIAPI
 */
#define GUIAPI

/**
 * \brief A type definition for Handle to window or control.
 */
typedef struct SGL_Window* HWND;

/**
 * \brief A type definition for Handle to Menu.
 */
typedef HWND HMENU;

/**
 * \brief A type definition for Handle to Font.
 */
typedef Uint32 HFONT;

/**
 * \brief A type definition for Handle to Bitmap.
 */
typedef VOID* HBITMAP;

/**
 * \brief A type definition for Window identifier.
 */
typedef Uint16 WID;

/**
 * \brief A macro returns the number of elements in a \a table.
 */
#define TABLESIZE(table)    (sizeof(table)/sizeof(table[0]))

/**
 * \brief A macro definition for invalid resource id 
 */
#ifndef RESID_INVALID
#define RESID_INVALID	0xFFFFFFFFL
#endif


//--------- 系统常量定义区 -------------------------------------
/**
 * SGL最大支持的窗口数（包括子窗口）
 */
#ifndef SGL_MAX_WINDOW_COUNT
#define SGL_MAX_WINDOW_COUNT		40
#endif

/**
 * \按键按下多长时间开始响应重复按键
 */
#ifndef SGL_KEYREPEAT_START_INTERVAL
#define SGL_KEYREPEAT_START_INTERVAL		200
#endif

/**
 * \重复按键响应频率
 */
#ifndef SGL_KEYREPEAT_INTERVAL
#define SGL_KEYREPEAT_INTERVAL			100
#endif 

/**
 * \响应双击的时间间隔
 */
#ifndef SGL_RECEIVE_DOUBCLICK_INTERVAL
#define SGL_RECEIVE_DOUBCLICK_INTERVAL		500
#endif

//定义Toast显示时间长度
#define TOAST_SHOW_LONG		3000
#define TOAST_SHOW_SHORT	2000
#define TOAST_SHOW_EVER		-1		//不会自动消失，需要手动调用SGL_ToastHide

/**
 * 系统桌面窗口，任何窗口的父窗口，他的子窗口即桌面
 */
#define HWND_DESKTOP	(HWND)(0xFFFFFFFF)


//--------- 回调函数原型声明区 -------------------------------------------------
/**
 * 窗口过程SGL_WndProc类型定义
 */
typedef LRESULT (*SGL_WndProc)(HWND, UINT, WPARAM, LPARAM);

/**
 * 窗口事件过滤器原型
 */
typedef LRESULT (*SGL_AppEventFilter)(UINT, WPARAM, LPARAM);

/**
 * 窗口背景绘制回调原型
 */
typedef VOID (*SGL_WndDrawFunc)(HWND);



//--------- 数据结构声明区 -----------------------------------------------------
/**
 * 窗口数据结构
 */
typedef struct SGL_Window
{
	/** 窗口ID */
	WID id;
	/** space align use 本SGL模块使用了这个参数来代表窗口过程函数是内部还是外部的 */
	Uint16 reserve;		
	/** 窗口样式 such as WS_TRANSPARENT etc. */
	DWORD style;
	/** 窗口位置大小 */
	int left, top, width, height;
	/** 窗口背景色、前景色 */
	Uint32 bgcolor, fgcolor;
	/** 窗口过程回调函数 */
	SGL_WndProc wndproc;
	/** 窗口绘制（目前只对背景有效）回调函数 */
	SGL_WndDrawFunc drawFunc;
	/** 父窗口句柄 顶级窗口的父窗口是HWND_DESKTOP */
	HWND parent; 
	/** 第一个子窗口 */
	HWND child;
	/** 下一个兄弟窗口 */
	HWND next;
	/** 是一个兄弟窗口 */
	HWND prev;
	/** 获得焦点的子窗口 */
	HWND focus;
	/** 监听事件的窗口 为非NULL时 SGL_NotifyParent 会将消息发送到监听窗口 */
	HWND listener;
	/** 用户设置给窗口的数据 */
	DWORD userdata;
	/** 窗口数据 */
	DWORD windata;
}SGL_Window;

/**
 * 编辑框数据结构
 */
typedef struct SGL_EditInfo
{
	/** 本地输入法窗口显示的标题 */
	PCWSTR title; 
	/** 预置内容 */
	PWSTR buffer;
	/** 预置内容长度 */
	Uint32 size;
}SGL_EditInfo;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif