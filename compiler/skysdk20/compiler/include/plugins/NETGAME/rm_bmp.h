
#ifndef _RM_BMP_H
#define _RM_BMP_H

#include "mrc_base.h"
//#include "ext.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	/*FOR FREE ADJ*/
#define RES_INTERNAL_PRIO 255//don't change to other value
#define RES_MEMSTREAM_PRIO 254//?OK?
	
#define RES_MRP_FILENAME_MAXLEN (32)//(MPS_FP_FILENAME_LEN)
#define RES_RES_FILENAME_MAXLEN (12)
	
#define RES_BMPID_BASE	0
#define RES_BMPID_SYS_BASE	(RES_BMPID_BASE+0)
#define RES_BMPID_SYS_MAX 500
#define RES_BMPID_APPS_BASE	(RES_BMPID_BASE+1000)
	
	typedef struct
	{// do not change follow numbers order, must corresponding to struct - resIdSt
		uint16 *memPtr;//bmp�ڴ��ַָ��
		uint16 beginId;//��ʼbmp id
		uint16 endId;//����bmp id
		uint16 w;//bmp��Ԫ��
		uint16 h;//bmp��Ԫ��
		
		uint8 *auxPtr;//bmp�ڴ��ͷŵ�ַָ��,������NULL�����ͷ�ʱ��*memPtrΪ׼
	}resBmpSt;
	/*
	Լ��:
	1/������һ��bmp�е�һ��id���м䱻�滻(ȡ�м�һ��id����ע��)�����������滻��������Ϊ��Ч
	2/�滻����:1��beginId����һ�£�ֻ���Ǵ�id��Χ�滻��С�ġ�
	3/ÿ��ʹ��bmpʱ������ʹ��SGL_resLoadBmp(id)���»�ȡ���ڴ�ptr���������ǰ��bmp�Ѿ����ͷŻ��滻
	*/
	
	
	
	int32 mrc_Bmp_Init(void);
	
	/*
	ע��bmp group(�����ͬһmrp�ļ���)
	����:	
	const char *mrpName: bmp����mrp����
	���:
	����:
	>0  -group handle
	����	-����ʧ��
	*/
	int32 mrc_Bmp_RegGroup(const char *mrpName);
	
	
	/*
	��bmp�������ע��bmp
	����:	
	resBmpSt *regAtt: bmp�������,���resBmpSt
	const char *mrpName: bmp����mrp����
	int32 group: RM_resRegBmpGroup() ���ص�bmp groupֵ
	uint8 prio :       bmp�ļ������ȼ�
	���:
	����:
	MR_SUCCESS		�����ɹ�
	MR_FAILED		����ʧ��
	*/
	int32 mrc_Bmp_RegId(resBmpSt *regAtt, const char *bmpName, int32 group, uint8 prio);
	
	
	/*
	��ȡbmp �ڴ�ָ��
	����:	
	uint16 bmpID: bmp id
	uint8 prio: ���ȼ�����Ҫ��������app
	���:
	int32* width:bmp width
	int32* height:bmp height
	int32* index:bmp������е�index������������RM_resGetBmpPtr()��
	����Ĭ����NULL���򲻷���index
	����:
	NULL	����ʧ��
	����	bmp�ڴ�ָ��
	*/
	uint16* mrc_Bmp_Load(uint16 bmpID, int32* width, int32* height, int32* index, uint8 prio);
	
	/*
	����!:ʹ�ñ�����ǰ�������mrc_exRamInitEx��ʼ����չ�ڴ�
	ͬrm_resLoadBmp, ���������ͬ�����ȴ���չ�ڴ�����ڴ�ռ�, ���䲻��ʱ�Ŵ����ڴ����
	����:	
	uint16 bmpID: bmp id
	uint8 prio: ���ȼ�����Ҫ��������app
	���:
	int32* width:bmp width
	int32* height:bmp height
	int32* index:bmp������е�index������������RM_resGetBmpPtr()��
	����Ĭ����NULL���򲻷���index
	����:
	NULL	����ʧ��
	����	bmp�ڴ�ָ��
	*/
	uint16* mrc_Bmp_LoadExmem(uint16 bmpID, int32* width, int32* height, int32* index, uint8 prio);
	
	
	/*
	��������,����RM_resLoadBmp()���ص�index��ȡbmp���ڴ�ָ��,�����Щ��ȡ�ٶȡ�
	����:	
	uint16 bmpID: bmp id
	uint16 index: bmp ��bmp������е�index����
	���:
	����:
	NULL	����ʧ��
	����	bmp�ڴ�ָ��
	*/
	uint16* mrc_Bmp_GetPtr(uint16 bmpID, uint16 index);
	
	/*
	�ͷ�bmp�ڴ�
	����:	
	uint16 bmpID: bmp id
	uint8 unReg: �Ƿ�ͬʱж��bmpID��bmp�������ע����Ϣ
	(�����ʹ��,Ӧ��ģ��ֻ�����ͷ�����reg��bmp)
	���:
	����:
	*/
	void mrc_Bmp_Free(uint16 bmpID, uint8 unReg);
	
	
	/*
	�ͷ�ָ�����ȼ���bmp�ڴ�
	����:	
	uint8 prio: 
	���:
	����:
	*/
	void mrc_Bmp_FreeByPrio(uint8 prio);
	
	/*
	�ͷ�ָ��group��bmp�ڴ�
	����:	
	int32 groupId
	���:
	����:
	*/
	void mrc_Bmp_FreeByGroup(int32 groupId);
	
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _RM_BMP_H */

