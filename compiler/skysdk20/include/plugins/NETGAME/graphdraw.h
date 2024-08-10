#ifndef _GRAPHDRAW_H_
#define _GRAPHDRAW_H_

#include "mrc_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct  
{
	int16 x;	//��ͼ������Ļ��ʼ��ĺ�����
	int16 y;	//��ͼ������Ļ��ʼ���������
	int16 w;	//�����ص�ͼƬ���
	int16 h;	//�����ص�ͼƬ�߶�
	int16 mw;	//ԴͼƬ��ԭʼ���
	int16 sx;	//��ԴͼƬ���ص���ʼ���
	int16 sy;	//��ԴͼƬ���ص���ʼ�߶�
}drawBmpParamSt;

typedef struct  
{
	int16 x;	//����ʾ���ı����ϽǺ�����
	int16 y;	//����ʾ���ı����Ͻ�������
	mr_screenRectSt rect;	//��ʾ���ַ����Ʒ�Χ
}drawTextParamSt;

typedef struct 
{
	int16 x;
	int16 y;
	uint16 mw;
	uint16 w;
	uint16 h;
	uint16 rop;
	int16 sx;
	int16 sy;
	uint16 transcolor;
}GifDecodeParaSt;

enum
{
	GIF2SCREEN,
	GIF2BMP16,
	GIF2BMP256
};

/* ����graphdraw��ͼģ��,���ص�cache */
int32 mrc_graphdraw_cacheLoad(void);

/* ����graphdraw��ͼģ�� */
int32 mrc_graphdraw_load(void);

int32 mrc_graphdraw_reload(void);

/* ж��graphdraw��ͼģ�� */
int32 mrc_graphdraw_unload(void);

/* ����ӿ� */
void mrc_graphdraw_drawPoint(int32 x, int32 y, uint16 color565);

/* ���߽ӿ�,�Ի�ֱ�߻��б�������Ż�,(x1,y1)-->(x2,y2); */
void mrc_graphdraw_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint16 color565);

/* ����������,������ɫ�������,x,yΪ�������Ͻ���ʼ��λ��,w,h�ֱ�Ϊ���ο�Ⱥ͸߶� */
void mrc_graphdraw_drawRect(int32 x, int32 y, int32 w, int32 h, uint16 color565);

/* ����������,����RGB��ɫ���,x,yΪ�������Ͻ���ʼ��λ��,w,h�ֱ�Ϊ���ο�Ⱥ͸߶� */
void mrc_graphdraw_drawRectRGB(mr_screenRectSt* rect, uint8 r, uint8 g, uint8 b);
/* 
 * �������ľ��κ���
 * color565������ʾ�ò�����ҪRGB565��ʽ����ɫ,����ʹ������ĺ���ת��
 * �μ�SGL_common.h
 * 1.RGB2PIXEL565
 * 2.COLOR2PIXEL565
 */

 /* �����α߿�,�߿���ɫ����color565,x,yΪ�������Ͻ���ʼ��λ��,w,h�ֱ�Ϊ���ο�Ⱥ͸߶� */
void mrc_graphdraw_drawRectangle(int32 x, int32 y, int32 w, int32 h, uint16 color565);

/*
 * [��mrc_drawEffSetCon�����ļ��ٰ�]
 * ����Ļ�����Ͻ�x,y�����Ϊw,h�ľ��������ڵ�r,g,b�ֱ�
 * ��ǿperr/256, perg/256, perb/256����
 * ��perr=256, perg=0, perb=0����ֻ�������������ڵĺ�ɫ����
 * perr=perg= perb=128����ʹ���������ڲ���һ�ְ�͸����Ч����
 * ����:x,y,w,h	��Ļλ�á�perr, perg, perb	r,g,b�仯����ֵ��
 */
void  mrc_graphdraw_drawEffSetCon(mr_screenRectSt *pRect, int32 perr, int32 perg, int32 perb);
/*
λͼ��ͼ�ӿ�,���Ǽ��ٻ�ͼ�ӿ�

����˵��: 
p: 16λλͼ����;
pRectParam: ��ͼ������Ϣ;
rop: ��ͼ��������:
BM_OR,			//SRC .OR. DST*   	��͸��Ч��
BM_XOR, 		//SRC .XOR. DST*	������
BM_COPY,		//DST = SRC*		����/����
BM_NOT,			//DST = (!SRC)*		ȡ������
BM_MERGENOT,	//DST .OR. (!SRC)	���ȡ������
BM_ANDNOT,     	//DST .AND. (!SRC)	���ȡ������
BM_TRANSPARENT,	//͸��ɫ����ʾ��ͼƬ�ĵ�һ�����أ����Ͻǵ����أ���͸��ɫ
BM_AND,        	//DST AND SRC		�����
BM_GRAY,        //�ҶȻ�ͼ,�൱��BM_TRANSPARENT���ҶȻ�ͼ��DST�Ҷ� �� 30%R + 59%G + 11%BͼƬ���
BM_REVERSE,		//�����ͼ,�൱��BM_TRANSPARENT�������ͼ(V1939)

��ropΪ BM_COPY ���� BM_TRANSPARENT ����ʱ������֧����ת����
�� rop = BM_COPY | MTRANS_ROT90;��ʾ˳ʱ����ת90�Ȼ�ͼ.
rop = BM_TRANSPARENT | MTRANS_ROT90;��ʾ��ת90�Ⱥ���͸��ɫ������ͼ.

transcolor: ͸��ɫ,Ĭ�Ͻ���һ�����ص���Ϊ͸��ɫ.
*/
//���������궨��ֻΪ���ְ汾ǰ�������
#define FLIP_NONE	0x0000
#define	FLIP_HORI	0x0100	//ˮƽ��ת
#define FLIP_VERT	0x0200	//��ֱ��ת

#define MTRANS_NONE             0x0000  //û�н���ת���ͷ�ת.
#define MTRANS_MIRROR			0x0100  //ˮƽ(����)��ת<==>FLIP_HORI
#define MTRANS_MIRROR_ROT180    0x0200  //ˮƽ(����)��ת��180��ת��<==>FLIP_VERT��ֱ(����)��ת
#define MTRANS_ROT90            0x0300  //90��ת��
#define MTRANS_ROT180			0x0400  //180��ת��
#define MTRANS_ROT270			0x0500  //270��ת��
#define MTRANS_MIRROR_ROT90		0x0600  //ˮƽ(����)��ת��90��ת��
#define MTRANS_MIRROR_ROT270    0x0700  //ˮƽ(����)��ת��270��ת��

int32 mrc_graphdraw_drawBitmap(uint16* p, drawBmpParamSt *pRectParam, uint16 rop, uint16 transcolor);

int32 mrc_graphdraw_bitmapShow(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy);

int32 mrc_graphdraw_bitmapShowTrans(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy, uint16 transcolor);

/*
���ֽӿ�,�����һ���.

  pcText: �ַ���������;
  pRectParam: ����������Ϣ;
  pColor:	������ɫ;
  flag: ���ֱ�ʶ,����ȡ����ֵ:
  #define DRAW_TEXT_EX_IS_UNICODE         1 //�Ƿ�ʹ��UNICODE��, �����ֽ���
  #define DRAW_TEXT_EX_IS_AUTO_NEWLINE    2 //�Ƿ��Զ�����
  font: �����С����: 
  MR_FONT_SMALL	//С����
  MR_FONT_MEDIUM	//��������
  MR_FONT_BIG		//������
*/
int32 mrc_graphdraw_drawText(char* pcText, drawTextParamSt *pRectParam, mr_colourSt *pColor, int32 flag, uint16 font);

int32 mrc_graphdraw_drawTextLeft(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int32 flag, uint16 font);

/*
 ����������Ļ��ֽӿ�һ��,��ͬ���ǽ��ַ������ҵ�����Ƶ���Ӧ����
*/
int32 mrc_graphdraw_drawTextR(char* pcText, drawTextParamSt *pRectParam, mr_colourSt *pColor, int32 flag, uint16 font);

