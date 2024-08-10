
#ifndef _RM_GIF_H
#define _RM_GIF_H

#include "mrc_base.h"
#include "verdload.h"
//#include "ext.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	
#ifndef GIF_FILE_NAME_LEN
#define GIF_FILE_NAME_LEN	64
#endif
	
//GIF ͼƬ�Ĵ��Ŀ¼
#define GIFFILE_DIR		"gwy/gifs/"

	typedef struct _gif_file_desc
	{
		char filename[GIF_FILE_NAME_LEN];
		uint16 version;
		uint32  fileLen;    
		uint32  fileId;
	}gif_file_desc_t;


	/*��ʼ��*/
	int32 mrc_Gif_Init(void);
	
	/*
	����ͼƬ��������SGL_resLoadBmp���Ỻ��ͼƬ
	����:	
	int32 group:���õ�ģ��ID
	char filename: gifͼƬ�ļ���
	unit8 bNeedDownload:���ͼƬ�����ڣ��Ƿ�ȥ����������
	���:
	uint16* width:bmp width
	uint16* height:bmp height
	����:
	NULL	����ʧ��
	����	bmp�ڴ�ָ�룬ͼƬ�ᱻ���棬����ͷ���Ҫ����rm_gifFreeByFileName��rm_gifFreeByGroup
	*/
	uint16* mrc_Gif_Load(int32 group, char *filename, uint16*width, uint16*height, uint8 bNeedDownload);
	
	/*
	����ͼƬ���ͷ�ͼƬ
	����:	
	char filename: gifͼƬ�ļ���
	���:
	����:
	*/
	int32 mrc_Gif_FreeByFileName(char * filename);
	
	/*
	����ģ��ID�ͷ�ͼƬ
	����:	
	int32 group: ģ��ID
	���:
	����:
	*/
	int32 mrc_Gif_FreeByGroup(int32 group);
	
	
	/*
	��ʾgifͼƬ,����ͼƬ���л��档
	����:
	char * filename: gifͼƬ��
	int16 x:	��ʼλ��x
	int16 y:    ��ʼλ��y
	unit8 bNeedDownload:���ͼƬ�����ڣ��Ƿ�ȥ����������
	���:
	MR_SUCCESS ��ʾ�ɹ�
	MR_FAILED  ʧ�ܣ������needDownLoadFlagȥ�����ļ���
	����:
	*/
	int32 mrc_Gif_Show(char * filename, int16 x, int16 y, uint16 *width, uint16 *height, uint8 bNeedDownload);
	
	/* 
		gif_file_desc_t* file: gifͼƬ����Ϣ
		MR_DL_CB DL_CB:���ػص�����
		ui:          ui��ȡֵ�����ֿ��ܣ�
		0                       ���ؽ��Ƚ��������ز������,����ǰ�����û�ȷ�ϡ�
		MR_DL_FLAG_UIHINT      ���ؽ��Ƚ��������ز����������ǰ���û�ȷ�ϡ�
		MR_DL_FLAG_UIBYAPP   ���ؽ�����APP����, ���ز���������κε�UI��

		  title:       ���ؽ������,ui & VERDLOAD_FLAG_UIBYAPP == 0 ʱ��Ҫ�ṩ
		  hint:        ����ȷ�Ͻ������ʾ,ui =VERDLOAD_FLAG_HINT ʱ��Ҫ�ṩ
	*/
	int32 mrc_Gif_TryDownLoadFile(gif_file_desc_t* file,MR_DL_CB DL_CB,int32 ui,uint16 *title,uint16 *hint);
	int32 mrc_Gif_CancelDownLoad(void);
	/*
		�ͷ�bmpPool�����е�ͼƬ.
	*/
	int32 mrc_Gif_FreeAll(void);
	
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _RM_GIF_H */

