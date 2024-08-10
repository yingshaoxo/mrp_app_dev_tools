/*
** gal.h: graphic absract layer header file.
**
** Copyright (C) 2012 JianbinZhu AS.  All rights reserved.
**
** 最后修改日期：2012/1/30
**
** 单独出来的图形绘制库，没有小字体封装，其他一样
** 使用前先调用 GAL_Initialize
*/

#ifndef _SGL_GAL_H
#define _SGL_GAL_H

#include "types.h"
#include "mrc_base.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/*** 刷新方式 **/
typedef enum
{
	PULLUP=0,
	PULLDOWN,
	CHECKUP,
	CHECKDOWN,
	UPDOWN
}REFSTYLE;

/*** GAL_ShadeFillRectEx 方式 **/
typedef enum
{
	SHADE_UPDOWN,		//从上到下
	SHADE_LEFTRIGHT,	//从左到右
	SHADE_DOWNUP,		//从下到上
	SHADE_RIGHTLEFT		//从右到左
}ORIENTATION;

/**
 * \常用的颜色
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


//------------------图形绘制区-----------------------------------------------------------------------
/**
 * 画水平线
 */
#define GAL_drawHLine(x, y, w, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)((x)+(w)-1), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * 画竖直线
 */
#define GAL_drawVLine(x, y, h, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)(x), (int16)((y)+(h)-1), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * 画一个点
 */
#define GAL_drawPixel(x, y, pixel) \
	mrc_drawPointEx((int16)(x), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * 2点画直线
 */
#define GAL_drawLine(x1, y1, x2, y2, pixel) \
	mrc_drawLine((int16)(x1), (int16)(y1), (int16)(x2), (int16)(y2), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * 画水平虚线
 */
#define GAL_drawDotHLine(x, y, w, pixel) \
	do{\
		int i; \
		for(i=0; i<w; i+=6) GAL_DrawHLine(x+i, y, 3, pixel); \
	}while(0)

/**
 * 画矩形边框
 */
#define GAL_drawRect(x, y, w, h, pixel) \
	do \
	{ \
		GAL_drawHLine(x, y, w, pixel); \
		GAL_drawHLine(x, y+h-1, w, pixel); \
		GAL_drawVLine(x, y, h, pixel); \
		GAL_drawVLine(x+w-1, y, h, pixel); \
	} while (0)
	
//画矩形边框，4个顶点不绘制，有圆弧效果
#define GAL_drawRect2(x, y, w, h, pixel) \
	do \
	{ \
	GAL_drawHLine(x+1, y, w-2, pixel); \
	GAL_drawHLine(x+1, y+h-1, w-2, pixel); \
	GAL_drawVLine(x, y+1, h-2, pixel); \
	GAL_drawVLine(x+w-1, y+1, h-2, pixel); \
	} while (0)
	
//画矩形边框，4个顶点不绘制，内部4个顶点
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
 * 填充矩形区域
 */
#define GAL_fillRect(x, y, w, h, pixel) \
	mrc_drawRect((int16)(x), (int16)(y), (int16)(w), (int16)(h), PIXEL888RED((pixel)), PIXEL888GREEN((pixel)), PIXEL888BLUE((pixel)))

/**
 * \以d为渐变值填充渐变矩形（带1个像素圆角）
 */
void GAL_fillRect2(int x, int y, int w, int h, Uint32 topclr, int d);

/**
 * \以d为渐变值填充渐变矩形
 */
void GAL_fillRect3(int x, int y, int w, int h, Uint32 topclr, int d);

/**
 * 渐变填充矩形区域
 * topclr：渐变起始色
 * d：渐变值
 * o：方向
 */
void GAL_shadeFillRectEx(int x, int y, int w, int h, uint8 AR, uint8 AG, uint8 AB, uint8 BR, uint8 BG, uint8 BB, ORIENTATION mode);
void GAL_shadeFillRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, ORIENTATION mode);

/**
 * \填充一个圆角矩形
 */
VOID GAL_fillRoundRect(int x, int y, int w, int h, Uint32 pixel);
//画圆角矩形边框
VOID GAL_drawRoundRect(int x, int y, int w, int h, Uint32 pixel);

/**
 * \或一个钩
 * \参数 x,y：钩子的交点坐标
 * \参数 d：钩子厚度
 */
VOID GAL_drawHook(int ax, int ay, int lw, int rw, int d, uint32 pixel );

//画渐变线
VOID GAL_shadeDrawHLine(int x, int y, int w, uint32 pixelA, uint32 pixelB);


//------------------半透明绘图区-----------------------------------------------------------------------
/**
 * \用指定半透明色填充区域
 */
#define GAL_effFillBox(x, y, w, h, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, (int16)w, (int16)h, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))

/**
 * \半透明屏幕区域
 */
#define GAL_effRegion(x_off, sy, sw, sh, alpha) \
	mrc_EffSetCon((int16)x_off, (int16)sy, (int16)sw, (int16)sh, (int16)alpha, (int16)alpha, (int16)alpha)

/**
 * \用指定半透明色画水平线
 */
#define GAL_effDrawHLine(x, y, w, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, (int16)w, 1, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))

/**
 * \用指定半透明色画竖直线
 */
#define GAL_effDrawVLine(x, y, h, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, 1, (int16)h, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))

/**
 * \用指定半透明色画一个像素点
 */
#define GAL_effDrawPixel(x, y, pixel) \
	mrc_EffSetCon((int16)x, (int16)y, 1, 1, (int16)PIXEL888RED(pixel), (int16)PIXEL888GREEN(pixel), (int16)PIXEL888BLUE(pixel))



//------------------刷新屏幕区----------------------------------------------------
/**
 * \brief Flush a region of the screen.
 * 刷新指定范围屏幕
 */
#define GAL_refreshScreen(x,  y,  w, h) \
	mrc_refreshScreen((int16)(x), (int16)(y), (uint16)(w), (uint16)(h))

/* *
 * \ 指定 style 刷新风格  
 * \（x, y） 刷新区域起始坐标
 * \(w, h)  刷新宽高
 */
VOID GAL_refreshScreenEx(REFSTYLE style, int16 x, int16 y, uint16 w, uint16 h);

/**
 * 用指定颜色清除整个屏幕
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
//干掉此函数，多占16K代码	
	//mrc_drawText((char*)pText, (int16)(x), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel), (int)isuni, (uint16)font)

#define GAL_textLineWidthHeight(pText, font, fw, fh) \
	mrc_unicodeTextWidthHeight((uint16*)(pText), (uint16)(font), (int32*)(fw), (int32*)(fh))

#define GAL_drawTextLine(pText, x, y, r, pixel, font) \
	{ \
		mr_colourSt cc ; \
		SET_COLOR(cc, pixel); \
		mrc_drawTextLeft((char*)(pText), (int16)(x), (int16)(y), r, cc, 0, font); \
	}

//------------------ 其他常用宏 ---------------------------
/**
 * \设置一个点的值
 */
#define SET_POINT(po, ax, ay) \
	do{\
		po.x = (int)ax; \
		po.y = (int)ay; \
	}while(0)

/**
 * 检查一个坐标是否在指定矩形内
 */
#define POINT_IS_INRECT(pr, x, y) \
	(x >= pr.left && x < pr.left + pr.width \
	&& y >= pr.top && y < pr.top + pr.height)

/**
 * 设置区域值1 
 * mr_screenRectSt r;
 */
#define SET_RECT1(r, ax, ay, aw, ah) \
	r.x = (uint16)(ax), r.y = (uint16)(ay), r.w = (uint16)(aw), r.h = (uint16)(ah)

/**
 * 设置区域值2 
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
 * 设置区域值3 
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
 * 设置颜色值 
 * mr_colourSt color;
 */
#define SET_COLOR(color, pixel)	\
	color.r = PIXEL888RED(pixel), color.g = PIXEL888GREEN(pixel), color.b = PIXEL888BLUE(pixel)

/** 
 * \反转颜色(255-原始) 
 */
#define REVE_COLOR(pixel)\
	RGB2PIXEL888((255-PIXEL888RED(pixel)), (255-PIXEL888GREEN(pixel)), (255-PIXEL888BLUE(pixel)))


//-------- 下面是兼容旧版的代码，最好用上面的 --------------------------------------------------
/**
 * \brief Fill the box with a specific color.
 * 用指定颜色填充空白区
 */
#define GAL_FillBox(gc, x, y, w, h, pixel) \
	GAL_fillRect(x, y, w, h, pixel)

/**
 * \以d为渐变值填充渐变矩形（带1个像素圆角）
 */
#define GAL_FillBox2(gc, x, y, w, h, pixel, d) \
	GAL_fillRect2(x, y, w, h, pixel, d)

/**
 * \以d为渐变值填充渐变矩形
 */
#define GAL_FillBox3(gc, x, y, w, h, pixel, d) \
	GAL_fillRect3(x, y, w, h, pixel, d)

/**
 * 渐变填充矩形区域
 * topclr：渐变起始色
 * d：渐变值
 * o：方向
 */
#define GAL_ShadeFillRectEx GAL_shadeFillRectEx
#define GAL_ShadeFillRect GAL_shadeFillRect

/**
 * 画矩形边框
 */
#define GAL_Rectangle(gc, x, y, w, h, pixel) \
	GAL_drawRect(x, y, w, h, pixel)
	
/**
 * 画一个特定边厚度度的矩形
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
 * \4个顶点不绘制
 */
#define GAL_Rectangle3(gc, x, y, w, h, pixel) \
	GAL_drawRect2(x, y, w, h, pixel)

/**
 * \4个顶点不绘制
 */
#define GAL_Rectangle4(gc, x, y, w, h, pixel) \
	GAL_drawRect3(x, y, w, h, pixel)

/**
 * 画水平线
 */
#define GAL_DrawHLine(gc, x, y, w, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)((x)+(w)-1), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * 画竖直线
 */
#define GAL_DrawVLine(gc, x, y, h, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)(x), (int16)((y)+(h)-1), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * 画一个像素点
 */
#define GAL_DrawPixel(gc, x, y, pixel) \
	mrc_drawPointEx((int16)(x), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \brief Daw a normal line.
 */
#define GAL_Line(gc, x1, y1, x2, y2, pixel) \
	mrc_drawLine((int16)(x1), (int16)(y1), (int16)(x2), (int16)(y2), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \画水平虚线
 */
#define GAL_DrawDotHLine(gc, x, y, w, pixel) \
	do{\
		int i; \
		for(i=0; i<w; i+=6) GAL_DrawHLine(gc, x+i, y, 3, pixel); \
	}while(0)

#define GAL_ShadeHLine(gc, x, y, w, pixA, pixB) \
	GAL_shadeDrawHLine(x, y, w, pixA, pixB)

//-------- 上面是兼容旧版的代码，最好用上面的 --------------------------------------------------


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _SGL_GAL_H */

