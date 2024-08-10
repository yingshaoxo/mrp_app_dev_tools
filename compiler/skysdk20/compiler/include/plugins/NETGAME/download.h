#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ����download��ͼģ�� */
int32 mrc_download_load(void);

/* ж��download��ͼģ�� */
int32 mrc_download_unload(void);

int32 mrc_dl_init_network(void);
//download status or result, report to user
typedef enum
{
	/*for update phase*/
	DLOAD_GETVERSION_FAILED = 0,//��ȡ�ļ��汾��Ϣʧ�ܣ�busy, or send appid failed, or recv timeout, or recv proc failed.
	DLOAD_GETVERSION_SUCCESS,//��ȡ�ļ��汾��Ϣ�ɹ���
	DLOAD_GETVERSION_NONE,//��ǰ�������°汾��
	/*for fileload phase*/
	DLOAD_ALOAD_START,//ĳһ�ļ���ʼ����, +fileID
	DLOAD_DATA_COMING,//ĳһ�ļ������ݵ���,+fileID
	DLOAD_ALOAD_END,//ĳһ�ļ��������,+fileID
	DLOAD_ALLLOAD_END,//ȫ���ļ��������
	DLOAD_LOAD_FAILED,//ĳһ�ļ�����ʧ��,+fileID +suberror
	
	DLOAD_RET_MAX
}DloadStatusEn;

//sub error code for DLOAD_LOAD_FAILED, reposrt to user
typedef enum
{
	DLOAD_NO_ERROR = 0,
	DLOAD_FAILED_FILE_ERROR = 1,//�ļ�����У��ʧ��
	DLOAD_FAILED_FFS_ERROR = 2,//�ļ�����ʧ��
	DLOAD_FAILED_NET_ERROR = 3,//���糬ʱ�ȴ���
	DLOAD_FAILED_SERVER_ERROR = 4,//�������

    DLOAD_FAILED_CHK2REP_REMMRP_ERROR = 5,
    DLOAD_FAILED_CHK2REP_RENAME_ERROR = 6,
    DLOAD_FAILED_CHK2REP_REMCFG_ERROR = 7,
    DLOAD_FAILED_CHK2REP_WRONG_STATE = 8,

	DLOAD_FAILED_FILE_NOT_FOUND = 9,//�ļ�δ�ҵ�
	
	DLOAD_FAILED_MAX
}DloadSubErrEn;

//result code for: c <-> s
enum
{
	DL_RESULT_PARAM_INVALID = -3, //��������
	DL_RESULT_TIMEOUT = -2, //������ʱ
	DL_RESULT_FAILED = -1, //����ʧ��
	DL_RESULT_SUCCESS = 0,	//�ɹ�
	DL_RESULT_NO_NEWVERS = 1 //���������°汾���޶�Ӧ�ļ���(���κδ����ص�����plugs ?)
};

#define DLOAD_FILES_MAX		64//���ض����п��Դ��ڴ������ļ���������
#define DLOAD_FULLPATH_MAX_LEN 64//·��+�ļ��������������

typedef int32  DloadResult;
//typedef int32 (*DloadResult)(DloadStatusEn status, uint32 fileId, uint32 param, int32 p3, int32 p4, int32 p5);

typedef struct
{
	uint32 fileId; 			//����Ϊÿ�������ļ�����Ψһ��ID�š�
	uint32 fileLen; 			//�ļ��ܳ��� 
	uint32 curLen; 			//��ǰ�����س��� 
	uint8 filenameLen;		//�ļ�������
	char *filename;		//gb,
}DloadFileAttSt;

typedef struct
{
	uint32 num;							//�ļ���Ŀ
	uint32 fileIds[DLOAD_FILES_MAX]; 		//����Ϊÿ�������ļ�����Ψһ��ID�š�
}DloadFileids;

typedef void (*DL_CB)(int32 result, int32 file, int32 param, int32 FileNum, int32 p4, int32 p5);


/*
ע��״̬������
����:	
		int32 extHandle:Ӧ��ģ���ext���
		DloadResult resultCb: ״̬����ص�
���:
����:
		MR_FAILED				����ʧ��
		MR_SUCCESS		 		ע��ɹ�
*/
int32 mrc_DloadRegResultCb(int32 extId, DL_CB dl_callback);

/*
ע��״̬������,�����������κβ���
����:	
		handle		mrc_DloadUnRegResultCb()�õ��ľ��
���:
����:
		MR_SUCCESS		�����ɹ�
*/
int32 mrc_DloadUnRegResultCb(int32 handle);



/*
����ʼ�ļ��������ص�����
����:	
		int32 flag: �Ƿ��Ǻ�̨����,1 �ǣ�0��
���:
����:
		MR_SUCCESS		�����ɹ�
		����				����ʧ��
*/
int32 mrc_DloadStartDownloadReq(int32 flag);


/*
��ȡĳһ�ļ�������״̬
����:	
		uint32 fileId: ����Ϊÿ�������ļ�����Ψһ��ID�š�
���:
����:
		NULL		����ʧ��
		����		�����ɹ�		
*/
const DloadFileAttSt* mrc_DloadGetStatus(uint32 fileId);

/*
��ȡ��ǰ���ض��������е�fileid
����:	
		DloadFileids* fileids:  
���:
		DloadFileids* fileids: ��ǰ���д����ص�fileid
����:
		MR_SUCCESS		�����ɹ�
		����				����ʧ��
*/
int32 mrc_DloadGetFileids(DloadFileids* fileids);

/*
����Ƿ�����ʱ���ص��ļ���Ҫ�滻
����:	
���:
����:
		MR_SUCCESS		�����ɹ�
		����				����ʧ��
*/
int32 mrc_DloadTdCheck2Replace(void);


/*
�����ļ�����Ŀ¼
����:	
���:
����:
MR_SUCCESS		�����ɹ�
����				����ʧ��
ע�⣺����·��������"gwy/"��ʾ�������б��
*/
int32 mrc_DloadSetSaveFilePath(char * path);

/*
���������ļ�����
����:	
���:
����:
*/
void mrc_DloadSetDownloadFileType(int isMrpFile);

/*
ȡ���ļ�����
����:	
���:
����:
*/
void mrc_DloadCancel(void);

/*
�����ļ��б�
����:	
���:
����:
MR_SUCCESS		�����ɹ�
����				����ʧ��
*/
int32 mrc_DloadFileListInd(char *data, int32 len);


/*
��ȡ�ļ���Ϣ
����:	uint16	modInstId ����ģ���module Instance id 
		char ** fileNames �ļ����б�
	    int		count	  �ļ���
example:
char * fileNames[] = {"dwol.gif", "xxx.gif", "yyy.gif"};
		
DloadSetSaveFilePath("test/"); // ����������Ŀ¼ 
//ע�����ػص���downloadCBShell��EXT_ID_ROOMLISTģ��ĺ���
DloadRegResultCb(EXT_ID_ROOMLIST, downloadCBShell); 
DloadGetFileInfoReq(0xBB01, fileNames, 3);  // ������3���ļ�


���:
����:
MR_SUCCESS			�����ɹ�
����				����ʧ��
*/
int32 mrc_DloadGetFileInfoReq(uint16 modInstId, char * fileNames, int count);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif