/** arraylist.h declaration of arraylist.h
 ** 
 ** 单向可变长度线性链表 工具
 **
 ** 注：第一个节点的index是0
 **
 ** @Author: JianbinZhu
 ** 创建日期：2012/2/16
 ** 最后修改日期：2012/2/17
 **/

#ifndef _ARRAYLIST_H_
#define _ARRAYLIST_H_

#include "mrc_base.h"
//#include "filelist.h"


#ifndef boolean

typedef unsigned char boolean;	//定义 布尔类型

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
//注：第一个节点的index是0


//---------------------------------------------------------
//删除列表节点后的回调函数，
//data即为该节点的数据，可以再回调函数里释放申请的内存
//避免手动遍历链表去释放
//---------------------------------------------------------
typedef void (*AL_removeCallBack)(int32 data);

//创建链表
PARRAYLIST AL_create(void);

//添加数据，新建一个节点保存 数据data
//data可为数值或者结构体指针，结构体指针可以保存多个数据
void AL_add(PARRAYLIST pal, int32 data);

//删除index处节点，返回该节点存储的数据
int32 AL_remove(PARRAYLIST pal, int index);

//获取index节点存储的数据
int32 AL_get(PARRAYLIST pal, int index);

//销毁链表，cb为链表销毁节点的过程中的回调函数，
//可以在cb中释放该节点的数据
void AL_destroy(PARRAYLIST pal, AL_removeCallBack cb);

//获取链表大小
int AL_size(PARRAYLIST pal);

//检测链表是不是空
boolean AL_isEmpty(PARRAYLIST pal);


#define _AL_GET(index, type) \
	(type)AL_get(index)

#define _AL_ADD(index, data) \
	AL_add(index, (int32)data)

#endif