#include "plat.h"

/* ������ֲ����ʾһ��ͼƬ��ͼƬ��ʾ��λ������ֲ��ȷ����������idle������icon�������������ط�����ʾ�ȵ������
* ���ݽṹ������
*/
/*	����˵����
*	appid������ͼ��Ӧ�õ�id
*	time�� ͼƬ������ʱ�䵥λ�ǣ�ms�����룩���Ϊ0 ��ʾ����������
*	img_type:ͼƬ����
*      0��BMP ͼƬ��1��jpg ͼƬ��2��png ͼƬ��3��gif ͼƬ
*      �����bmpͼƬ�Ļ���buffer ָ����ǽ�ѹ��ĵ���������Ϣ
*	img_size:buff ���ָ��ָ��Ŀռ�Ĵ�С
*	color�� ͸��ɫ��bmp ͼƬʱ�������ã�
*	buff��  ͼƬ���ݡ�
*	destId: 0����ʾ��idle�µ�һ��Сͼ��
*          1����idle�µ���������

*	���ݲ�ͬ��timer��С���ײ㽫ȷ�����������Ĳ���
*	timer��     0 ����ʾicon���������������QQ��ʾ����ͼ�꣩
*				1000����������ʾicon�������QQ��ʾ����ͼ�꣩
*				1����ʾ�뿪״̬��QQͷ��QQר�ã�
*				2����ʾ����״̬��QQͷ��QQר�ã�
*			    5��MRSTARƽ̨ר�ã�����mrp֪ͨ��ֲ�㣬��ǰQQ�Ѿ���������ʵ��end��ת��̨��QQר�ã���
*			    1001����ʾ�뿪״̬��QQͷ��QQר�ã�
*			    1002����ʾ����״̬��QQͷ��QQר�ã�
*			    -1��Ϊ��ʾ����״̬��QQͷ��QQר�ã�
*
*/
typedef struct
{
	int32	appid;
	int32	time;
	int32	img_type;
	int32	img_size;
	int32	width;
	int32	height;
	uint16	color;
	uint8	*buff;
	uint8	destId;
}mr_pic_req;

/**
 * ����
 * ���ܣ���̨���к����ֻ�״̬����ʾһ��Сͼ��
 */
static void AppIconFicker(int32 appid, int32 time)
{
    mr_pic_req picture;

    memset(&picture, 0, sizeof(mr_pic_req));
    picture.appid = appid;
    picture.time = time;

   // picture.img_type = 2;
   // picture.color= 0xff;
    picture.destId = 0;
	
	picture.width = picture.height = 16;

    mrc_platEx(1005, (uint8*)&picture, sizeof(mr_pic_req), 0, 0, 0);
}

/**
 * ����1.��̨���г���
 * ������BackRunApp
 */
int32 BackRunApp(int32 appid)
{
	mr_backstage_st  backRunIn;
    int32 ret; 

	AppIconFicker(appid, 0);
    memset(&backRunIn, 0, sizeof(mr_backstage_st));
 
    backRunIn.appid = appid; /*��ǰ��ֲ��ֻ����appid���жϣ�
							   ����������QQ����������Ʒ���Ӷ�����appid��ʾ��ͬ��icon��*/
    ret = mrc_platEx(1004, (uint8*)&backRunIn, sizeof(mr_backstage_st), 0, 0, 0);

    return ret;
}

/**
 * ����2.�ָ���̨���г���
 * ������BackRunApp
 */
int32 AwakeApp(int32 appid)
{
	int32 ret = MR_FAILED;
    ret =  mrc_plat(1003, appid);
    if (ret == MR_SUCCESS)
    {
      /*���ѳɹ���Ҫֹͣ��̨icon�����������ｨ����mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_APPRESUME, 0);
        �����н��е��ñ��ӿڣ��������������������Ѻͱ������Ѷ����Թرպ�̨icon��������*/
      mrc_plat(1006, appid);
    }
	return ret;
}