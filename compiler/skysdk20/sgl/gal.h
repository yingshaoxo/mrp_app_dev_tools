/*
** gal.h: graphic absract layer header file.
**
** Copyright (C) 2012 JianbinZhu AS.  All rights reserved.
**
** ����޸����ڣ�2012/1/30
**
** ����������ͼ�λ��ƿ⣬û��С�����װ������һ��
** ʹ��ǰ�ȵ��� GAL_Initialize
*/

#ifndef _SGL_GAL_H
#define _SGL_GAL_H

#include "types.h"
#include "mrc_base.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/*** ˢ�·�ʽ **/
typedef enum
{
	PULLUP=0,
	PULLDOWN,
	CHECKUP,
	CHECKDOWN,
	UPDOWN
}REFSTYLE;

/*** GAL_ShadeFillRectEx ��ʽ **/
typedef enum
{
	SHADE_UPDOWN,		//���ϵ���
	SHADE_LEFTRIGHT,	//������
	SHADE_DOWNUP,		//���µ���
	SHADE_RIGHTLEFT		//���ҵ���
}ORIENTATION;

/**
 * \���õ���ɫ
 */
#define COLOR_transparent	0x00000000
#define COLOR_darkred		0x00800000
#define COLOR_darkgreen		0x00008000
#define COLOR_darkyellow	0x00808000
#define COLOR_darkblue		0x00000080
#define COLOR_darkmagenta	0x00800080
#define COLOR_darkcyan		0x00008080
#define COLOR_lightgray		0x00c0c0c0
#define COLOR_darkgray		0x00808080
#define COLOR_red			0x00ff0000
#define COLOR_green			0x0000ff00
#define COLOR_yellow		0x00ffff00
#define COLOR_blue			0x000000f0
#define COLOR_magenta		0x008080ff
#define COLOR_cyan			0x0000ffff
#define COLOR_lightwhite	0x00ffffff
#define COLOR_black			0x00000000


//------------------ͼ�λ�����-----------------------------------------------------------------------
/**
 * ��ˮƽ��
 */
#define GAL_drawHLine(x, y, w, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)((x)+(w)-1), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * ����ֱ��
 */
#define GAL_drawVLine(x, y, h, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)(x), (int16)((y)+(h)-1), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * ��һ����
 */
#define GAL_drawPixel(x, y, pixel) \
	mrc_drawPointEx((int16)(x), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * 2�㻭ֱ��
 */
#define GAL_drawLine(x1, y1, x2, y2, pixel) \
	mrc_drawLine((int16)(x1), (int16)(y1), (int16)(x2), (int16)(y2), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * ��ˮƽ����
 */
#define GAL_drawDotHLine(x, y, w, pixel) \
	do{\
		int i; \
		for(i=0; i<w; i+=6) GAL_DrawHLine(x+i, y, 3, pixel); \
	}while(0)

/**
 * �����α߿�
 */
#define GAL_drawRect(x, y, w, h, pixel) \
	do \
	{ \
		GAL_drawHLine(x, y, w, pixel); \
		GAL_drawHLine(x, y+h-1, w, pixel); \
		GAL_drawVLine(x, y, h, pixel); \
		GAL_drawVLine(x+w-1, y, h, pixel); \
	} while (0)
	
//�����α߿�4�����㲻���ƣ���Բ��Ч��
#define GAL_drawRect2(x, y, w, h, pixel) \
	do \
	{ \
	GAL_drawHLine(x+1, y, w-2, pixel); \
	GAL_drawHLine(x+1, y+h-1, w-2, pixel); \
	GAL_drawVLine(x, y+1, h-2, pixel); \
	GAL_drawVLine(x+w-1, y+1, h-2, pixel); \
	} while (0)
	
//�����α߿�4�����㲻���ƣ��ڲ�4������
#define GAL_drawRect3(x, y, w, h, pixel) \
	do \
	{ \
	GAL_drawHLine(x+1, y, w-2, pixel); \
	GAL_drawHLine(x+1, y+h-1, w-2, pixel); \
	GAL_drawVLine(x, y+1, h-2, pixel); \
	GAL_drawVLine(x+w-1, y+1, h-2, pixel); \
	GAL_drawPixel(x+1, y+1, pixel); \
	GAL_drawPixel(x+1, y+h-2, pixel); \
	GAL_drawPixel(x+w-2, y+1, pixel); \
	GAL_drawPixel(x+w-2, y+h-2, pixel); \
	} while (0)

/**
 * ����������
 */
#define GAL_fillRect(x, y, w, h, pixel) \
	mrc_drawRect((int16)(x), (int16)(y), (int16)(w), (int16)(h), PIXEL888RED((pixel)), PIXEL888GREEN((pixel)), PIXEL888BLUE((pixel)))

/**
 * \��dΪ����ֵ��佥����Σ���1������Բ�ǣ�
 */
void GAL_fillRect2(int x, int y, int w, int h, Uint32 topclr, int d);

/**
 * \��dΪ����ֵ��佥�����
 */
void GAL_fillRect3(int x, int y, int w, int h, Uint32 topclr, int d);

/**
 * ��������������
 * topclr��������ʼɫ
 * d������ֵ
 * o������
 */
void GAL_shadeFillRectEx(int x, int y, int w, int h, uint8 AR, uint8 AG, uint8 AB, uint8 BR, uint8 BG, uint8 BB, ORIENTATION mode);
void GAL_shadeFillRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, ORIENTATION mode);

/**
 * \���һ��Բ�Ǿ���
 */
VOID GAL_fillRoundRect(int x, int y, int w, int h, Uint32 pixel);
//��Բ�Ǿ��α߿�
VOID GAL_drawRoundRect(int x, int y, int w, int h, Uint32 pixel);

/**
 * \��һ����
 * \���� x,y�����ӵĽ�������
 * \���� d�����Ӻ��
 */
VOID GAL_drawHook(int ax, int ay, int lw, int rw, int d, uint32 pixel );

//��������
VOID GAL_shadeDrawHLine(int x, int y, int w, uint32 pixelA, uint32 pixelB);


//------------------��͸����ͼ��-----------------------------------------------------------------------
/**
 * \��ָ����͸��ɫ�������
 */
#define GAL_effFillBox(x, y, w, h, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, (int16)w, (int16)h, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))

/**
 * \��͸����Ļ����
 */
#define GAL_effRegion(x_off, sy, sw, sh, alpha) \
	mrc_EffSetCon((int16)x_off, (int16)sy, (int16)sw, (int16)sh, (int16)alpha, (int16)alpha, (int16)alpha)

/**
 * \��ָ����͸��ɫ��ˮƽ��
 */
#define GAL_effDrawHLine(x, y, w, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, (int16)w, 1, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))

/**
 * \��ָ����͸��ɫ����ֱ��
 */
#define GAL_effDrawVLine(x, y, h, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, 1, (int16)h, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))

/**
 * \��ָ����͸��ɫ��һ�����ص�
 */
#define GAL_effDrawPixel(x, y, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, 1, 1, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))



//------------------ˢ����Ļ��----------------------------------------------------
/**
 * \brief Flush a region of the screen.
 * ˢ��ָ����Χ��Ļ
 */
#define GAL_refreshScreen(x,  y,  w, h) \
	mrc_refreshScreen((int16)(x), (int16)(y), (uint16)(w), (uint16)(h))

/* *
 * \ ָ�� style ˢ�·��  
 * \��x, y�� ˢ��������ʼ����
 * \(w, h)  ˢ�¿��
 */
VOID GAL_refreshScreenEx(REFSTYLE style, int16 x, int16 y, uint16 w, uint16 h);

/**
 * ��ָ����ɫ���������Ļ
 */
#define GAL_clearScreen(pixel) \
	mrc_clearScreen(PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));

#define GAL_drawText(pText, x,y, pixel, font) \
	{ \
		mr_screenRectSt ar; \
		int32 fw, fh; \
		GAL_textLineWidthHeight(pText, font, &fw, &fh); \
		ar.x = x, ar.y = y, ar.h = fh, ar.w = fw; \
		GAL_drawTextLine(pText, x, y, ar, pixel, font); \
	}
//�ɵ��˺�������ռ16K����	
	//mrc_drawText((char*)pText, (int16)(x), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel), (int)isuni, (uint16)font)

#define GAL_textLineWidthHeight(pText, font, fw, fh) \
	mrc_unicodeTextWidthHeight((uint16*)(pText), (uint16)(font), (int32*)(fw), (int32*)(fh))

#define GAL_drawTextLine(pText, x, y, r, pixel, font) \
	{ \
		mr_colourSt cc ; \
		SET_COLOR(cc, pixel); \
		mrc_drawTextLeft((char*)(pText), (int16)(x), (int16)(y), r, cc, 0, font); \
	}

//------------------ �������ú� ---------------------------
/**
 * \����һ�����ֵ
 */
#define SET_POINT(po, ax, ay) \
	do{\
		po.x = (int)ax; \
		po.y = (int)ay; \
	}while(0)

/**
 * ���һ�������Ƿ���ָ��������
 */
#define POINT_IS_INRECT(pr, x, y) \
	(x >= pr.left && x < pr.left + pr.width \
	&& y >= pr.top && y < pr.top + pr.height)

/**
 * ��������ֵ1 
 * mr_screenRectSt r;
 */
#define SET_RECT1(r, ax, ay, aw, ah) \
	r.x = (uint16)(ax), r.y = (uint16)(ay), r.w = (uint16)(aw), r.h = (uint16)(ah)

/**
 * ��������ֵ2 
 * RECT r;
 */
#define SET_RECT2(r, ax, ay, aw, ah)	\
	r.left = (int)(ax), r.top = (int)(ay), r.width = (int)(aw), r.height = (int)(ah)

#define SET_RECT21(r, al, at, ar, ab)	\
	do{	\
	r.left = (int)(al);	\
	r.top = (int)(at); 	\
	r.width = (int)(ar-al); 	\
	r.height = (int)(ab-at);	\
	}while(0)

/**
 * ��������ֵ3 
 * skyfont_screenRect_t r;
 */
#define SET_RECT3(r, ax, ay, aw, ah)	\
	do{	\
		r.x = (int16)(ax);	\
		r.y = (int16)(ay); 	\
		r.w = (int16)(aw); 	\
		r.h = (int16)(ah);	\
	}while(0)

/* 
 * ������ɫֵ 
 * mr_colourSt color;
 */
#define SET_COLOR(color, pixel)	\
	color.r = PIXEL888RED(pixel), color.g = PIXEL888GREEN(pixel), color.b = PIXEL888BLUE(pixel)

/** 
 * \��ת��ɫ(255-ԭʼ) 
 */
#define REVE_COLOR(pixel)\
	RGB2PIXEL888((255-PIXEL888RED(pixel)), (255-PIXEL888GREEN(pixel)), (255-PIXEL888BLUE(pixel)))


//-------- �����Ǽ��ݾɰ�Ĵ��룬���������� --------------------------------------------------
/**
 * \brief Fill the box with a specific color.
 * ��ָ����ɫ���հ���
 */
#define GAL_FillBox(gc, x, y, w, h, pixel) \
	GAL_fillRect(x, y, w, h, pixel)

/**
 * \��dΪ����ֵ��佥����Σ���1������Բ�ǣ�
 */
#define GAL_FillBox2(gc, x, y, w, h, pixel, d) \
	GAL_fillRect2(x, y, w, h, pixel, d)

/**
 * \��dΪ����ֵ��佥�����
 */
#define GAL_FillBox3(gc, x, y, w, h, pixel, d) \
	GAL_fillRect3(x, y, w, h, pixel, d)

/**
 * ��������������
 * topclr��������ʼɫ
 * d������ֵ
 * o������
 */
#define GAL_ShadeFillRectEx GAL_shadeFillRectEx
#define GAL_ShadeFillRect GAL_shadeFillRect

/**
 * �����α߿�
 */
#define GAL_Rectangle(gc, x, y, w, h, pixel) \
	GAL_drawRect(x, y, w, h, pixel)
	
/**
 * ��һ���ض��ߺ�ȶȵľ���
 */
#define GAL_Rectangle2(gc, x, y, w, h, line, pixel) \
	do \
	{ \
	int i;\
	for(i=0; i<line; i++){ \
		GAL_drawRect(x++, y++, w, h); \
		w -= 2, h -= 2; }\
	} while (0)
	
/**
 * \4�����㲻����
 */
#define GAL_Rectangle3(gc, x, y, w, h, pixel) \
	GAL_drawRect2(x, y, w, h, pixel)

/**
 * \4�����㲻����
 */
#define GAL_Rectangle4(gc, x, y, w, h, pixel) \
	GAL_drawRect3(x, y, w, h, pixel)

/**
 * ��ˮƽ��
 */
#define GAL_DrawHLine(gc, x, y, w, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)((x)+(w)-1), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * ����ֱ��
 */
#define GAL_DrawVLine(gc, x, y, h, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)(x), (int16)((y)+(h)-1), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * ��һ�����ص�
 */
#define GAL_DrawPixel(gc, x, y, pixel) \
	mrc_drawPointEx((int16)(x), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \brief Daw a normal line.
 */
#define GAL_Line(gc, x1, y1, x2, y2, pixel) \
	mrc_drawLine((int16)(x1), (int16)(y1), (int16)(x2), (int16)(y2), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \��ˮƽ����
 */
#define GAL_DrawDotHLine(gc, x, y, w, pixel) \
	do{\
		int i; \
		for(i=0; i<w; i+=6) GAL_DrawHLine(gc, x+i, y, 3, pixel); \
	}while(0)

#define GAL_ShadeHLine(gc, x, y, w, pixA, pixB) \
	GAL_shadeDrawHLine(x, y, w, pixA, pixB)

//-------- �����Ǽ��ݾɰ�Ĵ��룬���������� --------------------------------------------------


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _SGL_GAL_H */

