#ifndef _TS_MAIN_H_
#define _TS_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mrc_base.h"
#include "mrc_base_i.h"


//��ӡlog����
#define ZHU_LOG mrc_printf


//--------����EXTID---------------------
//ϵͳextid
#define ZHU_EXTID_CFUNCTION		1			//cfunction.ext �� EXTID

	typedef enum {
		TSMODULE_ID_BASE = 1000,

		ZHU_EXTID_MAIN,			//�û���������ext�� EXTID

		ZHU_EXTID_GRAPHICS,					//TS ��ͼ ģ��
		ZHU_EXTID_XML,						//TS XML ģ��
		ZHU_EXTID_FSCANER,					//TS �ļ���� ģ��
		ZHU_EXTID_GB2UNI,					//TS GB2312 ת UNICODE ģ��
		ZHU_EXTID_GZIP,						//TS GZIP ģ��
		ZHU_EXTID_SGL,						//TS SGL ģ��

		TSMODULE_ID_MAX
	}TS_MODULE_ID;

//#define TSMODULE_ID_BASE		1000
//#define ZHU_EXTID_LOADER		2			//����ģ��
//#define ZHU_EXTID_BROWSER		4			//�����ģ��
//#define ZHU_EXTID_GRAPHICS		5			//TS ��ͼ ģ��
//#define ZHU_EXTID_XML			6			//TS XML ģ��
//#define ZHU_EXTID_FSCANER		7			//TS �ļ���� ģ��
//#define ZHU_EXTID_GB2UNI		8			//TS GB2312 ת UNICODE ģ��
//#define ZHU_EXTID_GZIP			9			//TS GZIP ģ��
//#define ZHU_EXTID_SGL			10			//TS SGL ģ��

//#define ZHU_EXTID_MAIN		2000	//�û���������ext�� EXTID


//�����ߵ�extid ������������ֵ��ʼ
#define ZHU_EXTID_USEREXTID_START 0x1001	//�Զ���EXTID��ID


