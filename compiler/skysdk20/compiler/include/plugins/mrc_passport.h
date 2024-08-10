#ifndef _MRC_PASSPORT_H_
#define _MRC_PASSPORT_H_

//#include "aps_verdload.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PASSPORT_LAST_VER       17  //��ǰ�汾

#define PASSPORT_MRP_NAME	 "passpt.mrp"	
#define PASSPORT_MRP_FOLDER	"plugins"

//�����û���Ϣ����
#define MAX_SAVE_USR_COUNT		3 		

#define MAX_PWD_LEN             12*2
#define MAX_USERNAME_LEN        63
#define MAX_NICKNAME_LEN		12  //12���ַ�
//��һ�ֽ���������
#define MAX_PWD_CONTENT_LEN     1+32 

//#define ENCRYPT_IMSI_LEN        24
#define MAX_IMSI_LEN 16

typedef enum
{
	// ͳһͨ��֤������������
	E_PASSPORT_SUCCESS  = 200,	//�ɹ�
    E_PASSPORT_FORCE_UPDATE = 505, //ǿ�Ƹ���
	E_PASSPORT_OPTIONAL_UPDATE = 506 //��ѡ����(�Ѿ�������ɹ�)
	//����ʧ��
}PASSPORT_SERVER_RESULT_CODE;

//������ʷ��¼�е��˻���Ϣ
typedef struct
{
    uint8  imsi1[MAX_IMSI_LEN];
	uint8  imsi2[MAX_IMSI_LEN];
	uint8  isAvailable;			// �Ƿ���ã�0x01���ã����ֶ��ڻ�ȡ�˺���ʷ��¼�������ж��˺��Ƿ���ã������˺�ʱ���á�
    uint8  pwdLen;               // �û�ʵ�ʵ����볤�ȣ�ע�⣺�ڸ����˺���ʷ�ļ�ʱpwdLenΪ0���򲻸���pwdLen����������pwdLen��
    uint8  isSavePwd;			// �Ƿ��ס����
	uint8  isAutoLogin;			// �Զ���¼
    uint16 userName[MAX_USERNAME_LEN+1];		// �˻��� utf-16-be
	uint8  pwdContentLen;		// pwdContentLen ����
    uint8  pwdContent[MAX_PWD_CONTENT_LEN];		// �ɷ�����·���������ܴ���
    uint8  reserved[16];			// ����
}AccountInfoSt;

//����DSM�еģ���¼�ɹ����û�����Ϣ
typedef struct
{
	uint32 skyid;
	uint8  token[8];	
	uint16 loginDate[6]; //Year(2),month(2),day(2),hour(2),minute(2),second(2)
	uint16 userName[MAX_USERNAME_LEN+1];
	uint16 nickName[MAX_NICKNAME_LEN+1];
	uint8  gender;
	uint8  age;
	//uint8  star;		
	uint8  portrait[16+1];
    uint8  flags;	//1 bit ��ʶ�Ƿ������˰�ȫ����
	uint8  mobile[11+1];
}LOGININFO;

#define DSM_USERINFO LOGININFO

//��¼��Ҫ��account��Ϣ
typedef struct
{  
	uint8  isSavePwd;//�Ƿ񱣴�����
	uint8  isAutoLogin;//�Ƿ��Զ���¼
	uint16 userName[MAX_USERNAME_LEN+1];//�û�����\x00\x00����						
	int8   pwdContentLen; //��pwdContentLenΪ0ʱ����ʾ���ģ�����Ϊ����
	uint8  pwdContent[MAX_PWD_CONTENT_LEN];  //�������Ļ�����
}ACCOUNTINFO;	


typedef enum //accesspoint���������ݷ���
{
	ACCESSPOINT_ENTER_PASSPORT,
	ACCESSPOINT_ENTER_REG,	           
	ACCESSPOINT_ENTER_NORMAL_REG,		
	ACCESSPOINT_ENTER_MOBILE_REG	
}ACCESSPOINT;


/*
mrc_passport�ص�����: ���ú����󣬲��ۺ������óɹ�ʧ�ܣ��ص��������ᱻ���á�																
	result:	200 �ɹ�������ʧ��							    
	resultMsg: resultMsg���о������ʾ��Ϣ(��ȡ��ȫ����ɹ�ʱ�����ֶηŰ�ȫ����)��
	extraInfo: �Ը�����ʾ������result�ж���ǿ�Ƹ��»��ǿ�ѡ����(E_PASSPORT_FORCE_UPDATEΪǿ�ƣ�����Ϊ��ѡ)��
			   NULL  ��Ч
			   ����  ��ʾ��
				
*/
typedef void (*MR_Passport_CB)(int32 result, uint16 * resultMsg, void * extraInfo);


