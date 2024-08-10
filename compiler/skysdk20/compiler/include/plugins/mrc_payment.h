#ifndef SKY_PAYMENT_H
#define SKY_PAYMENT_H
#include "mrc_base.h"

#define PAYMENT
#define PAYMENT_TOM

#define PAY_MAXRESENDTIMES 3
 //���м���ͨ����
 typedef enum{
	ZHANGSHANGLINGTONG_2RMB=0,		// 0��ʾ2Ԫ������ͨ��ͨ�����ƶ�1Ԫ����ͨ2Ԫ
	PAYMENT_TEST,										// 1��ʾ����
	ZHIYUNSHIDAI_2RMB, 							// 2 ��ʾָ��ʱ����ͨ�����ƶ�1Ԫ����ͨ2Ԫ
	ZHANGSHANGLINGTONG_2RMB_MMS,	// 3��ʾ������ͨ��2��Ǯ����ͨ����ֻ���ƶ�����20080808��ʼ��10662000300350��15ʡ2Ԫ���Ŵ��档
	ZHIYUNSHIDAI_1RMB,							// 4ָ��ʱ��1RMB��ͨͨ����
	XINQINGHUDONG_1RMB,						// 5���黥��1RMB��ͨͨ����
	GUNSHIYIDONG_2RMB,							// 6��ʯ�ƶ�2RMB���ƶ�����ͨ��2Ԫ��
	CHUANGYIHEXIAN_2RMB,						// 7�������2RMB���ƶ�����ͨ��2Ԫ��
	TOM_2RMB,												// 8TOMͨ�����ƶ�����ͨ��2Ԫ��
	TOM_TEST2RMB,										// 9TOM����ͨ�����ƶ�����ͨ��2Ԫ��
	ZHIYUNSHIDAI_2RMB_JS,						// 10ָ��ʱ���ƶ�1Ԫ��ͨ2Ԫ���޽���ʡʹ�á�����ʡ�ݲ����á�
	ZHIYUNSHIDAI_2RMB_MMS,					// 11 ָ��ʱ������ͨ�����ƶ���ͨ��2Ԫ��
	ZHIYUNSHIDAI_1RMB_MT,						// 12ָ��ʱ���ƶ���MO��MT����ͨ��MO��MTͨ�����ƶ���ͨ��1Ԫ��	
	ZHANGSHANGLINGTONG_2RMB_318,	//13������ͨ2RMB��
	ZHIYUNSHIDAI_ZMCC10MT_UNICOM20,// 14ָ��ʱ���ƶ�1Ԫ�����õ�MT��������ָ���ͨ2Ԫ
	BEIWEI_CMCC10_UNICOM20,				// 15��γͨ�����ƶ�1Ԫ����ͨ2Ԫ��	
	ZHANGSHANGLINGTONG_20,					//16 ��0��ͨ����ȫһ�£���Ҫ��Ϊ�˺��Ķ���Ʒ���ݶ� ���ӵ�ͨ������ʾ2Ԫ������ͨ��ͨ�����ƶ�1Ԫ����ͨ2Ԫ
	PAY_CHN_ZY31,										//17 ָ��ʱ���ƶ�3Ԫ����ͨ������ͨʹ��1Ԫָ�
	PAY_CHN_XQ21,										//18 ���黥���ƶ�2Ԫͨ������ͨʹ��1Ԫָ�
	INDONESIA_2000RB,								//19ӡ��������2000¬��ͨ����
	PAY_CHN_TMGAME22,							//20TOM������Ϸͨ�����ţ��ƶ���ͨ��2Ԫ
	PAY_CHN_TMBOOK22,							//21TOM���ŵ�����ͨ�����ƶ���ͨ��2Ԫ
	PAY_CHN_ZS22,										//22������ͨ10662000300350ͨ�����ƶ�����ͨ��2Ԫ��
	PAY_CHN_CY12,										//23���պ���ͨ�����ƶ�1Ԫ����ͨ2Ԫ
	PAY_CHN_GS22_GUANGDONG,				//24��ʯ�㶫ͨ�����ƶ�2Դ����ͨ2Դ��
	PAY_CHN_ZX11,										//25�ƶ���ɳ��Ѷ1Ԫ����ָͨ��1Ԫ
	PAY_CHN_ZX22,										//26�ƶ���ɳ��Ѷ2Ԫ����ָͨ��2Ԫ
	PAY_CHN_ZYRTT22,								// 27ָ��ʱ������ͨ�����ƶ���ͨ��2Ԫ��
	PAY_CHN_ZSSIK22,									// 28������ͨ����ͨ�����ƶ���ͨ��2Ԫ����318ͨ����
	PAY_CHN_CYAKJ22,								// 29������Ҷ���ͨ�����ƶ���ͨ��2Ԫ��
	PAY_CHN_ZYTGT22,								// 30ָ��ʱ������ͨ�����ƶ���ͨ��2Ԫ��
	PAY_CHN_KZ11,									// 31����ͨ�����ƶ���ͨ��1Ԫ��
	PAY_CHN_KZ22,									// 32����ͨ�����ƶ���ͨ��2Ԫ��	
	PAY_CHN_ZSD12,									// 33����ͨ�����ƶ�1Ԫ����ͨ2Ԫ��
	PAY_CHN_ZYSIGN22,								// 34����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��
	PAY_CHN_ZSDJF22,								// 35����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��	
	PAY_CHN_YXA22,									// 36����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��
	PAY_CHN_HDGC11,									// 37����ͨ�����ƶ�1Ԫ����ͨ1Ԫ��	
	PAY_CHN_HY12,									// 38����ͨ�����ƶ�1Ԫ����ͨ2Ԫ��		
	PAY_CHN_ZYYBT12,								// 39����ͨ�����ƶ�1Ԫ����ͨ2Ԫ��				
	PAY_CHN_40GD22,							// 40����ͨ�����ƶ�2Ԫ����ͨ2Ԫ���㶫ר�á�
	PAY_CHN_41GD22,							// 41����ͨ�����ƶ�2Ԫ����ͨ2Ԫ���㶫ר�á�
	PAY_CHN_42GD22,							// 42����ͨ�����ƶ�2Ԫ����ͨ2Ԫ���㶫ר�á�			
	PAY_CHN_43ZY11,							// 43����ͨ�����ƶ�1Ԫ����ͨ1Ԫ��
	PAY_CHN_44HY22,							// 44����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��
	PAY_CHN_45HY22,							// 45����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��				
	PAY_CHN_46ZY22,							// 46����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��
	PAY_CHN_47TMG11,						// 47����ͨ�����ƶ�1Ԫ����ͨ1Ԫ��					
	PAY_CHN_48KZ22,							// 48����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��
	PAY_CHN_49KZ11,							// 49����ͨ�����ƶ�1Ԫ����ͨ1Ԫ��						
	PAY_CHN_50ZX22,							// 50����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��					
	PAY_CHN_51YZ22, 						// 51����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��
	PAY_CHN_52DT11, 						// 52����ͨ�����ƶ�1Ԫ����ͨ1Ԫ��						
	PAY_CHN_53CY22, 						// 53����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��							
	PAY_CHN_54GG22,							// 54����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��
	PAY_CHN_55NJ11,							// 55����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��	
	PAY_CHN_56BW22, 						// 56����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��	
	PAY_CHN_57GS22, 						// 57����ͨ�����ƶ�2Ԫ����ͨ2Ԫ��	
	PAY_CHN_58KZ32, 						// 58����ͨ�����ƶ�3Ԫ����ͨ2Ԫ��		
	PAY_CHN_59GZ52, 						// 59����ͨ�����ƶ�5Ԫ����ͨxԪ��		
	PAY_CHN_60NJ11, 						// 60����ͨ�����ƶ�1Ԫ����ͨxԪ��		
	PAY_CHN_61HD22, 						// 61����ͨ�����ƶ�2Ԫ����ͨxԪ��		
	PAY_CHN_62HD31, 						// 62����ͨ�����ƶ�3Ԫ����ͨxԪ��		

	PAY_CHN_63XZ1, 							// 58����ͨ�����ƶ�1Ԫ
	PAY_CHN_64XZ1, 							// 59����ͨ�����ƶ�1Ԫ
	PAY_CHN_65XL2, 							// 60����ͨ�����ƶ�2Ԫ
	PAY_CHN_66XL1, 							// 61����ͨ�����ƶ�1Ԫ
	PAY_CHN_67XL2, 							// 62����ͨ�����ƶ�2Ԫ

	PAY_CHN_68NJ1, 							// 68����ͨ�����ƶ�1Ԫ
	PAY_CHN_69ZY1, 							// 69����ͨ�����ƶ�1Ԫ
	PAY_CHN_70XZ2, 							// 70����ͨ�����ƶ�2Ԫ
	MAX_PAY_CHANNEL				
}PAY_CHANNEL;


typedef void (*mrc_PayCB)(int32 data);
typedef struct{
		uint16 isReg;//�Ƿ���ע��
		uint32 Option;//�Ƿ�����ǩ���ļ���1:���ɣ�0:�����ɣ�
		uint16 Channel;//�շ�ͨ��
		uint16 value;//�շѽ��
		int32 timeOut;//��ʱʱ�䣬��Ϊ0�򲻵ȴ������̷���
		int32 data;//�����ص������Ĳ�����
		mrc_PayCB fcb;
}PAY_STATUS_t;
typedef struct{
		uint32 Option;//�Ƿ�����ǩ���ļ���1:���ɣ�0:�����ɣ�
		uint16 Channel;//�շ�ͨ��
		uint16 value;//�շѽ��
		int32 timeOut;//��ʱʱ�䣬��Ϊ0�򲻵ȴ������̷���
		int32 data;//�����ص������Ĳ�����
		mrc_PayCB fcb;
}PAY_CALLBACK_t;

 typedef enum{
	CHECK_IS_REGISTER=0x00,
	CHECK_IS_FREE=0x01,
	CHECK_IS_MONTHUSER=0x02,//�û��ǰ����û�
	CHECK_IS_REREG=0x03		//�û���Ҫ��ע�ᡣ
}PAY_CHECK_OPTION;

#define MR_REREG 3


 typedef enum{
	PAY_MSG_MAGIC=0,	//���߶���
	PAY_MSG_REGISTER,	//ע�����
	PAY_MSG_SCORE,	//�շѻ��ֶ���
	PAY_MSG_UNIPAY,	//ͳһ��ֵ����
	PAY_MSG_DATA,
	PAY_MSG_REREG=7	//��ע�ᡣ
}PAY_MSG_OPTION;






typedef struct{
		int32 MCC;			//�շ���ʾ���շѽ�������ID
		int32 AppLanguageID;		//����˵�����������ܵ�����ID
}PAY_CTR_INFO_T;
/*
mrc_initChargeOverSea����˵��:
	��ʼ���Ʒ�ģ�飬���������Դ���������ȵ��á�
	�ڶ�����԰汾�У��ʷ�˵�����շѽ�����˵������Ϸ��
	�������ֶ�������EXCEL�����ɵ���Դ�ļ��С�
1���Ʒѳ�ʼ��ʱ��ȡ�üƷѿ�����Ϣ��
2�����ݼƷѿ�����Ϣ��ȡ�ø����շѵ���շѽ�
3�����ע���Ϊ0����Ӧ�õ���ע����ʾ��
4����������շ�Ϊ0�����Ե���������Ϣ���û����ȷ������ö��ŷ��ͺ�����
���ŷ��ͺ��������سɹ���

����:
PAY_CTR_INFO_T *tPayCtrlInfo;
	
���:
PAY_CTR_INFO_T *tPayCtrlInfo;


ע��:
		�����ڷ���ģʽ������������ʱ���Ʒ�ģ�齫��ʼ��ʧ�ܡ�
		
����ֵ:
MR_SUCCESS:��ʼ���ɹ���
MR_FAILED:��ʼ��ʧ�ܡ�
*/
int32 mrc_initChargeOverSea(PAY_CTR_INFO_T *tPayCtrlInfo);

/*
mrc_checkChargeExOverSea����˵��:
	mrc_checkChargeExOverSea(CHECK_IS_REGISTER)==MR_SUCCESS���û���ע��Ӧ�ã�
	Ŀǰֻ֧��CHECK_IS_REGISTER���ԡ�
����:
	�ޡ�
	
���:
MR_SUCCESS:����ƥ�䣻
MR_FAILED:����ʧ�ܡ�
*/
int32 mrc_checkChargeExOverSea(int32 Option); 

/*
checkCharge����˵��:
	���Ӧ������Ŀ¼��appname��Ŀ¼���޳ɹ����ͼƷѶ��ŵ�ǩ���ļ���
���У��򷵻سɹ������򷵻�ʧ�ܡ�	

����:
	�ޡ�
	
���:
MR_REREG:��Ҫ��ע�ᡣ
MR_FAILED:û���ҵ�ָ����ǩ���ļ�����ǩ���ļ����Ǳ��ֻ��ġ�
MR_SUCCESS:�ҵ��ɹ������˼ƷѶ��ŵĺϷ�ǩ���ļ���
*/

int32 mrc_checkCharge(void); 

/*
checkChargeEx����˵��:
	mrc_checkChargeEx(CHECK_IS_REGISTER)==MR_SUCCESS���û���ע��Ӧ�ã�
	mrc_checkChargeEx(CHECK_IS_FREE)==MR_SUCCESS����Ϸ�ڸ���Ӫ����ѣ�	
	mrc_checkChargeEx(CHECK_IS_MONTHUSER)==MR_SUCCESS���û��ǰ����û���	

����:
	�ޡ�
	
���:
MR_SUCCESS:����ƥ�䣻
MR_FAILED:����ʧ�ܡ�
*/
int32 mrc_checkChargeEx(int32 Option); 
/*
mrc_RegisterSid����˵��:
����ǰ��Ϸ����ǰ�û�����ע���ļ���
(1)�����û���ע�ᣬ�����κβ�����
(2)����ע���ļ������ڣ���������ע���ļ���
(3)����ע���ļ��Ѵ��ڣ����û���IMSI��ӵ�ע���ļ��У�

����:
	�ޡ�
	
���:
MR_SUCCESS:ע��ɹ���
MR_FAILED:ע��ʧ�ܡ�
*/
int32 mrc_RegisterSid(void); 

/*
��������:	mrc_ChargeExOverSea
����˵��:	�ṩ���ص������������չ�ĺ�����շѹ��ܡ�
	�����һ��MO���ŷ��سɹ���״̬����󣬻ص���������ִ�С�

����:
Option:	ֵΪ0ʱ����ʾ��д��¼�����ڹ�����ߵȶ�η��͵ĳ��ϣ�
				ֵΪ1ʱ����ʾҪд��¼�����ڲ�Ʒ����ע��ȳ��ϡ������
				���Ѹ��ѵĴ��̼�¼���򱾺������̷��سɹ���
				
Channel:	�շ���������˹�����塢���䡣
value:		�շѽ���λΪͨ���ļƷѵ�λ�����
				ȡֵ��ΧΪ10����ͨ��SP���ŵĶ�Ӧֵ�����磬1.5��ͨ�����ֵΪ150��
uint16 mcc:
				��Excel����ȡ�õ�MCCֵ����Ҫ������У�飬�������
				
uint16 ChargePoint:
				�Ʒѵ��š�

int32 data:�ص������Ĳ�����
				
mrc_PayCB f:�ص���������f=NULLʱ���ص������������ԣ���ʱ��������Ϊ
	��mrc_ChargeEx��ȫһ�¡�
	
int32 timeOut:
	��ʱ������ʱ���ȴ��ĺ������������ʱʱ�仹��û���յ����е�״̬���棬
	������ʱʱ�䵽��ʱ���ص��������뱻���á��ڴ���Ӧ�� ��data������ָ����δ
	�յ��Ķ�������������ֵΪ0�������ж����ύЭ��ջ�ɹ������̷��أ�����ֵ
	Ϊ�����������õȴ���ֱ���յ��㹻�Ķ��ŷ��ͳɹ�״̬���档
	
���:
MR_SUCCESS:��һ��MO���ųɹ��ύЭ��ջ���������Ž��첽MO��
MR_FAILED:�շ�ʧ�ܡ�
*/
int32 mrc_ChargeExOverSea(uint32 Option,uint16 Channel,uint16 value,uint16 mcc,uint16 ChargePoint,int32 data,mrc_PayCB f,int32 timeOut);

/*
releaseCharge����˵��:
	�ͷżƷ�ģ�������Դ��
	
����:
	�ޡ�

���:
MR_SUCCESS:�ɹ��ͷ���Դ��
MR_FAILED:�ͷ�ʧ�ܡ�
*/
int32 mrc_releaseChargeOverSea(void);



/*
mrc_initDefaultChn����˵��:
	����SDK�Ʒ�ģ���Ĭ��������SP�Ʒ�ͨ����
	SDK�Ʒ�ģ�����ǳ�������ʹ����ͨ�������շѣ�
	���ͨ�����۸����շѽ�����ʹ�ø�ͨ�������շѣ�
	Ŀǰ��ʱ����֧�ֽ���ͨ���շ����ת����ͨ����ȡ�ķ�ʽ��
	�պ������֧�֡�
��SDKv1.0.2.42�Լ�֮ǰ�İ汾�У���֧�ָ�ͨ������ͨ�����ƶ���ͨҲ����ͬ�ؽ����л���
��SDKv1.0.2.43�汾�У�ֻ����ͨ��֧��ͨ���л�����ͨ����֧���л���
��SDKv1.0.3.0��SDKv1.0.3.15�汾�У�����ͨ����֧���ƶ�����ͨ�ֿ����á�
��SDKv1.0.3.16֮��İ汾�У�����ͨ����֧���л���

����:
uint16 priChn:	
	��ͨ��ID��
	���ֽ�Ϊ��ͨ��ͨ��ID�����ֽ�Ϊ�ƶ���ͨ��ID��
	�����ֽ�Ϊ0ʱ����ͨ��ͨ��ID���ƶ���ͨ��ID��ͬ��

uint16 slvChn:	
	��ͨ��ID��
	���ֽ�Ϊ��ͨ��ͨ��ID�����ֽ�Ϊ�ƶ���ͨ��ID��
	�����ֽ�Ϊ0ʱ����ͨ��ͨ��ID���ƶ���ͨ��ID��ͬ��	
	
���:
�ޡ�

ע��:
	�ڼƷѳ�ʼ��֮�󣬵��÷����շѶ��ŵĽӿ�֮ǰ���������mrc_initDefaultChn��
����ֵ:
MR_SUCCESS:���óɹ���
MR_FAILED:����ʧ�ܡ�
*/
int32 mrc_initDefaultChn(uint32 priChn,uint32 slvChn);
/*
mrc_ConfirmSms����˵��:	�û����ж���ȷ�ϡ�
	��mrc_ChargeExConfirm���ú����ͨ����Ҫ����ȷ�ϣ����յ�ȷ������
���ź󣬻ص�����cbQueConfirm�������ã�Ȼ��Ӧ�ý���ʾ�û��Ƿ�ȷ��
������Ϊ��Ӧ�þ���Ҫ����mrc_ConfirmSms��������֪�û�̬�ȡ�

(1)������û�ѡ������Ʒ�ģ�齫�ܿ�͵���mrc_PayCB f�����ں�̨�첽��
��ɶ���ȷ���Լ������������ŵ��Զ�����ȷ�ϣ�
(2)������û�ѡ�񲻹�����Ʒ�ģ�齫��ն��ŷ��Ͷ��У��ص�����
mrc_PayCB f�����ᱻִ�С�
	
����:
int32 isBuy:
	0:�û���������ȷ�ϣ�
	1:�û�ѡ�����ȷ�ϡ�

*/
void mrc_ConfirmSms(int32 isBuy);

