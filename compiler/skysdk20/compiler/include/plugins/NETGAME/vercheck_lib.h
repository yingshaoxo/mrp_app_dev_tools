#include "mrc_base.h"
#include "verdload.h"

#ifndef _VERCHECK_LIB_H_
#define _VERCHECK_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif


#define VER_CHECK_MAX_MRP_NAME     64

enum
{
	PLATFORM_INVALID	= 0,
	PLATFORM_MTK		= 21,		/* ������ʷԭ��, Ϊ�����֮ǰ��phoneType��ͻ, ��21��ʼ����ƽ̨ */
	PLATFORM_SPR		= 22,
	PLATFORM_SYMBIAN	= 23,
	PLATFORM_VIA		= 24,
	PLATFORM_MSTAR		= 25,
	/* append platform define above */
	PLATFORM_MAX
};

/*******************************************************************************
* product screen size define
*******************************************************************************/
enum
{
	SCREEN_INVALID		= 0,
	SCREEN_240X320		= 1,
	SCREEN_240X400		= 2,
	SCREEN_176X220		= 3,
	/* append screen size define above */
	SCREEN_MAX
};

/*******************************************************************************
* plugins type define
*******************************************************************************/
enum
{
	PLUGIN_INVALID 		= 0,
		PLUGIN_BASEMODULE 	= 1,	//�������
		PLUGIN_APPMODULE 	= 2,	//Ӧ�ò��
		PLUGIN_BRWMODULE 	= 3,	//��������
		PLUGIN_BRWAPP	 	= 4,	//�����Ӧ�ò��
		PLUGIN_MAX
};

typedef struct
{
	uint16	nMsgCode;		//��ϢID
	uint16	PlatType;		//ƽ̨����
	uint16  nScrType;		//��Ļ����
	uint32 	appID;			//appID
}REQ_PLUGIN_VER_PACKET;

typedef struct
{
	uint32 nPluginId;			//�����ID
	uint32 nVersion;			//����汾��
}PLUGIN_VER_REC;

typedef struct  
{
	uint16 nPluginNum;	//���������Ϣ
	PLUGIN_VER_REC *nPluginVer;
}PLUGIN_VER_DEPEND_INFO;

typedef struct __tagPluginRspInfo
{
	uint32 	appID;		//���ID
	uint32 	appVersion;	//����汾
	uint16 	appType;	//�������
	uint16 	rebootFlag;	//������ʶ
	uint16 	updateFlag;	//���±�ʶ
	uint16  appNameLen;	//app���Ƴ���
	uint8  	mrpName[VER_CHECK_MAX_MRP_NAME];	//MRP�ļ�����
	
}PLUGIN_VERSION_RSPINFO;


typedef struct __tagUpdatePluginInfo
{
	uint16	nMsgCode;		//��ϢID
	uint16 	nPluginCount;	//�������
	PLUGIN_VERSION_RSPINFO *VerRspInfo;	//�����ϸ��Ϣ
	
}UPDATE_PLUGIN_INFO;

typedef void (*update_plugin_cb)(UPDATE_PLUGIN_INFO *pUpPlugInfo);

/*
* ���Ͱ汾��Ϣ����,���ڴӷ�������ȡ��ǰMRP������ģ����Ϣ
* �������:
* 	info: ��ǰMRP��Ϣ,Ӧ��ID��ƽ̨���ͣ���Ļ����;��ϢID����Ϊ��;
*  pDependInfo: �����Ĳ����Ϣ��������������ID������汾��;
*  cb:   �������Ļص�����.
*  �ڻص����������û����Ա�����Ҫ�İ汾������Ϣ;����������ģ��ӿ�������Ӧ�Ĳ��.
*  ����ص������ĺ�������pUpPlugInfoΪNULL�����ʾû���յ��汾��Ϣ��Ӧ�����������.
* typedef void (*update_plugin_cb)(UPDATE_PLUGIN_INFO *pUpPlugInfo);
* ����ɰ汾�������ش������Ҫ����mrc_vercheck_verInfo_Release���ͷ������Դ.
 */
int32 mrc_vercheck_verInfo_sendReq(REQ_PLUGIN_VER_PACKET *info,PLUGIN_VER_DEPEND_INFO *pDependInfo,update_plugin_cb cb);

/*
* �ͷŵ��� mrc_vercheck_verInfo_sendReq ռ�õ���Դ;
* ȷ����һЩ�����쳣ʱ���ڴ���Դй©;
* �����ȡ���²����,�ڿ�ʼ���ظ��²��֮ǰ��������øýӿ��ͷ��ڴ��������Դ
 */
int32 mrc_vercheck_verInfo_Release(void);

/* ���ظ��²��,��Ӧ�ĸ�����Ϣͨ�����ýӿ�mrc_vercheck_verInfo_sendReq�����õĻص����������;
* ��Ҫע����ǣ��ڻ�ð汾������Ϣ�󣬱������mrc_vercheck_verInfo_Release�ͷ���Ӧ��������Դ;
* �������:
* 	pVerRspInfo: ��Ҫ���µĲ����Ϣ
*  cb:			 ����״̬��ʾ�Ļص�����
* �ڻص��������Ҫ���쳣����������״̬���б�Ҫ�Ĵ���.
 */

int32 mrc_vercheck_verInfo_UpdatePlugin(PLUGIN_VERSION_RSPINFO *pVerRspInfo,MR_DL_CB cb);
/*
* ���±��ز���ӿ�
* �ӵ�ǰMRP�ļ��ж�ȡplugins.lst�ļ�,��plugins.lst�ļ��л�ȡ��Ӧ��MRP�汾��
* ��鱾��Ŀ¼�Ƿ���ڶ�Ӧ��MRP�ļ�����������ڵĻ�����ӵ�ǰMRP�ļ��н�ѹ������Ŀ¼
* �������Ŀ¼���ڶ�Ӧ��MRP�ļ����Ұ汾�űȵ�ǰMRP�еİ汾��ҪС������ǰMRP�д����MRP�汾���£�
* ��ӵ�ǰMRP�ļ��н�ѹ�����mrp�ļ����ǵ�����Ŀ¼�е��ļ�.
 */
int32 mrc_vercheck_verInfo_UpdateMRP(void);
/*
 * ɨ����չ�ڴ�
 */
int32 mrc_vercheck_MemoryScan(void);

/* unicode�ַ������� */
//int32 mrc_vercheck_uniStrcat(char *dest, char *src);

/* unicode �ַ������� */
//int32 mrc_vercheck_uniStrcpy(char * dest, char * src);

/* ��ʽ������ַ��� */
//int32 mrc_vercheck_unisprintf(char * dst,  const char * format, ...);

/* ���ݽ������ */
//int32 mrc_vercheck_unpack(char *in, int inLen, const char* fmt, ...);

//int32 mrc_vercheck_pack(char *out, const char* fmt, ...);
/*
����ȥ�������еĿո�ͻس��ķǷ�����
*/
//char * mrc_vercheck_uniStrtrim(char* string);

/* �����ַ����Ļس������ַ� */
//char * mrc_vercheck_uniStrtrimLF(char* string);

int32 mrc_vercheck_unloadVerd(void);
int32 mrc_vercheck_loadVerd(void);
int32 mrc_vercheck_preconfig_net(int32 pre_cfgnet);


#ifdef __cplusplus
}
#endif

#endif
