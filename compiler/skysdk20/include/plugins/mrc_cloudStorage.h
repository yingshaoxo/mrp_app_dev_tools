#ifndef __CLOUD_STORAGE_H__
#define __CLOUD_STORAGE_H__

#include "mrc_base.h"

typedef enum
{
	MR_CLOUD_SUCCESS		 = 0,	//�ɹ�
	MR_CLOUD_INVAILD_PARAM	 = 1,	//��������
	MR_CLOUD_BLACK_LIST		 = 2,	//�������û�
	MR_CLOUD_FORCE_UPDATE	 = 3,	//ǿ�Ƹ���
	MR_CLOUD_SUGG_UPDATE	 = 4,	//�������
	MR_CLOUD_INVAILD_USER	 = 5,	//������û���������
	MR_CLOUD_SERVER_BUSY	 = 6,	//������æ
	MR_CLOUD_OTHER_FAILURE	 = 7	//��������,һ���ɱ��ش�������,������ʧ�ܵ�
}MR_cloudStorage_errcode_E;

typedef enum
{
	MR_SQLTYPE_BYTEA	= 2,	//bytea			���ֽ�
	MR_SQLTYPE_BOOLEAN	= 1,	//boolean		���ֽ�
	MR_SQLTYPE_INTERGER	= 4,	//interger		���ֽ�
	MR_SQLTYPE_BIGINT	= 0,	//bigint		���ֽ�
	MR_SQLTYPE_TIMESTP	= 5,	//timestamp		���ֽ�
	MR_SQLTYPE_VCHAR	= 3,	//char varying	������
	MR_SQLTYPE_UNIVCHAR = 6		//unicode varChar������
}MR_sqlType_E;

typedef enum
{
	MR_CLOUD_RESULT_TYPE_MODIFY = 1,	//��������ͣ��޸�
	MR_CLOUD_RESULT_TYPE_QUERY  = 2		//��������ͣ���ѯ
}MR_cloudStorage_RsType_E;

typedef struct  
{
	int32 responseCode;			//���������ص�code
	uint16* responseMsg;		//ִ�н���Ĵ�����ʾutf-16BE���루�Ǳ��룬������NULL��
	int32 resultSetNum;			//ִ�н���Ľ������g�������Ǳ��룬����ǲ�ѯ�����Ϣ��û���ˣ�
}MR_SQL_EXE_INFO_T;

typedef struct  
{
	MR_cloudStorage_RsType_E rsType;	//ִ�н����״̬�룬ȡֵΪMR_cloudStorage_errcode_Eö���е�����
	int32 resultCode;
	uint16* errorMsg;
	int32 columnCount;
	int32 rowCount;
}MR_SQL_RS_INFO_T;

typedef struct  
{
	int16 year;
	int16 month;
	int16 day;
	int16 hour;
	int16 minute;
	int16 seconds;
	int16 milliSecond;
}MR_SQL_DATE_T;

/*ִ��mrc_sql_execute�󷵻ؽ���Ļص�����*/
typedef void (*MR_SQL_EXE_CB)(int32 exeResult, MR_SQL_EXE_INFO_T mr_sqlExeInfo);	//ִ��SQL�����ɺ�Ļص�����

typedef void (*MR_Complete_CB)(int32 dlResult);


/************************************************************************/
/*
	���ܣ���ʼ���ƴ洢
	���룺
		cb�������Ʋ���Ļص�
	���أ�
		MR_OPENSKY_WAITING���汾���ڸ��£���Ҫ�ȴ��ص�
		MR_OPENSKY_SUCCESS�����سɹ���û�лص�
		MR_OPENSKY_FAILED������ʧ�ܣ�û�лص�
		MR_OPENSKY_PARAM_ERROR����������û�лص�
*/
/************************************************************************/
int32 mrc_sql_start(MR_Complete_CB cb);

/************************************************************************/
/*
	�����ƴ洢��Կ������ܳ���64��ASCII�ַ�
	���룺
		secretStr����Կ�ַ�������˹���ṩ
	���أ�
		void
*/
/************************************************************************/
void mrc_sql_setSecretStr(char* secretStr);

