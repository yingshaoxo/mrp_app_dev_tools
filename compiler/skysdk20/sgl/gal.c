#include "debug.h"
#include "types.h"
#include "gal.h"



void printmem(void)
{
	int size, i1,i2,i3,i4;
	
	size = mrc_getMemStatus(&i1, &i2, NULL, NULL, &i3, &i4);
	FPRINTF("�ڴ���Ϣ��i1=%d, i2=%d, i3=%d, i4=%d", 
		i1, i2, i3, i4);
}

//��dΪ����ֵ��佥����Σ���1������Բ�ǣ�
void GAL_fillRect2(int x, int y, int w, int h, Uint32 topclr, int d)
{
	int i;
	uint8 r, g, b;

	r = PIXEL888RED(topclr);
	g = PIXEL888GREEN(topclr);
	b = PIXEL888BLUE(topclr);

	mrc_drawLine(x+1, y, x+w-2, y, r, g, b);
	r -= d, g -= d, b -= d; 
	//����ɫ
	for(i=1; i<h-1; i++)
	{
		mrc_drawLine(x, y+i, x+w-1, y+i, r, g, b);
		r -= d, g -= d, b -= d; 
	}
	mrc_drawLine(x+1, y+h-1, x+w-2, y+h-1, r, g, b);
}

//��dΪ����ֵ��佥�����
void GAL_fillRect3(int x, int y, int w, int h, Uint32 topclr, int d)
{
	int i;
	uint8 r, g, b;

	r = PIXEL888RED(topclr);
	g = PIXEL888GREEN(topclr);
	b = PIXEL888BLUE(topclr);

	//����ɫ
	for(i=0; i<h; i++)
	{
		mrc_drawLine(x, y+i, x+w-1, y+i, r, g, b);
		r -= d, g -= d, b -= d; 
	}
}

void GAL_shadeFillRectEx(int x, int y, int w, int h, 
						 uint8 AR, uint8 AG, uint8 AB, 
						 uint8 BR, uint8 BG, uint8 BB, 
						 ORIENTATION mode)
{
	int i;
	int dr, dg, db;

	dr = BR-AR, dg = BG - AG, db = BB - AB;

	switch(mode)
	{
	case SHADE_UPDOWN:
		for(i=0; i<h; i++)
			mrc_drawLine(x, y+i, x+w-1, y+i, (AR+dr*i/h), (AG+dr*i/h), (AB+dr*i/h));
		break;
	case SHADE_DOWNUP:
		for(i=0; i<h; i++)
			mrc_drawLine(x, y+i, x+w-1, y+i, (BR-dr*i/h), (BG-dg*i/h), (BB-db*i/h));
		break;
	case SHADE_LEFTRIGHT:
		for(i=0; i<w; i++)
			mrc_drawLine(x+i, y, x+i, y+h-1, (AR+dr*i/w), (AG+dg*i/w), (AB+db*i/w));
		break;
	case SHADE_RIGHTLEFT:
		for(i=0; i<w; i++)
			mrc_drawLine(x+i, y, x+i, y+h-1, (BR-dr*i/w), (BG-dg*i/w), (BB-db*i/w));
		break;
	}
}

void GAL_shadeFillRect(int16 x, int16 y, int16 w, int16 h, 
					   uint32 pixelA, uint32 pixelB, 
					   ORIENTATION mode)
{
	uint8 AR,AG,AB;
	uint8 BR,BG,BB;

	AR = PIXEL888RED(pixelA);
	AG = PIXEL888GREEN(pixelA);
	AB = PIXEL888BLUE(pixelA);

	BR = PIXEL888RED(pixelB);
	BG = PIXEL888GREEN(pixelB);
	BB = PIXEL888BLUE(pixelB);

	GAL_shadeFillRectEx(x,y,w,h,AR,AG,AB,BR,BG,BB,mode);
}

//���Բ�Ǿ���
VOID GAL_fillRoundRect(int x, int y, int w, int h, Uint32 pixel)
{
	uint8 r, g, b;

	r = PIXEL888RED(pixel);
	g = PIXEL888GREEN(pixel);
	b = PIXEL888BLUE(pixel);

	GAL_drawHLine(x+4, y, w-8, pixel);
	mrc_drawPointEx(x+3, y, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-4, y, r-100, g-100, b-100);

	GAL_drawHLine(x+2, y+1, w-4, pixel);
	//GAL_EffDrawHLine(gc, x+2, y, w-4, 0x808080);

	GAL_drawHLine(x+1, y+2, w-2, pixel);
	//GAL_EffDrawHLine(gc, x+1, y+1, w-2, 0x909090);

	GAL_drawHLine(x+1, y+3, w-2, pixel);
	mrc_drawPointEx(x, y+3, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-1, y+3, r-100, g-100, b-100);

	GAL_fillRect(x, y+4, w, h-8, pixel);
	
	y = y+h-4;
	GAL_drawHLine(x+4, y+3, w-8, pixel);
	mrc_drawPointEx(x+3, y+3, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-4, y+3, r-100, g-100, b-100);
	GAL_drawHLine(x+2, y+2, w-4, pixel);
	GAL_drawHLine(x+1, y+1, w-2, pixel);
	GAL_drawHLine(x+1, y, w-2, pixel);
	mrc_drawPointEx(x, y, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-1, y, r-100, g-100, b-100);	
}

