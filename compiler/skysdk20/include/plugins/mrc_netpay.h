#ifndef __MRC_NETPAY_H__
#define __MRC_NETPAY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
	SMS_APP_PRE_NETWORK_NONE = 0,		/*��Ԥ����������*/
	SMS_APP_PRE_NETWORK_CMWAP = 1,		/*Ԥ������CMWAP */
	SMS_APP_PRE_NETWORK_CMNET = 3		/*Ԥ������CMNET */
}SMS_APP_PRE_NETWORK;

typedef enum pay_type
{
	PAY_MSG_TYPE_REGISTER = 0,
	PAY_MSG_TYPE_MAGIC = 1,
	PAY_MSG_TYPE_REREG = 5
}SMS_PAY_TYPE;

typedef enum pay_result
{
	PAY_SUCCESS = 0,
	PAY_FAILED = -1,
	PAY_REREG = -2,
	PAY_ERROR = -101
}SMS_PAY_RESULT;

/*
����ģ���ʼ������������ʱ����һ��
�������MR_FAILED�����ʾĿǰ�޷�����
����ֵ��
MR_SUCCESS��0 ��ʼ���ɹ�
MR_FAILED��-1 ��ʼ��ʧ��
*/
int32 mrc_netPayInit(void);

/*
int32 result: 
MR_SUCCESS��0	���ѳɹ�
MR_FAILED��	-1	����ʧ��
MR_NOTFULL��-2	���Ѳ��ֳɹ�
MR_BADERRPR��-101	���ش����ڻص��в����ٽ��и�����صĲ���
*/
#define MR_NOTFULL	-2
#define MR_BADERRPR	-101
typedef int32 (*mrc_netPayCB)(int32 result, int32 retlen, char *retdata);

/*
tag			�ֶ���			�ֶ�����		˵�� 
10100		priceTotal		int32			�����ܽ���λ�� 
10101		pricePaid		int32			�Ѹ��ѽ���λ�� 
10102		priceServ		int32			������֧�ֽ���λ�� 
												����˵����priceTotal=500����Ŀǰֻ��2Ԫͨ������priceServ=400�������������ż���ɹ�
10200		payType			int32			��������
											  0 = ע��
											  1 = ����
											  3 = ����
											  4 = ��ֵ
											  5 = ��ע��

											  10 = ���£�BAOYUE��
											  11 = ���Σ�BAOCI��
											  12 = �㲥��DIANBO��
										˵����0 = ע�ᣬ�ж��ŷ�����Ϣ���棬֧�ֲ�������������ϴ�δ��������ţ�����ŷ������
										      1 = ���ߣ�û�ж��ŷ�����Ϣ���棬ÿ�ζ��Ǵ��»�ȡ��ͨ���и���
											  5 = ��ע�ᣬʹ�ñ���Ķ���ͨ����Ϣ����
10201		payPoint		int32			�Ʒѵ���(��Ʒ���)
10202		payDesc			char			�Ʒѵ�˵��(��Ʒ����)
											  a-z,0-9
											  ASCII���룬��Ҫ����10���ֽ� 
10203		hintTitle		char			�Ʒ���ʾҳ���� (unicode����)
10204		hintText		char			�Ʒ���ʾҳ���� (unicode����)
10205		userdata		bin				�Ʒ��ļ��û��Զ�����Ϣ
10206		paytime			char			����ʱ�䣬��ʽΪYYYYMMDDHHMiSS��
											  ASCII���룬���磺
											  20100102193015
10207		netInit			int32			������������
											  0 = û������
											  1 = �Ѿ�����(CMWAP)
											  2 = �Ѿ�����(CMNET��Ӧ����Ҫ��GPRS)
											  3 = �Ѿ�����(CMNET��Ӧ�ò���Ҫ��GPRS)
										˵����1��Ӧ�ñ���û���������磬					netInit=0
											  2��Ӧ���Ѿ�������·�Ҳ��ŷ�ʽΪCMWAP		netInit=1
											  3��Ӧ���Ѿ�������·�Ҳ��ŷ�ʽΪCMNET		netInit=2
											     (���������Ӧ�ø���ǰ��Ͽ����磬���ѻص����ٻָ�����)
											  4��Ӧ���Ѿ�������·�Ҳ��ŷ�ʽΪCMNET		netInit=3
											     (���������Ӧ�ø���ǰ����Ҫ�Ͽ�����)
10208		appId			int32			�Ʒѵ�Ӧ��ID
10209		appName			char			�Ʒѵ�Ӧ�ö�����
10210		appVer			int32			�Ʒѵ�Ӧ�ð汾��
10211		transcationId	char			������
10212		skyid			char			�û���skyid
10213		productnum		int32			�Ʒѵ��Ӧ������(��Ʒ����) >=0
10214		productdate		char			�Ʒѵ��Ӧ����Ʒ�Ĺ���ʱ�䣬��ʽΪYYYYMMDD
10215		businessId		int32			ҵ���ţ�
											  100 = �Ķ�
											  200 = ����
											  300 = ������Ϸ
											  400 = ����
											  500 = ����
											  600 = SNS
10216		businesName		char			ҵ�����ƣ�ASCII����
10217		portalId		int32			��ڱ�ţ�ֻ��WAP����ʱ��д(ֵΪ100)
10218		portalName		char			������ƣ�ASCII���룬ֻ��WAP����ʱ��д(ֵΪWAP)
10219		priceUint		int32			���ѵ�λ��
											  10 �����
											  11 ��Ԫ
											  12 KB
10220		channelId		int32			�������
											  100 = OTA
											  200 = T��Ԥ��
											  300 = �̻�
10221		channelName		char			��������
10300		cbdata			bin				�ص������û��Զ�����Ϣ 
10400		createOldSid	int32			ʹ����Ӧ�ýӿ��Ƿ�������ϷSID�ļ�
											0	������
											1	����
10401		deleteOldSid	int32			ɾ��Ӧ�ø�����Ϣʱ���Ƿ�ɾ����ϷSID�ļ�
											0	��ɾ��
											1	ɾ��
*/