/************************************************************************/
/*��������mrc_sql_appendStatement
	���ܣ����úý�Ҫִ�е�sql��䣬����append��Σ��Դ�����sql��䣬ע�����һ��Ҳ��Ҫ���ö��
	format˵����%A MR_SQLTYPE_BYTEA ע��!!!����������Ӧ����������һ����bytea���׵�ַ����һ����bytea�ĳ���!!!
					����"...%A...", ...,byteaAddr,byteaSize,... Ҳ������byteaAddr���⸽����bytea�ĳ��ȣ�
					������ȱ�������Ҫ�κ�%��Ӧ��������ʽ������Ҫ��һһ��Ӧ����
				%O MR_SQLTYPE_BOOLEAN
				%I MR_SQLTYPE_INTERGER
				%B MR_SQLTYPE_BIGINT
				%T MR_SQLTYPE_TIMESTP
				%V MR_SQLTYPE_VCHAR
				%U MR_SQLTYPE_UNIVCHAR
	����ע��������ݴ���ʱ��Ҫ�Ա�����ʽ���룬�����ڴ�bigInt����ʱ�����ֱ��д����4���������Ĭ�Ͻ��䵱int32����
				  ���º���������ջ���ݶ�ȡ���󣬵���������ɱ��������
	���룺
		  sqlStatement ��Ҫ�����sql���
		  paramFormat ������ʽ�ַ���
		  ...		  �����Ĳ���	
	�����MR_OPENSKY_RETCODE ö��:
				
*/

/*
	������ִ�е�SQL��䣨��ͬʱ���ö�����
	���룺
		sqlStatement��SQL���
		paramFormat��SQL�����Ĳ�����ʽ���ַ�������printf����
		...����paramFormat����ʹ�õĲ���
			˵����paramFormat��ǰ��֧�ֶ��ָ�ʽ���ַ�����
				%A���������ͣ������������һ�������׵�ַ�����鳤��
				%O�������ͣ�������������һ��0��1
				%I�����ͣ�������������һ��int32
				%T��ʱ���ͣ�������������һ��ʱ��
				%V���ַ�����������������һ���ַ���
				%U��UNICODE�ַ�����������������һ��Unicode�ַ���
			ע�⣺
				���ݴ���ʱ��Ҫ�Ա�����ʽ���룬�����ڴ�bigInt����ʱ�����ֱ��д����4���������Ĭ�Ͻ��䵱int32����
				���º���������ջ���ݶ�ȡ���󣬵���������ɱ��������
			����
				mrc_sql_appendStatement("select * from T where id=?", "%I", 100); //ִ��select * from T where id=100
	���أ�
		MR_OPENSKY_RETCODEö��:
			MR_OPENSKY_SUCCESS��ִ�гɹ�
			MR_OPENSKY_FAILED��ִ��ʧ��
			MR_OPENSKY_PARAM_ERROR�������������
			MR_OPENSKY_MALLOC_FAILED: �����ڴ�ʧ��
			MR_OPENSKY_STORAGE_REJECT : ���������ִ�У����Ծܾ�ִ�е�ǰ����
			(�ú�������ֵֻ��Ҫ�ж��Ƿ���MR_OPENSKY_SUCCESS���ɣ���������ֵ����FAILED���͵�ϸ��)
*/
/************************************************************************/
int32 mrc_sql_appendStatement(const char* sqlStatement, const char* paramFormat, ...);

/************************************************************************/
/*
	ִ�����úõ�SQL���
	���룺
		query_cb��ִ�к�Ľ���ص�����
		skyid������Ҫskyid����д��������Ҫ����0
	���أ�
		MR_OPENSKY_WAITING���ȴ�ִ�н���ص�
		����(MR_OPENSKY_FAILED / MR_OPENSKY_PARAM_ERROR / MR_OPENSKY_STORAGE_REJECT)��ʧ��
*/
/************************************************************************/
int32 mrc_sql_execute(MR_SQL_EXE_CB query_cb, int32 skyid);

/************************************************************************/
/*
	��ȡSQL������������Ϣ
	���룺
		resultSetIndex������������ţ���0��ʼ
	�����
		mr_sqlRsInfo���������ϢMR_SQL_RS_INFO_T
	���أ�
		MR_OPENSKY_SUCCESS��ִ�гɹ�
		MR_OPENSKY_FAILED��ִ��ʧ��
*/
/************************************************************************/
int32 mrc_sql_getResultSetInfo(int32 resultSetIndex, MR_SQL_RS_INFO_T* mr_sqlRsInfo);

/************************************************************************/
/*
	����SQL�����
	���룺
		resultSetIndex������������ţ���0��ʼ
		row��Ŀ�����ݵ������У���0��ʼ
		column��Ŀ�����ݵ������У���0��ʼ
	�����
		pData��Ŀ������
			ע�⣺Ŀ���������ͱ�����SQL���е���ȫ��Ӧ��������ܳ��ֲ���Ԥ�ϵĽ����
				  ���磬SQL����Ϊint32ʱ��void*����Ϊint32*��
		len������ռ�õĳ��ȣ����ֽ�Ϊ��λ��int32ռ4���ֽ�
	���أ�
		MR_OPENSKY_SUCCESS��ִ�гɹ�
		MR_OPENSKY_FAILED��ִ��ʧ��
*/
/************************************************************************/
int32 mrc_sql_resolveRS(int32 resultSetIndex, int32 row, int32 column, void* pData, int32* len);

