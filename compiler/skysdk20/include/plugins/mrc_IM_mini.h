#ifndef _MRC_IM_MINI_H_
#define _MRC_IM_MINI_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	IM_EVENT_START_DIAL,				//��ʼ����
	IM_EVENT_DIALED_DIAL_SUCCESS,		//���ųɹ�
	IM_EVENT_DIALED_DIAL_FAILED,		//����ʧ��

	IM_EVENT_CONNECT_SUCCESS,			//���ӷ������ɹ�
	IM_EVENT_CONNECT_TRY_NEXT,			//�������ض���, ������һ��������
											
	IM_EVENT_DISCONNECTED,			//����������ӶϿ����᳢������������
	IM_EVENT_CONNECT_FAILED,			//���ӷ�����ʧ��
	/***************************************************************************/
	IM_EVENT_LOGIN_SUCESS,					//IM��¼��֤�ɹ�
	IM_EVENT_LOGIN_FAILED,	 //IM��¼��֤ʧ�ܻ�ʱ

	IM_EVENT_WIN_OPEN,		//���촰�ڿ�������Ҫ��Ϸ��ϻ���ȫ��
	IM_EVENT_WIN_CLOSE		//���촰�ڹر�

}im_event;


typedef enum
{	
	MSG_STA_CONNECTING,//δ���ϣ�������
	MSG_STA_REDIRECTING,//δ���ϣ��ض�����
	MSG_STA_CONNECTED,//���ӱ�����
	MSG_STA_RECONNECTING,//δ���ϣ���������
	MSG_STA_DISCONNECTED//δ���ϣ�����ʧ��
}msgStatus;
/*********************************�ص���������****************************************************************/

/*
  IM mini�Ļص�֪ͨ����
*/
typedef void (*msgStatusNotifyCb)(im_event event);


/*��Ϣģ����ջص�
pData:	��Ϣ���ݣ�����Ҫ�ͷ�
len:      ��Ϣ����
*/
typedef void (*msg_recv_cb)(uint8* pData,int32 len,uint16 srcid);


/************************IM_MINI**********************************/
/*
	��������mrc_IM_mini_start
	���ܣ�����IM mini�����ӽ������
	���룺statusCb��
	���������ֵ
	MR_SUCCESS,����IM����ɹ�
	MR_FAILED,����IM���ʧ��
*/
int32 mrc_IM_mini_start(msgStatusNotifyCb statusCb);



/*
	��������mrc_IM_mini_off_line_hint
	���ܣ����Ƶ�����ʾ�����ú�ֻҪ���ߺ������������ʾ��Ϸ����
*/
int32 mrc_IM_mini_off_line_hint(void);


/*
	��������mrc_IM_mini_login
	���ܣ���������һ�κ��ѵ�¼���������mrc_IM_mini_start�Ļص���֪ͨ

*/
int32 mrc_IM_mini_login(void);


/* 
	��������mrc_IM_mini_set_draw_pos
	���ܣ�������Ϣ��ʾͼ����Ƶ�λ��
	���룺x,y ���Ƶ�λ�����ĵ�λ������

    ��x=-1��y=-1ʱ��ʾ����ʾ��Ϣ��ʾͼ�꣬���Ҳ���ʾ����ص�
	��x=-2��y=-2ʱ��ʾ����ʾ��Ϣ��ʾͼ�꣬���ǻ���ʾ����ص�
	��x��yΪ������������ʱ����ʾ��Ϣ��ʾͼ�꣬������ʾ����ص�
*/

int32 mrc_IM_mini_set_draw_pos(int32 x,int32 y);


/*
	��������mrc_IM_mini_isVisible
	���ܣ�	��ȡ��ǰ��IM�ɼ����õ�״̬
	�����0�����ɼ���������ص�
		  1�����ɼ���������ص�
		  2���ɼ���������ص�
*/
int32 mrc_IM_mini_isVisible(void);


/*  
	��������mrc_IM_mini_restart
	���ܣ����½����������ӣ�����״̬ά�ֲ���
*/

int32 mrc_IM_mini_restart(void);


/*  
	��������mrc_IM_mini_stop
	���ܣ��ر�IM_mini
*/

int32 mrc_IM_mini_stop(void);



/************************MSG�Ľӿ�ʹ��************************/

/*����Ϣģ��ע��
������� 
extId :ģ��id
eventCB :�¼��ص�����
recvCB  :��Ϣ���ջص�����
����  	MR_SUCCESS :ע��ɹ�
MR_FAILED			:ע��ʧ��*/
int32 mrc_msgRegist(int32 srcId, msg_recv_cb recvCB);


/*ȡ��ע����Ϣ���
������� 
extId :ģ��id
����  	MR_SUCCESS :ȡ���ɹ�
MR_FAILED     :ȡ��ʧ��*/
int32 mrc_msgUnRegist(int32 srcId);


/*��ӷ�����Ϣ
������� 
srcId   :�ն˵�ģ���
dstId	:�����ģ���
pData   :������Ϣ������ָ�룬�������ú�������ͷ�
len     :������Ϣ�ĳ���
����
MR_SUCCESS :��ӳɹ�
MR_FAILED  :���ʧ��*/
int32 mrc_msgSendMsg(int32 srcId,int32 dstId,uint8* pData, int32 len);



/*��ӷ�����Ϣ��չ
���mrc_msgSendMsg
����ָ����Ϣ�ı�ʶλtag
*/
int32 mrc_msgSendMsgEx(int32 extId,int32 serId,uint8* pData, int32 len,int32 tag);

/*
�����ϢԤ���ռ�

  �������:
	srcId   :�ն˵�ģ���
	dstId	:�����ģ���
	length  :������Ϣ�ĳ���

  ���� �����Ϣ��ַ
*/
uint8* mrc_msgPrepare(int32 srcId,int32  dstId, int32 length);


/*
���ʹ����Ϣ
  ���������
  handle��û���õ�
  pData������mrc_msgPrepare���صĴ����Ϣ��ַ
  seqId��û���õ�
  priority��û���õ�	

*/
int32 mrc_msgSendPacket(int32 handle, uint8* pData, int32 seqId, int32 priority);


/*
��ȡmsg��״̬
  ���������
���أ�msg�ĵ�ǰ״̬	

*/

msgStatus mrc_msgGetStatus(void);

/*
��ͣ��Ϣ����
����msg�洢n����Ϣ
�������:	num ��Ϣ����
srcId �ն�ģ���
����
MR_SUCCESS :�ɹ�
MR_FAILED  :ʧ��
*/
int32 mrc_msgPause(uint8 num,int32 srcId);

/*
�ָ�������Ϣ
�������:	srcId �ն�ģ���
����ڼ��д洢����Ϣ������������
*/
int32 mrc_msgResume(int32 srcId);

#ifdef __cplusplus
}
#endif

#endif
