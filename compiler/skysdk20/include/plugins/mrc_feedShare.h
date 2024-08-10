#ifndef __MRC_FEED_H_
#define __MRC_FEED_H_
#include "mrc_base.h"
#include "mrc_base_i.h"
#include "verdload.h"


typedef enum
{
	MR_FEEDErrorCode_SUCCESS = 0,			/* �ɹ�           */
	MR_FEEDErrorCode_UserCancel,			/* �û�ȡ��       */
	MR_FEEDErrorCode_Network,				/* �������       */
	MR_FEEDErrorCode_FormatError,			/*��ʽ����		  */
	MR_FEEDErrorCode_BlackListUser,			/* �������û�     */ 
	MR_FEEDErrorCode_updatePlugins,			/*���²��		  */
// 	MR_FEEDErrorCode_updateLst,				/*�����б�		  */
	MR_FEEDErrorCode_invalidUser=7,			/*�û�������	  */
	MR_FEEDErrorCode_serverBusy,			/*������æ	   	  */
	MR_FEEDErrorCode_otherError				/*��������		  */
}MR_FEEDErrorCode;

/*
	errorCode:���MR_FEEDErrorCode
	resultMsg:����˷��ص���ʾ�ַ���	
*/
typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg);

/*
	��ʼ��feed������
	����:
		MR_DL_CB cb:������ɵĻص�
	���:	
		��
	����ֵ��
		MR_FAILED ʧ�ܣ��ص�ΪNULL
		MR_WAITING	�ȴ����������ز��
		MR_SUCCESS �ɹ���ͨ�����ذ汾���
*/
int32 mrc_feed_initVersion(MR_DL_CB cb);

/*
	����feed����������UI
	����:
		int32 isDump:�Ƿ���dumpģʽ���أ�0��ʾ�÷�dumpģʽ����
						1��ʾ��dumpģʽ����
		MR_FEED_CB cb:�˳�����Ļص�
		char* templates:ģ�壬��ascii���룬������ʽ�������
		�ɱ����:
			%d	Int32���͵ĸ�ʽ������
			%s	Ascii�����ַ���
			������ɱ����ʱ����ͬʱ����һ���������ȣ�һ����ʽ���������ʾ�����£�
		ʾ��:
		int32 data;
		char* wStr = "\x00\x73\x00\x6a\x00\x66\x00\x6b\x00\x6a\x00\x73\x00\x6b\x00\x6c\x00\x64\x00\x6a\x00\x66\x00\x00";
		data = 2;
		mrc_feed_share(0,feed_cb,"hello:%d%s",4,&data,mrc_wstrlen(wStr),wStr);
		ʾ��˵����ģ���е�"%d"��Ӧ4�Լ�&data��4��ʾdata���ݵĳ��ȣ�&data��������;ģ���е�"%s"��Ӧ�Ĳ�����
		mrc_wstrlen(wStr)�Լ�wStr,mrc_wstrlen(wStr)
		��wStr�ĳ��ȣ�wStr��������
	���:	
		��
	�ص���
	typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg)
		int32 errorCode:MR_FEEDErrorCode������
		resultMsg:utf16be�������ʾ�ַ���
	����ֵ��
		MR_SUCCESS �ɹ�����feed������
		MR_WAITING ��������feed������
		MR_FAILED	�ص�Ϊ��
*/
int32 mrc_feed_share(int32 isDump,MR_FEED_CB cb,char* templates,...);

/*
	����feed������������ѡ���Ƿ��UI
	����:
		int32 isDump:�Ƿ���dumpģʽ���أ�0��ʾ�÷�dumpģʽ����
						1��ʾ��dumpģʽ����
		int32 isShow:�Ƿ��UI
		int32 target:ָ������Ҫʲô�ط����������UI����target��Ӧ��ָ����
					�����UI����ȫѡ��
					Bit-map��
						��0λ���Ƿ����ð�ݿռ�
						��1λ���Ƿ����ð�ݹ���
						��2λ���Ƿ��������΢��

		MR_FEED_CB cb:�˳�����Ļص�
		char* templates:ģ�壬��ascii���룬������ʽ�������
		�ɱ����:
			%d	Int32���͵ĸ�ʽ������
			%s	Ascii�����ַ���
			������ɱ����ʱ����ͬʱ����һ���������ȣ�һ����ʽ���������ʾ�����£�
		ʾ��:
		int32 data;
		char* wStr = "\x00\x73\x00\x6a\x00\x66\x00\x6b\x00\x6a\x00\x73\x00\x6b\x00\x6c\x00\x64\x00\x6a\x00\x66\x00\x00";
		data = 2;
		mrc_feed_share(0,feed_cb,"hello:%d%s",4,&data,mrc_wstrlen(wStr),wStr);
		ʾ��˵����ģ���е�"%d"��Ӧ4�Լ�&data��4��ʾdata���ݵĳ��ȣ�&data��������;ģ���е�"%s"��Ӧ�Ĳ�����
		mrc_wstrlen(wStr)�Լ�wStr,mrc_wstrlen(wStr)
		��wStr�ĳ��ȣ�wStr��������
	���:	
		��
	�ص���
	typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg)
		int32 errorCode:MR_FEEDErrorCode������
		resultMsg:utf16be�������ʾ�ַ���
	����ֵ��
		MR_SUCCESS �ɹ�����feed������
		MR_WAITING ��������feed������
		MR_FAILED	�ص�Ϊ��
*/
int32 mrc_feed_shareEx(int32 isDump,int32 isShow,int32 target,MR_FEED_CB cb,char* templates,...);


/*
	�Ӻ���Ϊ��ע����UI
	����:
		int32 desSkyid:Ŀ��skyid������ӵĺ��ѵ�skyid
		MR_FEED_CB cb:�˳�����Ļص�
	���:	
		��
	�ص���
	typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg)
		int32 errorCode:MR_FEEDErrorCode������
		resultMsg:utf16be�������ʾ�ַ���
	����ֵ��
		MR_SUCCESS �ɹ�����feed������
		MR_WAITING ��������feed������
		MR_FAILED	�ص�Ϊ��
*/
int32 mrc_feed_attend(int32 desSkyid,MR_FEED_CB cb);


#endif