//Բ�Ǿ��α߿�
VOID GAL_drawRoundRect(int x, int y, int w, int h, Uint32 pixel){
	uint8 r, g, b;

	r = PIXEL888RED(pixel);
	g = PIXEL888GREEN(pixel);
	b = PIXEL888BLUE(pixel);

	GAL_drawRect3(x+1, y+1, w-2, h-2, pixel);

	GAL_drawHLine(x+4, y, w-8, pixel);
	mrc_drawPointEx(x+3, y, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-4, y, r-100, g-100, b-100);

	mrc_drawPointEx(x, y+3, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-1, y+3, r-100, g-100, b-100);
	GAL_drawVLine(x, y+4, h-8, pixel);
	GAL_drawVLine(x+w-1, y+4, h-8, pixel);

	y = y+h-4;
	GAL_drawHLine(x+4, y+3, w-8, pixel);
	mrc_drawPointEx(x+3, y+3, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-4, y+3, r-100, g-100, b-100);
	
	mrc_drawPointEx(x, y, r-100, g-100, b-100);
	mrc_drawPointEx(x+w-1, y, r-100, g-100, b-100);	
}

VOID GAL_drawHook(int ax, int ay, int lw, int rw, int d, uint32 pixel )
{
	while(d > 0)
	{
		GAL_drawHLine(ay, ax-lw, ay-lw, pixel);
		GAL_drawHLine(ay, ax+rw, ay-rw, pixel);
		d--;
		ay--;
	}
}

//���ع���ˮƽ��
VOID GAL_shadeDrawHLine(int x, int y, int w, uint32 pixelA, uint32 pixelB){
	int i;
	uint8 r, g, b;
	uint8 dr, dg, db;

	r = PIXEL888RED(pixelA);
	g = PIXEL888GREEN(pixelA);
	b = PIXEL888BLUE(pixelA);

	dr = (PIXEL888RED(pixelB) - r)/(w/2);	//A->B
	dg = (PIXEL888GREEN(pixelB) - g)/(w/2);
	db = (PIXEL888BLUE(pixelB) - b)/(w/2);

	for (i=0; i<w/2; i++){
		mrc_drawPointEx(x+i, y, r, g, b);
		mrc_drawPointEx(x+w-1-i, y, r, g, b);
		r+=dr, g+=dg, b+=db;
	}
	if(0 != MOD(w,2))
		mrc_drawPointEx(x+(w/2), y, r, g, b);
}

//����Ч��ʱ��ÿ������ĸ߶�
#define CHECKHEIGHT 30
//ÿ��ˢ�����ظ߶�
#define PIXEL 1
/* *
 * \ ָ�� style ˢ�·��  
 * \��x, y�� ˢ��������ʼ����
 * \(w, h)  ˢ�¿��
 */
VOID GAL_refreshScreenEx(REFSTYLE style, int16 x, int16 y, uint16 w, uint16 h)
{
	int32 i, j;

	switch (style)
	{
	case PULLUP:   //��������Ч��
	{
		for(i=0; i<(int32)h; i++)
		{
			mrc_refreshScreen(x, (int16)i+y, w, PIXEL);
		}
		break;
	}

	case PULLDOWN: //��������Ч��
	{
		for(i=(int32)h; i>0; i--)
		{
			mrc_refreshScreen(x, (int16)i+y, w, PIXEL);
		}
		break;
	}

	case CHECKUP: //��������Ч��
	{
		for(i=CHECKHEIGHT; i>0; i--)
		{
			for(j=0; j< ( (int32)h / CHECKHEIGHT) ; j++)  
			{
				mrc_refreshScreen(x, y+(int16)j*CHECKHEIGHT+(int16)i, w, PIXEL);
				if(  (( (int32)h / CHECKHEIGHT)*CHECKHEIGHT) <h)  //�ж����������Ƿ�����
				{
					mrc_refreshScreen(x, (( (int16)h / CHECKHEIGHT)*CHECKHEIGHT)+(int16)i, w, PIXEL);
				}
			}
		}
		break;
	}

	case CHECKDOWN:  //��������Ч��
	{
		for(i=0; i<CHECKHEIGHT; i++)
		{
			for(j=0; j< ( (int32)h / CHECKHEIGHT) ; j++)  
			{
				mrc_refreshScreen(x, y+(int16)j*CHECKHEIGHT+(int16)i, w, PIXEL);
				if(  (( (int32)h / CHECKHEIGHT)*CHECKHEIGHT) <h)  //�ж������Ƿ�����
				{
					mrc_refreshScreen(x, (( (int16)h / CHECKHEIGHT)*CHECKHEIGHT)+(int16)i, w, PIXEL);
				}
			}
		}
		break;
	}

	case UPDOWN:   //����ͬʱ����
	{
		for(i=(x+h/2); i<(int32)h; i++)
		{
			mrc_refreshScreen(x, (int16)h-(int16)i+y, w, PIXEL);
			mrc_refreshScreen(x, (int16)i+y, w, PIXEL);    //��
		}
		break;
	}

	}
}