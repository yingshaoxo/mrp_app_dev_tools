/** arraylist.c implement of arraylist.h
 ** 
 ** 单向可变长度线性链表 工具
 **
 ** 创建日期：2012/2/16
 ** 最后修改日期：2012/2/17
 **/

#include "arraylist.h"


PARRAYLIST AL_create(void) {
	PARRAYLIST pal;

	pal = malloc(sizeof(ARRAYLIST));
	memset(pal, 0, sizeof(ARRAYLIST));

	return pal;
}

void AL_add(PARRAYLIST pal, int32 data) {
	PNODE p, p2;

	if(!pal) return;

	p = malloc(sizeof(NODE));
	memset(p, 0, sizeof(NODE));

	p->data = data;

	if(pal->size == 0){
		pal->head = p;
	}else{
		p2 = pal->head;
		while(p2->next) p2 = p2->next;
		p2->next = p;
	}

	pal->size++;
}

int32 AL_remove(PARRAYLIST pal, int index){
	PNODE del, p;
	int32 data;

	if(!pal || pal->size == 0) return 0;

	if(index == 0){
		del = pal->head;
		pal->head = del->next;
	}else if(index == pal->size-1){
		p = pal->head;
		while(p->next->next) p = p->next;
		del = p->next;
		p->next = NULL;
	}else{
		int i;

		p = pal->head;
		for(i=0; i<index-1; i++) p = p->next;
		del = p->next;
		p->next = del->next;
	}

	data = del->data;
	free(del);
	pal->size--;

	return data;
}

int32 AL_get(PARRAYLIST pal, int index){
	int i;
	PNODE p;

	if(!pal || index > pal->size-1) return 0;

	p = pal->head;
	for(i=0; i<index; i++)
		p = p->next;

	return p->data;
}

void AL_destroy(PARRAYLIST pal, AL_removeCallBack cb){
	PNODE p, p2;

	if(!pal) return;

	p = pal->head;
	while(p){
		p2 = p->next;
		//...
		if(cb) cb(p->data);
		free(p);
		p = p2;
	}
	free(pal);
}

int AL_size(PARRAYLIST pal){
	if(!pal) return -1;
	return pal->size;
}

boolean AL_isEmpty(PARRAYLIST pal){
	if(!pal) return true;
	return (0 == pal->size);
}