/*
mrc_passport window�ص�����:																	
	errCode: 0 �ɹ�����0 ʧ��							    				
*/
typedef void (*MR_Passport_WIN_CB)(int32 errCode);


/*
���汾																
����ֵ:
	MR_FAILED ʧ�ܣ���Ҫ����
	MR_SUCCESS �ɹ����Ѿ������µ�
*/
int32 mrc_passport_checkVersion(void);

/*���ع��̻ص�����(����ص�ͬMR_DL_CB������ϸ�Ķ�����ο�verdload.h)

  status:    ��ǰ֪ͨ��״̬
  reason:    ����ԭ��,���� MR_DL_STATUS_EXCEPTIONʱ��Ч������״̬��ΪMR_DL_ERROR_REASON_NONE
  p1:        status == MR_DL_STATUS_DOWNLOADING ʱΪ�����ذٷֱ�ֵ,��20%,p1=20,
  ����status״̬��Ϊ���ò���
  (p2-p3):   status == MR_DL_STATUS_DOWNLOADING ʱp2 Ϊ��ǰ���������ݣ�p3ΪӦ����ȫ�����ݳ���
  ����status״̬��Ŀǰ��Ϊ���ò���
*/
typedef void (*MR_Passport_DL_CB)(int32 status,int32 reason,int32 p1,int32 p2,int32 p3);

/*
����verdloadģ��������أ�Ӧ����Ҫ�������ػص����������ؽ��档																
����ֵ:
MR_FAILED ��������ģ��ʧ��
MR_SUCCESS ��������ģ��ɹ�
*/
int32 mrc_passport_verDownload(MR_Passport_DL_CB cb);

/*
�û�����ע��																
���룺																					
	uint16 * userName:�û���						
	uint16 * pwd:��������
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ:
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_reg(uint16 * userName, uint16 * pwd, MR_Passport_CB cb);

/*
����ע��																
����:																					
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_quickReg(MR_Passport_CB cb);

/*
�û���¼
����:																					
	ACCOUNTINFO * accountInfo:�˻���Ϣ  
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_passport_login(ACCOUNTINFO * accountInfo, MR_Passport_CB cb);
/*
�û��ǳ�																
����:																					
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_logout(MR_Passport_CB cb);

/*
���ð�ȫ����																
����:						
	uint16 * pwd:��������
	uint16 * secretq:��ȫ����
	uint16 * secreta:��ȫ�����
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_passport_setSecretq(uint16 * pwd,
                              uint16 * secretq,
                              uint16 * secreta, 
                              MR_Passport_CB cb);
                              
/*
��ȡ��ȫ����																
����:																					
	uint16 * userName:�û���								
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
  
CB����:
	��resultΪ200ʱ����ȡ��ȫ����ɹ�����ȫ��������resultMsg��
*/                              
int32 mrc_passport_getSecretq(uint16 * userName, MR_Passport_CB cb);

/*
��������																
����:																					
	uint16 * userName:�û���
	uint16 * pwd:���ú����������						
	uint16 * secretq:��ȫ����
	uint16 * secreta:��ȫ�����	
	MR_Passport_CB cb: �ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_passport_resetPwd(uint16 * userName, 
                            uint16 * pwd,
                            uint16 * secretq, 
                            uint16 * secreta, 
                            MR_Passport_CB cb);                            

/*
�����û�����
����:																					
	uint16 * pwd:����������
	uint16 * newPwd:����������
	MR_Passport_CB cb: �ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/                            
int32 mrc_passport_setPwd(uint16 * pwd, uint16 * newPwd, MR_Passport_CB cb);

/*
�����û���Ϣ��ֻ�ṩ���û�����Ϣ��������Ϣ�����������ͨ��WAP����
����:																					
	uint16 * nickName:�û��ǳ�
	uint8 gender:�Ա�
	MR_Passport_CB cb: �ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_passport_setUserInfo(uint16 * niceName, uint8 gender, MR_Passport_CB cb);    

/*
�û���֤
����:																					
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_checkLogin(MR_Passport_CB cb);


/*
��ȡDSM�б���ĵ�¼״̬��Ϣ
����:					
	LOGININFO* userinfo							
���:
	*userinfo
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_passport_getLoginInfo(LOGININFO * loginInfo);

/*
���DSM�б���ĵ�¼״̬��Ϣ�������еǳ�����
����:
	��					
���:
	��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_passport_clearLoginInfo(void);

/*
ȡ��֮ǰ������
����:
	��					
���:
	��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_passport_cancelReq(void);

/*
���ò�������
����:
uint8 dialType: ��������(1 CMWAP, 2 CMNET)
���:
��
����ֵ��
MR_FAILED ʧ��
MR_SUCCESS �ɹ�
*/
int32 mrc_passport_setDialType(uint8 dialType);

