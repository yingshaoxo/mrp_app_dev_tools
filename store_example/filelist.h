
#ifndef __FILELIST__
#define __FILELIST__
/*
XGUI
������View�ؼ�
֧��������ɫ ���õ���¼�

Ӱ�Ӿ��ֲ���վ��www.yzjlb.net
�ֻ�C��̳��bbs.yzjlb.net
�ֻ�C����QQȺ��606757263

*/
#include "mrc_base.h"
#include "mpc.h"
typedef struct{
    char *path;
    char DisplayName[25]; //28:52
    char Desc[65]; //128:192
    int isDownload; //�Ƿ��������ļ�
    int isUTF;
} MRPHEADER;

typedef struct {
  char *name;           //�ؼ�����
  int id;               //�ؼ�id
  int isshow;           //�Ƿ���ʾ ����ʾʱ������ִ�е���¼�
  int isdown;           //�Ƿ���
  int x;                //λ��x
  int y;                //λ��y
  int w;                //���
  int h;                //�߶�
  int backgroundColor;  //����ɫ
  int draw_mode;        //ˢ��ģʽ
  int32 timer;          //��ʱ�����
  void *onclick;        //����¼��ص�����
    MRPHEADER **list; //list�б�
    int list_windowsize; //һ���������ɵ�item��
    int list_max; //�б���󳤶�
    int list_len; //��ǰ�б���
    int list_index; //���λ��
    int list_progress; //list��ʾ���� �ǰٷֱ� ����len����ʾ����

    //������mrp��Ϣ
    
} ListView;

void *list_create(int x, int y, int w, int h);
void list_setid(ListView *view, int id);
int list_getid(ListView *view);
void list_draw(ListView *view);
void list_setMode(ListView *view, int mode) ;
void list_show(ListView *view);
void list_hide(ListView *view);
int list_isshow(ListView *view);
void list_setBackgroundColor(ListView *view, int color);
void list_setonclick(ListView *view, void *click);
void list_add(ListView *view, char *filename);
void list_clear(ListView *view);
void list_addData(ListView *view, char *label,char *desc,char *downloadUrl);
void list_find_txt(ListView *view, char *file);
int list_event(ListView *view, int32 type, int32 p1, int32 p2);
void list_free(ListView *view);
MRPHEADER *list_getCurItem(ListView *list);

#endif
