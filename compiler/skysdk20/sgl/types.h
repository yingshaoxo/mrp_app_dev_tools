/*
** types.h: the types header file, contains all the base types for simple gui library.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_TYPES_H_
#define _SGL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \brief A type definition for an 8-bit unsigned character.
 */
typedef unsigned char   Uint8;
/**
 * \brief A type definition for an 8-bit signed character.
 */
typedef signed char     Sint8;
/**
 * \brief A type definition for a 16-bit unsigned integer.
 */
typedef unsigned short  Uint16;
/**
 * \brief A type definition for a 16-bit signed integer.
 */
typedef signed short    Sint16;
/**
 * \brief A type definition for a 32-bit unsigned integer.
 */
typedef unsigned int    Uint32;
/**
 * \brief A type definition for a 32-bit signed integer.
 */
typedef signed int      Sint32;

/**
 * \brief A type definition for boolean value.
 */
typedef unsigned char	BOOL;

/**
 * \brief FALSE value, defined as 0 by SGL.
 */
#ifndef FALSE
#define FALSE       0
#endif

/**
 * \brief TRUE value, defined as 1 by SGL.
 */
#ifndef TRUE
#define TRUE        1
#endif

/**
 * \brief A value indicates null pointer.
 */
#ifndef NULL
#define NULL            ((void *)0)
#endif

/**
 * \brief A type definition for VOID
 */
#define VOID            void

/**
 * \brief A type definition for an 8-bit signed character.
 */
typedef char CHAR;

/**
 * \brief A type definition for an 8-bit unsigned character.
 */
typedef Uint8 UCHAR;

/**
 * \brief A type definition for pointer to wstring.
 */
typedef UCHAR* PWSTR;

/**
 * \brief A type definition for pointer to const wstring.
 */
typedef const UCHAR* PCWSTR;

/**
 * \brief A type definition for pointer to string.
 */
typedef char* PSTR;

/**
 * \brief A type definition for pointer to const string.
 */
typedef const char* PCSTR;

/**
 * \brief A type definition for unsigned character (byte).
 */
typedef unsigned char   BYTE;

/**
 * \brief A type definition for pointer to byte.
 */
typedef BYTE*		PBYTE;

/**
 * \brief A type definition for signed character.
 */
typedef signed char     SBYTE;

/**
 * \brief A type definition for unsigned short integer (word).
 */
typedef unsigned short  WORD;

/**
 * \brief A type definition for signed short integer.
 */
typedef signed short    SWORD;

/**
 * \brief A type definition for unsigned long integer (double word).
 */
typedef unsigned long   DWORD;

/**
 * \brief A type definition for signed long integer.
 */
typedef signed long     SDWORD;

/**
 * \brief A type definition for unsigned integer.
 */
typedef unsigned int    UINT;

/**
 * \brief A type definition for long integer.
 */
typedef long            LONG;

/**
 * \brief Makes a double word from \a low word and \a high word.
 */
#define MAKELONG(low, high) ((DWORD)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))

/**
 * \brief A rectangle defined by upper-left coordinates and width/height. 
 */
typedef struct _RECT
{
    /** the x coordinate of the upper-left corner of the rectangle. */
    int left;
    /** the y coordinate of the upper-left corner of the rectangle. */
    int top;
    /** the width of the rectangle. */
    int width;
    /** the height of the rectangle. */
    int height;
} RECT, *PRECT;

/**
 * \brief A macro return the right position of the rect \a r.
 */
#define RECT_RIGHT(r) (r->left + r->width - 1)

/**
 * \brief A macro return  the bottom position of the rect \a r.
 */
#define RECT_BOTTOM(r) (r->top + r->height - 1)

/**
 * \brief Point structure.
 */
typedef struct _POINT
{
    /** the x coordinate of the point. */
    int x;
    /** the y coordinate of the point. */
    int y;
} POINT, * PPOINT;

/**
 * \brief Size structure of a 2-dimension object.
 */
typedef struct _SIZE
{
    /** the extent in x coordinate of a 2D object. */
    int cx;
    /** the extent in y coordinate of a 2D object. */
    int cy;
} SIZE, *PSIZE;

/* create 24 bit 8/8/8 format pixel (0x00RRGGBB) from RGB triplet*/
#define RGB2PIXEL888(r,g,b)    \
    (((r) << 16) | ((g) << 8) | (b))

/* create 16 bit 5/6/5 format pixel from RGB triplet */
#define RGB2PIXEL565(r,g,b)    \
    ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))

/* create 24 bit 8/8/8 format pixel from RGB colorval (0x00RRGGBB)*/
#define COLOR2PIXEL888(c)    \
    ((((c) & 0xff) << 16) | ((c) & 0xff00) | (((c) & 0xff0000) >> 16))

/* create 16 bit 5/6/5 format pixel from RGB colorval (0x00RRGGBB)*/
#define COLOR2PIXEL565(c)    \
    ((((c) & 0xf8) << 8) | (((c) & 0xfc00) >> 5) | (((c) & 0xf80000) >> 19))

/* return 8/8/8 bit r, g, or b, component of 24 bit pixel */
#define PIXEL888RED(pixel)        (Uint8)(((pixel) >> 16) & 0xff)
#define PIXEL888GREEN(pixel)      (Uint8)(((pixel) >> 8) & 0xff)
#define PIXEL888BLUE(pixel)       (Uint8)((pixel) & 0xff)
#define PIXEL888ALPHA(pixel)      (Uint8)(((pixel) >> 24))

/* return 5/6/5 bit r, g or b component of 16 bit pixel*/
#define PIXEL565RED(pixel)        (Uint8)(((pixel) >> 11) & 0x1f)
#define PIXEL565GREEN(pixel)      (Uint8)(((pixel) >> 5) & 0x3f)
#define PIXEL565BLUE(pixel)       (Uint8)((pixel) & 0x1f)

/**
 * \brief A macro returns the maximum of \a x and \a y.
 */
#ifndef MAX
#define MAX(x, y)           (((x) > (y))?(x):(y))
#endif

/**
 * \brief A macro returns the minimum of \a x and \a y.
 */
#ifndef MIN
#define MIN(x, y)           (((x) < (y))?(x):(y))
#endif

/**
 * \brief A macro returns the absolute value of \a x.
 */
#ifndef ABS
#define ABS(x)              (((x)<0) ? -(x) : (x))
#endif

/**
 * \brief A macro return the value of \a x / \a y
 */
#ifndef DIV
#define DIV(x, y)		((x) / (y))
#endif

#ifndef HALF
#define HALF(x) ((x) / 2)
#endif

#ifndef SAFEDIV
#define SAFEDIV(a, b) (a/(b==0? 1 : b))
#endif


/**
 * \brief A macro return the value of \a x % \a y
 */
#ifndef MOD
#define MOD(x, y)		((x) % (y))
#endif

typedef struct _BMP16 {
	const Uint16 *bmp;
	Uint16 w, h;//bmp width & height
} T_BMP16, *PT_BMP16;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SGL_TYPES_H */

