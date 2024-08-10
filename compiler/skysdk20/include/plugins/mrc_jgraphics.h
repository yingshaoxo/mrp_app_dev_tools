#ifndef _MRC_JGRAPHICS_H
#define _MRC_JGRAPHICS_H

#include "mrc_base.h"
#include "mrc_bmp.h"



#define TOP_Graphics            16
#define BASELINE_Graphics       64
#define BOTTOM_Graphics         32
#define VCENTER_Graphics        0x2


#define LEFT_Graphics           0x4
#define HCENTER_Graphics        0x1
#define RIGHT_Graphics          0x8


#define AP_V_MASK_Graphics      (TOP_Graphics|BASELINE_Graphics|BOTTOM_Graphics|VCENTER_Graphics)
#define AP_H_MASK_Graphics      (LEFT_Graphics|HCENTER_Graphics|RIGHT_Graphics)




#define TRANS_NONE_Sprite          0
#define TRANS_MIRROR_ROT180_Sprite 1
#define TRANS_MIRROR_Sprite        2
#define TRANS_ROT180_Sprite        3

#define TRANS_MIRROR_ROT270_Sprite 4
#define TRANS_ROT90_Sprite         5
#define TRANS_ROT270_Sprite        6
#define TRANS_MIRROR_ROT90_Sprite  7

//ͼ�ο�������
typedef struct _mrc_jgraphics_context_t {    
    struct {

        /*
         *  Clip ������ ����ֵ��Ϊ����translate������ġ�
         *  ����� ScreenBuffer�����ꡣ
         */
        int clipX;
        int clipY;
        int clipWidth;
        int clipHeight;
        int clipXRight;    /* = clipX + clipWidth�� Ϊ���ٶȶ����õ�����ֵ */
        int clipYBottom;   /* = clipY + clipBottom��Ϊ���ٶȶ����õ�����ֵ */
        
        int translateX;
        int translateY;
        
        int font;
        
        uint8  color_R, color_G, color_B;
        uint16 color_565;
    }mutableValues;
    
    
    uint16 * __SJC_SCREEN_BUFFER__; //�ٵ���Ļ����(����)
    int __SJC_SCREEN_WIDTH__; //�ٵ���Ļ��(�����)
    int __SJC_SCREEN_HEIGHT__; //�ٵ���Ļ��(�����)

	int flag; //��ʶ�Ƿ�����Ļbuffer��0-��1-��
	uint16 * __REAL_SJC_SCREEN_BUFFER__; //��������Ļ�建
	int __REAL_SJC_SCREEN_WIDTH__; //��������Ļ��
    int __REAL_SJC_SCREEN_HEIGHT__; //��������Ļ��
}mrc_jgraphics_context_t;


typedef struct {
	uint16* data;
	uint16  width;
	uint16	height;
	uint16  trans; // 0 - ͼƬ�в�����͸��ɫ��1�� ͼƬ���Ͻǵ�һ������Ϊ͸��ɫ; 2: transcolorָ����
	uint16  transcolor;  //�������͸��ɫ�� ��ôtranscolor��565��ֵ��
} mrc_jImageSt;

//������
typedef struct _mrc_jgraphics_func_table_t {
    void (*setClip)(mrc_jgraphics_context_t * gContext, int x, int y, int width, int height);
    void (*clipRect)(mrc_jgraphics_context_t * gContext, int x, int y, int width, int height);
    void (*drawLine)(mrc_jgraphics_context_t * gContent, int x1, int y1, int x2, int y2);
    void (*drawImage)(mrc_jgraphics_context_t * gContext, mrc_jImageSt * img, int x, int y, int anchor);
    void (*drawString)(mrc_jgraphics_context_t * gContext, char* str, int x, int y, int anchor);
    void (*fillRect)(mrc_jgraphics_context_t* gContext, int x, int y, int width, int height);
    void (*drawRect)(mrc_jgraphics_context_t * gContext, int x, int y, int width, int height);
    void (*drawRegion)(mrc_jgraphics_context_t * gContext, mrc_jImageSt * src, int x_src, int y_src, int width,   int height,     int transform,    int x_dest,     int y_dest,   int anchor);
	void (*copyArea)(mrc_jgraphics_context_t * context, int x_src, int y_src, int width, int height, int x_dest, int y_dest, int anchor);
} mrc_jgraphics_func_table_t;

extern mrc_jgraphics_func_table_t mrc_jgraphics_func_table;

extern mrc_jgraphics_context_t * mrc_jgraphics_context;

/*
 * ��������mrc_jgraphics_init
 * ����  ����ʼ��sjc_Graphics����ȡ��Ļ�ĸߺͿ�
 * ��ʼ���ü����򣬳�ʼ��ƽ�Ʋ�������ȡ��Ļ����
 * �ڻ��ƿ�ʼǰ���ȵ��ã���ʼ����ͼ��
 *
 *   ������
 *      mode : 0 - ��ͨģʽ
 *           : 1 - ����ģʽ
 *       
 *   ����ֵ��
 *             0 - ��ͨģʽ���سɹ�
 *             1 - ����ģʽ���سɹ�
 *
 */
extern int32 mrc_jgraphics_init(int32 mode);


/* �����Ƿ�֧�ּ���ģʽ
 * 1-֧�� 0-��֧��
 */
extern int mrc_jgraphics_supportSpeed(void);


/*��������mrc_jgraphics_resetContext
 *����  �����³�ʼ���ü�����ȫ��Ļ����
 *		���³�ʼ��ƽ�Ʋ�����0��0��
 *		������Ⱦ����ѭ����ʼʱ����
 * ������
 *	context mrc_jgraphics_context_tָ��
*/
extern void mrc_jgraphics_resetContext(mrc_jgraphics_context_t * context);

/*
 * ������	��mrc_jgraphics_translate
 * ����	��ƽ�Ƶ�ǰ����ϵ
 * ������
 * context mrc_jgraphics_context_tָ��
 */
extern void mrc_jgraphics_translate(mrc_jgraphics_context_t * context, int x, int y);

/*
 * ������	��mrc_jgraphics_getTranslateX
 * ����	����ȡ��ǰ
 * ������
 *	context mrc_jgraphics_context_tָ��
*/
extern int mrc_jgraphics_getTranslateX(mrc_jgraphics_context_t * context);

/*
 * ������	��mrc_jgraphics_getTranslateY
 * ����	��ƽ�Ƶ�ǰ����ϵ
 * ������
 *	context mrc_jgraphics_context_tָ��
*/
extern int mrc_jgraphics_getTranslateY(mrc_jgraphics_context_t * context);

/* ������	��mrc_jgraphics_setClip
 * ����	�����赱ǰ�ü����ο�,��translateӰ��
 * ������
 *	context mrc_jgraphics_context_tָ��
 */
#define mrc_jgraphics_setClip(context, x, y, width, height) \
	mrc_jgraphics_func_table.setClip((context), (x), (y), (width), (height))

/*
 * ������	��mrc_jgraphics_clipRect
 * ����	�����òü����Ρ�
 * ���������
 *			context mrc_jgraphics_context_tָ��
 *			x		--��ǰ����ϵ����Ҫ���òü��������ϵ��x����
 *			y		--��ǰ����ϵ����Ҫ���òü��������ϵ��y����
 *			width	--��Ҫ���òü����εĿ�
 *			height	--��Ҫ���òü����εĸ�
 * !!��Ҫ���õĲü����γ�����ǰ�ü����εĲ��ֽ�����ǰ�ü����βü�*/
#define mrc_jgraphics_clipRect(context, x,y,width,height) \
	mrc_jgraphics_func_table.clipRect((context), (x), (y), (width), (height))

 /*
 *������	��mrc_jgraphics_getClipHeight
 *����	����ȡ��ǰ�ü����εĸ�
 * ������
 *	context mrc_jgraphics_context_tָ��
 */
extern int 	mrc_jgraphics_getClipHeight(mrc_jgraphics_context_t * context);

/*
 * ������	��mrc_jgraphics_getClipWidth
 * ����	����ȡ��ǰ�ü����εĿ�
 * ������
 *	context mrc_jgraphics_context_tָ��
 */
extern int 	mrc_jgraphics_getClipWidth(mrc_jgraphics_context_t * context);

/*
 * ������	��mrc_jgraphics_getClipX
 * ����	����ȡ��ǰ�ü����ε����ϵ�����Ļ����ϵ�µ�x����
 * ������
 *	context mrc_jgraphics_context_tָ��
 */
extern int 	mrc_jgraphics_getClipX(mrc_jgraphics_context_t * context);

/*
 * ������	��mrc_jgraphics_getClipY
 * ����	����ȡ��ǰ�ü����ε����ϵ�����Ļ����ϵ�µ�y����
 * ������
 *	context mrc_jgraphics_context_tָ��
 */
extern int 	mrc_jgraphics_getClipY(mrc_jgraphics_context_t * context); 





/*
 * ������	��mrc_jgraphics_setFont
 * ����	�����õ�ǰ����
 *		MR_FONT_SMALL,
 *		MR_FONT_MEDIUM,
 *		MR_FONT_BIG
 * ������
 *	context mrc_jgraphics_context_tָ��
 */
extern void mrc_jgraphics_setFont(mrc_jgraphics_context_t * context, uint16 font);

/*
 * ������	��mrc_jgraphics_getFont
 * ����	����ȡ��ǰ����
 * ������
 *	context mrc_jgraphics_context_tָ��
*/
extern uint16 mrc_jgraphics_getFont(mrc_jgraphics_context_t * context);

/*
 * ������	��mrc_jgraphics_setColor
 * ����	�����õ�ǰ��ɫ
 * ������
 *	context mrc_jgraphics_context_tָ��
 * ����    ��int RGB
 */
extern void mrc_jgraphics_setColor(mrc_jgraphics_context_t * context, int RGB);

