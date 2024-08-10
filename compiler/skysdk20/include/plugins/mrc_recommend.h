#ifndef _MRC_RECOMMEND_H_
#define _MRC_RECOMMEND_H_

#ifdef __cplusplus
extern "C" {
#endif

enum{
	RMD_USER_QUIT,
	RMD_DLOAD_FAILED,
	RMD_LOAD_FAILED	
};


/*����Ӫ������ص�
code:����״̬:
	
	RMD_USER_QUIT:		�û��˳����������Ϸ
	RMD_DLOAD_FAILED:	���ز��ʧ��(�����Ϸ����û�д��������᳢������)
	RMD_LOAD_FAILED:	���ز��ʧ��	
	p1,p2,p3,p4,p5:û���õ�������
*/

typedef void (*recommendGame_cb)(int32 code,int32 p1,int32 p2,int32 p3,int32 p4,int32 p5);

/*��������Ӫ�����
�������룺
	cb:	�ص�����
���������	
*/

void mrc_recommendGame(recommendGame_cb cb);


#ifdef __cplusplus
}
#endif
#endif