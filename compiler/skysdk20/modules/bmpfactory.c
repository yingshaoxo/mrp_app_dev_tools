#include "bmpfactory.h"


//取屏幕缓冲区的内存地址
extern uint16 * w_getScreenBuffer(void);
//获取屏幕缓冲区宽高
extern void mrc_getScreenSize(int32 *w,int32 *h);
//设置屏幕缓冲区宽高
extern void mrc_setScreenSize(int32 w, int32 h);


/**
例子：在内存中双字0x01020304(DWORD)的存储方式 
MEM 0000 0001 0002 0003 
LE  04   03   02   01 
BE  01   02   03   04  
*/
//以小端字节序将32位数据写入内存
int32 writeLittleEndianU32(uint32 data, uint8 *p)
{
	if (NULL == p)
		return -1;

	*p = (uint8)(data) & 0xff;
	*(p + 1) = (uint8)((data >> 8)) & 0xff;
	*(p + 2) = (uint8)((data >> 16)) & 0xff;
	*(p + 3) = (uint8)((data >> 24)) & 0xff;

	return 0;
}

//以小端字节序将16位数据写入内存
int32 writeLittleEndianU16(uint16 data, uint8 *p)
{
	if (NULL == p)
		return -1;

	*p = (uint8)(data) & 0xff;
	*(p + 1) = (uint8)((data >> 8)) & 0xff;

	return 0;
}

PBMPHEAD565 makeBmpHead565(DWORD w, DWORD h)
{
	int a = 0;

	PBMPHEAD565 head = (PBMPHEAD565)malloc(sizeof(BMPHEAD565));
	if(head == NULL) return NULL;
	memset(head, 0, sizeof(BMPHEAD565));

#ifdef MR_SPREADTRUM_MOD	//展讯模式
	//文件头
	head->bmfHeader.bfType = 0x424D0000;
	writeLittleEndianU32((70 + w*h*2), (uint8*)&head->bmfHeader.bfSize);
	head->bmfHeader.bfReserved1 = 0x0000;
	head->bmfHeader.bfReserved2 = 0x0000;
	head->bmfHeader.bfOffBits = 0x46000000;//70

	//信息头
	head->bmiHeader.biSize = 0x28000000;//40;
	writeLittleEndianU32(w, (uint8*)&head->bmiHeader.biWidth);
	writeLittleEndianU32(h, (uint8*)&head->bmiHeader.biHeight);
	head->bmiHeader.biPlanes = 0x0100;
	head->bmiHeader.biBitCount = 0x1000;//16
	head->bmiHeader.biCompression = 0x03000000;
	writeLittleEndianU32(w*h*2, (uint8*)&head->bmiHeader.biSizeImage);
	head->bmiHeader.biXPelsPerMeter = 0xA00F0000;//4000
	head->bmiHeader.biYPelsPerMeter = 0xA00F0000;
	head->bmiHeader.biClrUsed = 0x00000000;
	head->bmiHeader.biClrImportant = 0x00000000;

	//调色板 00F8 0000 E007 0000 1F00 0000
	a = 0x00F80000;
	memcpy(&head->bmiColors[0], &a, 4);
	a = 0xE0070000;
	memcpy(&head->bmiColors[1], &a, 4);
	a = 0x1F000000;
	memcpy(&head->bmiColors[2], &a, 4);
	a = 0x00000000;
	memcpy(&head->bmiColors[3], &a, 4);
#else
	//文件头
	head->bmfHeader.bfType = 0x00004D42;
	head->bmfHeader.bfSize = (DWORD)(70 + w*h*2);
	head->bmfHeader.bfReserved1 = 0x0000;
	head->bmfHeader.bfReserved2 = 0x0000;
	head->bmfHeader.bfOffBits = 0x00000046;//70

	//信息头
	head->bmiHeader.biSize = 0x00000028;//40;
	head->bmiHeader.biWidth = w;
	head->bmiHeader.biHeight = h;
	head->bmiHeader.biPlanes = 0x0001;
	head->bmiHeader.biBitCount = 0x0010;//16
	head->bmiHeader.biCompression = 0x0003;
	head->bmiHeader.biSizeImage = w*h*2;
	head->bmiHeader.biXPelsPerMeter = 0x00000FA0;//4000
	head->bmiHeader.biYPelsPerMeter = 0x00000FA0;
	head->bmiHeader.biClrUsed = 0x00000000;
	head->bmiHeader.biClrImportant = 0x00000000;

	//调色板 00F8 0000 E007 0000 1F00 0000
	a = 0x0000F800;
	memcpy(&head->bmiColors[0], &a, 4);
	a = 0x000007E0;
	memcpy(&head->bmiColors[1], &a, 4);
	a = 0x0000001F;
	memcpy(&head->bmiColors[2], &a, 4);
	a = 0x00000000;
	memcpy(&head->bmiColors[3], &a, 4);
#endif

	return head;
}

int32 printScreen( const char *filename )
{
	uint16 *p1;
	int32 w, h;
	int i;
	int32 fd;

	if(!filename)
		return MR_FAILED;

	//获取屏幕缓冲区数据
	mrc_getScreenSize(&w, &h);
	p1 = (uint16*)w_getScreenBuffer();	

	fd = mrc_open(filename, MR_FILE_RDWR|MR_FILE_CREATE);
	if(fd > 0)
	{
		//写BMP头
		PBMPHEAD565 bmp = makeBmpHead565(w, h);
		BYTE head[70] = {0};
		uint16 *line = malloc(w*2);

		
		memcpy(head, &bmp->bmfHeader, 16);
		memmove(head+2, head+4, 12);
		memcpy(head+14, &bmp->bmiHeader, 40);
		memcpy(head+54, &bmp->bmiColors, 16);
		mrc_write(fd, head, 70);

#ifdef MR_SPREADTRUM_MOD	//展讯模式
		{
			uint8 *p2;
			uint8 tmp;

			p2 = (uint8*)p1;
			for(i=0; i<w*h*2; i+=2){
				tmp = p2[i];
				p2[i] = p2[i+1];
				p2[i+1] = tmp;
			}
		}
#endif

		//写BMP数据
		//for(i=h-1; i>=0; i--)
		//{
		//	mrc_write(fd, p1+i*w, w*2);
		//}

		//交换行序
		{
			int a = w<<1, c = h>>1;

			for(i=0; i<c; i++){
				memcpy(line, p1+i*w, a);
				memcpy(p1+i*w, p1+(h-i-1)*w, a);
				memcpy(p1+(h-i-1)*w, line, a);
			}

			mrc_write(fd, p1, w*h*2);
		}
		

		//释放数据
		mrc_close(fd);

		return MR_SUCCESS;
	}else
	{
		return MR_FAILED;
	}
}

//截图函数，自动按当前时间生成唯一文件名，
int32 ScreenShot(PCSTR todir)
{
	int32 ret = MR_FAILED;
	char name[128] = {0};


	if(MR_IS_DIR != mrc_fileState(todir))
		mrc_mkDir(todir);

	{
		static int pcount;
		mr_datetime timenow;

		mrc_getDatetime(&timenow);

		sprintf(name, "%s%s%d%02d%02d%02d%02d-%02d.bmp", 
			todir, 
			todir[0] != 0? "/" : "",
			timenow.year, timenow.month, timenow.day, 
			timenow.hour, timenow.minute, 
			pcount++);
	}

	ret = printScreen(name);


	return ret;
}