int32 mrc_graphdraw_drawTextRight(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int32 flag, uint16 font);

/*
 * [��ʼ��gif��ͼ]
 * ��������������ͬ��gifͼƬ,ֻ�����һ��graphics_startGif,Ȼ��ɶ�ε���graphics_SpeedupgifDecode,�����Ҫ����һ��graphics_endGif��
 * �������ͬ��ߵ�gifͼƬ,����Ҫÿ�ζ�����graphics_startGif��graphics_endGif.
 * ����ʾ��1:
 *		graphics_startGif(48,48);
 *		graphics_SpeedupgifDecode(&gifBuf1, 1234, GIF2SCREEN, &param1);
 *		graphics_endGif();
 *		graphics_startGif(36,48);
 *		graphics_SpeedupgifDecode(&gifBuf2, 2345, GIF2SCREEN, &param2);
 *		graphics_endGif();
 * ����ʾ��2:
 *		graphics_startGif(48,48);
 *		graphics_SpeedupgifDecode(&gifBuf1, 1234, GIF2SCREEN, &param1);
 *		graphics_SpeedupgifDecode(&gifBuf2, 2345, GIF2SCREEN, &param2);
 *		graphics_endGif();
 * ����:
 *		gifWid: [����]gifͼƬ���
 *		gifHei: [����]gifͼƬ�߶�
 *		isLoadCache :[����] 
 *			1: �������õ���buffer��cache������  
 *			0: ����cache������
 */
int32 mrc_graphdraw_startGif(int16 gifWid, int16 gifHei);

/*
 * [����gif��ͼ]
 */
void mrc_graphdraw_endGif(void);

/*
 * [gif����]
 * ����:
 *		gifBuffer:		[����]gifͼƬ����
 *		gifBufferLen:	[����]gif�����ֽ���
 *		[ֱ�ӻ���]
 *			outType=GIF2SCREEN
 *			pPara:[����]GifDecodeParaSt����ָ��,ָ��ͼƬ��Χ(��ʱֻ֧��: rop = BM_COPY, sx = 0, sy = 0)
 *		
 *		[gifת16λbmp]
 *			outType=GIF2BMP16
 *			pPara:[���]16λbmp��bufferָ�룬��Ҫ�û��Լ�����ռ䲢�ͷ�,�ռ��СΪ (gifWid*gifHei*2)��
 *				  �����16λbmp����ֱ�ӵ���bitmapShow�Ƚӿڽ��л�ͼ��
 *				
 *		[gifת8λbmp]
 *			outType=GIF2BMP256
 *			pPara:[���]8λbmp��bufferָ��,	��Ҫ�û��Լ�����ռ䲢�ͷ�,�ռ��СΪ (gifWid*gifHei+769).
 *				  �����8λbmpֻ�ܵ���graphics_DrawBmp256�ӿڽ��л�ͼ��
 *	
 */
int32 mrc_graphdraw_SpeedupgifDecode(uint8 **gifBuffer, int32 gifBufferLen, int8 outType, void *pPara);

/*
 * [8λbmp��ͼ]
 * ���ܴ����׼8λͼ,ֻ�ܴ�����graphics_SpeedupgifDecode�����8λbmp,����ο�graphics_SpeedupgifDecode��[gifת8λbmp]
 * ����:
 *		bmpBuffer:	[����]8λbmp����
 *		pPara:		[����]GifDecodeParaSt����ָ��,ָ����ͼ��Χ,֧��sx,sy����		
 */
void mrc_graphdraw_DrawBmp256(uint8* bmpBuffer, void *pPara);

/*
 * ��mrc_unicodeTextWidthHeight���ܲ�࣬��������Ǽ��ٰ�, ��֧��12��С����
 * ����:
 * pcText:         ����ʾ���ַ���
 * font:           �����С�����ܵ�ֵ��   
 *                  MR_FONT_SMALL		ʹ�ó���װ��12��С����
 *                  MR_FONT_MEDIUM		
 *                  MR_FONT_BIG
 * ���:
 * w���ַ�������ʾ���
 * h���ַ�������ʾ�߶�
 * ����:
 *  MR_SUCCESS     �����ɹ�
 *  MR_FAILED      ����ʧ��
 */
