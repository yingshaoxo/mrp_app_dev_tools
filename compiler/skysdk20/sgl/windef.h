#ifndef _WINDEF_H_
#define _WINDEF_H_

#include "types.h"


#ifdef __cplusplus
extern "C" {
#endif


//---------- �������Ͷ��� ----------------------------------------------
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


//--------- ϵͳ���������� -------------------------------------
/**
 * SGL���֧�ֵĴ������������Ӵ��ڣ�
 */
#ifndef SGL_MAX_WINDOW_COUNT
#define SGL_MAX_WINDOW_COUNT		40
#endif

/**
 * \�������¶೤ʱ�俪ʼ��Ӧ�ظ�����
 */
#ifndef SGL_KEYREPEAT_START_INTERVAL
#define SGL_KEYREPEAT_START_INTERVAL		200
#endif

/**
 * \�ظ�������ӦƵ��
 */
#ifndef SGL_KEYREPEAT_INTERVAL
#define SGL_KEYREPEAT_INTERVAL			100
#endif 

/**
 * \��Ӧ˫����ʱ����
 */
#ifndef SGL_RECEIVE_DOUBCLICK_INTERVAL
#define SGL_RECEIVE_DOUBCLICK_INTERVAL		500
#endif

//����Toast��ʾʱ�䳤��
#define TOAST_SHOW_LONG		3000
#define TOAST_SHOW_SHORT	2000
#define TOAST_SHOW_EVER		-1		//�����Զ���ʧ����Ҫ�ֶ�����SGL_ToastHide

/**
 * ϵͳ���洰�ڣ��κδ��ڵĸ����ڣ������Ӵ��ڼ�����
 */
#define HWND_DESKTOP	(HWND)(0xFFFFFFFF)


//--------- �ص�����ԭ�������� -------------------------------------------------
/**
 * ���ڹ���SGL_WndProc���Ͷ���
 */
typedef LRESULT (*SGL_WndProc)(HWND, UINT, WPARAM, LPARAM);

/**
 * �����¼�������ԭ��
 */
typedef LRESULT (*SGL_AppEventFilter)(UINT, WPARAM, LPARAM);

/**
 * ���ڱ������ƻص�ԭ��
 */
typedef VOID (*SGL_WndDrawFunc)(HWND);



//--------- ���ݽṹ������ -----------------------------------------------------
/**
 * �������ݽṹ
 */
typedef struct SGL_Window
{
	/** ����ID */
	WID id;
	/** space align use ��SGLģ��ʹ������������������ڹ��̺������ڲ������ⲿ�� */
	Uint16 reserve;		
	/** ������ʽ such as WS_TRANSPARENT etc. */
	DWORD style;
	/** ����λ�ô�С */
	int left, top, width, height;
	/** ���ڱ���ɫ��ǰ��ɫ */
	Uint32 bgcolor, fgcolor;
	/** ���ڹ��̻ص����� */
	SGL_WndProc wndproc;
	/** ���ڻ��ƣ�Ŀǰֻ�Ա�����Ч���ص����� */
	SGL_WndDrawFunc drawFunc;
	/** �����ھ�� �������ڵĸ�������HWND_DESKTOP */
	HWND parent; 
	/** ��һ���Ӵ��� */
	HWND child;
	/** ��һ���ֵܴ��� */
	HWND next;
	/** ��һ���ֵܴ��� */
	HWND prev;
	/** ��ý�����Ӵ��� */
	HWND focus;
	/** �����¼��Ĵ��� Ϊ��NULLʱ SGL_NotifyParent �Ὣ��Ϣ���͵��������� */
	HWND listener;
	/** �û����ø����ڵ����� */
	DWORD userdata;
	/** �������� */
	DWORD windata;
}SGL_Window;

/**
 * �༭�����ݽṹ
 */
typedef struct SGL_EditInfo
{
	/** �������뷨������ʾ�ı��� */
	PCWSTR title; 
	/** Ԥ������ */
	PWSTR buffer;
	/** Ԥ�����ݳ��� */
	Uint32 size;
}SGL_EditInfo;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif