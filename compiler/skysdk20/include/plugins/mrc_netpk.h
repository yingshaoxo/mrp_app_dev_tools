#ifndef _MRC_NETPKSAFE_H_
#define _MRC_NETPKSAFE_H_
#include "mrc_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PREFIXDATA_COUNT   20

#define MAX_NICKNAME_LEN  12
#define MAX_TITLE_LEN     12
#define MAX_IDENTIFY_LEN 32
/*
  ��Ϸ��ɫ	
*/
typedef struct 
{	
	uint32 ctrl;                            /*0: �ն˿���PK�������  1: ����˿���PK�������*/
	uint32 type;							/*����Ϊ��ȡ���Ľ�ɫ��Ϣ��0����ͨ��ң�1���ٷ�BOSS*/
	int32  win;                             /*ʤ������*/ 
	int32  lost;                            /*�ܳ�����*/
	int32  StrengthValue;	                /*ʵ��ָ��*/
	int32 sex;								/*1��ʾ�У�2��ʾŮ,0��ʾ�Ա���*/
	int32 bitmap;							/*�����Ƿ���ĳ��bitmap���ԣ�1��ʾ�У�0��ʾû��*/
	int32 day_win;							/*������Ӯ����*/
	int32 week_win;							/*������Ӯ����*/
	/*************** Ԥ����Ԫ�����Ը�ʽ ****************/
	/* MAX_PREFIXDATA_COUNT ������ֵ                   
	VALUE1,VALUE2,VALUE3,...                         
	����Ϸ�Լ��������ĵ�ԪΪ���ֺ���,�綨��:               
	  prefixData[0] �������                         
	  prefixData[1] ��ŵȼ�                            
	  ������Ԫ��ȱʡֵΪ0                            
	  ��Ԫ��������ڴӷ���˲�ѯ�ض�����Ϸ��ɫ       */
	/***************************************************/

	int32 prefixData[MAX_PREFIXDATA_COUNT];
    
	/************** �Զ��嵥Ԫ��ĸ�ʽ *****************/ 
	/* Ԥ����Ԫ��ֻ�ܴ��int32����ֵ���޷�������ַ��� 
	 �����ֽ������������͵����ݣ�customData���ڴ����
	 ��Ϸ�Լ���֯�����ݸ�ʽ,������Զ�����������������
	 �����ڲ�ѯ��                                    */ 
	/***************************************************/
	uint8 *customData;      /*�Զ�������*/            
	int32 dataLen;			/*�Զ������ݳ���*/ 
	uint32 skyid;			/*skyid*/
	uint16 nickName[MAX_NICKNAME_LEN+1];    /*�ǳ�*/ 
	uint16 title[MAX_TITLE_LEN+1];  		/*ͷ��*/
	char  identify[MAX_IDENTIFY_LEN];      /*����������*/

}MR_Character;

/*��ɫ��Ϣ�б�*/
typedef struct tagCharacterList
{
	MR_Character *pCharacter;
	struct tagCharacterList *pNext;
}MR_CharacterList;


/*Ԥ���������*/
typedef enum
{
   MR_PKFilter_STRENGTH = 0,	     /*�ۺ�ʵ����ǿ*/
   MR_PKFilter_SIMILAR,	             /*ʵ�����*/
   MR_PKFilter_FIGHTWITHME,	         /*�����ս���Լ�*/
   MR_PKFilter_FEMALE,	             /*Ů�����*/
   MR_PKFilter_DAY_CHAMPION,	     /*�չھ�*/
   MR_PKFilter_WEEK_CHAMPION,	     /*�ܹھ�*/
   MR_PKFilter_CUSTOM,   	         /*�Զ���*/ 
   MR_PKFilter_BUDDY				/*��ȡ���ѵĽ�ɫ�б�*/
}MR_PKFilter;

/*������*/
typedef enum
{
	MR_PKErrorCode_SUCCESS = 0,			/* �ɹ�           */
	MR_PKErrorCode_UserCancel,			/* �û�ȡ��       */
	MR_PKErrorCode_DIAL,				/* ���Ŵ���       */
	MR_PKErrorCode_Network,				/* �������       */
	MR_PKErrorCode_InvalidateUser,		/* ��Ч�û�       */
	MR_PKErrorCode_BlackListUser,		/* �������û�     */ 
	MR_PKErrorCode_FormatError,			/*��ʽ����			*/
	MR_PKErrorCode_NotEnoughMem,		 /*�ڴ治��			*/
	MR_PKErrorCode_OtherError,
	MR_PKErrorCode_ForceUpdate,			/*ǿ�Ƹ���*/
	MR_PKErrorCode_RecommendedUpdate	/*�������*/	
}MR_PKErrorCode;



typedef enum
{
	MR_PKPageFlag_noPage,	//��ȡ�����б�û����һҳ��Ҳû����һҳ
	MR_PKPageFlag_next,		//��ȡ�����б�ֻ����һҳ
	MR_PKPageFlag_previous,	//��ȡ�����б�ֻ����һҳ
	MR_PKPageFlag_both		//��ȡ�����б�����һҳҲ����һҳ
}MR_PKPageFlag;

/*
	����PK�ص�����ԭ�ͣ�
		typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg,void* extraInfo);
	������
		resultCode�������룬���MR_PKErrorCode
		resultMsg������˷��صĴ�����ʾ�ַ���

*/
typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg);

/*****************************************************/
/* mrc_getPkList�Ļص�ԭ��                           
 errCode: ������                                    
 lst:  PK��ɫ�б�,����errCodeΪMR_PKErrorCode_SUCCESSʱ��Ч.                     
		 lst�ͷ�˵����
			 �llst��һ������ṹ�����һ���ڵ�ָ��NULL��Ӧ�ø����ͷ�ÿ���ڵ���pCharacter��ָ����ڴ棬
			 �Լ�pNext��ָ����ڴ棨���pNext��ΪNULL�����ر�ע�⣬MR_Character��customDataָ����ָ���
			 �ڴ�Ҳ����Ӧ���ͷš�
			 ���͵��ͷ����£�
		 void demo_freeList(MR_CharacterList* lst)
		 {
			 MR_CharacterList* pNextNode;
			 if(lst != NULL)
			 {
				 pNextNode = lst->pNext;
				 while(pNextNode)
				 {	
					 lst->pNext = pNextNode->pNext;
					 if(pNextNode->pCharacter->customData != NULL)
					 {
						 free(pNextNode->pCharacter->customData);
						 pNextNode->pCharacter->customData =NULL;
					 }
					 free(pNextNode->pCharacter);
					 pNextNode->pCharacter = NULL;
					 free(pNextNode);
					 pNextNode = pNextNode->pNext;
				 }
				 if(lst->pCharacter->customData != NULL)
				 {
					 free(lst->pCharacter->customData);
					 lst->pCharacter->customData = NULL;
					 
				 }
				 if(lst->pCharacter!=NULL)
				 {
					 free(lst->pCharacter);
					 lst->pCharacter = NULL;
				 }
				 free(lst);
				 lst = NULL;
			 }
		  }								               
*/
/*****************************************************/
typedef void (*MR_GetPKList_CB)(MR_PKErrorCode errCode,MR_CharacterList*  lst,MR_PKPageFlag pageFlag);



/****************************************************************************************/
/*    ����ɸѡ�����õ�PK�б�                                                           
    ����:                                                                             
        nTop:    �õ�����������ǰ������¼                                             
        filter:  Ԥ���������,��Ԥ�����������ΪPKFilter_CUSTOMʱ,customFilter��Ч   
        customFilter: �Զ����������,����filterΪPKFilter_CUSTOMʱ��Ч             
                           customFilter��mrc_character��prefixDataΪɸѡ��λ           
						customFilterΪ�򵥹����ɸѡ���ʽ,֧������Ĺ���:		       					
							   C(N) = Value     ����                                            
							   C(N) <> Value    ������                                 
							   C(N) > Value     ����                                    
							   C(N) < Value     С��                                   
							   orderby C(N)  desc(asc)   ����                          
							   desc: ����                                               
							   asc:����                                                 
							   and  ����                                                
							   or   ����                                               
							   (..) �߼��ж����ȼ�                                     
                                                                                    
							Value Ϊint32����ֵ                                        
							1<=N<MAX_PREFIXDATA_COUNT                                  
                                                                                     
			��:   C1=1000 and (C2>241 or C5<>0)  orderby C1 desc                        
			�������Ϊ: ����Ϊ1000����(�ȼ�����241����ħ��������0)�ļ�¼ɸѡ������      
			����������Ľ���������ɸѡ													                    
        nOffset: ѡ���¼���ĵ�nOffset��ʼ                                             
        nCount:    �õ�nOffset֮���nCount����¼
		flag:	�ڻ�ȡ�б��ʱ���Ƿ�ͬʱҲ��ȡ�������ݣ����ͬʱ��ȡ�������ݣ���ص������ĵ�һ�������
				�Ľ�ɫ��Ϣ�Ǳ��ص�¼��ɫ��flagΪTRUE��ʾ��Ҫ��Я���������ݣ�����������£���õĽ�ɫ
				���nCount��һ����flagΪFALSE��ʾ����ҪЯ����������.
        cb:      �õ�PK��ɫ�Ļص�                                                   
                                                                                      
     ����:                                                                            
                                                                                       
        MR_FAILED:    ʧ��,�������ݴ���
		MR_WAITING:	  �������ز��
        MR_SUCCESS:   �ȴ�                                                            */
/****************************************************************************************/    
int32 mrc_netpk_getPKList(MR_PKFilter filter,
						  const char* customFilter,
						  int32 nOffset,
						  int32 nCount,
						  int32 flag,
						  MR_GetPKList_CB cb);


/*
	˵����
		�ڻ�ȡ�б��ͬʱ�ϴ���������
	����ԭ�ͣ�
	int32 mrc_netpk_getPKListWithAppdata(MR_PKFilter filter,
										const char* customFilter,
										int32 nOffset,
										int32 nCount,
										int32 flag,
										MR_Character* owner,
										uint16* feed_desc,
										int32 propNum,
										uint16** propStr,
										MR_GetPKList_CB cb);
	������
		���룺
			filter��Ԥ�����ɸѡ�����������MR_PKFilter
			customFilter��ֻ����filterΪMR_PKFilter_CUSTOMʱ�ò�����Ч��
			nOffset����ѯ���Ľ���е�ƫ����
			nCount���ܹ�Ҫ��ȡ��������
			flag��	�ڻ�ȡ�б��ʱ���Ƿ�ͬʱҲ��ȡ�������ݣ����ͬʱ��ȡ�������ݣ���ص������ĵ�һ�������
					�Ľ�ɫ��Ϣ�Ǳ��ص�¼��ɫ��flagΪTRUE��ʾ��Ҫ��Я���������ݣ�����������£���õĽ�ɫ
					���nCount��һ����flagΪFALSE��ʾ����ҪЯ����������.
			owner�����ؽ�ɫ��Ϣ
			feed_desc�����ϴ����ַ�����utf16be����
			propNum���ϴ����Զ����ַ��������������5��������wap����ʾ
			propStr���ϴ����ַ�������
			cb���õ�PK��ɫ�Ļص�
		�����	MR_SUCCESS: ���ڽ�����������
				MR_WAITING: ���������ƴ洢���
				MR_FAILED:	ʧ��
*/
int32 mrc_netpk_getPKListWithAppdata(MR_PKFilter filter,
									 const char* customFilter,
									 int32 nOffset,
									 int32 nCount,
									 int32 flag,
									 MR_Character* owner,
									 uint16* feed_desc,
									 int32 propNum,
									 uint16** propStr,
									 MR_GetPKList_CB cb);



