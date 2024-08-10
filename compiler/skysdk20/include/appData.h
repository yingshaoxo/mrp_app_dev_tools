
#ifndef _H_APP_DATA_H_
#define _H_APP_DATA_H_
/*
int32 mrc_WriteAppData(const char*fileName, uint8* data, uint32 dataLen)
int32 mrc_ReadAppData(const char*fileName, uint8** dataBuf, uint32 *bufLen)
ʾ��:
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
��      ��:�洢Ӧ���ض�������ָ���ļ����ļ�����·���ɽӿ��ڶ���
����  ˵��:fileName    �洢���ļ����ƣ���:gwkpz.dat
		   data        Ӧ�����洢������ָ��,Ҫ��������������1K���ڡ�
		   dataLen     Ӧ�����洢�����ݳ��ȣ�Ҫ��dataLen < 1024
����ֵ˵��:
			MR_SUCCESS  �洢�ɹ�
			MR_FAILED   �洢ʧ��
*/
int32 mrc_WriteAppData(const char*fileName, uint8* data, uint32 dataLen);
/*
��      ��:��ȡӦ��ָ���ļ�����mrc_WriteAppData�ӿ�����洢�����ݡ�
����  ˵��:fileName    �洢���ļ����ƣ���:gwkpz.dat
		   data        ��ȡ��������ָ�룬�ڴ�ռ���ϵͳ���䣬����NULL���롣
		   dataLen     ��ȡ�������ݳ���
����ֵ˵��:
			MR_SUCCESS  �洢�ɹ�
			MR_FAILED   �洢ʧ��
*/
int32 mrc_ReadAppData(const char*fileName, uint8** dataBuf, uint32 *bufLen);
#endif