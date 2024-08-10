/** arraylist.h declaration of arraylist.h
 ** 
 ** ����ɱ䳤���������� ����
 **
 ** ע����һ���ڵ��index��0
 **
 ** @Author: JianbinZhu
 ** �������ڣ�2012/2/16
 ** ����޸����ڣ�2012/2/17
 **/

#ifndef _ARRAYLIST_H_
#define _ARRAYLIST_H_

#include "mrc_base.h"
//#include "filelist.h"


#ifndef boolean

typedef unsigned char boolean;	//���� ��������

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#endif


typedef struct _node {
	int32 data;
	struct _node *next;
}NODE, *PNODE;

typedef struct _arrayList {
	PNODE head;
	int size;
}ARRAYLIST, *PARRAYLIST;
//ע����һ���ڵ��index��0


//---------------------------------------------------------
//ɾ���б�ڵ��Ļص�������
//data��Ϊ�ýڵ�����ݣ������ٻص��������ͷ�������ڴ�
//�����ֶ���������ȥ�ͷ�
//---------------------------------------------------------
typedef void (*AL_removeCallBack)(int32 data);

//��������
PARRAYLIST AL_create(void);

//������ݣ��½�һ���ڵ㱣�� ����data
//data��Ϊ��ֵ���߽ṹ��ָ�룬�ṹ��ָ����Ա���������
void AL_add(PARRAYLIST pal, int32 data);

//ɾ��index���ڵ㣬���ظýڵ�洢������
int32 AL_remove(PARRAYLIST pal, int index);

//��ȡindex�ڵ�洢������
int32 AL_get(PARRAYLIST pal, int index);

//��������cbΪ�������ٽڵ�Ĺ����еĻص�������
//������cb���ͷŸýڵ������
void AL_destroy(PARRAYLIST pal, AL_removeCallBack cb);

//��ȡ�����С
int AL_size(PARRAYLIST pal);

//��������ǲ��ǿ�
boolean AL_isEmpty(PARRAYLIST pal);


#define _AL_GET(index, type) \
	(type)AL_get(index)

#define _AL_ADD(index, data) \
	AL_add(index, (int32)data)

#endif