#define TAG_PRICETOTAL		10100
#define TAG_PRICEPAID		10101
#define TAG_PRICESERV		10102

#define TAG_PAYTYPE			10200
#define TAG_PAYPOINT		10201
#define TAG_PAYDESC			10202
#define TAG_PAYHINT_TITLE	10203
#define TAG_PAYHINT_TEXT	10204
#define TAG_PAYUSERDATA		10205
#define TAG_PAYTIME			10206
#define TAG_PAYNETINIT		10207
#define TAG_APPID			10208
#define TAG_APPNAME			10209
#define TAG_APPVER			10210

#define TAG_CBDATA			10300
#define TAG_CREATEOLDSID	10400
#define TAG_DELETEOLDSID	10401

/*
���𸶷ѣ���������Ҫ������tag:
10100,10200,10201,10202,10203,10204,10205,10206,10207,10300
�ص��а�����tag:
10100,10101,10102,10206,10300
*/
int32 mrc_netPayUI(int32 varlen, char *vardata, mrc_netPayCB cb);

/*
��鸶�ѣ���������Ҫ������tag:
10201,10300
�ص��а�����tag:
10100,10101,10102,10205,10206,10300
*/
int32 mrc_netCheckCharge(int32 varlen, char *vardata, mrc_netPayCB cb);

/*
ɾ�����ѣ���������Ҫ������tag:
10201
�ص��а�����tag:

˵�������ϣ������ע�ᣬ���ֲ�ϣ���ǲ����������ȵ��ô˽ӿڣ�ɾ������ͨ���ļ�

cb��resultֵ
MR_SUCCESS��0	ɾ���ļ��ɹ�
MR_FAILED��	-1	ɾ���ļ�ʧ��
MR_NOTFULL��-2	�ļ�������
*/
int32 mrc_netRemoveSMSFile(int32 varlen, char *vardata, mrc_netPayCB cb);

/*
Ӧ���ڸ���ǰ�����Ѿ�ʹ��GPRS���ŷ�ʽ������ΪSMS_APP_PRE_NETWORK
���ô˺�����������󣬸���ʱ�����ܶϿ�ԭ����GPRS���һص�����Ҫ������GPRS

ע��
networkType	��ʽ��mrc_netPayUI�еĵ�10207		netInitҪ��Ӧ
*/
void mrc_netSetAppNetworkType(SMS_APP_PRE_NETWORK networkType);

/*
����Ӧ�û�ȡע��ʱ�Ѹ��ѵĽ��
param	varlen ��������ʱû��
	vardata ��������ʱû��
return 	payPrice�Ѹ��ѵĽ���λ����
	MR_FAILED ��ȡ���ѽ��ʧ��
	!MR_FAILED ��ȡ���ĸ��ѽ��
*/
int32 mrc_appGameGetPayPrice(int32 varlen, char *vardata);

/*
����Ӧ��ɾ��ע��ʱ�ı���ĸ�����Ϣ
param	varlen ��������ʱû��
	vardata ��������ʱû��
return 	�Ƿ�ɾ���ɹ�
	MR_FAILED ʧ��
	MR_SUCCESS �ɹ�
*/
int32 mrc_appGameRemovePayFile(int32 varlen, char *vardata);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MRC_NETPAY_H__ */