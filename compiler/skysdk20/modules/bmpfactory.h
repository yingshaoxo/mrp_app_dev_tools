#ifndef _BMPFACTORY_H_
#define _BMPFACTORY_H_


#include "types.h"
#include "mrc_base.h"

/****************************************
 * BMP �ļ��ṹ
 * 1.BMP �ļ�ͷ ��14��
 * 2.BMP ��Ϣͷ ��40��
 * 3.BMP ��ɫ�� ����ȷ����
 * 4.BMP ������Ϣ����ȷ����
 ***************************************/


/**
 * 1.BMP�ļ�ͷ
 * BMP�ļ�ͷ���ݽṹ����BMP�ļ������͡��ļ���С��λͼ��ʼλ�õ���Ϣ��  
 * ��ṹ��������:
 */

typedef struct tagBITMAPFILEHEADER
{
	/**
	 * bfType ������ 2 ���ֽڣ������ܹ�14�ֽ�CPU�����䣬�ʰ���Ū��
	 * 4 �ֽڣ�ʹ�õ�ʱ��������ȥ��2�ֽ�OK
	 */
	DWORD	bfType;			//2 λͼ�ļ������ͣ�����ΪBM
	DWORD   bfSize;			//4 λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ  
	WORD	bfReserved1;	//2 λͼ�ļ������֣�����Ϊ0
	WORD	bfReserved2;	//2 λͼ�ļ������֣�����Ϊ0
	DWORD   bfOffBits;		//4 λͼ���ݵ���ʼλ�ã��������λͼ
}BITMAPFILEHEADER;

/**
 * 2.λͼ��Ϣͷ
 *	BMPλͼ��Ϣͷ��������˵��λͼ�ĳߴ����Ϣ��
 */

typedef struct tagBITMAPINFOHEADER
{
	DWORD   biSize;		//4 ���ṹ��ռ���ֽ���(ֻ����Ϣͷ��40)
	LONG	biWidth;	//4 λͼ�Ŀ�ȣ�������Ϊ��λ
	LONG	biHeight;   //4 λͼ�ĸ߶ȣ�������Ϊ��λ
	WORD	biPlanes;   //2 Ŀ���豸�ļ��𣬱���Ϊ1
	WORD	biBitCount;	//2 ÿ�����������λ���������� 1(˫ɫ), 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ
	DWORD   biCompression;   //4 λͼѹ�����ͣ������� 0(��ѹ��), 1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ
	DWORD   biSizeImage;		//4 λͼ�Ĵ�С�����ֽ�Ϊ��λ
	LONG	biXPelsPerMeter;	//4 λͼˮƽ�ֱ��ʣ�ÿ��������
	LONG	biYPelsPerMeter;	//4 λͼ��ֱ�ֱ��ʣ�ÿ��������
	DWORD   biClrUsed;			//4 λͼʵ��ʹ�õ���ɫ���е���ɫ��
	DWORD   biClrImportant;		//4 λͼ��ʾ��������Ҫ����ɫ��
}BITMAPINFOHEADER;

/**
 * 4.��ɫ��
 *����ɫ������˵��λͼ�е���ɫ���������ɸ����ÿһ��������һ��RGBQUAD���͵Ľṹ������һ����ɫ��
 *		��ɫ����RGBQUAD�ṹ���ݵĸ�����biBitCount��ȷ��:
 *��		��biBitCount=1,4,8ʱ���ֱ���2,16,256������;
 *			��biBitCount=24ʱ��û����ɫ��� 
 *	RGBQUAD�ṹ�Ķ�������:
 */
typedef struct tagRGBQUAD   
{
	BYTE	rgbBlue;	//1 ��ɫ������(ֵ��ΧΪ0-255)
	BYTE	rgbGreen;   //1 ��ɫ������(ֵ��ΧΪ0-255)
	BYTE	rgbRed;		//1 ��ɫ������(ֵ��ΧΪ0-255)
	BYTE	rgbReserved;//1 ����������Ϊ0
}RGBQUAD;

/*
��ɫ����RGBQUAD�ṹ���ݵĸ�����biBitCount��ȷ��:
��biBitCount=1,4,8ʱ���ֱ���2,16,256������;
��biBitCount=24ʱ��û����ɫ���
λͼ��Ϣͷ����ɫ�����λͼ��Ϣ��BITMAPINFO�ṹ��������:*/

typedef struct tagBITMAPINFO   
{
	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bmiHeader;   //   λͼ��Ϣͷ
	RGBQUAD   bmiColors[1];   //   ��ɫ��
}BITMAPINFO;

/*
5.λͼ����
��λͼ���ݼ�¼��λͼ��ÿһ������ֵ��
 ��¼˳������ɨ�������Ǵ�����,ɨ����֮���Ǵ��µ��ϡ�
 λͼ��һ������ֵ��ռ���ֽ���:
	��biBitCount=1ʱ��8������ռ1���ֽ�;
	��biBitCount=4ʱ��2������ռ1���ֽ�;
	��biBitCount=8ʱ��1������ռ1���ֽ�;
	��biBitCount=24ʱ,1������ռ3���ֽ�;
  Windows�涨һ��ɨ������ռ���ֽ���������
  4�ı���(����longΪ��λ),�������0��䣬
  һ��ɨ������ռ���ֽ������㷽��:
  DataSizePerLine=   (biWidth*   biBitCount+31)/8;  
  //   һ��ɨ������ռ���ֽ���
  DataSizePerLine=   DataSizePerLine/4*4;   //   �ֽ���������4�ı���
  λͼ���ݵĴ�С(��ѹ�������):
  DataSize=   DataSizePerLine*   biHeight; 
  */

typedef struct bmpHead565 
{
	BITMAPFILEHEADER   bmfHeader;		//λͼ�ļ�ͷ
	BITMAPINFOHEADER   bmiHeader;		//λͼ��Ϣͷ
	RGBQUAD			   bmiColors[4];	//��ɫ��
}BMPHEAD565, *PBMPHEAD565;

/**
 * ����16λBMP��Ϣͷ
 */
PBMPHEAD565 makeBmpHead565(DWORD w, DWORD h);


/**
���ӣ����ڴ���˫��0x01020304(DWORD)�Ĵ洢��ʽ 
MEM 0000 0001 0002 0003 
LE  04   03   02   01 
BE  01   02   03   04  
*/
//��С���ֽ���32λ����д���ڴ�
int32 writeLittleEndianU32(uint32 data, uint8 *p);

/**
 * \������filename ���浽���ļ���
 */
int32 printScreen(const char *filename);

//��ͼ�������Զ�����ǰʱ������Ψһ�ļ�����
int32 ScreenShot(PCSTR todir);

#endif