//--------��ģ�鹦�ܺ���ID---------------------
//ǰ׺��ZHU_FUNID_
typedef enum {
	ZHU_FUNID_BASE = 1000,		//��ʼ����ID

	ZHU_FUNID_EXIT,	//֪ͨ��ģ������Ӧ�ý��˳�

	ZHU_FUNID_GET_SCNSIZE,	//��ȡ��Ļ�ߴ�
	ZHU_FUNID_REGFUN,		//ע��EXT���ܺ���
	ZHU_FUNID_LOADEXT,		//����EXT
	ZHU_FUNID_REFSCN,		//ˢ����Ļ
	ZHU_FUNID_REFSCN_RECT,
	ZHU_FUNID_REFSCN_FULL,
	ZHU_FUNID_GET_EXTLOADMOD,	//��ȡ�������ģʽ
	ZHU_FUNID_BRINGEXTTOTOP,	//��EXT�ö�
	ZHU_FUNID_UNLOADEXT,		//ж��ext
	ZHU_FUNID_EXTPAUSE,		//EXT��ͣ
	ZHU_FUNID_EXTRESUME,	//EXT�ָ�
	ZHU_FUNID_SAVEVM,	//����VM״̬
	ZHU_FUNID_LOADVM,	//�ָ�VM״̬
	ZHU_FUNID_SETMOD,
	ZHU_FUNID_SETEXTALWAYSHANDELEVENTS,
	ZHU_FUNID_BLOCKEXT,
	ZHU_FUNID_UNBLOCKEXT,
	ZHU_FUNID_GETHANDEL,
	ZHU_FUNID_GETID,
	ZHU_FUNID_GETMRPINFO,
	ZHU_FUNID_RETRIEVEFILEFROMMRP,	//�ͷ�MRP�����ļ�
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
//��ģ����ں�����������Ϊģ���������²��ܴ���ͬ��������
//����ģ���������±������������ں�����Ϊ�����EXT��ں���
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
//����Ĺ��ܺ���ԭ��
//p0:  �Զ�����Ϣ���  p1~p5: ����
//���ܺ���ָ�Բ�� mrc_extSendAppEventEx �������Ϣ������ĺ���
//ע�⣺mrc_extRecvAppEventEx ������Ч
typedef int32 (*ExtFun)(int32 funid, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

//ע��extͨ�Ź��ܺ����������ģ�����ģ����� mrc_extSendAppEventEx
//������Ϣʱ�������� mrc_extRecvAppEventEx ���ܵ���Ϣ������ ����ں���
//ע�⣺mrc_extRecvAppEventEx ������Ч
#define Zhu_regExtFun(id, fun) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REGFUN, id, (int32)fun, 0,0,0)
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//ext���غ���
//--------------------------------------------------------------------------------
//EXT����ģʽ
typedef enum {
	ZHU_EXTLOAD_NULL,	//�ձ�ʾδ����
	ZHU_EXTLOAD_NORMAL,	//����������
	ZHU_EXTLOAD_DUMP,	//Dumpģʽ���أ�����Game Ext����Σ�
	ZHU_EXTLOAD_VM,		//���ص�VM�ڴ�
	ZHU_EXTLOAD_RESET	//ͨ��reset��ʽ����
}EXT_LOADMOD;

//���ز����ַ
typedef struct {
	void* addr;  /*Ԥ�����ڴ��ַ */
	int32 len;   /*���ṩ�����ڼ��ص��ڴ�ߴ�, ==0 ��ʾ��ָ������,��ʹ�ò��ʵ�ʴ�С�ڴ�*/  
}ZHU_MEMINFO, *PZHU_MEMINFO;

#ifdef SDK_MOD
typedef struct {
	mrc_init_t		mrc_init_f;
	mrc_event_t		mrc_event_f;
	mrc_pause_t		mrc_pause_f;
	mrc_resume_t	mrc_resume_f;
	mrc_exitApp_t	mrc_exitApp_f;
}mrcEnterFuncSt;

//���ر���MRP���ڵ�EXT
#define Zhu_loadExtSelf(id, extname, entryfun) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, 0, 0, (int32)entryfun)
	//ע������˳��EXTID EXT�� MRP·�� �������ڴ� SDK��ں���

//��ĳ��·���µ�MRP�ڼ���EXT
#define Zhu_loadExtPath(id, extname, mrppath, entryfun) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, (int32)mrppath, 0, (int32)entryfun)

#else

//���ر���MRP���ڵ�EXT
#define Zhu_loadExtSelf(id, extname) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, 0,0,0)
	//ע������˳��EXTID EXT�� MRP·�� �������ڴ�

//��ĳ��·���µ�MRP�ڼ���EXT
#define Zhu_loadExtPath(id, extname, mrppath) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, (int32)mrppath, 0,0)

#define Zhu_loadExtAddr(id, extname, mrppath, addr) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_LOADEXT, id, (int32)extname, (int32)mrppath, (int32)addr, 0)

#endif


//��ȡ�������ģʽ�����Բ�ѯ����Ƿ��Ѽ���
#define Zhu_getExtLoadMod(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_GET_EXTLOADMOD, id, 0,0,0,0)

//��EXT�ö�����ϵ�ext���Ƚ��հ����¼��������ص�ext�����
#define Zhu_bringExtToTop(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_BRINGEXTTOTOP, id, 0,0,0,0)

//ж��EXT����extidΪ���أ���ext��������ģ�����ã��򷵻�MR_FAILED��
//�ɹ�ж�أ�����MR_SUCCESS����ext��Ч
//�ɹ�ж��ʱ �����ext�� mrc_exitApp
#define Zhu_unloadExt(id, delay) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_UNLOADEXT, id, delay, 0,0,0)

#define Zhu_refScn(x, y, w, h) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REFSCN, x, y, w, h, 0)

#define Zhu_refScnRect(r) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REFSCN_RECT, (int32)r, 0,0,0,0)

#define Zhu_refScnFull() \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_REFSCN_FULL, 0,0,0,0,0)

//��ȡ��Ļ�ߴ�
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

//�������ͨ�����ôκ�������֪ͨ��ģ���˳��������
//mrc_exit��Ч
#define Zhu_exit() \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_EXIT, 0,0,0,0,0)

typedef enum {
	ZHU_EXTMOD_MAIN,	//��ģ��
	ZHU_EXTMOD_APP,		//��ͨ���
	ZHU_EXTMOD_PLUG		//Ӧ�ò��
}ZHU_EXTMOD;

//����extģʽ
#define Zhu_setExtMod(id, mod) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_SETMOD, id, mod, 0,0,0)

//����һ�� EXT���ǽ����¼�
//alwaysΪ1ʱ����ext�����ڲ��ڶ��㶼����������¼�
#define Zhu_setExtAlwaysHandelEvents(id, always) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_SETEXTALWAYSHANDELEVENTS, id, always, 0,0,0)

//EXT���� ����ͣ��Ϸ�Ķ�ʱ���� ����block�����¼�.
#define Zhu_blockExt(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_BLOCKEXT, id, 0,0,0,0)

#define Zhu_unBlockExt(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_UNBLOCKEXT, id, 0,0,0,0)

//��ȡEXT��� ����-1��ʾ��Ч mrc_extHandel ������
#define Zhu_getExtHandel(id) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_GETHANDEL, id, 0, 0,0,0)

//ͨ�������ȡEXTID ���� -1 ��ʾ��Ч
#define Zhu_getExtId(handel) \
	mrc_extSendAppEventEx(ZHU_EXTID_CFUNCTION, ZHU_FUNID_GETID, handel, 0, 0,0,0)


#ifdef __cplusplus
}
#endif

#endif