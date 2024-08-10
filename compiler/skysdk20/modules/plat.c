#include "plat.h"

/* 请求移植层显示一张图片（图片显示的位置由移植来确定，可能在idle，或者icon条，或者两个地方都显示等等情况）
* 数据结构：如下
*/
/*	参数说明：
*	appid：请求画图的应用的id
*	time： 图片闪动的时间单位是：ms（毫秒）如果为0 表示不用闪动；
*	img_type:图片类型
*      0：BMP 图片；1：jpg 图片；2：png 图片；3：gif 图片
*      如果是bmp图片的话，buffer 指向的是解压后的点阵数据信息
*	img_size:buff 这个指针指向的空间的大小
*	color： 透明色（bmp 图片时候起作用）
*	buff：  图片数据。
*	destId: 0：表示画idle下的一个小图标
*          1：画idle下的整个背景

*	根据不同的timer大小，底层将确定进行怎样的操作
*	timer：     0 ：显示icon但不闪动（如果是QQ显示在线图标）
*				1000：闪动的显示icon（如果是QQ显示在线图标）
*				1：显示离开状态的QQ头像（QQ专用）
*				2：显示隐身状态的QQ头像（QQ专用）
*			    5：MRSTAR平台专用，用于mrp通知移植层，当前QQ已经启动可以实现end键转后台（QQ专用）；
*			    1001：显示离开状态的QQ头像（QQ专用）
*			    1002：显示隐身状态的QQ头像（QQ专用）
*			    -1：为显示离线状态的QQ头像（QQ专用）
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
 * 函数
 * 功能：后台运行后在手机状态栏显示一个小图标
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
 * 函数1.后台运行程序
 * 函数名BackRunApp
 */
int32 BackRunApp(int32 appid)
{
	mr_backstage_st  backRunIn;
    int32 ret; 

	AppIconFicker(appid, 0);
    memset(&backRunIn, 0, sizeof(mr_backstage_st));
 
    backRunIn.appid = appid; /*当前移植层只做了appid的判断，
							   用于区分是QQ还是其他产品，从而根据appid显示不同的icon；*/
    ret = mrc_platEx(1004, (uint8*)&backRunIn, sizeof(mr_backstage_st), 0, 0, 0);

    return ret;
}

/**
 * 函数2.恢复后台运行程序
 * 函数名BackRunApp
 */
int32 AwakeApp(int32 appid)
{
	int32 ret = MR_FAILED;
    ret =  mrc_plat(1003, appid);
    if (ret == MR_SUCCESS)
    {
      /*唤醒成功需要停止后台icon的闪动，这里建议在mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_APPRESUME, 0);
        函数中进行调用本接口，这样可以做到主动唤醒和被动唤醒都可以关闭后台icon的闪动；*/
      mrc_plat(1006, appid);
    }
	return ret;
}