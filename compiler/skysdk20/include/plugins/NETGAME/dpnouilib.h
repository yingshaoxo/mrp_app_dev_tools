/*******************************************************************************
 * CopyRight (c) 2005-2010 SKY-MOBI Ltd. All rights reserved.
 * Filename:  dpnouilib.h
 * Creator:   lynn.mo
 * Version:   V1001
 * Date:      2011/04/11
 * Description:			
*******************************************************************************/
#ifndef __DPNOUILIB_H__
#define __DPNOUILIB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct  
{
	char*   p_merchantId;			//�̻���			
	char*	p_merchantPasswd;		//�̻�������
	uint16	instanceID;				//cp�ķ����ģ���
	char*	p_orderID;				//������
	char*	p_productId;			//��Ʒ�� 
	char*	p_productName;			//��Ʒ���� 
	/*
	������Ϣ��cp�ṩ
	p_merchantId					cp��Ӧ������ǰ��˹������
	p_merchantPasswd				cp��Ӧ������ǰ��˹������
	p_orderID						��cp����(�ַ�������ֻ��Ϊ����)
	p_productId						��cp����(�ַ�������ֻ��Ϊ����) 
	p_productName					��cp����(utf16-be����)
	*/

	char*	p_cardNum;				//����
	char*	p_cardPasswd;			//����
	uint16	cardType;				/*
									��ֵ���
									10		�����ƶ������г�ֵ��
									15		������ͨ��ֵ��
									21		������ų�ֵ��
									11		����ʢ����Ϸ��ֵ��
									12		������;��Ϸ��ֵ��
									13		��������Ϸ��ֵ��
									14		����QQ��Ϸ��ֵ��
									34		����ð�ݿ�
									*/
	uint16	amount;					//��ֵ���,�������ݣ��Է�Ϊ��λ
	//������Ϣ��Ҫ�û���д

}dpnouiChargeInfo_T;

/*
*����֧���ӿ�
*ʹ��˵��:	
���룺
		pDpnouiChargeInfo	: ָ��dpnouiChargeInfo_T�ṹ���ָ�룬���Ĵ��������پ���cp���
		p1~p5				:�����ֶΣ���ʱ���ã�����������ֵ

����ֵ��
		MR_FAILED ʧ��
		MR_SUCCESS �ɹ�
ʹ��ʾ����
#include "dpnouiLib.h"
void  testFunc(void)
{
	int32 ret = MR_FAILED;
	dpnouiChargeInfo_T *pdpnouiChargeInfo = NULL;

	pdpnouiChargeInfo = (dpnouiChargeInfo_T*)malloc(sizeof(dpnouiChargeInfo_T));
	pdpnouiChargeInfo->p_merchantId ="10180" ;
	pdpnouiChargeInfo->p_merchantPasswd ="kodfdsafdasfang123";
	pdpnouiChargeInfo->instanceID = 0x64d0;
	pdpnouiChargeInfo->p_orderID = "123";
	pdpnouiChargeInfo->p_productId = "1";			
	pdpnouiChargeInfo->p_productName = "ddd";
	pdpnouiChargeInfo->cardType = 10;
	pdpnouiChargeInfo->p_cardNum = "12123214213412";
	pdpnouiChargeInfo->p_cardPasswd = "23421342134";
	pdpnouiChargeInfo->amount = 20;
	
	ret = mrc_dpnoui_sendChargeInfo(pdpnouiChargeInfo, 0, 0, 0, 0, 0);
	free(pdpnouiChargeInfo);
	pdpnouiChargeInfo = 0;
			
	return;
}			
*/
int32 mrc_dpnoui_sendChargeInfo(int32 pDpnouiChargeInfo, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DPNOUILIB_H__ */

