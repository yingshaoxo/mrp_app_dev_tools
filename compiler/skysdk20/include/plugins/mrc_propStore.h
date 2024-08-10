#ifndef __MRC_PROPSTORE_H__
#define __MRC_PROPSTORE_H__

#include "mrc_base.h"
#include "mrc_cloudStorage.h"

#define WAP_PAGE_INDEX		1
#define WAP_PAGE_ADD_PACK	2

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
/*
	��������mrc_propStore_queryUserPack
	���ܣ�  ��ѯ�û����������ڷ��ص�ǰ��Ծ�û������ڵ����е���
	���룺
			columnName ���÷��ص���������ѡ�ֶΣ�specid,descid,remain,period,num,attr �м��Զ��Ÿ���
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
*/
int32 mrc_propStore_queryUserPack(char* columnName, MR_PROPSTORE_CB queryCb);

/*
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
*/
int32 mrc_propStore_consumeGoods(int32 id, int32 num, MR_PROPSTORE_CB queryCb);

/*
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
*/
int32 mrc_propStore_deleteGoods(int32* idArray, int32 itemCount, MR_PROPSTORE_DEL_CB queryCb);

/*
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
*/
int32 mrc_propStore_getGoods(int32 getKey, char* columnName, MR_PROPSTORE_CB queryCb, int32 isGetPack);

/*
	��������mrc_propStore_enterWap
	���ܣ����������������wapҳ�棬���߲鿴�û���������Ϣ������������ߵ���
	���룺
			page	��Ҫ�򿪵�ҳ��ȡֵ���£�
					ȡֵΪ��WAP_PAGE_INDEX		���û����߱�����ҳ
					ȡֵΪ��WAP_PAGE_ADD_PACK	�򿪱�������ҳ��
			wapCb	����������Ļص�����
					�ص��д��ص�resultCode����ȡֵΪ��
							MR_SUCCESS			����������ɹ�
							MR_FAILED			����ʧ��
*/
int32 mrc_propStore_enterWap(int32 page, MR_PROPSTORE_WAP_CB wapCb);

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
 * order:���Ա��
 * value:����ֵ
 * ���ϲ�����Ӧ�������˹�������, �����ֻ��һ��ת��
 * ע�⣺���������ڵ��� mrc_propStore_uploadItem �󽫱���գ��������ϴ�����Ҫ���µ��ñ��ӿ�
 **/
int32 mrc_propStore_uploadItem_setItemProperty(int32 order, int32 value);
/**
 * �ϴ����ߣ�ʹ�ñ���Ʒǰ���ȵ�¼ͨ��֤
 * ver:���߰汾��
 * cpGoodsId:����ID(��CP���ж���)�����ߵ�������μ� mrc_propStore_uploadItem_setItemProperty �ӿ�
 * num:��������
 * cb:�ص�����������ΪNULL
 * ע�⣺�ڵ��ñ�����ǰ��Ӧ��Ӧ����ͣ�Լ����߼����ر��Ǳ���Ҫֹͣ����ˢ�£���ֱ��cb�ص�֪ͨ������ܼ���Ӧ���߼�
 **/
int32 mrc_propStore_uploadItem(int32 ver, int32 cpGoodsId, int32 num, mrc_propStore_uploadItem_cb cb);

/********************************************************
				�����ӿڣ��������Ͳ���
********************************************************/

/*
	��������mrc_propStore_resolve
	���ܣ�  ���ݽ�����
	���룺
			row		��������������
			column	��������������
			pData	�����������
			len		������õ����ݵĳ��� 0��˵�����ݿ��ж�ӦΪNULL
	�����
			MR_SUCCESS �ɹ�
			MR_FAILED  ʧ��
*/
int32 mrc_propStore_resolve(int32 row, int32 column, void* pData, int32* len);

/********************************************************
				�ͷ���Դ�ӿ�
********************************************************/
void mrc_propStore_release(void);

#endif