/*
����Ӧ�ò���״̬
����:
uint8 status: ����״̬(0 δ����, 1 �Ѳ���)
���:
��
����ֵ��
MR_FAILED ʧ��
MR_SUCCESS �ɹ�
*/
int32 mrc_passport_setAppDialStatus(uint8 status);

/*
(����ǴӲ������ͨ��֤�ģ��Ҳ��ϣ���Դ�����������ͨ��֤�����ݽ���ͳ�ƣ����Ե�������ӿ�;����Ǵ�Ӧ��(�ǲ����ʽ)���õģ�����Ҫ���������������)
Ӧ�÷���ͳ��
����:
uint32 accessPoint:Ӧ����Դ(Ϊ��ͳ�Ʒ��㣬������д�����APPID)
���:
��
����ֵ��
MR_FAILED ʧ��
MR_SUCCESS �ɹ�
*/
int32 mrc_passport_access(uint32 accessPoint);

/*
����UI����
����:
int32 regType: ע������(0 Ĭ�ϡ�1 �ֻ�ע�ᡢ2 ��ͨע�ᡢ3 ����ע��)
int32 notShowSuccWin: �Ƿ���ʾ��¼�ɹ�����(0 ��ʾ��1 ����ʾ)
���:
��
����ֵ��
*/
void mrc_passport_setUIArgs(int32 regType, int32 notShowSuccWin, int32 p3, int32 p4);


/*
�ӱ�����ʷ�ļ��л�ȡ��ʷ��¼��Ϣ																
����:																							
	AccountInfoSt accountInfo[MAX_SAVE_USR_COUNT]��ָ�򱣴��¼�Ľṹ
�����
    accountInfo[MAX_SAVE_USR_COUNT]
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
int32 mrc_passport_getLoginHistory(AccountInfoSt accountInfo[MAX_SAVE_USR_COUNT]);


/*
�ֻ�����ע��																
���룺																					
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ:
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_mobile(MR_Passport_CB cb);

/*
�ֻ���������																
���룺																					
	uint16 * userName:�û���
	uint16 * pwd:������
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ:
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_mobResetPwd(uint16 * userName, uint16 * pwd, MR_Passport_CB cb);


/*
���ֻ�																
���룺																					
	uint16 * pwd:����
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ:
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_bindMob(uint16 * pwd, MR_Passport_CB cb);

/*
�ֻ����																
���룺																					
	uint16 * pwd:����
	MR_Passport_CB cb:�ص���������ɺ󱻵���
����ֵ:
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/ 
int32 mrc_passport_unbindMob(uint16 * pwd, MR_Passport_CB cb);

enum {
	PASSPORT_ID_ENTRY_WIN_DUMP = 1,
    PASSPORT_ID_ENTRY_WIN, 
	PASSPORT_ID_LOGIN_WIN_DUMP,
	PASSPORT_ID_LOGIN_WIN,
	PASSPORT_ID_REG_WIN_DUMP,
	PASSPORT_ID_REG_WIN,
	PASSPORT_ID_MODIFYUSERINFO_WIN_DUMP,
	PASSPORT_ID_MODIFYUSERINFO_WIN
};
int32 mrc_passport_funcEntry(int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

/*
ͨ��֤��ڣ�DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_entryWin_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_ENTRY_WIN_DUMP,(int32)cb,0,0,0,0)

/*
ͨ��֤��ڣ���DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_entryWin(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_ENTRY_WIN,(int32)cb,0,0,0,0)

/*
ǰ̨��¼(��UI)��DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_loginWIN_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_LOGIN_WIN_DUMP,(int32)cb,0,0,0,0)

/*
ǰ̨��¼(��UI)����DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_loginWIN(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_LOGIN_WIN,(int32)cb,0,0,0,0)

/*
ǰ̨ע��(��UI)��DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_regWIN_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_REG_WIN_DUMP,(int32)cb,0,0,0,0)

/*
ǰ̨ע��(��UI)��DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_regWIN(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_REG_WIN,(int32)cb,0,0,0,0)

/*
ǰ̨ע��(��UI)��DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_modifyUserInfoWIN_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_MODIFYUSERINFO_WIN_DUMP,(int32)cb,0,0,0,0)

/*
ǰ̨�޸��û���Ϣ(��UI)����DUMPģʽ
����:																							
	MR_Passport_WIN_CB cb:�ص��������Ӵ��ڷ��غ���ô˻ص�
�����
    ��
����ֵ��
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�    
*/
#define mrc_passport_modifyUserInfoWIN(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_MODIFYUSERINFO_WIN,(int32)cb,0,0,0,0)

#ifdef __cplusplus
}
#endif

#endif