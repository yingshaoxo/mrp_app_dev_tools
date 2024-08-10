#ifndef _MRC_FREE_CURRENCY_H_
#define _MRC_FREE_CURRENCY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mrc_base.h"

typedef void (*PluginLoadCB)(int32 result);
typedef void (*ChargeResultNotify)(int32 result);

// ��ʼ���ƷѲ��������ƷѲ�������ڻ�ȥ�Զ����أ�
//ui�����Ƿ���ʾ����汾�����棬
//needCloseNet���Ʋ��������罻�����Ƿ�ر����磬
//�ص�����loadCB֪ͨ�˼��ز���Ľ��
int32 mrc_freecurrency_init(int8 ui, int8 needCloseNet, 
							PluginLoadCB loadCB);

// ��鱾�ظ�����Ϣ�����regPoint�ļƷѵ��Ѿ����ѷ���MR_SUCCESS�����򷵻�MR_FAILED��
//����payIDΪ0�����������
// �����������������Ҫ���ûص�������֪ͨ��������ý����
//�ص�����������ΪMR_SUCCESS��ʾ��������óɹ���
int32 mrc_freecurrency_reg_check(int32 regPoint, 
								 ChargeResultNotify notifyCB);

// Ϊ�Ʒѵ�regPoint����֧����
// amount��֧��������ҵ����� 
// brief��detail�ֱ��ǼƷѵļ���������������������40���֣�
// ����������һ��Ļ��notifyCB֪ͨ�ص�����MR_SUCCESS��ʾ֧���ɹ��������ʾʧ�ܡ�
int32 mrc_freecurrency_reg_pay(int32 regPoint, int32 amount, char *brief, char *detail, 
							   ChargeResultNotify notifyCB);

// Ϊ�Ʒѵ�gamePoint����֧����
// amount��֧��������ҵ����� 
// brief��detail�ֱ��ǼƷѵļ���������������������40���֣�
// ����������һ��Ļ��notifyCB֪ͨ�ص�����MR_SUCCESS��ʾ֧���ɹ��������ʾʧ��
int32 mrc_freecurrency_prop_pay(int32 gamePoint, int32 amount, char *brief, char *detail, 
								ChargeResultNotify notifyCB);

// ж��֧�����
int32 mrc_freecurrency_unload(void);

#ifdef __cplusplus
}
#endif
#endif