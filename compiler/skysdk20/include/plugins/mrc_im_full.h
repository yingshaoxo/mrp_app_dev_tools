#ifndef __MRC_IM_FULL_H__
#define __MRC_IM_FULL_H__

#include "mrc_IM_mini.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	IM_FULL_EVENT_QUIT_SUC,//�����˳�
	IM_FULL_EVENT_LOAD_ERR,//����ʧ��
	IM_FULL_EVENT_DLOAD_ERR,//δ�������
	IM_FULL_EVENT_MAX
}IM_CB_EVEVT;

/*
������IM�Ļص�����
code:�ص��¼�֪ͨ
errcode:�������ͣ���code=IM_FULL_EVENT_QUIT_ERRʱ��Ч
*/

typedef void  (*full_cb)(IM_CB_EVEVT code);


typedef void (*msgStatusNotifyCb)(im_event event);


/*
�����������IM,�˳�ʱ�������Ȼ����
�������:

  cb 		�˳�IMʱ�ص�֪ͨ����
  status_cb 	����״̬֪ͨ

����
*/
void mrc_im_full_start(full_cb cb, msgStatusNotifyCb status_cb);


/*
�����������IM,�˳�ʱ�����Ҳж��
�������:

  cb �˳�IMʱ�ص�֪ͨ����

����
*/
void mrc_im_full_start_alone(full_cb cb);


void start_by_mini(int32 skyid,int32 gadtype,int32 eventIndex,full_cb cb);



#ifdef __cplusplus
}
#endif

#endif