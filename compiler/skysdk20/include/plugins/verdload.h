#ifndef _VERDLOAD_H_
#define _VERDLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

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


#define MR_DL_FLAG_UIBYAPP   0x0001    /* ��client���滭���ع���UI*/
#define MR_DL_FLAG_UIHINT    0x0002    /* ����verdload���滭���ع���ʱ���Ƿ���Ҫ������ʾ���� */
#define MR_DL_FIAG_HINT_EX	 0x0006	   /* ʹ��ȷ�϶Ի��򱳾�͸��Ч��*/


/*����Ԥ�����粦����Ϣ��Ӧ������Ѿ�����,����Ҫ���в���*/
typedef enum
{
	DL_PRECONFIG_NETWORK_NONE = 0,             /*��Ԥ����������*/
	DL_PRECONFIG_NETWORK_CMWAP,                /*Ԥ������CMWAP */
	DL_PRECONFIG_NETWORK_CMNET                 /*Ԥ������CMNET */ 
}MR_DL_PRECONFIG_NETWORK;

/*���ع��̻ص�����
  
   status:    ��ǰ֪ͨ��״̬
   reason:    ����ԭ��,���� MR_DL_STATUS_EXCEPTIONʱ��Ч������״̬��ΪMR_DL_ERROR_REASON_NONE
   p1:        status == MR_DL_STATUS_DOWNLOADING ʱΪ�����ذٷֱ�ֵ,��20%,p1=20,
              ����status״̬��Ϊ���ò���
   (p2-p3):   status == MR_DL_STATUS_DOWNLOADING ʱp2 Ϊ��ǰ���������ݣ�p3ΪӦ����ȫ�����ݳ���
              ����status״̬��Ŀǰ��Ϊ���ò���
	!!!!!!!!!ע��dumpģʽ���ص����ز����֧��ʵʱ���ȵ�֪ͨ!!!!!!!!!!!!
*/
typedef void (*MR_DL_CB)(MR_DL_STATUS status,MR_DL_ERROR_REASON reason,int32 p1,int32 p2,int32 p3);

/*
    ��ʼ����������,��ǰ��֧��һ�����������,��������������δ���֮ǰ����
 ��һ����������

    ����: 
	  srcid:      �����ص���ԴID,��ID�ܹ�������Ψһ�ر�ʶһ����Դ
	  pathName:   ���ص��ļ��������Ŀ�ľ���·��(·��+�ļ���)
      cb:         ����֪ͨ�ص�

	  ui:          ui��ȡֵ�����ֿ��ܣ�
		           0                       ���ؽ��Ƚ��������ز������,����ǰ�����û�ȷ�ϡ�
				   VERDLOAD_FLAG_HINT      ���ؽ��Ƚ��������ز����������ǰ���û�ȷ�ϡ�
				   VERDLOAD_FLAG_UIBYAPP   ���ؽ�����APP����, ���ز���������κε�UI��
				   
	  title:       ���ؽ������,ui & VERDLOAD_FLAG_UIBYAPP == 0 ʱ��Ҫ�ṩ
	  hint:        ����ȷ�Ͻ������ʾ,ui =VERDLOAD_FLAG_HINT ʱ��Ҫ�ṩ
	���:
	  MR_SUCCESS:  �ɹ�
	  MR_FAILED:   ʧ��


	***NOTE********************************************************************
	 ��� ����ֵΪMR_SUCCESS ���� ui Ϊ 0 ���� VERDLOAD_FLAG_HINT, 
               ��ôӦ�ñ���ֹͣ����Ľ���ˢ�£� ���������ز����UI��ͻ��
    ****************************************************************************
*/

int32 mrc_dl_file_start(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  ������ mrc_dl_file_startһ��,�������ʹ��dump��ʽ����
*/
int32 mrc_dl_file_startDump(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  ������ mrc_dl_file_startһ��,�������������Ԥ�����������,����ָʾ�����ع������費��Ҫ���²���
  ����:
	precfg_network:  Ԥ�ȵĲ��Ż���,�����ΪDL_PRECONFIG_NETWORK_NONE�������ع����н����������²��Ŷ���	
  ���:
	 MR_SUCCESS:  �ɹ�
	 MR_FAILED:   ʧ��
*/
int32 mrc_dl_file_startEx(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint,MR_DL_PRECONFIG_NETWORK precfg_network);

/*
  ������ mrc_dl_file_startExһ��,�������ʹ��dump��ʽ����
*/
int32 mrc_dl_file_startExDump(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint,MR_DL_PRECONFIG_NETWORK precfg_network);

/*
    ������ mrc_dl_file_startһ�£�����������ص��ļ�ͨ��������������
	���룺
	srcVer:		��������Դ��Ŀ��汾��
	reqVer:		��������Դ����Ͱ汾��
	����������mrc_dl_file_startһ��
*/
int32 mrc_dl_fileFromCookie_start(int32 appid,int32 srcVer, int32 reqVer, const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  ������ mrc_dl_fileFromCookie_startһ��,�������ʹ��dump��ʽ����
*/
int32 mrc_dl_fileFromCookie_startDump(int32 appid,int32 srcVer, int32 reqVer, const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
������ mrc_dl_file_startһ�£�����������ص��ļ�ͨ��������������
���룺
fileName:	�����ļ���
cpCode:		CPΨһ��������
����������mrc_dl_file_startһ��
*/
int32 mrc_dl_fileByFileName_start(char* fileName, int16 cpCode, const char* pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  ������ mrc_dl_fileByFileName_startһ��,�������ʹ��dump��ʽ����
*/
int32 mrc_dl_fileByFileName_startDump(char* fileName, int16 cpCode, const char* pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
    ȡ����ǰ��������
	���:
	  MR_SUCCESS: �ɹ�
	  MR_FAILED: ʧ��
*/
int32 mrc_dl_file_cancel(void);

/*
	���õ����ߵ�extId�������ֵ�����
	���룺���ò����extID
	�������
*/
void mrc_dl_set_ExtID(int32 extID);

#ifdef __cplusplus
}
#endif
#endif