/************************************************************************/
/*
	�ͷ�������Դ����ж�ز��
*/
/************************************************************************/
void mrc_sql_releaseResource(void);	//�ͷű�ռ�õ���Դ

/************************************************************************/
/*
	�ͷ�������Դ����ж�ز��
*/
/************************************************************************/
void mrc_sql_releaseResourceOnly(void);

/*********************************************************
	�����ȡ���ݶ����ӵĽӿڣ����½ӿڿ���
	ͨ�ý�������mrc_sql_resolveRSʵ����ͬ����
	���ϰ��ʹ��mrc_sql_resolveRS���溯���ɺ���
**********************************************************/

/************************************************************************/
/*
	����SQL������е�boolean����
	���룺
		resultSetIndex������������ţ���0��ʼ
		row��Ŀ�����ݵ������У���0��ʼ
		column��Ŀ�����ݵ������У���0��ʼ
	�����
		len������ռ�õĳ��ȣ����ֽ�Ϊ��λ���������Ϊ0�����ǽ���ʧ��
	���أ�
		�������booleanֵ
*/
/************************************************************************/
uint8 mrc_sql_resolve_boolen(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	����SQL������е�int32����
	���룺
		resultSetIndex������������ţ���0��ʼ
		row��Ŀ�����ݵ������У���0��ʼ
		column��Ŀ�����ݵ������У���0��ʼ
	�����
		len������ռ�õĳ��ȣ����ֽ�Ϊ��λ���������Ϊ0�����ǽ���ʧ��
	���أ�
		�������int32ֵ
*/
/************************************************************************/
uint32 mrc_sql_resolve_int(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	����SQL������е�bigint����
	���룺
		resultSetIndex������������ţ���0��ʼ
		row��Ŀ�����ݵ������У���0��ʼ
		column��Ŀ�����ݵ������У���0��ʼ
	�����
		len������ռ�õĳ��ȣ����ֽ�Ϊ��λ���������Ϊ0�����ǽ���ʧ��
	���أ�
		�������bigintֵ
*/
/************************************************************************/
uint64 mrc_sql_resolve_bigInt(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	����SQL������е�date����
	���룺
		resultSetIndex������������ţ���0��ʼ
		row��Ŀ�����ݵ������У���0��ʼ
		column��Ŀ�����ݵ������У���0��ʼ
	�����
		len������ռ�õĳ��ȣ����ֽ�Ϊ��λ���������Ϊ0�����ǽ���ʧ��
	���أ�
		�������MR_SQL_DATE_Tֵ
*/
/************************************************************************/
MR_SQL_DATE_T mrc_sql_resolve_timeStamp(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	����SQL������е�byte��������
	���룺
		resultSetIndex������������ţ���0��ʼ
		row��Ŀ�����ݵ������У���0��ʼ
		column��Ŀ�����ݵ������У���0��ʼ
	�����
		len������ռ�õĳ��ȣ����ֽ�Ϊ��λ���������Ϊ0�����ǽ���ʧ��
	���أ�
		�������byte�����׵�ַָ��
*/
/************************************************************************/
const uint8* mrc_sql_resolve_bytea(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	����SQL������е�varchar����
	���룺
		resultSetIndex������������ţ���0��ʼ
		row��Ŀ�����ݵ������У���0��ʼ
		column��Ŀ�����ݵ������У���0��ʼ
	�����
		len������ռ�õĳ��ȣ����ֽ�Ϊ��λ���������Ϊ0�����ǽ���ʧ��
	���أ�
		�������char�����׵�ַָ��
*/
/************************************************************************/
const char* mrc_sql_resolve_varChar(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	��ȡ������ʱ��Ļص�
	�����
		result���Ƿ�ɹ�
		date��ʱ���������result==MR_OPENSKY_SUCCESSʱ��Ч
*/
/************************************************************************/
typedef void (*MR_SQL_GETTIME_CB)(int32 result, MR_SQL_DATE_T date);

/************************************************************************/
/*
	��ȡ������ʱ��
	���룺
		cb���ص�����
	���أ�
		MR_OPENSKY_RETCODE����MR_OPENSKY_WAITINGʱ��Ҫ�ȴ��ص�
*/
/************************************************************************/
int32 mrc_sql_getServerTime(MR_SQL_GETTIME_CB cb);

/************************************************************************/
/*
	�ӻ����з�����ʱ��
	���أ�
		�����б���ķ�����ʱ��������ʱ��������һ���Ʋ����������б�����
*/
/************************************************************************/
MR_SQL_DATE_T mrc_sql_getServerTimeFromBuffer(void);

#endif