/****************************************/
/*mrc_netpk_upload_appdata�Ļص�ԭ�� 
errCode:   ������                
respData:  �ӷ���˷���������,��Ӧ�ø����ͷţ�ĿǰΪutf16be���������    
len:       respData�ĳ���				*/  
/****************************************/
typedef void (*MR_Netpk_upload_appdata_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

/****************************************************************************************************/
/*  ������PK����˷��ͽ�ɫ��Ϣ���Ը��¸ý�ɫ��Ϣ                                                  
    �������Ҫ�Ļ���Я��һ��FEED �ı���Ϣ,FEED��Ϣ������WAPվ���չ�֡�                       
															                                     
   FEED �ı���Ϣ��ʽΪUTF16BE�ı���,������һЩ�������������滻�ı�ʶ:						                                                       
     ��ʶ:  ${SKYID:<XXX>}  ��ɫID  �����滻������ǳ�									
            ${APPID:<XXX>} Ӧ��ID �����滻����Ϸ����											
     ��: "${SKYID:<4503044>}��${APPID:<80345>}��սʤ��${SKYID:<4503044>},�ȼ�������5��"    
	�ڷ���˻ᱻ�滻��:                                                                            
		 "������������վ��սʤ�˱�����ӥ,�ȼ�������5��"																					
   ����:																						
         owner	:     ����µĽ�ɫ��Ϣ															
         feed_desc:     Feed��Ϣ(UTF16BE)															
   ����:																							
          MR_FAILED:   ʧ�ܣ��������ݴ���															
          MR_SUCCESS:  �ȴ�																		
   �˺������������³���ʹ��:                                                                      
          1. ���µ�ǰ��Ϸ��ɫ��Ϣ																	
		  2. �ϴ�feed�Խ��й㲥												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_upload_appdata(MR_Character* owner,const uint16* feed_desc,MR_Netpk_upload_appdata_CB cb);


/****************************************************************************************************/
/*  ������PK����˷��ͽ�ɫ��Ϣ���Ը��¸ý�ɫ��Ϣ                                                  
	�������Ҫ�Ļ���Я��һ��FEED �ı���Ϣ,FEED��Ϣ������WAPվ���չ�֡��ýӿ�ͬʱ�����ϴ������ַ������������ˣ�
	Ŀǰ���ϴ�������ϢҲ����Ϊ��Ϸ�Զ����ַ�����utf16-be���룩ʹ�ã����ַ����������ڱ��ص�¼�Ľ�ɫ������ϴ���
	��ɫ��û�б��ؽ�ɫ��ô���ϴ����ַ�����                      
															                                     
   FEED �ı���Ϣ��ʽΪUTF16BE�ı���,������һЩ�������������滻�ı�ʶ:						                                                       
     ��ʶ:  ${SKYID:<XXX>}  ��ɫID  �����滻������ǳ�									
            ${APPID:<XXX>} Ӧ��ID �����滻����Ϸ����											
     ��: "${SKYID:<4503044>}��${APPID:<80345>}��սʤ��${SKYID:<4503044>},�ȼ�������5��"    
	�ڷ���˻ᱻ�滻��:                                                                            
		 "������������վ��սʤ�˱�����ӥ,�ȼ�������5��"																					
   ����:																						
         owner	:     ����µĽ�ɫ��Ϣ															
         feed_desc:     Feed��Ϣ(UTF16BE)
		 propNum��	�ϴ����߻���Ϸ�Զ����ַ���������
		 propStr��  ָ����߻���Ϸ�Զ����ַ��������ָ�룬�ַ�����utf16-be����
   ����:																							
          MR_FAILED:   ʧ�ܣ��������ݴ���															
          MR_SUCCESS:  �ȴ�																		
   �˺������������³���ʹ��:                                                                      
          1. ���µ�ǰ��Ϸ��ɫ��Ϣ																	
		  2. �ϴ�feed�Խ��й㲥												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_upload_appdataEx(MR_Character* owner,
								 const uint16* feed_desc,
								 int32 propNum,
								 uint16** propStr,
								 MR_Netpk_upload_appdata_CB cb);


/*PK���*/
typedef struct 
{
	uint32 winner;    /*Ӯ��SKYID*/ 
	uint32 loster;    /*���SKYID*/ 
}MR_PKResult;


/************************************/
/*mrc_netpk_commit_pkresult�Ļص�ԭ�� 
errCode:   ������                
respData:  �ӷ���˷��������ݣ���Ӧ�ø����ͷţ�ĿǰΪutf16be���������     
len:       respData�ĳ���        */  
/************************************/
typedef void (*MR_Netpk_commit_PKResult_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

/****************************************************************************************************/
/*  ������PK�����PK���																		
    �������Ҫ�Ļ���Я��һ��FEED �ı���Ϣ,FEED��Ϣ������WAPվ���չ�֡�                          
															                                      
   FEED �ı���Ϣ��ʽΪUTF16BE�ı���,������һЩ�������������滻�ı�ʶ:						                                                            
     ��ʶ:  ${SKYID:<XXX>}  ��ɫID  �����滻������ǳ�									
            ${APPID:<XXX>} Ӧ��ID �����滻����Ϸ����											
     ��: "${SKYID:<4503044>}��${APPID:<80345>}��սʤ��${SKYID:<4503044>},�ȼ�������5��"     
	�ڷ���˻ᱻ�滻��:                                                                             
		 "������������վ��սʤ�˱�����ӥ,�ȼ�������5��"																					
   ����:																						
		   result	:	  ����PK�е�winner�Լ�loster���û���������ָ����ڴ���Ӧ���Լ��ͷš�									
         owner	:     ����µĽ�ɫ��Ϣ															
		   count	:	  ָ����Ҫ���¶��ٸ���ɫ��Ϣ,Ŀǰ��ȡֵӦΪ0��1��2						
		   lstHead	:	  ��Ҫ���µĽ�ɫ������lstHead��ָ����ڴ���Ӧ���Լ��ͷ�													
         feed_desc:     Feed��Ϣ(UTF16BE)������ָ����ڴ���Ӧ���Լ��ͷ�	
   ����:																						
          MR_FAILED:   ʧ�ܣ��������ݴ���														
          MR_SUCCESS:  �ȴ�																	 
   �˺������������³���ʹ��:                                                                      
			1. PK��Ծ��ý��и���																	
			2. �ϴ�feed�Խ��й㲥												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_commit_pkresult(MR_PKResult* result,
								int32 count,
								MR_Character* lstHead,
								const uint16* feed_desc,
								MR_Netpk_commit_PKResult_CB cb);




/****************************************************************************************************/
/*  ������PK�����PK���																		
    �������Ҫ�Ļ���Я��һ��FEED �ı���Ϣ,FEED��Ϣ������WAPվ���չ�֡��ýӿ�ͬʱ�����ϴ������ַ������������ˣ�
	Ŀǰ���ϴ�������ϢҲ����Ϊ��Ϸ�Զ����ַ�����utf16-be���룩ʹ�ã����ַ����������ڱ��ص�¼�Ľ�ɫ������ϴ���
	��ɫ��û�б��ؽ�ɫ��ô���ϴ����ַ�����
															                                      
   FEED �ı���Ϣ��ʽΪUTF16BE�ı���,������һЩ�������������滻�ı�ʶ:						                                                            
     ��ʶ:  ${SKYID:<XXX>}  ��ɫID  �����滻������ǳ�									
            ${APPID:<XXX>} Ӧ��ID �����滻����Ϸ����											
     ��: "${SKYID:<4503044>}��${APPID:<80345>}��սʤ��${SKYID:<4503044>},�ȼ�������5��"     
	�ڷ���˻ᱻ�滻��:                                                                             
		 "������������վ��սʤ�˱�����ӥ,�ȼ�������5��"																					
   ����:																						
		   result	:	  ����PK�е�winner�Լ�loster���û���������ָ����ڴ���Ӧ���Լ��ͷš�									
         owner	:     ����µĽ�ɫ��Ϣ															
		   count	:	  ָ����Ҫ���¶��ٸ���ɫ��Ϣ,Ŀǰ��ȡֵӦΪ0��1��2						
		   lstHead	:	  ��Ҫ���µĽ�ɫ������lstHead��ָ����ڴ���Ӧ���Լ��ͷ�													
         feed_desc:     Feed��Ϣ(UTF16BE)������ָ����ڴ���Ӧ���Լ��ͷ�				
		 propNum��	�ϴ����߻���Ϸ�Զ����ַ���������
		 propStr��  ָ����߻���Ϸ�Զ����ַ��������ָ�룬�ַ�����utf16-be����
   ����:																						
          MR_FAILED:   ʧ�ܣ��������ݴ���														
          MR_SUCCESS:  �ȴ�																	 
   �˺������������³���ʹ��:                                                                      
          1. PK��Ծ��ý��и���																	
			2. �ϴ�feed�Խ��й㲥												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_commit_pkresultEx(MR_PKResult* result,
								  int32 count,
								  MR_Character* lstHead,
								  const uint16* feed_desc,
								  int32 propNum,
								  uint16** propStr,
								  MR_Netpk_commit_PKResult_CB cb);


/****************************************************************************************************/
/*	mrc_netpk_openWap�Ļص�ԭ��																		
	result:																							
		MR_SUCCESS:������ɹ����к��˳�																
		MR_FAILED:�����δ�ܳɹ�����																*/
/****************************************************************************************************/
typedef void (*MR_Netpk_openWap_CB)(int32 result);


/****************************************************************************************************/
/*  ���ʵ�ǰ��Ϸ������PKר��																	
	����:��																							
	���:                                                                                           
      MR_SUCCESS:�ȴ�                                                                                                        
		MR_FAILED: ʧ�ܣ��������ݴ��� 																
�˺������������³���ʹ��:																			                                                                     
          1. �û�����wap�˵ĵ�ǰ��Ϸ������PKר��													
			2. �鿴����feed��Ϣ																		*/		
/****************************************************************************************************/ 
int32 mrc_netpk_openWap(MR_Netpk_openWap_CB cb);


/********************************************************************/
/*
	��ȡ��ǰ���ù�������
	���룺��
	�����
		���ص�ǰ���õĹ����������ݲ����ƹ�������
	�˺����������³��ϣ�
		1.�ڻ�ȡ������Ϣǰ��ȡ��������
*/
/********************************************************************/
int32 mrc_netpk_getNoticeNum(void);


/********************************************************************************************/
/*	��ȡ��ǰ���õĵ�index�����棨index��0��ʼȡ��ȡֵ��Χ�ڣ�0��mrc_netpk_getNoticeNum-1��֮�䣩
	���룺��
	�����
		NULL���޹�����û�index����
		�ַ�����utf16-be���룬��\x00\x00��β��
�˺��������������³��ϣ�
	1.�û���ȡ��ǰ���õĹ���
*/
/******************************************************************************************/
uint16* mrc_netpk_getNotice(int32 index);


/********************************************************************************************/
/*
	����ȷ������PK�����pluginsĿ¼�£��ƴ洢���
	���룺mr_safeNetpk_cb
	�����
		MR_SUCCESS:ͨ���˱��ز���汾���
		MR_WAITING:���������ƴ洢���
		MR_FAILED:ʧ��
*/
/******************************************************************************************/
int32 mrc_netpk_initVersion(mr_safeNetpk_cb cb);

/********************************************************************************************/
/*
	����������������Ϸ�󣬽���ԭ��Ϸ·���¼���ͬ��mrp��ʹԭ��Ϸ������
	���룺��
	�����
		MR_FAILED:���ص���Ϸ·������ԭ��Ϸ·����
		MR_SUCCESS:���ڼ�����Ϸ
	�˺��������������³��ϣ�
		1.����Ϸ�Ը���֮�󣬶���Ϸ��������
*/
/******************************************************************************************/
int32 mrc_netpk_restartSelf(void);

/*
	�ͷŹ�����ռ���ڴ�ռ�
*/
int32 mrc_netpk_releaseRes(void);

#ifdef __cplusplus
}
#endif

#endif