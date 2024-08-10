//mrc_opensky.h
//���ļ���������OPENSKYģ�����ں�������


#ifndef __MRC_OPENSKY_COMMON__
#define __MRC_OPENSKY_COMMON__

#include "mrc_base.h"

#ifdef __cplusplus
extern "C" {
#endif

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////�궨��//////////////////////////////////////////
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//netPK ģ��
#define MAX_PREFIXDATA_COUNT   20

#define MAX_NICKNAME_LEN  12
#define MAX_TITLE_LEN     12
#define MAX_IDENTIFY_LEN 32
#define MAX_SERVERNAME_LEN	12

//���ģ��
#define MRC_OPENSKY_ENTRY_START_TYPE_PARAM 100

typedef enum
{
	MR_OPENSKY_CLOUD_STORAGE		=	0,			//�ƴ���
	MR_OPENSKY_CLOUD_STORE			=	1,		//�����̳�
	MR_OPENSKY_CLOUD_ACHIEVEMENT	=	2,		//�ɾ�
	MR_OPENSKY_CLOUD_FEED			=	3,		//feed
	MR_OPENSKY_CLOUD_NETPK			=	4,		//����PK
	MR_OPENSKY_CLOUD_ENTRY			=	5,		//���

	MR_OPENSKY_CLOUD_MAX
	
}MR_OPENSKY_CLOUD_TYPE;

typedef enum
{
	//common def
	MR_OPENSKY_FAILED			=	MR_FAILED,		//-1, ʧ��
	MR_OPENSKY_SUCCESS		=	MR_SUCCESS,	//0, �ɹ�
	MR_OPENSKY_IGNORE		=	MR_IGNORE,		//1, ����
	MR_OPENSKY_WAITING		=	MR_WAITING,		//2, waiting
	MR_OPENSKY_PARAM_ERROR	=	5, 				//5, �����������		
	MR_OPENSKY_MALLOC_FAILED,					//6, mallocʧ��

	MR_OPENSKY_FORCE_UPDATE,					//ǿ�Ƹ���
	MR_OPENSKY_SUGG_UPDATE,						//�������
	MR_OPENSKY_INVAILD_USER,						//������û���������
	MR_OPENSKY_SERVER_BUSY,						//������æ

	//�ƴ���
	MR_OPENSKY_CLOUD_REJECT	=	0x080 			//�����������ִ���У�����ͬʱִ����һ���
	
	

	
}MR_OPENSKY_RETCODE;


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////�ƴ���(cloude storage)ģ��///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_CLOUDE_STORAGE	//�ָ������ʵ������
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//�����ƴ�����һ�������Ϊ���ְ汾�����ԣ�����ֵ���屣�ֲ���
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

//ִ��mrc_sql_start��Ļص�����
typedef void (*MR_Complete_CB)(int32 dlResult);

/*��������mrc_sql_start
	���ܣ���ʼ���ƴ洢
	�����MR_WAITING���汾���ڸ���
		  MR_SUCCESS ���سɹ�
		  MR_FAILED ����ʧ��
*/
int32 mrc_sql_start(MR_Complete_CB cb);

/****************************************************************************************************
	��������mrc_sql_setSecretStr
	���ܣ�����cp ����Ӧ�õ�secret�ַ���
	�������
****************************************************************************************************/
void mrc_sql_setSecretStr(char* secretStr);

/****************************************************************************************************
	��������mrc_sql_appendStatement
	���ܣ����úý�Ҫִ�е�sql��䣬����append��Σ��Դ�����sql��䣬ע�����һ��Ҳ��Ҫ���ö��
	format˵����%A MR_SQLTYPE_BYTEA ע��!!!����������Ӧ����������һ����bytea���׵�ַ����һ����bytea�ĳ���!!!
					����"...%A...", ...,byteaAddr,byteaSize,... Ҳ������byteaAddr���⸽����bytea�ĳ��ȣ�
					������ȱ�������Ҫ�κ�%��Ӧ��������ʽ������Ҫ��һһ��Ӧ����
				%O MR_SQLTYPE_BOOLEAN
				%I MR_SQLTYPE_INTERGER
				%B MR_SQLTYPE_BIGINT
				%T MR_SQLTYPE_TIMESTP
				%V MR_SQLTYPE_VCHAR
	����ע��������ݴ���ʱ��Ҫ�Ա�����ʽ���룬�����ڴ�bigInt����ʱ�����ֱ��д����4���������Ĭ�Ͻ��䵱int32����
				  ���º���������ջ���ݶ�ȡ���󣬵���������ɱ��������
	���룺
		  sqlStatement ��Ҫ�����sql���
		  paramFormat ������ʽ�ַ���
		  ...		  �����Ĳ���	
	�����MR_SUCCESS �ɹ�
		  MR_FAILED  ʧ��		  
****************************************************************************************************/
int32 mrc_sql_appendStatement(const char* sqlStatement, const char* paramFormat, ...);

/*
  ��mrc_sql_appendStatement���ƣ�������������Ϊvoid**�ṹ����Ҫ����Ӧ���������õ�����ָ��
  �������ݱ�����format�е�һһ��Ӧ��format��ʽ��mrc_sql_appendStatement����ȫһ��
*/
int32 mrc_sql_appendStatementEx(const char* sql, const char* format, void** arr);

/****************************************************************************************************
	��������mrc_sql_execute
	���ܣ�ִ�����úõ�sql���
	���룺query_cb ֪ͨ�����ߵĻص�����
		  skyId: ��ȡ��skyId��û������0
	�����MR_WAITING
****************************************************************************************************/
int32 mrc_sql_execute(MR_SQL_EXE_CB query_cb, int32 skyid);

/****************************************************************************************************
	��������mrc_sql_getResultSetInfo
	���ܣ���ý�����������Ϣ
	���룺resultSetIndex �����������
	�����mr_sqlRsInfo����õ���Ϣ
	���أ�MR_SUCCESS �ɹ�
		  MR_FAILED  �ɹ�
****************************************************************************************************/
int32 mrc_sql_getResultSetInfo(int32 resultSetIndex, MR_SQL_RS_INFO_T* mr_sqlRsInfo);

/****************************************************************************************************
	��������mrc_sql_resolveRS
	���ܣ�ͨ�ý������ݺ������ɽ����������SQLtype֧�����͵�����
	���룺resultSetIndex �����������
		  row Ŀ�����ݵ�������
		  column Ŀ������������
	�����pData����Ż�õ�����
		  len��
	���أ�MR_SUCCESS �ɹ�
		  MR_FAILED  �ɹ�
****************************************************************************************************/
int32 mrc_sql_resolveRS(int32 resultSetIndex, int32 row, int32 column, void* pData, int32* len);

/****************************************************************************************************
	��������mrc_sql_releaseResource
	���ܣ��ͷ�������Դ������sql����
	���룺��
	�������
****************************************************************************************************/
void  mrc_sql_releaseResource(void);	//�ͷű�ռ�õ���Դ

/*�����ܽӿڽ���*/




/*һЩ�����ӿ�*/
/*********************************************************
	�����ȡ���ݶ����ӵĽӿڣ����½ӿڿ���
	ͨ�ý�������mrc_sql_resolveRSʵ����ͬ����
	���ϰ��ʹ��mrc_sql_resolveRS���溯���ɺ���
**********************************************************/
/****************************************************************************************************
	��������mrc_sql_resolve_boolen
	���ܣ�����boolen����
	���룺resultSetIndex �����������
		  row Ŀ�����ݵ�������
		  column Ŀ������������
		  len 
	���أ�������õ�boolen����
****************************************************************************************************/
uint8 mrc_sql_resolve_boolen(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	��������mrc_sql_resolve_int
	���ܣ�����int����
	���룺resultSetIndex �����������
		  row Ŀ�����ݵ�������
		  column Ŀ������������
	���أ�������õ�int����
****************************************************************************************************/
uint32 mrc_sql_resolve_int(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	��������mrc_sql_resolve_bigInt
	���ܣ�����bigInt����
	���룺resultSetIndex �����������
		  row Ŀ�����ݵ�������
		  column Ŀ������������
	���أ�������õ�bigInt����
****************************************************************************************************/
uint64 mrc_sql_resolve_bigInt(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	��������mrc_sql_resolve_timeStamp
	���ܣ�����ʱ����������
	���룺resultSetIndex �����������
		  row Ŀ�����ݵ�������
		  column Ŀ������������
	���أ�������õ�timeStamp����
****************************************************************************************************/
MR_SQL_DATE_T mrc_sql_resolve_timeStamp(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	��������mrc_sql_resolve_byte
	���ܣ�����byte��������
	���룺resultSetIndex �����������
		  row Ŀ�����ݵ�������
		  column Ŀ������������
	�����len ��õ����ݳ���
	���أ�������õ�byte���ݵ���ָ��
****************************************************************************************************/
const uint8* mrc_sql_resolve_bytea(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	��������mrc_sql_resolve_byte
	���ܣ�����varchar����
	���룺resultSetIndex �����������
		  row Ŀ�����ݵ�������
		  column Ŀ������������
	�����len ��õ��ַ����ĳ���
	���أ�������õ�char���ݵ���ָ��
****************************************************************************************************/
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

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////�����̳�(prop store)ģ��///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_PROP_STORE	//�ָ������ʵ������
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef enum
{
	OPENSKY_STORE_WAP_PAGE_INDEX = 1,
	OPENSKY_STORE_WAP_PAGE_ADD_PACK = 2
}MR_OPENSKY_STORE_WAP_PAGE_TYPE;

typedef enum{
	MR_PROPSTORE_FAILED  = -1,	//ʧ��
	MR_PROPSTORE_SUCCESS = 0,	//�ɹ�
	MR_PROPSTORE_EXPIRED,		//��Ʒ����
	MR_PROPSTORE_INVAILD,		//��Ʒ������
	MR_PROPSTORE_LIMIT,			//��Ʒ��������
	MR_PROPSTORE_INVAILD_PARAM,	//��������
	MR_PROPSTORE_BLACK_LIST	,	//�������û�
	MR_PROPSTORE_FORCE_UPDATE,	//ǿ�Ƹ���
	MR_PROPSTORE_SUGG_UPDATE,	//�������
	MR_PROPSTORE_INVAILD_USER,	//������û���������
	MR_PROPSTORE_SERVER_BUSY,	//������æ
	MR_PROPSTORE_OTHER_FAILURE	//��������,һ���ɱ��ش�������,������ʧ�ܵ�
}MR_ResultCode_E;


typedef void (*MR_PROPSTORE_WAP_CB)(int32 resultCode);

typedef void (*MR_PROPSTORE_CB)(MR_ResultCode_E resultCode, int32 packVolumn, int32 itemCount, uint16* serverMsg);

typedef void (*MR_PROPSTORE_DEL_CB)(MR_ResultCode_E resultCode, int8* exeResult, uint16* serverMsg);

/********************************************************
					������ؽӿ�
********************************************************/
/****************************************************************************************************
	��������mrc_propStore_queryUserPack
	���ܣ�  ��ѯ�û����������ڷ��ص�ǰ��Ծ�û������ڵ����е���
	���룺
			columnName ���÷��ص���������ѡ�ֶΣ�specid,descid,remain,period,num,attr �м��Զ��Ÿ�����
			���ֶκ���: 
			         specid��ʵ���ţ��ڷ���˵ľ����ţ���������delete�Ȳ���
				  descid:  CP�ύ��˹���ĵ��߱�ţ�������Ϸ�ڲ��ĵ���ID	
				  remain����ʾ����ʣ����;öȣ�������ʹ�ö�����
				  period:   ��ʾ�����ܵ��;ö�
				  num:   ��������
				  attr:   �������Խṹ
			queryCb    ��ѯ�ص�
	�����
			MR_SUCCESS �ɹ�
			MR_FAILED  ʧ��
	�ص������
			result	   ִ�н�� �ɹ���MR_SUCCESS ʧ�ܣ�MR_FAILED
			resultCode ��Ч
			packVolumn ����������
			itemCount  ��Ʒ����
			serverMsg  �������˷��ص���Ϣ������ΪNULL		
****************************************************************************************************/
int32 mrc_propStore_queryUserPack(char* columnName, MR_PROPSTORE_CB queryCb);

/****************************************************************************************************
	��������mrc_propStore_consumeGoods
	���ܣ�	������������Ʒ
	���룺
			id		   ������Ʒ��ID��
			num		   ���ĵ�����
			queryCb    ��ѯ�ص�
	�����
			MR_SUCCESS �ɹ�
			MR_FAILED  ʧ��
	�ص������
			result	   ִ�н�� �ɹ���MR_SUCCESS ʧ�ܣ�MR_FAILED
			resultCode ��MR_ResultCode_E����
			packVolumn ��Ч
			itemCount  ��Ч
			serverMsg  �������˷��ص���Ϣ������ΪNULL
****************************************************************************************************/
int32 mrc_propStore_consumeGoods(int32 id, int32 num, MR_PROPSTORE_CB queryCb);

/****************************************************************************************************
	��������mrc_propStore_deleteGoods
	���ܣ�	ɾ����Ʒ��ͬʱ����ת���������˱��������أ�ʹ�÷���������ߣ���ͬʱ���ж��ɾ�������������ʽ����
	���룺
			idArray		ɾ����Ʒ��ID������
			itemCount	��Ʒ����
			queryCb     ��ѯ�ص�
	�����
			MR_SUCCESS �ɹ�
			MR_FAILED  ʧ��
	�ص������
			result		ִ�н��
			exeResult   �����ɹ���Ϣ�����飬��idArrayһһ��Ӧ���ɹ�ΪMR_SUCCESS�����ɹ�ΪMR_FAILED
			serverMsg	���������ص���Ϣ
****************************************************************************************************/
int32 mrc_propStore_deleteGoods(int32* idArray, int32 itemCount, MR_PROPSTORE_DEL_CB queryCb);

/****************************************************************************************************
	��������mrc_propStore_getGoods
	���ܣ�	����key��ȡ��Ʒ
	���룺
			getKey		�����Ʒ��key��������һ������ID��boss��ID��
			columnName	���÷��ص���������ѡ�ֶΣ�specid,descid,remain,period,num,attr �м��Զ��Ÿ���
			queryCb     ��ѯ�ص�
			isGetPack	�Ƿ��ػ�õ��ߺ�������������ǽ����ش˴λ�õĵ��ߣ�0���ػ�õĵ��ߣ�1������������
	�����
			MR_SUCCESS �ɹ�
			MR_FAILED  ʧ��
	�ص������
			result	   ִ�н�� �ɹ���MR_SUCCESS ʧ�ܣ�MR_FAILED
			resultCode ��MR_ResultCode_E����
			packVolumn isGetPack = 0 ʱ��Ч��isGetPack = 1 ʱ���ر�������
			itemCount  ��õ���Ʒ��
			serverMsg  �������˷��ص���Ϣ������ΪNULL		
****************************************************************************************************/
int32 mrc_propStore_getGoods(int32 getKey, char* columnName, MR_PROPSTORE_CB queryCb, int32 isGetPack);

/****************************************************************************************************
	��������mrc_propStore_enterWap
	���ܣ����������������wapҳ�棬���߲鿴�û���������Ϣ������������ߵ���
	���룺
			page	��Ҫ�򿪵�ҳ��ȡֵ���£�
					ȡֵΪ��OPENSKY_STORE_WAP_PAGE_INDEX		���û����߱�����ҳ
					ȡֵΪ��OPENSKY_STORE_WAP_PAGE_ADD_PACK	�򿪱�������ҳ��
			wapCb	����������Ļص�����
					�ص��д��ص�resultCode����ȡֵΪ��
							MR_SUCCESS			����������ɹ�
							MR_FAILED			����ʧ��
****************************************************************************************************/
int32 mrc_propStore_enterWap(int32 page, MR_PROPSTORE_WAP_CB wapCb);

/********************************************************
				�����ӿڣ��������Ͳ���
********************************************************/

/****************************************************************************************************
	��������mrc_propStore_resolve
	���ܣ�  ���ݽ�����
	���룺
			row		��������������
			column	��������������
			pData	�����������
			len		������õ����ݵ�ռ���ֽڳ��� 0��˵�����ݿ��ж�ӦΪNULL
	�����
			MR_SUCCESS �ɹ�
			MR_FAILED  ʧ��
****************************************************************************************************/
int32 mrc_propStore_resolve(int32 row, int32 column, void* pData, int32* len);

/********************************************************
				�ͷ���Դ�ӿ�
********************************************************/
void mrc_propStore_release(void);

/**
* �ϴ����߻ص�
* resultCode:�ϴ����
*      MR_FAILED ʧ��
*      MR_SUCCESS �ɹ�
*      ����:
*          2:��Ʒ����
*          3:��Ʒ������
*          4:��Ʒ��������
*          5:�ϴ����߰汾����
*          6:�ϴ��������Կ��ɣ�������⣬��ʶΪ��������
*          7:�ϴ�����������Ŀ��ƥ��
*          8:�Ƿ������ϴ��ɹ���������⣬��ʶΪ�Ƿ��汾�ϴ�
*          9:���ɵ����ϴ��ɹ���������⣬��ʶΪ���ɰ汾�ϴ���3�ο��ɰ汾�ϴ�������Ϊ��һ�ηǷ��ϴ�
* ver:��һ���ϴ�����������İ汾�ţ���resultCode==MR_FAILEDʱ����ֵ��ȷ��
* ע�⣺ʧ��ʱ����������������һ��
**/
typedef void (*mrc_propStore_uploadItem_cb)(int32 resultCode, int32 ver);
/**
* �ϴ�����ǰ��-���õ�������
* ע�⣺�������256������
* order:���Ա��
* value:����ֵ
* ���ϲ�����Ӧ�������˹�������, �����ֻ��һ��ת��
* ע�⣺���������ڵ��� mrc_propStore_uploadItem �󽫱���գ��������ϴ�����Ҫ���µ��ñ��ӿ�
* ����:
*		MR_OPENSKY_SUCCESS:�ɹ��������
*		MR_OPENSKY_IGNORE:�Ѿ����ó���256,�����Ա�����,�ϴ����Ժ�������������
**/
int32 mrc_propStore_uploadItem_setItemProperty(int32 order, int32 value);
/**
* �ϴ����ߣ�ʹ�ñ���Ʒǰ���ȵ�¼ͨ��֤
* ver:���߰汾��
* cpGoodsId:����ID(��CP���ж���)�����ߵ�������μ� mrc_propStore_uploadItem_setItemProperty �ӿ�
* num:��������
* cb:�ص�����������ΪNULL
* ע�⣺�ڵ��ñ�����ǰ��Ӧ��Ӧ����ͣ�Լ����߼����ر��Ǳ���Ҫֹͣ����ˢ�£���ֱ��cb�ص�֪ͨ������ܼ���Ӧ���߼�
* ������ķ���ֵ
**/
int32 mrc_propStore_uploadItem(int32 ver, int32 cpGoodsId, int32 num, mrc_propStore_uploadItem_cb cb);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////�ɾ�ϵͳ(achievement)ģ��///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_ACHIEVEMENT	//�ָ������ʵ������
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef enum
{
	MR_ACHIEVEMENT_ERR_FORCE_UPDATE		 = 3,	//ǿ�Ƹ���
	MR_ACHIEVEMENT_ERR_RECOMMEND_UPDATE	 = 4,	//�������
	MR_ACHIEVEMENT_ERR_NOSKYID				 = 5,	//skyid������
	MR_ACHIEVEMENT_ERR_NOID					 = 8,	//�ɾ�id������
	MR_ACHIEVEMENT_ERR_UNLOCKED			 = 9,	//�Ѿ�����(�ظ�����)
	MR_ACHIEVEMENT_ERR_SERVERERROR			 = 99	//����������
}MR_Opensky_Achievement_errcode_E;
//����������ο��Ʋ������ֵ


/**
 * ����������ص�����
 * ���:
 * result   �ص����
 *          MR_SUCCESS �ɹ�
 *          ���� �������
 */
typedef void (*MR_ACHIEVEMENT_STARTBROWSER_CB)(int32 result/*ErrorCode*/, int32 var1, int32 var2, int32 var3, int32 var4, int32 var5);

/**
 * ������ػص�����
 * ���:
 * result   �ص����
 *          MR_SUCCESS �ɹ�
 *          MR_FAILED  ʧ��
 */
typedef void (*MR_ACHIEVEMENT_DL_CB)(int32 result);

/**
 * �����ɾ͵Ļص�����(���ҽ���mrc_achievement_unlock����ΪMR_SUCCESS�ᱻ����)
 * ���:
 * result   �ص����
 *          MR_SUCCESS �ɹ�
 *          MR_FAILED  ʧ��
 *          MR_Opensky_Achievement_errcode_E ö�ٵ�  ��������ֵ
*/
typedef void (*MR_ACHIEVEMENT_UNLOCK_CB)(int32 result);

/****************************************************************************************************
 * �����ʼ��
 * ����:
 * dlcb     ������ػص�����
 * ����:
 *          MR_SUCCESS �ɹ�
 *          MR_FAILED ʧ��
 *          MR_WAITING �ȴ���, �ȴ�������ػص�
 *          ��ȴ��ص���������
****************************************************************************************************/
int32 mrc_achievement_init(MR_ACHIEVEMENT_DL_CB dlcb);

/****************************************************************************************************
 * �������ʼ��, ����ʼ����, ��Ҫ���¹���, �����ٴε���mrc_achievement_init
 * ����:
 *          MR_SUCCESS �ɹ�
 *          MR_FAILED  ʧ��
****************************************************************************************************/
int32 mrc_achievement_uninit(void);

/****************************************************************************************************
 * �����ɾ�(�����ȵ�¼)
 * ����:
 * cb              �ص�����
 * achievementId   �ɾ͵��ʶ(ͨ���ɾ�ϵͳ��̨���в�ѯ)
 * costTime        (ѡ��)����Ϊѡ����,��ʶ�����ɾ������ѵ�ʱ��(����(����)Ϊ��λ)
 * ����:
 * MR_SUCCESS   �ɹ�
 * MR_FAILED    ʧ��(��ȷ���Ѿ���¼)
****************************************************************************************************/
int32 mrc_achievement_unlock(MR_ACHIEVEMENT_UNLOCK_CB cb, int32 achievementId, int32 costTime);

/****************************************************************************************************
 * ���������, ����ɾ�ϵͳ
 * ����:
 * cb              �ص�����
 * ����:
 * MR_SUCCESS: ������ɹ����к��˳�
 * MR_FAILED: �����δ�ܳɹ�����
****************************************************************************************************/
int32 mrc_achievement_startBrowser(MR_ACHIEVEMENT_STARTBROWSER_CB cb);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////FEED�ͺ���(feed)ģ��///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_FEED	//�ָ������ʵ������
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef enum
{
	MR_FEED_SUCCESS		 = 0,	//�ɹ�
	MR_FEED_INVAILD_PARAM	 = 1,	//��������
	MR_FEED_BLACK_LIST		 = 2,	//�������û�
	MR_FEED_FORCE_UPDATE	 = 3,	//ǿ�Ƹ���
	MR_FEED_SUGG_UPDATE	 = 4,	//�������
	MR_FEED_INVAILD_USER	 = 5,	//������û���������
	MR_FEED_SERVER_BUSY	 = 6,	//������æ
	MR_FEED_OTHER_FAILURE	 = 7	//��������,һ���ɱ��ش�������,������ʧ�ܵ�
}MR_OPENSKY_feedAndBuddy_errcode_E;

/************************************************************************/
/*
	ִ�ж���feed�Լ����Ѳ����Ļص�
	in:
		resultCode:���MR_feedAndBuddy_errcode_E
		resultMsg:�ϴ�����ɷ���˷��ص���ʾ�ַ���
*/
/***********************************************************************/
typedef void (*mrc_feedAndBuddy_cb)(int32 resultCode,uint16* resultMsg);

/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_uploadFeed
	����:�ϴ������û���feed
	in:
		skyid:Ҫ�ϴ�feed����ҵ�skyid
		feed:feed�ı���utf16be����
		category:feed�ϴ��Ķ�̬���
		cb:mrc_feedAndBuddy_cb���͵Ļص�
	out:
		MR_SUCCESS:�����ϴ�feed
		MR_WAITING:���������ƴ洢���
		MR_FAILED:������������������ʧ��
	cb:
		mrc_feedAndBuddy_cb���ͻص�
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_uploadFeed(int32 skyid,uint16* feed,uint16* category,mrc_feedAndBuddy_cb cb);


/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_uploadFeedEx
	����:�ϴ������û���feed,���ӷ���˻�ȡfeed
	in:
		skyid:Ҫ�ϴ�feed����ҵ�skyid
		feed:feed�ı���utf16be����
		category:feed�ϴ��Ķ�̬���
		type:typeΪ1��ʾ��ȡ����Ӧ�õ�feed��typeΪ2��ʾ��ȡ���Ǻ��ѵ�feed
		startOffset:feed��ƫ����
		limit:feed��ȡ������
		getCategory:��ȡ��feed�Ķ�̬���
		cb:mrc_feedAndBuddy_cb���͵Ļص�
	out:
		MR_SUCCESS:���������˽���
		MR_WAITING:���������ƴ洢���
		MR_FAILED:������������������ʧ��
	cb:
		mrc_feedAndBuddy_cb���ͻص�
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_uploadFeedEx(int32 skyid,uint16* feed,uint16* category,int32 type,int32 startOffset,int32 limit,uint16* getCategory,mrc_feedAndBuddy_cb cb);


/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_getFriendFeed
	����:��ȡ�����û��ĺ���feed
	in:
		skyid:Ҫ��ȡfeed����ҵ�skyid
		startOffset:feed��ƫ����
		limit:feed��ȡ������
		category:��ȡ��feed�Ķ�̬���
		cb:mrc_feedAndBuddy_cb���͵Ļص�
	out:
		MR_SUCCESS:���ڻ�ȡfeed
		MR_WAITING:���������ƴ洢���
		MR_FAILED:������������������ʧ��
	cb:
		mrc_feedAndBuddy_cb���ͻص�
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getFriendFeed(int32 skyid,int32 startOffset,int32 limit,uint16* category,mrc_feedAndBuddy_cb cb);

/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_getAppFeed
	����:��ȡӦ��feed
	in:
		startOffset:feed��ƫ����
		limit:feed��ȡ������
		category:��ȡ��feed�Ķ�̬���
		cb:mrc_feedAndBuddy_cb���͵Ļص�
	out:
		MR_SUCCESS:���ڻ�ȡfeed
		MR_WAITING:���������ƴ洢���
		MR_FAILED:������������������ʧ��
	cb:
		mrc_feedAndBuddy_cb���ͻص�
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getAppFeed(int32 startOffset,int32 limit,uint16* category,mrc_feedAndBuddy_cb cb);

/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_addFriend
	����:�Ӻ���
	in:
		curSkyid:��ǰ��Ч��skyid
		recvSkyid:���պ��������skyid
		cb:mrc_feedAndBuddy_cb���͵Ļص�
	out:
		MR_SUCCESS:���ڻ�ȡfeed
		MR_WAITING:���������ƴ洢���
		MR_FAILED:������������������ʧ��
	cb:
		mrc_feedAndBuddy_cb���ͻص�
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_addFriend(int32 curSkyid,int32 recvSkyid,mrc_feedAndBuddy_cb cb);


/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_sendMsg
	����:���ض�skyid������Ϣ
	in:
		curSkyid:��ǰ��Ч��skyid
		recvSkyid:������Ϣ��skyid
		content:���͵���Ϣ���ݣ�utf16be����
		cb:mrc_feedAndBuddy_cb���͵Ļص�
	out:
		MR_SUCCESS:���ڻ�ȡfeed
		MR_WAITING:���������ƴ洢���
		MR_FAILED:������������������ʧ��
	cb:
		mrc_feedAndBuddy_cb���ͻص�
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_sendMsg(int32 curSkyid,int32 recvSkyid,uint16* content,mrc_feedAndBuddy_cb cb);



/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_getNoticeNum(ͬ���ӿ�)
	in:
		��
	out:
		�ܻ�ȡ������Ч�Ĺ���������0��ʾû��
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getNoticeNum(void);


/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_getNotice(ͬ���ӿ�)
	in:
		index:��ȡ�����index����0��(mrc_feedAndBuddy_getNoticeNum-1)
	out:
		��ȡ���Ĺ��棬utf16be���룬�����ȡ�������򷵻�NULL
*/
/***********************************************************************/
uint16* mrc_feedAndBuddy_getNotice(int32 index);

/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_getFeedNum(ͬ���ӿ�)
	in:
		��
	out:
		�ܻ�ȡ������Ч��feed������0��ʾû��
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getFeedNum(void);

/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_getFeed(ͬ���ӿ�)
	in:
		index:��ȡ�����index����0��(mrc_feedAndBuddy_getFeedNum-1)
	out:
		��ȡ����feed��utf16be���룬�����ȡ�������򷵻�NULL
*/
/***********************************************************************/
uint16* mrc_feedAndBuddy_getFeed(int32 index);

/***********************************************************************/
/*	
	������:mrc_feedAndBuddy_releaseResource(ͬ���ӿ�)
	˵��:���ͷŵ���Դ�������ɸ�feedAndBuddyģ�������û��洢feed�Լ�notice���ڴ�
	in:
		��
	out:
		MR_SUCCESS:�ͷųɹ�
*/
/***********************************************************************/
void mrc_feedAndBuddy_releaseResource(void);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////����PK(netPK)ģ��///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_NEKPK	//�ָ������ʵ������
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

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

/*PK���*/
typedef struct 
{
	uint32 winner;    /*Ӯ��SKYID*/ 
	uint32 loster;    /*���SKYID*/ 
}MR_PKResult;

/*PK������������Ϣ*/
typedef struct  
{
	int32 serverID;							//����������ID
	uint16 serverName[MAX_SERVERNAME_LEN+1];	//��������������
	int32 ServerUserTotal;					//���������������ɵ�����
	int32 serverStatus;						//������������״̬
	int32 femaleNum;						//����������Ŀǰ��Ů���������
}MR_PKServerInfo;

/*
	����PK�ص�����ԭ�ͣ�
		typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg,void* extraInfo);
	������
		resultCode�������룬MR_PKErrorCode_SUCCESS��ʾ�Ʋ�����سɹ�
					MR_PKErrorCode_OtherError��ʾ����ʧ��
		resultMsg������˷��صĴ�����ʾ�ַ���

*/
typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg);

/************************************/
/*mrc_netpk_commit_pkresult�Ļص�ԭ�� 
errCode:   ������                
respData:  �ӷ���˷��������ݣ�����ҪӦ���ͷţ�ĿǰΪutf16be���������     
len:       respData�ĳ���        */  
/************************************/
typedef void (*MR_Netpk_commit_PKResult_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

/****************************************/
/*mrc_netpk_upload_appdata�Ļص�ԭ�� 
errCode:   ������                
respData:  �ӷ���˷���������,����ҪӦ���ͷţ�ĿǰΪutf16be���������    
len:       respData�ĳ���				*/  
/****************************************/
typedef void (*MR_Netpk_upload_appdata_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

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

/****************************************************************************************************/
/*	mrc_netpk_openWap�Ļص�ԭ��																		
	result:																							
		MR_SUCCESS:������ɹ����к��˳�																
		MR_FAILED:�����δ�ܳɹ�����																*/
/****************************************************************************************************/
typedef void (*MR_Netpk_openWap_CB)(int32 result);


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


/****************************************************************************************
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
****************************************************************************************/    
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
	�ͷ���ռ���ڴ�ռ�
*/
int32 mrc_netpk_releaseRes(void);



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////���(entry)ģ��///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_ENTRY	//�ָ������ʵ������
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



/******************************************************************************************
 * ��ʼ���ص�
 * �� result != MR_OPENSKY_SUCCESS ��ʾ�������ʧ�ܣ�
 *     ʧ��ԭ����Ը���result��ֵ����ӦMR_OPENSKY_RETCODE��ѯ
 *     ��ʱ����޷�������Ӧ��Ӧ�����д���˽���Է���Ӧ���߼���
 * �� result == MR_OPENSKY_SUCCESS ��ʾ����Ѿ�������������
 *     Ӧ����Ҫ��ͣ�Լ��Ľ���ˢ�£��Եȴ������ص�����֪ͨ��
******************************************************************************************/
typedef void (*mrc_opensky_entry_init_cb)(int32 result);

/******************************************************************************************
 * �˳��ص�
 * �� result != MR_OPENSKY_SUCCESS && result != MRC_OPENSKY_ENTRY_START_TYPE_PARAM ʱ��ʾ����쳣�˳���
 *     �쳣ԭ����Ը���result��Ӧ��MR_OPENSKY_RETCODEֵ��ѯ
 * �� result == MR_OPENSKY_SUCCESS ʱ��ʾ��������˳�
 * �� result == MRC_OPENSKY_ENTRY_START_TYPE_PARAM ʱ��ʾ��������ظ�Ӧ��һ����������
 *     �����������������б�༭����д,���庬����Ӧ�þ���,Ӧ�������Ҫ���������,������memcpy
 * ����ص���������֪ͨOpenSKY����Ѿ��˳���Ӧ�ÿ��Լ���ִ�б�����߼���
******************************************************************************************/
typedef void (*mrc_opensky_entry_exit_cb)(int32 result, const char* data);

/******************************************************************************************
 * ��ʼ��������OSE
 * ���ӿڽ���ʼ��OpenSKY��ڲ��������������������ʾ���ؽ��档
 * Ӧ���ڵ��ô˽ӿ�ǰӦ����ͣ����ˢ�£�ֱ���ص��������ز��ܼ���Ӧ���߼���
 * �������룺 
 *     secretStr��SecretУ���룬��˹������õ�
 *     initCB����ʼ���ص�
 *     exitCB���˳��ص�
 * ���أ�
 *     �������
******************************************************************************************/
int32 mrc_opensky_entry_initEx(int8 isSupprotIM, char* secretStr, mrc_opensky_entry_init_cb initCB, mrc_opensky_entry_exit_cb exitCB);

#define mrc_opensky_entry_init(str,initCB,exitCB) mrc_opensky_entry_initEx(1,(char*)(str),(mrc_opensky_entry_init_cb)(initCB),(mrc_opensky_entry_exit_cb)(exitCB))

/******************************************************************************************
 * ����URL�ص�
 * �� result != MR_OPENSKY_SUCCESS ʱ��ʾ�������ʧ�ܣ���ʱ����޷�������Ӧ��Ӧ�����д���˽���Է���Ӧ���߼���
 * �� result == MR_OPENSKY_SUCCESS ʱ��ʾ����Ѿ������˳��������Ӧ�ÿ��Լ���ִ�б�����߼���
******************************************************************************************/
typedef void (*mrc_opensky_entry_loadURL_cb)(int32 result);

/******************************************************************************************
 * ��OSE�����������ָ����URL
 * ���ӿڽ�����OpenSKY��ڲ�����������������һ��URL��ҳ������������������ʾ���ؽ��档
 * Ӧ���ڵ��ô˽ӿ�ǰӦ����ͣ����ˢ�£�ֱ���ص��������ز��ܼ���Ӧ���߼���
 * ʹ�ñ��ӿ�ǰ������� mrc_opensky_entry_init
 * �������룺
 *     secretStr��SecretУ���룬��˹������õ�
 *     url����ҳ��URL
 *     cb������URL�ص�
 * ���أ�
 *     �������
 ******************************************************************************************/
int32 mrc_opensky_entry_loadURLEx(int8 isSupprotIM, char* secretStr, const char* url, mrc_opensky_entry_loadURL_cb cb);

#define mrc_opensky_entry_loadURL(str,url,cb) mrc_opensky_entry_loadURLEx(1,(char*)(str),(const char*)(url),(mrc_opensky_entry_loadURL_cb)(cb))

/******************************************************************************************
 * ��ѯIM������棩�Ƿ��Զ�����
 * ���أ�
 *     0 - ���Զ�����
 *     1 - �Զ�����
 * ���IM������Ϊ�Զ�������Ӧ����Ҫ���е���IM����ؽӿ�����ȡIM��״̬��������ز�����
 * ���ӿڽ��������ڲ�ѯ��OSE�������ʱ���û��Ƿ�������IM
******************************************************************************************/
int32 mrc_opensky_isIMStarted(void);


#ifdef __cplusplus
}
#endif

#endif // __MRC_OPENSKY_COMMON__