/*
��������:	mrc_scoreConfirm
����˵��:	֧�ֶ���ȷ�Ϻͻ����ϴ��Ķ��żƷѽӿڡ�
1���������ʹ�õ�ͨ������Ҫ����ȷ�ϣ��򱾺�������Ϊ��mrc_ChargeExOverseaһ�£�
2���������ʹ�õ�ͨ����Ҫ����ȷ�ϣ������յ���Ӫ�̻ظ��ĵ�һ���Ļظ������
���ûص�����cbQueConfirm��

����:
Option:	ֵΪ0ʱ����ʾ��д��¼�����ڹ�����ߵȶ�η��͵ĳ��ϣ�
				ֵΪ1ʱ����ʾҪд��¼�����ڲ�Ʒ����ע��ȳ��ϡ������
				���Ѹ��ѵĴ��̼�¼���򱾺������̷��سɹ���
				
value:		�շѽ���λΪͨ���ļƷѵ�λ�����
				ȡֵ��ΧΪ10����ͨ��SP���ŵĶ�Ӧֵ�����磬1.5��ͨ�����ֵΪ150��
				
mrc_PayCB f:���ŷ�����ϵĻص���������f=NULLʱ���ص������������ԣ���ʱ��������Ϊ
	��mrc_ChargeEx��ȫһ�¡�
	
int32 data:
	��f����������ʱ�Ļص�����������
	
int32 timeOut:
	��ʱ������ʱ���ȴ��ĺ������������ʱʱ�仹��û���յ����е�״̬���棬
	������ʱʱ�䵽��ʱ���ص��������뱻���á��ڴ���Ӧ�� ��data������ָ����δ
	�յ��Ķ�������������ֵΪ0�������ж����ύЭ��ջ�ɹ������̷��أ�����ֵ
	Ϊ�����������õȴ���ֱ���յ��㹻�Ķ��ŷ��ͳɹ�״̬���档

mrc_PayCB 	cbQueConfirm:	����ȷ�϶���֪ͨ������
	����ͨ����Ҫ����ȷ��ʱ���յ�����ȷ�����Ѻ󣬸ú�������ִ�С�

uint16 mcc:	
	��Excel����ȡ�õ�MCCֵ����Ҫ������У�飬�������
	
int32 score:
	����Ҫ�ϴ����֣�����ñ�������
���:
MR_SUCCESS:��һ��MO���ųɹ��ύЭ��ջ���������Ž��첽MO��
MR_FAILED:�շ�ʧ�ܡ�
*/

int32 mrc_scoreConfirm(uint32 Option,uint16 value,mrc_PayCB f,int32 data,int32 timeOut,mrc_PayCB cbQueConfirm,uint16 mcc,int32 score);

/*
��������:	mrc_payMoney
����˵��:	����¼�Ʒѵ㹦�ܵĸ��ѽӿڡ������ϴ�ʱ�����ܼ�¼�Ʒѵ㡣

����:
Option:	ֵΪ0ʱ����ʾ��д��¼�����ڹ�����ߵȶ�η��͵ĳ��ϣ�
				ֵΪ1ʱ����ʾҪд��¼�����ڲ�Ʒ����ע��ȳ��ϡ������
				���Ѹ��ѵĴ��̼�¼���򱾺������̷��سɹ���
				
value:		�շѽ���λΪͨ���ļƷѵ�λ�����
				ȡֵ��ΧΪ10����ͨ��SP���ŵĶ�Ӧֵ�����磬1.5��ͨ�����ֵΪ150��
				
mrc_PayCB f:���ŷ�����ϵĻص���������f=NULLʱ���ص������������ԣ���ʱ��������Ϊ
	��mrc_ChargeEx��ȫһ�¡�
	
int32 data:
	��f����������ʱ�Ļص�����������
	
int32 timeOut:
	��ʱ������ʱ���ȴ��ĺ������������ʱʱ�仹��û���յ����е�״̬���棬
	������ʱʱ�䵽��ʱ���ص��������뱻���á��ڴ���Ӧ�� ��data������ָ����δ
	�յ��Ķ�������������ֵΪ0�������ж����ύЭ��ջ�ɹ������̷��أ�����ֵ
	Ϊ�����������õȴ���ֱ���յ��㹻�Ķ��ŷ��ͳɹ�״̬���档

mrc_PayCB 	cbQueConfirm:	����ȷ�϶���֪ͨ������
	����ͨ����Ҫ����ȷ��ʱ���յ�����ȷ�����Ѻ󣬸ú�������ִ�С�

uint16 mcc:	
	��Excel����ȡ�õ�MCCֵ����Ҫ������У�飬�������

int32 score:
	����Ҫ�ϴ����֣�����ñ�������
	
uint8 chargePont:
	�Ʒѵ��ţ�ȡֵ��ΧΪ0��255��Ĭ��ֵΪ0.

���:
MR_SUCCESS:�ύЭ��ջ�ɹ���
MR_FAILED:�ύЭ��ջʧ�ܣ�û���κμƷѶ��ŷ��͡�
*/
int32 mrc_payMoney(uint32 Option,uint16 value,mrc_PayCB cbFinishPay,int32 data,int32 timeOut,mrc_PayCB cbConfirmAgain,uint16 mcc,int32 score,uint8 chargePont);

