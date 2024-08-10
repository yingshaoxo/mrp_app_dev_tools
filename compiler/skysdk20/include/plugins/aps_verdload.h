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
/* װ������ģ�� */
#define aps_verdload_loadExt()  \
	aps_main_loadExt(APS_EXT_ID_VERDLOAD,VERDLOAD_MRP_NAME,VERDLOAD_MRP_FOLDER,VERDLOAD_EXT_NAME)

#else
/* ģ����װ������ģ�� */
int32 aps_verdload_loadExt();

#endif

/*����ж������ģ��*/	
#define	aps_verdload_unloadExt() \
	aps_main_unloadExt(APS_EXT_ID_VERDLOAD,0)

/*�ӳ�ж������ģ��*/
#define aps_verdload_delay_unloadExt() \
	aps_main_unloadExt(APS_EXT_ID_VERDLOAD,1)

/*��ͣ����ģ��*/	
#define aps_verdload_pauseExt() \
	aps_main_pauseExt(APS_EXT_ID_VERDLOAD)

/*��������ģ��*/	
#define aps_verdload_resumeExt() \
	aps_main_resumeExt(APS_EXT_ID_VERDLOAD)

/*�õ����ذ汾��*/
#define aps_verdload_getExtVersion() \
	aps_main_getExtVersion(APS_EXT_ID_VERDLOAD)


#define VERDLOAD_ID_DL_SRC                1
#define VERDLOAD_ID_DL_CANCEL             2
#define VERDLOAD_ID_DL_PRECONFIG_NETWORK  3


/* ����״̬ */
typedef enum
{
	MR_DL_STATUS_DIALING = 0,              /* ���ڲ���*/
    MR_DL_STATUS_CONNECTING,               /* �����������ط����� */
	MR_DL_STATUS_DOWNLOADING,              /* ������������ */
    MR_DL_STATUS_COMPLETE,                 /*�������     */

	MR_DL_STATUS_EXCEPTION                /*�����쳣     */
}MR_DL_STATUS;

typedef enum
{
	MR_DL_ERROR_REASON_NONE = 0,          /* ����    */ 
    MR_DL_ERROR_REASON_DIAL,              /*���Ŵ��� */
	MR_DL_ERROR_REASON_CONNECT,           /*�������ط���������*/
	MR_DL_ERROR_REASON_NOT_FOUND,         /*������δ�ҵ���������Դ*/
	MR_DL_ERROR_REASON_TIMEOUT,           /*���س�ʱ*/
	MR_DL_ERROR_REASON_FILE,              /*�ļ�����*/

	MR_DL_ERROR_REASON_CANCEL               /*�û���ֹ����*/                
}MR_DL_ERROR_REASON;


/*����Ԥ�����粦����Ϣ��Ӧ������Ѿ�����,����Ҫ���в���*/
typedef enum
{
	DL_PRECONFIG_NETWORK_NONE = 0,             /*��Ԥ����������*/
	DL_PRECONFIG_NETWORK_CMWAP,            /*Ԥ������CMWAP */
	DL_PRECONFIG_NETWORK_CMNET             /*Ԥ������CMNET */ 
}MR_DL_PRECONFIG_NETWORK;

/*���ع��̻ص�����
  
   status:    ��ǰ֪ͨ��״̬
   reason:    ����ԭ��,���� MR_DL_STATUS_EXCEPTIONʱ��Ч������״̬��ΪMR_DL_ERROR_REASON_NONE
   p1:        status == MR_DL_STATUS_DOWNLOADING ʱΪ�����ذٷֱ�ֵ,��20%,p1=20,
              ����status״̬��Ϊ���ò���
   (p2-p3):   status == MR_DL_STATUS_DOWNLOADING ʱp2 Ϊ��ǰ���������ݣ�p3ΪӦ����ȫ�����ݳ���
              ����status״̬��Ŀǰ��Ϊ���ò���
*/
typedef void (*MR_DL_CB)(MR_DL_STATUS status,MR_DL_ERROR_REASON reason,int32 p1,int32 p2,int32 p3);
/*
  ��ʼ������Դ�ļ�
  srcid:     ��ԴID  
  pathname:  ��Դ���·��
  cb:        MR_DL_CB���͵Ļص�����������֪ͨ���صĹ��̡� 
  ui:        VERDLOAD_FLAG_UIBYAPP:  ��Ӧ������ʾUI,����������ģ������ʾUI
             VERDLOAD_FLAG_HINT: ��������ʾ
  
*/
#define MR_DL_FLAG_UIBYAPP   0x0001    /* ��client���滭���ع���UI*/
#define MR_DL_FLAG_UIHINT      0x0002    /* ����verdload���滭���ع���ʱ���Ƿ���Ҫ������ʾ���� */

#define MR_DL_SRC_RES          0  /* ��ͨ��Դ�� */   
#define MR_DL_SRC_PLUGIN       1  /* ���       */
#define MR_DL_SRC_COOKIE_PROXY 2

typedef struct  
{
	int32 srcType; /*  MR_DL_SRC_RES ���� MR_DL_SRC_PLUGIN */
	int32 srcid;   /*  �����ص���ԴID */
	int32 srcver;  /*  ��Ҫ���µ��İ汾��*/
	int32 reqver;  /*  ��Ҫ���µ�����Ͱ汾��*/
}MR_DL_SrcInfo;

typedef struct 
{
	uint16 *title;  /*���ؽ������  unicode*/ 
	uint16 *hint;   /*���ش��ļ�����ϸ˵��  unicode*/
}MR_DL_UI_TIP;


/*Ԥ�����ò������
  precfg_network:  Ԥ�ȵĲ������,�����ΪDL_PRECONFIG_NETWORK_NONE����VREDLOADģ�齫�����в��Ŷ���

  ����ָʾ�����ع������費��Ҫ���²���
  ����:
	precfg_network:  Ԥ�ȵĲ��Ż���,�����ΪDL_PRECONFIG_NETWORK_NONE�������ع����н����������²��Ŷ���	
  ���:
	 MR_SUCCESS:  �ɹ�
	 MR_FAILED:   ʧ��
*/
#define aps_verdload_preconfig_network(precfg_network)\
	mrc_extSendAppEventEx(APS_EXT_ID_VERDLOAD,VERDLOAD_ID_DL_PRECONFIG_NETWORK,(int32)(precfg_network),0,0,0,0)


/*
  �����ļ�

  srcinfo [MR_DL_SrcInfo*]:   �ļ���Դ��Ϣ 
  pathname[char*]:            �ļ���ŵı��ؾ���·��
  cb[MR_DL_CB]:               ����֪ͨ���صĹ���
  ui[int32]:                  VERDLOAD_FLAG_UIBYAPP��VERDLOAD_FLAG_HINT
  ui_tip[MR_DL_UI_TIP*]:      �����verdload�������ؽ���,���ṩ������ı�˵��                
*/

#define aps_verdload_dl(srcinfo,pathname,cb,ui,ui_tip)\
    mrc_extSendAppEventEx(APS_EXT_ID_VERDLOAD, VERDLOAD_ID_DL_SRC,(int32)(srcinfo),(int32)(pathname),(int32)(cb),(int32)ui,(int32)ui_tip)


/*
   ȡ����ǰ��������
*/
#define aps_verdload_dl_cancel()\
	mrc_extSendAppEventEx(APS_EXT_ID_VERDLOAD,VERDLOAD_ID_DL_CANCEL,0,0,0,0,0)


#ifdef __cplusplus
}
#endif
#endif


