#ifndef _MRP_VERFIY_H_

#define _MRP_VERFIY_H_

typedef enum
{
	E_VERIFY_RESULT_SUC,			//ǩ��У��ͨ��
	E_VERIFY_RESULT_FAILED,		//ǩ��У��ʧ��
	E_VERIFY_RESULT_NOCER,		//δ����ǩ��֤��
	E_VERIFY_RESULT_MAX
	//...	
}E_MRPVERIFY_RESULT;

/*
��������:��֤MRPǩ��֤��ĺϷ���

����˵��:mrpname-->ָ�����ĸ�MRP����ǩ��У��
						 Ҫ�������Mythroadϵͳ��Ŀ¼������·��
						 NULL��ʶCheck������MRP
						 

��������ֵ : ��E_MRPVERIFY_RESULT ����
	
*/
E_MRPVERIFY_RESULT  mrc_verifyMrpCertificate_V1(const char *mrpName);

E_MRPVERIFY_RESULT  mrc_verifyMrpCertificate_V2(const char *mrpName);

E_MRPVERIFY_RESULT  mrc_verifyMrpCertificate_V3(const char *mrpName);

E_MRPVERIFY_RESULT  mrc_verifyMrpCertificate_V4(const char *mrpName);
#endif