//����WAP�������ѯ���֡�
void mrc_LookScore(void);


/*
��������:	mrc_getPolicy
����˵��:	��ȡSDK���շѲ��Եȼ���

����:
int32 inputPolicy:	Ӧ�õ�Ĭ���շѲ��Եȼ���������0����������ʵ���ڵĵĵȼ���
int32 lockFlag:	Ӧ���Ƿ��������ԡ������ò����ļ�Ĭ�����ã�
0��ʾ��������1��ʾ����������������Ӧ�ã����صĲ����ļ�
�����о�APPID��������ǿ�ƽ�����־��

���:
	�ޡ�

����ֵ:
	Ӧ�õ��շѲ��ԡ�

*/

int32 mrc_getPolicy(int32 inputPolicy,int32 lockFlag);

/*
��������:	mrc_setPolicy
����˵��:	����Ӧ�õ��շѲ��Եȼ���

����:
int32 finalPolicy:	Ӧ�����ղ��õ��շѲ��Եȼ���������0����������ʵ���ڵĵĵȼ���
���:
	�ޡ�

����ֵ:
	���سɹ���˵�����õĲ�����Ч��
	������ʧ�ܣ���Ӧ��Ҫ��飬���Ӧ��Ĭ�ϲ����Ǹ��շѲ��ԣ���Ӧ��������������ԡ�

*/

int32 mrc_setPolicy(int32 finalPolicy);

typedef struct{
	int32 smsNum;		//��Ҫ���͵Ķ�������
	int32 price;		//���ż۸�λ������ҽ�
	int32 spStrEnable;	//ֵΪ1ʱ��ʾ��ʾ����Ч��
	int32 spStrLen;		//SP������ʾ���ֽ�����	
	char  *spStrBuf;	//SP������ʾ����ջ�����ָ�룬����unicode���룬���ڴ����SDK�Ʒ�ģ�������ֹӦ���ͷŸ��ڴ��							
}PAY_CTR_SPINFO_T;
/*
��������:	mrc_getSPInfo
����˵��:	��ȡ������Ϣ

����:
uint32 Option:		��������
uint16 value:		�շ��ܽ���λ������ҽǡ�
uint16 mcc:		�й���½��MCCĬ����дΪ460��
���:

PAY_CTR_SPINFO_T *spInfo	:���������سɹ�����û����������塣	

����ֵ:
	�����سɹ�����spInfo�����壻���Ƿ��سɹ���Ȼ���ܳ���
	mrc_getSPInfo->spStrBufΪNULLd���������Ҫ����spInfo[0].spStrEnable�ж���
	������ʧ�ܣ���spInfo�����塣

*/
int32 mrc_getSPInfo(uint32 Option,uint16 value,uint16 mcc,PAY_CTR_SPINFO_T *spInfo);


