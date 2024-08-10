#ifndef _MRC_ADVBAR_H_
#define _MRC_ADVBAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mrc_base.h"

// ���������ĸ߶ȣ�С��Ļ�ֻ�Ϊ30������Ļ�ֻ�Ϊ40
#ifdef SCREEN_176_220
#define ADV_BAR_HEIGHT	30
#else
#define ADV_BAR_HEIGHT	40
#endif

typedef enum {
	NOTIFY_TYPE_DOWNLOAD,
	NOTIFY_TYPE_BROWSER
}NOTIFY_TYPE;


typedef void (*AdvBarPause)();
typedef void (*AdvBarResume)();
typedef void (*AdvBarLoadCB)(int32 result);
typedef void (*AdvActivityCB)(int32 result);
typedef void (*AdvDoneNotify)(NOTIFY_TYPE type);


// �������ʼ������������һ��
// advChunkID	���λID�����ֶ���SKY�����Ӫ����
// x			���������ʼx����
// y			���������ʼy����
// w			������Ŀ��
// smallFont	�Ƿ�ʹ��SKY��С��������TRUE��ʾʹ��SKY�ֿ⣬FALSE��ʾʹ���ֻ������ֿ�
// pauseCB		�����֪ͨӦ�ù���Ļص�����
// resumeCB		�����֪ͨӦ�ûָ��Ļص�����
// loadCB		�����֪ͨӦ���Ƿ���سɹ��Ļص�����
int32 mrc_advInit(uint32 advChunkID, 
	uint16 x, uint16 y, uint16 w, 
	int8 smallFont, 
	AdvBarPause pauseCB, AdvBarResume resumeCB, AdvBarLoadCB loadCB);


// ��ʼ�������֮�����Ҫ�������λ�����ô˷������и���
// advChunkID	���λID�����ֶ���SKY�����Ӫ����
int32 mrc_advChange(uint32 advChunkID);


// ���ù��ִ�����֪ͨ����
// notifyCB		Ӧ�ÿ����ߴ����֪ͨ�ص���������������еĹ�汻�����ߵ����ִ����Ϻ�
// �����ͨ���η���֪ͨӦ�á�type == 1��ʾ������ɡ�type == 2��ʾ������������
int32 mrc_advSetNotify(AdvDoneNotify notifyCB);


// �������ʼ��ͼ
int32 mrc_advStartDraw(void);


// �����ֹͣ��ͼ
int32 mrc_advStopDraw(void);


// ǿ��ˢ��һ�ν��棬�������forceΪTRUE�Ļ�������Լ�ǿ�Ƶ���mrc_refreshScreenˢ�¹������
int32 mrc_advRefresh(int32 force);


// �������Ϊ���㣬��ʱ��SELECT�������ᴥ�����Ӧ��
int32 mrc_advFocus(void);


// �����ʧȥ����
int32 mrc_advUnFocus(void);


// ���ü��������ݵİ�����ֵ��Ĭ��ΪSELECT������
int32 mrc_setActiveKey(int32 key);



#ifdef __cplusplus
}
#endif
#endif