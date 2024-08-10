
#ifndef _H_APP_DATA_H_
#define _H_APP_DATA_H_
/*
int32 mrc_WriteAppData(const char*fileName, uint8* data, uint32 dataLen)
int32 mrc_ReadAppData(const char*fileName, uint8** dataBuf, uint32 *bufLen)
示例:
uint8* buf = NULL;
uint32 bufLen = 0;
char *strTest = "abcdefg+12323aadfas";
int32 fileH = 0;
if(MR_SUCCESS == mrc_WriteAppData("aqb.txt", (uint8 *)strTest, mrc_strlen(strTest)))
{
	mrc_printf("write data ok ");
	mrc_ReadAppData("aqb.txt", (uint8**)&buf, &bufLen);
	mrc_printf((char*)buf);
	mrc_printf("bufLen = %d", bufLen);

}
else
{
	mrc_printf("write data error ");
}
*/
/*
功      能:存储应用特定数据至指定文件，文件保存路径由接口内定。
参数  说明:fileName    存储的文件名称，如:gwkpz.dat
		   data        应用欲存储的数据指针,要求数据量控制在1K以内。
		   dataLen     应用欲存储的数据长度，要求dataLen < 1024
返回值说明:
			MR_SUCCESS  存储成功
			MR_FAILED   存储失败
*/
int32 mrc_WriteAppData(const char*fileName, uint8* data, uint32 dataLen);
/*
功      能:读取应用指定文件中由mrc_WriteAppData接口特殊存储的数据。
参数  说明:fileName    存储的文件名称，如:gwkpz.dat
		   data        读取到的数据指针，内存空间由系统分配，请置NULL传入。
		   dataLen     读取到的数据长度
返回值说明:
			MR_SUCCESS  存储成功
			MR_FAILED   存储失败
*/
int32 mrc_ReadAppData(const char*fileName, uint8** dataBuf, uint32 *bufLen);
#endif