/*
��������:	���ѻص�����mrc_uniPayCB
����˵��:	����������Ϻ�Ļص�����

����:
int32 result:	
		MR_FAILED:	�û����ȡ��
		MR_SUCCESS:	�û����ȷ�������Ҹ��ѳɹ�
		MR_INGNORE:	�û����ȷ��������ʵ�ʸ��ѳ���ʧ��

int32 cbData:	Ӧ�ø���ʱͨ��mrc_uniPayUI�������ѻص������Ĳ�����
*/
typedef void (*mrc_uniPayCB)(int32 result,int32	cbData);
typedef struct{
uint32	mcc;				//����MCC
uint32	Option;				//�������ͣ�����һ����ע������PAY_MSG_REREG�����ע��ʱ���һ���Ƿ�ע���״̬��
int32	value;				//�շѽ����й���½���շѽ��ĵ�λΪ����ҽ�
uint8	chargePoint;		//�Ʒѵ��š����ж���շѵ�ʱ��ÿ���շѵ㸳�費ͬ�ļƷѵ��ţ��Ա������Ӫ���ݷ�����
char	*uincodeTitle;		//����ҳ��ı��⣻������Ӫ�����ߺ�ʵ�������Ҫ��˹��������ӡ��޸ġ�ɾ�����ֶ����ݡ�
char	*unicodePayText;	//����ҳ������ġ�������Ӫ�����ߺ�ʵ�������Ҫ��˹��������ӡ��޸ġ�ɾ�����ֶ����ݡ�
int32	dataLen;			//��ҪЯ����������Ϣ�ֽ���
char	*data;				//��ҪЯ��������ָ��
int32	timeOut;			//�û�ȷ�ϸ��Ѻ󡢷��Ͷ��ŵĵȺ�ʱʱ�䣬��λ�Ǻ�����������F420�ȷǹ淶��ֲ�汾�������Ʒ�ģ�齫�Զ��޸�Ϊ30000ms��
int32	cbData;				//�����ص������Ĳ�����
mrc_uniPayCB uniPayCallBack;	//���ѻص��������ص�������result����С��0��ʾ�û�ѡ��ȡ�����ѣ����ڵ���0��ʾ�û�������ȷ�ϡ�
}PAY_CTR_UNIPAY_T;

/*
��������:	mrc_uniPay
����˵��:	˹���ṩ��ͳһ��֧���������̽��档
	Ŀǰ��ʱ��֧���ֿ⡣

����:
	PAY_CTR_UNIPAY_T *ctrlUnipay:	������صĿ�����Ϣ��
���:
	�ޣ���������ͨ���ص������첽���ء�

����ֵ:
	MR_SUCCESS:	�����Ϸ������壬������˹���ṩ��UI���ѽ��档
	MR_FAILED:�����Ƿ���������һ�θ��ѻ��ڴ����У��������˹��UI���ѽ��棬�ص��������ᱻִ�С�

*/
void mrc_uniPayUI(PAY_CTR_UNIPAY_T *ctrlUnipay);


/*
��������:	mrc_uniPayUI_active
����˵��:	�ж�Ӧ���Ƿ���uniPayUI�ĸ��ѽ����С�

����:
	�ޡ�
���:
	�ޡ�
����ֵ:
	������0����ʾ���ڸ��ѽ����У�
	�����ط�0ֵ����ʾ�ڸ��ѽ����С�
*/
int32 mrc_uniPayUI_active(void);

enum GAME_NETWORK_STYLE
{
	GAME_NETWORK_NONE,			// ����Ҫ��������
	GAME_NETWORK_CONNECT		// ��Ҫ��������
};

typedef struct{
	int32 networkStyle;			// ������������ GAME_NETWORK_STYLE
	int32 gameRegPrice;			// Gameע��ʱ��Ҫ�Ľ���λ:��
	int32 reserve1;				// ����1
	int32 reserve2;				// ����2
	int32 reserve3;				// ����3
	int32 reserve4;				// ����4
}UNIPAY_SETPARAM_T;
/*
��������:	mrc_uniPaySetParam
����˵��:	����mrc_uniPayUI����ʱԤ�������
ע:
	��Ҫ��mrc_initChargeOverSea֮ǰ����
����:
	UNIPAY_SETPARAM_T��
���:
	�ޡ�
����ֵ:
*/
void mrc_uniPaySetParam(UNIPAY_SETPARAM_T *setParam);
#endif
