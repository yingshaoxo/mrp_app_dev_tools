#include "mrc_base.h"

int32 SCRW,SCRH;
//sound
enum{_MIDI=1,_WAVE,_MP3,_AMR,_PCM,_M4A,_AMR_WB};

//��������ֵ��δ�������������Ҳ����ʹ�ã�����֪�����ֵ��
enum {  
   _0,           //���� 0
   _1,           //���� 1
   _2,           //���� 2
   _3,           //���� 3
   _4,           //���� 4
   _5,           //���� 5
   _6,           //���� 6
   _7,           //���� 7
   _8,           //���� 8
   _9,           //���� 9
   _STAR,        //���� *
   _POUND,       //���� #
   _UP,          //���� ��
   _DOWN,        //���� ��
   _LEFT,        //���� ��
   _RIGHT,       //���� ��
   _SLEFT=17,    //���� �����
   _SRIGHT,      //���� �����
   _SEND,        //���� ������
   _SELECT       //���� ȷ��/ѡ����������м���ȷ�ϼ���������Ϊ�ü���
};

//�����¼��������¼����Լ����壩
enum {
    KY_DOWN,     //��������
    KY_UP,       //�����ͷ�
    MS_DOWN,     //��갴��
    MS_UP,       //����ͷ�
    MN_SLT, //�˵�ѡ��
    MN_RET, //�˵�����
    MR_DIALOG, //�Ի���
    MS_MOVE=12   //����ƶ�
};

enum {
    DLG_OK,         //�Ի���/�ı���ȵ�"ȷ��"�������(ѡ��)
    DLG_CANCEL  //�Ի���/�ı���ȵ�"ȡ��"("����")�������(ѡ��)
};

enum
{
    SEEK_SET,             //���ļ���ʼ��ʼ
    SEEK_CUR,             //�ӵ�ǰλ�ÿ�ʼ
    SEEK_END             //���ļ�ĩβ��ʼ
};
enum
{
    IS_FILE=1,      //�ļ�
    IS_DIR=2,      //Ŀ¼
    IS_INVALID=8  //��Ч(���ļ�����Ŀ¼)
};

typedef struct {
    uint16            x;
    uint16            y;
    uint16            w;
    uint16            h;
}rectst;

typedef struct {
    uint8            r;
    uint8            g;
    uint8            b;
}colorst;



enum
{
    SHADE_UPDOWN,       //���ϵ���
    SHADE_LEFTRIGHT,    //������
    SHADE_DOWNUP,       //���µ���
    SHADE_RIGHTLEFT     //���ҵ���
};

void mpc_init()
{
    mr_screeninfo screen;

    mrc_getScreenInfo(&screen);
    SCRW = screen.width;
    SCRH = screen.height;
}

void *readmrpfile(char *filename,int32 *len)
{
return mrc_readFileFromMrp (filename,len,0);


}

void freefiledata(void *data,int32 filelen)
{
 mrc_freeFileData(data,(int32)filelen);
}


void ShadeRect(int x, int y, int w, int h, int AR, int AG, int AB, int BR, int BG, int BB, int mode)
{
    int16 i,j,t;

    BR-=AR; BG-=AG; BB-=AB;
    switch(mode)
    {
    case SHADE_UPDOWN:
        t=x+w-1;
        for(i=0;     i<h;     i++)
            mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*i/h),(uint8)(AG+BG*i/h),(uint8)(AB+BB*i/h));
        return;
    case SHADE_DOWNUP:
        t=x+w-1;
        for(i=h-1,j=0;    i>=0;    i--,j++)
            mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*j/h),(uint8)(AG+BG*j/h),(uint8)(AB+BB*j/h));
        return;
    case SHADE_LEFTRIGHT:
        t=y+h-1;
        for(i=0;     i<w;     i++)
            mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*i/w),(uint8)(AG+BG*i/w),(uint8)(AB+BB*i/w));
        return;
    case SHADE_RIGHTLEFT:
        t=y+h-1;
        for(i=w-1,j=0;    i>=0;    i--,j++)
            mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*j/w),(uint8)(AG+BG*j/w),(uint8)(AB+BB*j/w));
        return;
    }
}

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode)
{
    //RGBA+(RGBB-RGBA)*N/Step
    int32 AR,AG,AB;
    int32 BR,BG,BB;

    AR = (uint8)(pixelA >> 16) & 0xff;
    AG = (uint8)(pixelA >> 8) & 0xff;
    AB = (uint8)(pixelA) & 0xff;

    BR = (uint8)((pixelB >> 16) & 0xff);
    BG = (uint8)((pixelB >> 8) & 0xff);
    BB = (uint8)((pixelB) & 0xff);
    ShadeRect(x,y,w,h,AR,AG,AB,BR,BG,BB,mode);

}