/*
 * ������	��mrc_jgraphics_setColor_Ex
 * ����	    �����õ�ǰ��ɫ
 * ����     ��int red, int green, int blue
 * ������
 *	context mrc_jgraphics_context_tָ��
 */
extern void mrc_jgraphics_setColor_Ex(mrc_jgraphics_context_t * context, 
									  int red, int green, int blue);

/*
 * ������	��mrc_jgraphics_getColor
 * ����	����ȡ��ǰ��ɫ
 * ������
 *	context mrc_jgraphics_context_tָ��
 * ����    ��8/8/8 format pixel (0x00RRGGBB)
 */
extern int  mrc_jgraphics_getColor(mrc_jgraphics_context_t * context);



////////////////////////////////////////////////////////////////////////////////////////////////////
//   ����Ϊ������ͼ�κ�����                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * ������	��mrc_jgraphics_drawImage
 * ����	������ͼ��
 *		typedef struct {
 *	uint8*  data; //ԴͼƬָ��
 *	uint16  width;
 *	uint16	height;
 *	uint8   trans;//1:ʹ��͸��ɫ;0:��ʹ��
 *		} Image_s;
 */
#define mrc_jgraphics_drawImage(context, img, x, y, anchor) \
	mrc_jgraphics_func_table.drawImage((context),(img),(x),(y),(anchor))

/*
 * ������	��mrc_jgraphics_drawRegion
 * ����	������ͼ��
 * ����	��	src,					--ͼ��	Image_s�ṹָ��
 * 			x_src, y_src,			--Դͼ��
 * 			width,height,			--Դͼ���͸�
 * 			transform,				--��ת������
 * 									TRANS_NONE_Sprite			�����任
 * 									TRANS_MIRROR_Sprite			����
 * 									TRANS_ROT180_Sprite			��ת180
 * 									TRANS_MIRROR_ROT180_Sprite	������ת180
 * 									TRANS_ROT90_Sprite 			��ת90
 * 									TRANS_MIRROR_ROT90_Sprite 	������ת90
 * 									TRANS_ROT270_Sprite 		��ת270
 * 									TRANS_MIRROR_ROT270_Sprite 	������ת270
 * 			x_dest,y_dest,			--��Ļ�ϵ�λ��
 *             anchor					--ê����������ǰֻ֧��Ϊ0�����ϵ㣩��
 */
 
#define mrc_jgraphics_drawRegion(context, src,x_src,y_src,width,height,transform,x_dest,y_dest,anchor) \
	mrc_jgraphics_func_table.drawRegion((context),(src),(x_src),(y_src),(width),(height),(transform),(x_dest),(y_dest),(anchor))

/**
 * ��������mrc_jgraphics_newContext
 * ���ܣ�������һ������Ļ�����mrc_jgraphics_context
 * ������
 *		buf�������ڴ��ַ
 *		width�������
 *		height�������
 * ����ֵ��
 *		mrc_jgraphics_context_t ָ��
 */
extern mrc_jgraphics_context_t * mrc_jgraphics_newContext(uint16 * buf, int32 width, int32 height);

/**
 * ��������mrc_jgraphics_deleteContext
 * ���ܣ�����һ�� mrc_jgraphics_context_t ָ��
 */
extern void mrc_jgraphics_deleteContext(mrc_jgraphics_context_t * context);

/*
 * ������	��mrc_jgraphics_drawString
 * ����	�������ַ���,������Unicode�ַ�������
 */
#define mrc_jgraphics_drawString(context, str, x, y, anchor) \
	mrc_jgraphics_func_table.drawString((context), (str), (x), (y), (anchor))

/*
 * ������	��mrc_jgraphics_drawLine
 * ����	������ֱ��
 * ����    ��	x1:��ǰ����ϵ����Ҫ����ֱ��һ�˵�x����
 *			y1:��ǰ����ϵ����Ҫ����ֱ��һ�˵�y����
 *			x2:��ǰ����ϵ����Ҫ����ֱ����һ�˵�x����
 *			y2:��ǰ����ϵ����Ҫ����ֱ����һ�˵�y����
 */
#define mrc_jgraphics_drawLine(context, x1, y1, x2, y2) \
	mrc_jgraphics_func_table.drawLine((context), (x1), (y1), (x2), (y2))

/*
 * ������	��mrc_jgraphics_fillRect
 * ����	������������
 */
#define mrc_jgraphics_fillRect(context, x, y, width, height) \
	mrc_jgraphics_func_table.fillRect((context), (x), (y), (width), (height))

/*
 * ������	��mrc_jgraphics_drawRect
 * ����	�����ƾ��ο�
 */
#define mrc_jgraphics_drawRect(context, x, y, width, height) \
	mrc_jgraphics_func_table.drawRect((context), (x), (y), (width), (height))

#define mrc_jgraphics_copyArea(context, x_src, y_src, width, height, x_dest, y_dest, anchor) \
	mrc_jgraphics_func_table.copyArea((context), (x_src), (y_src), (width), (height), (x_dest), (y_dest), (anchor))

#endif