int32 mrc_graphdraw_unicodeTextWidthHeight(uint16* pcText, uint16 font, int32* w, int32* h);

/*
 * ��mrc_unicodeTextRow���ܲ�࣬��������Ǽ��ٰ�, ��֧��12��С����
 * ������Unicode�ַ�������
 * ���ش���ʾ�ַ�������ʾ�ڿ�Ϊw�������
 * ��Ҫ��������
 * pcText:         ����ʾ���ַ���
 * font:              �����С�����ܵ�ֵ��   
 *                      MR_FONT_SMALL		ʹ�ó���װ��12��С����
 *                      MR_FONT_MEDIUM
 *                      MR_FONT_BIG
 * w                    ����ʾ���
 * ����:
 *    MR_FAILED      ����ʧ��
 *    ����           �ַ�������
 */
int32 mrc_graphdraw_unicodeTextRow(uint16* pcText, uint16 font, int32 w);

/*
* ��С�����ļ����
 */
int32 mrc_graphdraw_OpenSmallFont(void);

/*
* �ر�С�����ļ����
 */
int32 mrc_graphdraw_CloseSmallFont(void);

/*
 * ����С�������ģʽ��
 * ����:
 *	fontNum:	��������������ÿ����Ҫ20���ֽ�, fontNum��Χ[1-10008]
 * ����ֵ:
 *	MR_SUCCESS:	�ɹ�
 *	MR_FAILED:	ʧ��
 */
int32 mrc_graphdraw_smallFontCacheLoad(int32 fontNum);

/*
 * �ر�С�������ģʽ��
 */
void mrc_graphdraw_smallFontCacheUnload(void);

/* ��ͼ���ٽӿ� */
/* ��ͼ���ٳ�ʼ���ӿ�
* ���û�ͼ���ٽӿ�֮ǰ������ô˽ӿڳ�ʼ��,����һ�μ���.
* ��Ҫ��ͼ���ٽӿ�: mrc_graphdraw_bitmapShowDAC,mrc_graphdraw_bitmapShowDACTrans,mrc_graphdraw_drawRectDAC
 */
int32 mrc_graphics_DACInit(void);

/* λͼ���Ƽ��ٽӿ�,������ϸ�μ������mrc_graphdraw_drawBitmap�ӿ� 
* mrc_graphdraw_bitmapShowDAC,����Ҫ����͸��ɫʱ,ʹ�õ�һ��������Ϊ͸��ɫ
* mrc_graphdraw_bitmapShowDACTrans,����Ҫ����͸��ɫʱ,ʹ��transcolor��Ϊ͸��ɫ
*/

//��ͼ���ٽӿڵĺ���ԭ��
/*
int32 mrc_graphdraw_bitmapShowDAC(uint16* p, int16 x, int16 y, 
								  uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy);

int32 mrc_graphdraw_bitmapShowDACTrans(uint16* p, int16 x, int16 y, 
									   uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy, uint16 transcolor);

// ����������Ƽ��ٽӿ�,������ϸ�μ������mrc_graphdraw_drawRect�ӿ�
void mrc_graphdraw_drawRectDAC(int32 x, int32 y, int32 w, int32 h, uint8 r, uint8 g, uint8 b);
*/

typedef int32 (*bitmapShowType)(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
								int16 sx, int16 sy);
typedef int32 (*bitmapShowTransType)(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
									 int16 sx, int16 sy, uint16 transcolor);
typedef void (*drawRectType)(int32 x, int32 y, int32 w, int32 h, uint8 r, uint8 g, uint8 b);

extern bitmapShowType mrc_graphdraw_bitmapShowDAC;
extern bitmapShowTransType mrc_graphdraw_bitmapShowDACTrans;
extern drawRectType mrc_graphdraw_drawRectDAC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif