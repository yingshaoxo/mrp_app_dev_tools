/*
修订记录:
20080909 李波:
部分BMP图片头ImageDataSize等于0而导致不能正确显示，遂修改。
*/


#ifdef	 MR_OLD
#include "cfunction.h"
#include "mrc_bmp.h"
#else
#include "mrc_base.h"
#include "mrc_base_i.h"
#include "mrc_bmp.h"
#include "Mr_helper_exb.h"
#include "mrc_network.h"
#endif

//#define LIB_DEBUG

#ifdef LIB_DEBUG
#define DBG_PRINTF(a) mrc_printf a
#else
#define DBG_PRINTF(a) 
#endif

/*
//Bitmap header
typedef struct _mrc_bmpHeader
{
	uint16		Magic;				// Magic identifier: "BM" 
	uint32		FileSize;			// Size of the BMP file in bytes 
	uint16		Reserved1;		// Reserved 
	uint16		Reserved2;		// Reserved 
	uint32		DataOffset;		// Offset of image data relative to the file's start 
	uint32		HeaderSize;		// Size of the header in bytes
	uint32		Width;				// Bitmap's width 
	uint32		Height;				// Bitmap's height 
	uint16		Planes;				// Number of color planes in the bitmap 
	uint16		BitsPerPixel;		// Number of bits per pixel 
	uint32		CompressionType;	// Compression type 
	uint32		ImageDataSize;		// Size of uncompressed image's data 
	uint32		HPixelsPerMeter;	// Horizontal resolution (pixels per meter) 
	uint32		VPixelsPerMeter;		// Vertical resolution (pixels per meter) 
	uint32		ColorsUsed;				// Number of color indexes in the color table that are actually used by the bitmap 
	uint32		ColorsRequired;		// Number of color indexes that are required for displaying the bitmap 
} mrc_bmpHeader;//程序员应当通过封装的函数访问BMP数据。


//Private data structure
struct _BMP
{
	mrc_bmpHeader	Header;
	uint8*		Palette;
	uint8*		Data;
	uint32		PaletteType;		// 0:no need free;1:need free;
	uint32		DataType;			// 0:no need free;1:need free;
	uint8*	MrpBuffer;//20071126李波为了实现mrc_bmpLoadFromMrp而增加的
	//字段，使得本来符合BMP位图格式规范的_BMP头不再符合规范。因此，
	//凡是在设计BMP头操作的地方，比如使用sizeof(_BMP)、memcpy之类，都要
	//需要特别注意。
};
*/

uint16 temppata[256];

#define SCREENBUFINDEX 30//BITMAPMAX

/* Holds the last error code */
static mrc_bmpSTATUS mrc_bmpLAST_ERROR_CODE;

/* Size of the palette data for 8 BPP bitmaps */
#define mrc_bmpPALETTE_SIZE	( 256 * 4 )
#define mrc_bmpPALETTE16_SIZE	( 16 * 4 )



/*********************************** Forward declarations **********************************/
int		ReadHeader	( BMP* bmp, int32 f );
int		WriteHeader	( BMP* bmp, int32 f );

int		Readuint32	( uint32* x, int32 f );
int		Readuint16	( uint16 *x, int32 f );

int		Writeuint32	( uint32 x, int32 f );
int		Writeuint16	( uint16 x, int32 f );



int	BufferReadHeader( BMP* bmp, int32 f );

int	BufferReaduint32( uint32* x, int32 f );

int	BufferReaduint16( uint16 *x, int32 f );



/*********************************** Public methods **********************************/
#ifdef	 MR_OLD
请注意
//MR_OLD的情况下，需要将下面这行宏定义添加到C_function.h中。
#define mr_bitmap ((GET_HELPER()->mr_bitmap))
static int32 mr_bitmapGetInfo(uint32 di,mr_bitmapSt *bitmapInfo)
{
	if(di>SCREENBUFINDEX||!bitmapInfo)
		return MR_FAILED;
	MEMCPY(bitmapInfo,&mr_bitmap[di],sizeof(mr_bitmapSt));
	return MR_SUCCESS;	
}
#endif

/**************************************************************
	Creates a blank BMP image with the specified dimensions
	and bit depth.
**************************************************************/
BMP* mrc_bmpCreate( uint32 width, uint32 height, uint16 depth )
{
	BMP*	bmp;
	int		bytes_per_pixel = depth >> 3;
	uint32	bytes_per_row;

	if ( height == 0 || width == 0 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
		return NULL;
	}

	if ( depth != 4&&depth != 8 && depth!=16 && depth != 24 && depth != 32 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_NOT_SUPPORTED;
		return NULL;
	}

	/* Allocate the bitmap data structure */
	bmp = MR_MALLOC( sizeof( BMP ) );

	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;
		return NULL;
	}

	/* Set header' default values */
	bmp->Header.Magic				= 0x4D42;
	bmp->Header.Reserved1			= 0;
	bmp->Header.Reserved2			= 0;
	bmp->Header.HeaderSize			= 40;
	bmp->Header.Planes				= 1;
	bmp->Header.CompressionType		= 0;
	bmp->Header.HPixelsPerMeter		= 0;
	bmp->Header.VPixelsPerMeter		= 0;
	bmp->Header.ColorsUsed			= 0;
	bmp->Header.ColorsRequired		= 0;
	bmp->MrpBuffer=NULL;//20071126李波为了实现mrc_bmpLoadFromMrp而增加。
	//由于增加了该字段，导致BMP头和标准的不一样，因此在使用
	//时尤其要注意，对于sizeof(__BMP)和54 要谨慎使用

	/* Calculate the number of bytes used to store a single image row. This is always
	rounded up to the next multiple of 4. */
	bytes_per_row = width * bytes_per_pixel;
	bytes_per_row += ( bytes_per_row % 4 ? 4 - bytes_per_row % 4 : 0 );


	/* Set header's image specific values */
	bmp->Header.Width				= width;
	bmp->Header.Height				= height;
	bmp->Header.BitsPerPixel		= depth;
	bmp->Header.ImageDataSize		= bytes_per_row * height;
	switch(depth)
	{
		case 4:
			bytes_per_row = width/2+width%2;
			bytes_per_row += ( bytes_per_row % 4 ? 4 - bytes_per_row % 4 : 0 );	
			bmp->Header.ImageDataSize		= bytes_per_row * height;
			bmp->Header.FileSize			= bmp->Header.ImageDataSize + 54 +mrc_bmpPALETTE16_SIZE;
			bmp->Header.DataOffset			= 54 + mrc_bmpPALETTE16_SIZE;	
			
			bmp->Palette = (uint8*) MR_MALLOC( mrc_bmpPALETTE16_SIZE*sizeof( uint8 ) );

			if ( bmp->Palette == NULL )
			{
				mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;
				
#ifdef MR_OLD	
				MR_FREE(bmp,sizeof(BMP));
#else
				free( bmp );
#endif
				return NULL;
			}					
			break;
		case 8:
			bmp->Header.FileSize			= bmp->Header.ImageDataSize + 54 +mrc_bmpPALETTE_SIZE;
			bmp->Header.DataOffset			= 54 +mrc_bmpPALETTE_SIZE;						
			bmp->Palette = (uint8*) MR_MALLOC( mrc_bmpPALETTE_SIZE*sizeof( uint8 ) );
	
			if ( bmp->Palette == NULL )
			{
				mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;
				
#ifdef MR_OLD	
				MR_FREE(bmp,sizeof(BMP));
#else
				free( bmp );
#endif
				return NULL;
			}			
			break;
		default:
			bmp->Header.FileSize			= bmp->Header.ImageDataSize + 54 + 0;
			bmp->Header.DataOffset			= 54 +0;		
			bmp->Palette = NULL;
			break;
	}
   bmp->PaletteType = 1;


	/* Allocate pixels */
	bmp->Data = (uint8*) MR_MALLOC( bmp->Header.ImageDataSize*sizeof( uint8 ) );
	if ( bmp->Data == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;

#ifdef MR_OLD	
				if(bmp->Header.BitsPerPixel==4 && bmp->Palette)
					MR_FREE(bmp->Palette,mrc_bmpPALETTE16_SIZE);
				else if(bmp->Header.BitsPerPixel==8 && bmp->Palette)
					MR_FREE(bmp->Palette,mrc_bmpPALETTE_SIZE);
					
				MR_FREE(bmp,sizeof(BMP));
#else
				free( bmp->Palette );
				free( bmp );
#endif
		return NULL;
	}
     bmp->DataType = 1;
	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

	return bmp;
}


/**************************************************************
	Frees all the memory used by the specified BMP image.
**************************************************************/
void mrc_bmpFree( BMP* bmp )
{
	if ( bmp == NULL )
	{
		return;
	}
#ifdef MR_OLD
	if ( (bmp->PaletteType) && (bmp->Palette != NULL) )
	{
		if(bmp->Header.ColorsUsed!=0)
			MR_FREE( bmp->Palette,4*bmp->Header.ColorsUsed);
		else if(bmp->Header.BitsPerPixel==4)
			MR_FREE( bmp->Palette,mrc_bmpPALETTE16_SIZE);
		else if(bmp->Header.BitsPerPixel==8)
			MR_FREE( bmp->Palette,mrc_bmpPALETTE_SIZE );
	}

	if ( (bmp->DataType) && (bmp->Data != NULL) )
	{
		MR_FREE( bmp->Data, bmp->Header.ImageDataSize);
	}

	if(bmp->MrpBuffer)//20071126李波为了实现mrc_bmpLoadFromMrp而增加。	
		MR_FREE(bmp->MrpBuffer,bmp->Header.FileSize);
   
	MR_FREE( bmp,sizeof(BMP) );
#else
	if ( (bmp->PaletteType) && (bmp->Palette != NULL) )
	{
		free( bmp->Palette );
	}

	if ( (bmp->DataType) && (bmp->Data != NULL) )
	{
		free( bmp->Data );
	}

	if(bmp->MrpBuffer)//20071126李波为了实现mrc_bmpLoadFromMrp而增加。	
		free(bmp->MrpBuffer);
   
	free( bmp );
#endif
	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
}


/**************************************************************
	Reads the specified BMP image file.
**************************************************************/
BMP* mrc_bmpLoadFromFile( char* filename )
{
	BMP*	bmp;
     int32       f;
     int32 bmpPaletteSize=0;

	//mrc_printf("mrc_bmpLoadFromFile:%s",filename);
	if ( filename == NULL )
	{
		DBG_PRINTF(("mrc_bmpLoadFromFile:filename == NULL!"));
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
		return NULL;
	}
	DBG_PRINTF(("mrc_bmpLoadFromFile:%s",filename)); 
	/* Allocate */
	bmp = MR_MALLOC( sizeof( BMP ) );
	if ( bmp == NULL )
	{
		DBG_PRINTF(("mrc_bmpLoadFromFile:Allocate bmp failed!"));
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;
		return NULL;
	}
#ifdef	 MR_OLD
   MEMSET(bmp, 0, sizeof( BMP ));
	f = mr_open( filename, MR_FILE_RDONLY );
#else
	mrc_memset(bmp, 0, sizeof( BMP ));
	f = mrc_open( filename, MR_FILE_RDONLY );
#endif
	/* Open file */
	DBG_PRINTF(("mrc_bmpLoadFromFile:mrc_open=%0x",f)); 
	
	if ( f == 0 )
	{
		DBG_PRINTF(("mrc_bmpLoadFromFile:Open file failed!"));
		mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_NOT_FOUND;
		mrc_bmpFree( bmp );
		return NULL;
	}


	/* Read header */
	if ( ReadHeader( bmp, f ) != mrc_bmpOK || bmp->Header.Magic != 0x4D42 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_INVALID;
#ifdef	 MR_OLD		
		mr_close( f );
#else
		mrc_close( f );
#endif
		DBG_PRINTF(("mrc_bmpLoadFromFile:ReadHeader failed!"));
		mrc_bmpFree( bmp );
		return NULL;
	}
	DBG_PRINTF(("mrc_bmpLoadFromFile:ReadHeader Succeed!"));
	
	/* Verify that the bitmap variant is supported */
	if ( ( bmp->Header.BitsPerPixel != 32 && bmp->Header.BitsPerPixel != 24 && bmp->Header.BitsPerPixel != 8 && bmp->Header.BitsPerPixel != 4)
		|| bmp->Header.CompressionType != 0 || bmp->Header.HeaderSize != 40 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_NOT_SUPPORTED;
#ifdef	 MR_OLD				
		mr_close( f );
#else
		mrc_close( f );
#endif
		mrc_bmpFree( bmp );
		DBG_PRINTF(("mrc_bmpLoadFromFile:mrc_bmpFILE_NOT_SUPPORTED!"));
		return NULL;
	}


	/* Allocate and read palette */
	bmpPaletteSize=0;
	if(bmp->Header.BitsPerPixel == 8)
	{
		if(bmp->Header.ColorsUsed==0)
			bmpPaletteSize=mrc_bmpPALETTE_SIZE;
		else
			bmpPaletteSize=4*(bmp->Header.ColorsUsed);
	}else if(bmp->Header.BitsPerPixel == 4)
	{
		if(bmp->Header.ColorsUsed==0)
			bmpPaletteSize=mrc_bmpPALETTE16_SIZE;
		else
			bmpPaletteSize=4*(bmp->Header.ColorsUsed);
	}

	if ( bmpPaletteSize)
	{
		bmp->Palette = (uint8*) MR_MALLOC( bmpPaletteSize * sizeof( uint8 ) );	
		if ( bmp->Palette == NULL )
		{
			mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;
#ifdef	 MR_OLD				
          mr_close( f );
#else
          mrc_close( f );
#endif
			mrc_bmpFree( bmp );
			DBG_PRINTF(("mrc_bmpLoadFromFile:MR_MALLOC bmp->Palette == NULL !"));
			return NULL;
		}
		DBG_PRINTF(("mrc_bmpLoadFromFile:MR_MALLOC bmp->Palette =%0x!",bmp->Palette));
		
           bmp->PaletteType = 1;
#ifdef	 MR_OLD	
		if ( mr_read( f, bmp->Palette, sizeof( uint8 )*bmpPaletteSize ) != bmpPaletteSize )
#else
		if ( mrc_read( f, bmp->Palette, sizeof( uint8 )*bmpPaletteSize ) != bmpPaletteSize )
#endif			
		{
			mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_INVALID;
#ifdef	 MR_OLD				
          mr_close( f );
#else
          mrc_close( f );
#endif
			DBG_PRINTF(("mrc_bmpLoadFromFile:mrc_read bmp->Palette failed!"));
			mrc_bmpFree( bmp );
			return NULL;
		}
		DBG_PRINTF(("mrc_bmpLoadFromFile:mrc_read bmp->Palette Succeed!"));
	}
	else	/* Not an indexed image */
	{
		bmp->Palette = NULL;
         bmp->PaletteType = 0;
	}


	/* Allocate memory for image data */
	bmp->Data = (uint8*) MR_MALLOC( bmp->Header.ImageDataSize );
	if ( bmp->Data == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;
#ifdef	 MR_OLD			
       mr_close( f );
#else
       mrc_close( f );		
#endif
		mrc_bmpFree( bmp );
		DBG_PRINTF(("mrc_bmpLoadFromFile:MR_MALLOC bmp->Data failed!"));
		return NULL;
	}
   bmp->DataType = 1;


	/* Read image data */
#ifdef	 MR_OLD			
	if ( mr_read( f, bmp->Data, sizeof( uint8 )*bmp->Header.ImageDataSize ) != bmp->Header.ImageDataSize )
#else
	if ( mrc_read( f, bmp->Data, sizeof( uint8 )*bmp->Header.ImageDataSize ) != bmp->Header.ImageDataSize )
#endif		
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_INVALID;
#ifdef	 MR_OLD			
       mr_close( f );
#else
       mrc_close( f );		
#endif
		mrc_bmpFree( bmp );
		DBG_PRINTF(("mrc_bmpLoadFromFile:mrc_read bmp->Data failed!"));
		return NULL;
	}
	DBG_PRINTF(("mrc_bmpLoadFromFile:mrc_read bmp->Data Succeed!"));

#ifdef	 MR_OLD			
       mr_close( f );
#else
       mrc_close( f );		
#endif
	bmp->MrpBuffer=NULL;//20071126李波为了实现mrc_bmpLoadFromMrp而增加。
	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

	return bmp;
}

typedef struct
{
   uint8* p;                 //
   int32 pos;                  //
   int32 len;
}mrc_bufferSt;

int32 mrc_bmpReadBufferReader(int32 f,void *p,uint32 l)
{
   int32 size;
   mrc_bufferSt *bufstp = (mrc_bufferSt *)f;
   if ((bufstp->pos + l) <= bufstp->len){
      size = l;
   }else{
      size = bufstp->len - bufstp->pos;
   }
#ifdef	 MR_OLD   
   MEMCPY(p, bufstp->p + bufstp->pos, size);
#else
   mrc_memcpy(p, bufstp->p + bufstp->pos, size);
#endif
   //memcpy(p, bufstp->p + bufstp->pos, size);
   bufstp->pos = bufstp->pos + size;
   return size;
}



/**************************************************************
	Reads the specified BMP image file.
**************************************************************/
BMP* mrc_bmpLoadFromBuffer( uint8* buf, int32 len )
{
	BMP*	bmp=NULL;
   mrc_bufferSt bufferSt;
     int32 f = (int32)&(bufferSt);

     if(buf==NULL)
     {
	     mrc_bmpLAST_ERROR_CODE =mrc_bmpERROR;
	     return NULL;
     }

	/* Allocate */
	bmp = MR_MALLOC( sizeof( BMP ) );
	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOUT_OF_MEMORY;
		DBG_PRINTF(("mrc_bmpLoadFromBuffer:mrc_bmpOUT_OF_MEMORY"));    		
		return NULL;
	}
#ifdef	 MR_OLD
     MEMSET(bmp, 0, sizeof( BMP ));
#else
	mrc_memset(bmp, 0, sizeof( BMP ));
#endif
     bufferSt.p = buf; bufferSt.pos = 0; bufferSt.len = len;
     /* Read header */
     if ( BufferReadHeader( bmp, f ) != mrc_bmpOK || bmp->Header.Magic != 0x4D42 )
     {
        mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_INVALID;     
        mrc_bmpFree( bmp );
		DBG_PRINTF(("mrc_bmpLoadFromBuffer:mrc_bmpFILE_INVALID"));        
        return NULL;
        
     }



	/* Verify that the bitmap variant is supported */
	if ( ( bmp->Header.BitsPerPixel != 32 && bmp->Header.BitsPerPixel != 24 && bmp->Header.BitsPerPixel != 8 && bmp->Header.BitsPerPixel != 4 )
		|| bmp->Header.CompressionType != 0 || bmp->Header.HeaderSize != 40 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_NOT_SUPPORTED;
		mrc_bmpFree( bmp );
		DBG_PRINTF(("mrc_bmpLoadFromBuffer:mrc_bmpFILE_NOT_SUPPORTED"));  
		return NULL;
	}


	/* Allocate and read palette */
	if ( bmp->Header.BitsPerPixel == 8 )
	{
           bmp->PaletteType = 0;
           bmp->Palette = (uint8*)bufferSt.p + bufferSt.pos;
           if(bmp->Header.ColorsUsed==0)
	           bufferSt.pos += mrc_bmpPALETTE_SIZE;
           else
           	bufferSt.pos += 4*bmp->Header.ColorsUsed;
	}
	else if ( bmp->Header.BitsPerPixel == 4 )
	{
           bmp->PaletteType = 0;
           bmp->Palette = (uint8*)bufferSt.p + bufferSt.pos;
           if(bmp->Header.ColorsUsed==0)
	           bufferSt.pos += mrc_bmpPALETTE16_SIZE;
           else
           	bufferSt.pos += 4*bmp->Header.ColorsUsed;
	}	
	else	/* Not an indexed image */
	{
            bmp->Palette = NULL;
            bmp->PaletteType = 0;
	}


   bmp->DataType = 0;
   bmp->Data = (uint8*)bufferSt.p + bufferSt.pos;
   bufferSt.pos += bmp->Header.ImageDataSize;

	bmp->MrpBuffer=NULL;//20071126李波为了实现mrc_bmpLoadFromMrp而增加。
   mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
   DBG_PRINTF(("mrc_bmpLoadFromBuffer:mrc_bmpOK,hBMP=%0x",bmp));  

   return bmp;
}

BMP*	mrc_bmpLoadFromMrp	( char* filename )
{
	uint8 *Buffer=NULL;
	int32 filelen,ret; 
	BMP* bmp=NULL;
	mrc_bmpLAST_ERROR_CODE=mrc_bmpERROR;
#ifdef MR_OLD
	Buffer=(uint8 *)(_mr_readFile(filename,(int *)(&filelen),0));
#else
	ret=mrc_readFileFromMrpEx(NULL,filename,&Buffer,&filelen,0);
#endif		

	DBG_PRINTF(("mrc_readFileFromMrpEx: Buffer=%0x,filelen=%d,ret=%d",Buffer,filelen,ret));
	
	if(Buffer==NULL)
	{
		mrc_bmpLAST_ERROR_CODE=mrc_bmpFILE_NOT_FOUND;
		return NULL;
	}
	if(filelen<sizeof(BMP))
	{
		mrc_bmpLAST_ERROR_CODE=mrc_bmpFILE_INVALID;
		return NULL;	
	}
	bmp=mrc_bmpLoadFromBuffer(Buffer,filelen);
	//错误码由mrc_bmpLoadFromBuffer填写维护。	
	if(!bmp)
	{		
		//释放Buffer所占据的空间。
#ifdef MR_OLD		
		MR_FREE(Buffer,filelen);
#else
		mrc_free(Buffer);
#endif
		return NULL;			
	}
	bmp->MrpBuffer=Buffer;//20071126李波为了实现mrc_bmpLoadFromMrp而增加。	
	return bmp;	
}


/**************************************************************
	Writes the BMP image to the specified file.
**************************************************************/
void mrc_bmpWriteFile( BMP* bmp, char* filename )
{
   int32       f;
    int32 bmpPaletteSize=0;

	if ( filename == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
		return;
	}


	/* Open file */
#ifdef	 MR_OLD	
	f = mr_open( filename, MR_FILE_WRONLY |MR_FILE_CREATE);
#else
	f = mrc_open( filename, MR_FILE_WRONLY |MR_FILE_CREATE);
#endif
	if ( f == 0 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpFILE_NOT_FOUND;
		return;
	}


	/* Write header */
	if ( WriteHeader( bmp, f ) != mrc_bmpOK )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpIO_ERROR;
#ifdef	 MR_OLD			
       mr_close( f );
#else
       mrc_close( f );		
#endif
		return;
	}


	/* Write palette */
	if ( bmp->Palette)
	{
		bmpPaletteSize=0;
		if(bmp->Header.ColorsUsed)
			bmpPaletteSize=4*bmp->Header.ColorsUsed;
		else if(bmp->Header.BitsPerPixel==8)
			bmpPaletteSize=mrc_bmpPALETTE_SIZE;
		else if(bmp->Header.BitsPerPixel==4)
			bmpPaletteSize=mrc_bmpPALETTE16_SIZE;
#ifdef	 MR_OLD	
		if ( mr_write(f, bmp->Palette, sizeof( uint8 )*bmpPaletteSize ) != bmpPaletteSize )
#else
		if ( mrc_write(f, bmp->Palette, sizeof( uint8 )*bmpPaletteSize ) != bmpPaletteSize )
#endif		
		{
			mrc_bmpLAST_ERROR_CODE = mrc_bmpIO_ERROR;
#ifdef	 MR_OLD			
       	mr_close( f );
#else
       	mrc_close( f );		
#endif
			return;
		}			
			
	}

	/* Write data */
#ifdef	 MR_OLD			
	if ( mr_write(f, bmp->Data, sizeof( uint8 )*bmp->Header.ImageDataSize ) != bmp->Header.ImageDataSize )
#else		
	if ( mrc_write(f, bmp->Data, sizeof( uint8 )*bmp->Header.ImageDataSize ) != bmp->Header.ImageDataSize )		
#endif		
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpIO_ERROR;
#ifdef	 MR_OLD			
       mr_close( f );
#else
       mrc_close( f );		
#endif
		return;
	}


	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
#ifdef	 MR_OLD			
       mr_close( f );
#else
       mrc_close( f );		
#endif
}


/**************************************************************
	Returns the image's width.
**************************************************************/
uint32 mrc_bmpGetWidth( BMP* bmp )
{
	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
		return -1;
	}

	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

	return ( bmp->Header.Width );
}


/**************************************************************
	Returns the image's height.
**************************************************************/
uint32 mrc_bmpGetHeight( BMP* bmp )
{
	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
		return -1;
	}

	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

	return ( bmp->Header.Height );
}


/**************************************************************
	Returns the image's color depth (bits per pixel).
**************************************************************/
uint16 mrc_bmpGetDepth( BMP* bmp )
{
	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
		return 0;
	}

	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

	return ( bmp->Header.BitsPerPixel );
}


/**************************************************************
	Populates the arguments with the specified pixel's RGB
	values.
**************************************************************/
void mrc_bmpGetPixelRGB( BMP* bmp, uint32 x, uint32 y, uint8* r, uint8* g, uint8* b )
{
	uint8*	pixel=NULL;
	uint32	bytes_per_row=0;

	if ( bmp == NULL || x >= bmp->Header.Width || y >= bmp->Header.Height )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
	}
	else
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
		/* Row's size is rounded up to the next multiple of 4 bytes */
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;
		
		if(bmp->Header.BitsPerPixel==8)
		{
			/* Calculate the location of the relevant pixel (rows are flipped) */
			pixel = bmp->Data + ( ( bmp->Header.Height - y - 1 ) * bytes_per_row + x );
			/* In indexed color mode the pixel's value is an index within the palette */
			pixel = bmp->Palette + *pixel * 4;
		}
		else if(bmp->Header.BitsPerPixel==4)
		{
			/* Calculate the location of the relevant pixel (rows are flipped) */
			pixel = bmp->Data + ( bmp->Header.Height - y - 1 ) *bytes_per_row + x/2;			
			/* In indexed color mode the pixel's value is an index within the palette */
			if ( x&1 )
			{
				pixel = bmp->Palette + (0xf&(*pixel)) * 4;
				//pixel = bmp->Palette + ((0xf0&(*pixel))>>4)* 4;
			}
			else
			{
				pixel = bmp->Palette + ((0xf0&(*pixel))>>4)* 4;
				//pixel = bmp->Palette + (0xf&(*pixel)) * 4;
			}			
		}		
		
		/* Note: colors are stored in BGR order */
		if ( r )	*r = *( pixel + 2 );
		if ( g )	*g = *( pixel + 1 );
		if ( b )	*b = *( pixel + 0 );
	}
}

void mrc_bmpGet16BitLineFrom8BitData( BMP* bmp, int32 y, int32 sx, int32 w, uint16* row,uint16* Palette )
{
	uint8*	pixel;
	uint8*	src;
	uint32	bytes_per_row;
     uint16* dest = row;
     int32 i;

	if ( bmp == NULL || y < 0 || y >= bmp->Header.Height ||sx<0||w<=0||(sx+w)>bmp->Header.Width||row==NULL)
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
	}
	else
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

		/* Row's size is rounded up to the next multiple of 4 bytes */
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;
		if(bmp->Header.BitsPerPixel==8)
		{
			/* Calculate the location of the relevant pixel (rows are flipped) */
			pixel = bmp->Data + ( ( bmp->Header.Height - y - 1 ) * bytes_per_row )+sx;
			/* In indexed color mode the pixel's value is an index within the palette */
			DBG_PRINTF(("mrc_bmpGet16BitLineFrom8BitData:y=%d,Palette=%0x",y,Palette));
          for(i=0;i<w; i++)
          	{
				if(Palette==NULL)
				{
					src = bmp->Palette + ((*pixel) << 2);		
              	*dest = MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));
				}else
				{
					*dest=Palette[*pixel];
				}
               dest++;pixel++;
            }
		}else if(bmp->Header.BitsPerPixel==4)
		{
			/* Calculate the location of the relevant pixel (rows are flipped) */
			pixel = bmp->Data + ( bmp->Header.Height - y - 1 ) * bytes_per_row+sx/2;
			/* In indexed color mode the pixel's value is an index within the palette */
			if(sx&1)
			{
				//起点不是在字节边界，则这个点要单独处理。
				//src = bmp->Palette + ((0xf0&(*pixel))>>4)* 4;
				src = bmp->Palette + (0xf&(*pixel)) * 4;
              *dest = MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));
               dest++;pixel++;	
				//已经处理了一个点，所以需要复制的点数减1。
				w--;
			}
          for(i=0;i<w/2;i++)
          	{
				src = bmp->Palette + ((0xf0&(*pixel))>>4)* 4;
              *dest = MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));
               dest++;               
				src = bmp->Palette + (0xf&(*pixel)) * 4;
              *dest = MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));
               dest++;pixel++;	     
            }	
          //宽度为奇数个点，最后一个点单独处理。
          //注意dest和pixel指针在循环终止时已做增量处理。
	    	if(w&1)
			{      
				src = bmp->Palette + (0xf&(*pixel)) * 4;
				//src = bmp->Palette + ((0xf0&(*pixel))>>4)* 4;
              *dest = MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));			
			}
		}
	}
}


/**************************************************************
	Sets the specified pixel's RGB values.
**************************************************************/
void mrc_bmpSetPixelRGB( BMP* bmp, uint32 x, uint32 y, uint8 r, uint8 g, uint8 b )
{
	uint8*	pixel;
	uint32	bytes_per_row;
	uint8	bytes_per_pixel;

	if ( bmp == NULL || x >= bmp->Header.Width ||y >= bmp->Header.Height )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
	}

	else if ( bmp->Header.BitsPerPixel != 24 && bmp->Header.BitsPerPixel != 32 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpTYPE_MISMATCH;
	}
	else
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

		bytes_per_pixel = bmp->Header.BitsPerPixel >> 3;

		/* Row's size is rounded up to the next multiple of 4 bytes */
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;

		/* Calculate the location of the relevant pixel (rows are flipped) */
		pixel = bmp->Data + ( ( bmp->Header.Height - y - 1 ) * bytes_per_row + x * bytes_per_pixel );

		/* Note: colors are stored in BGR order */
		*( pixel + 2 ) = r;
		*( pixel + 1 ) = g;
		*( pixel + 0 ) = b;
	}
}


/**************************************************************
	Gets the specified pixel's color index.
**************************************************************/
void mrc_bmpGetPixelIndex( BMP* bmp, uint32 x, uint32 y, uint8* val )
{
	uint8*	pixel;
	uint32	bytes_per_row;

	if ( bmp == NULL || x >= bmp->Header.Width || y >= bmp->Header.Height )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
	}

	/* Row's size is rounded up to the next multiple of 4 bytes */
	bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;
	if(bmp->Header.BitsPerPixel==8)
	{
		/* Calculate the location of the relevant pixel */
		pixel = bmp->Data + ( ( bmp->Header.Height - y - 1 ) * bytes_per_row + x );
		if ( val )	*val = *pixel;		
	}
	else if(bmp->Header.BitsPerPixel==4)
	{
		/* Calculate the location of the relevant pixel (rows are flipped) */
		pixel = bmp->Data + ( bmp->Header.Height - y - 1 ) *bytes_per_row + x/2;

		/* In indexed color mode the pixel's value is an index within the palette */
		if ( x&1 )
		{
			//pixel = bmp->Palette + (0xf&(*pixel)) * 4;
			if ( val )	*val = 0xf&(*pixel);	//取低4bit				
		}
		else
		{
			//pixel = bmp->Palette + ((0xf0&(*pixel))>>4)* 4;
			if ( val )	*val = (0xf0&(*pixel))>>4;	//取高4bit。
		}
		
	}		
	else
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpTYPE_MISMATCH;
		return;
	}
	mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
	return;
}


/**************************************************************
	Sets the specified pixel's color index.
**************************************************************/
void mrc_bmpSetPixelIndex( BMP* bmp, uint32 x, uint32 y, uint8 val )
{
	uint8*	pixel;
	uint32	bytes_per_row;

	if ( bmp == NULL || x >= bmp->Header.Width || y >= bmp->Header.Height )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
	}
	else if(bmp->Header.BitsPerPixel == 8 )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

		/* Row's size is rounded up to the next multiple of 4 bytes */
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;

		/* Calculate the location of the relevant pixel */
		pixel = bmp->Data + ( ( bmp->Header.Height - y - 1 ) * bytes_per_row + x );

		*pixel = val;
	}
	else if(bmp->Header.BitsPerPixel == 4)
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;

		/* Row's size is rounded up to the next multiple of 4 bytes */
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;

		/* Calculate the location of the relevant pixel */
		pixel = bmp->Data + ( ( bmp->Header.Height - y - 1 ) * bytes_per_row + x/2 );	

		if ( x&1 )
		{
			*pixel =(*pixel&0xf0)|(0x0f&val);	//保存到低4bit
		}
		else
		{
			*pixel = (*pixel&0x0f)|((0x0f&val)<<4);//保存到高4bit			
		}		
	}
	else
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpTYPE_MISMATCH;
	}
}

/**************************************************************
	Gets the colorused value for the palette .
**************************************************************/
uint32 mrc_bmpGetColorUsed( BMP* bmp)
{
	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
		return 0;
	}
	else if(!bmp->Palette)
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpTYPE_MISMATCH;
		return 0;
	}else if(bmp->Header.ColorsUsed)
	{
		mrc_bmpLAST_ERROR_CODE =mrc_bmpOK;
		return bmp->Header.ColorsUsed;
	}else if(bmp->Header.BitsPerPixel==4)
	{
		mrc_bmpLAST_ERROR_CODE =mrc_bmpOK; 		
		return 16;
	}else if(bmp->Header.BitsPerPixel==8)
	{
		mrc_bmpLAST_ERROR_CODE =mrc_bmpOK; 	
		return 256;
	}
	return 0;
}



/**************************************************************
	Gets the color value for the specified palette index.
**************************************************************/
void mrc_bmpGetPaletteColor( BMP* bmp, uint8 index, uint8* r, uint8* g, uint8* b )
{
	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
	}
	else if ( bmp->Header.BitsPerPixel == 8 )
	{
		if ( r )	*r = *( bmp->Palette + index * 4 + 2 );
		if ( g )	*g = *( bmp->Palette + index * 4 + 1 );
		if ( b )	*b = *( bmp->Palette + index * 4 + 0 );

		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
	}
	else if ( bmp->Header.BitsPerPixel == 4 &&  index<16)
	{
		if ( r )	*r = *( bmp->Palette + index * 4 + 2 );
		if ( g )	*g = *( bmp->Palette + index * 4 + 1 );
		if ( b )	*b = *( bmp->Palette + index * 4 + 0 );

		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
	}	
	else
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpTYPE_MISMATCH;
	}	
}


/**************************************************************
	Sets the color value for the specified palette index.
**************************************************************/
void mrc_bmpSetPaletteColor( BMP* bmp, uint8 index, uint8 r, uint8 g, uint8 b )
{
	if ( bmp == NULL )
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpINVALID_ARGUMENT;
	}
	else if ( bmp->Header.BitsPerPixel == 8 )
	{
		*( bmp->Palette + index * 4 + 2 ) = r;
		*( bmp->Palette + index * 4 + 1 ) = g;
		*( bmp->Palette + index * 4 + 0 ) = b;
		
		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
	}
	else if ( bmp->Header.BitsPerPixel == 4 && index<16)
	{
		*( bmp->Palette + index * 4 + 2 ) = r;
		*( bmp->Palette + index * 4 + 1 ) = g;
		*( bmp->Palette + index * 4 + 0 ) = b;

		mrc_bmpLAST_ERROR_CODE = mrc_bmpOK;
	}	
	else
	{
		mrc_bmpLAST_ERROR_CODE = mrc_bmpTYPE_MISMATCH;
	}	
}


/**************************************************************
	Returns the last error code.
**************************************************************/
mrc_bmpSTATUS mrc_bmpGetError()
{
	return mrc_bmpLAST_ERROR_CODE;
}


/*********************************** Private methods **********************************/

/**************************************************************
	Reads the BMP file's header into the data structure.
	Returns mrc_bmpOK on success.
**************************************************************/
int	BufferReadHeader( BMP* bmp, int32 f )
{
	if ( bmp == NULL || f == 0 )
	{
		return mrc_bmpINVALID_ARGUMENT;
	}

	/* The header's fields are read one by one, and converted from the format's
	little endian to the system's native representation. */
	if ( !BufferReaduint16( &( bmp->Header.Magic ), f ) )			return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.FileSize ), f ) )		return mrc_bmpIO_ERROR;
	if ( !BufferReaduint16( &( bmp->Header.Reserved1 ), f ) )		return mrc_bmpIO_ERROR;
	if ( !BufferReaduint16( &( bmp->Header.Reserved2 ), f ) )		return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.DataOffset ), f ) )		return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.HeaderSize ), f ) )		return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.Width ), f ) )			return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.Height ), f ) )			return mrc_bmpIO_ERROR;
	if ( !BufferReaduint16( &( bmp->Header.Planes ), f ) )		return mrc_bmpIO_ERROR;
	if ( !BufferReaduint16( &( bmp->Header.BitsPerPixel ), f ) )	return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.CompressionType ), f ) )	return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.ImageDataSize ), f ) )	return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.HPixelsPerMeter ), f ) )	return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.VPixelsPerMeter ), f ) )	return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.ColorsUsed ), f ) )		return mrc_bmpIO_ERROR;
	if ( !BufferReaduint32( &( bmp->Header.ColorsRequired ), f ) )	return mrc_bmpIO_ERROR;

	return mrc_bmpOK;
}

/**************************************************************
	Reads a little-endian unsigned int from the file.
	Returns non-zero on success.
**************************************************************/
int	BufferReaduint32( uint32* x, int32 f )
{
	uint8 little[ 4 ];	/* BMPs use 32 bit ints */

	if ( x == NULL || f == 0 )
	{
		return 0;
	}

	if ( mrc_bmpReadBufferReader(f, little, 4 ) != 4 )
	{
		return 0;
	}

	*x = ( little[ 3 ] << 24 | little[ 2 ] << 16 | little[ 1 ] << 8 | little[ 0 ] );

	return 1;
}


/**************************************************************
	Reads a little-endian unsigned short int from the file.
	Returns non-zero on success.
**************************************************************/
int	BufferReaduint16( uint16 *x, int32 f )
{
	uint8 little[ 2 ];	/* BMPs use 16 bit shorts */

	if ( x == NULL || f == 0 )
	{
		return 0;
	}

	if ( mrc_bmpReadBufferReader(f, little, 2 ) != 2 )
	{
		return 0;
	}

	*x = ( little[ 1 ] << 8 | little[ 0 ] );

	return 1;
}





/**************************************************************
	Reads the BMP file's header into the data structure.
	Returns mrc_bmpOK on success.
**************************************************************/
int	ReadHeader( BMP* bmp, int32 f )
{
	if ( bmp == NULL || f == 0 )
	{
		return mrc_bmpINVALID_ARGUMENT;
	}

	/* The header's fields are read one by one, and converted from the format's
	little endian to the system's native representation. */
	if ( !Readuint16( &( bmp->Header.Magic ), f ) )			return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.FileSize ), f ) )		return mrc_bmpIO_ERROR;
	if ( !Readuint16( &( bmp->Header.Reserved1 ), f ) )		return mrc_bmpIO_ERROR;
	if ( !Readuint16( &( bmp->Header.Reserved2 ), f ) )		return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.DataOffset ), f ) )		return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.HeaderSize ), f ) )		return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.Width ), f ) )			return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.Height ), f ) )			return mrc_bmpIO_ERROR;
	if ( !Readuint16( &( bmp->Header.Planes ), f ) )		return mrc_bmpIO_ERROR;
	if ( !Readuint16( &( bmp->Header.BitsPerPixel ), f ) )	return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.CompressionType ), f ) )	return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.ImageDataSize ), f ) )	return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.HPixelsPerMeter ), f ) )	return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.VPixelsPerMeter ), f ) )	return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.ColorsUsed ), f ) )		return mrc_bmpIO_ERROR;
	if ( !Readuint32( &( bmp->Header.ColorsRequired ), f ) )	return mrc_bmpIO_ERROR;

	//libo20080909	发现部分BMP图片头ImageDataSize等于0而导致不能正确显示，遂修改。
	if(0==bmp->Header.ImageDataSize)
	{
		if(0!=bmp->Header.ColorsUsed)			//使用了调色板且颜色数和色深默认值不一致；
			bmp->Header.ImageDataSize = bmp->Header.FileSize-54-bmp->Header.ColorsUsed*4;
		else if(4==bmp->Header.BitsPerPixel)	// 4bit的索引色，调色板默认16种颜色。
			bmp->Header.ImageDataSize = bmp->Header.FileSize-54-16*4;
		else if(8==bmp->Header.BitsPerPixel)	// 8bit的索引色，调色板默认256种颜色。
			bmp->Header.ImageDataSize = bmp->Header.FileSize-54-256*4;
		else 															//非索引色，无调色板。
			bmp->Header.ImageDataSize = bmp->Header.FileSize-54;
	}
			
	return mrc_bmpOK;
}


/**************************************************************
	Writes the BMP file's header into the data structure.
	Returns mrc_bmpOK on success.
**************************************************************/
int	WriteHeader( BMP* bmp, int32 f )
{
	if ( bmp == NULL || f == 0 )
	{
		return mrc_bmpINVALID_ARGUMENT;
	}

	/* The header's fields are written one by one, and converted to the format's
	little endian representation. */
	if ( !Writeuint16( bmp->Header.Magic, f ) )			return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.FileSize, f ) )		return mrc_bmpIO_ERROR;
	if ( !Writeuint16( bmp->Header.Reserved1, f ) )		return mrc_bmpIO_ERROR;
	if ( !Writeuint16( bmp->Header.Reserved2, f ) )		return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.DataOffset, f ) )		return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.HeaderSize, f ) )		return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.Width, f ) )			return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.Height, f ) )			return mrc_bmpIO_ERROR;
	if ( !Writeuint16( bmp->Header.Planes, f ) )		return mrc_bmpIO_ERROR;
	if ( !Writeuint16( bmp->Header.BitsPerPixel, f ) )	return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.CompressionType, f ) )	return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.ImageDataSize, f ) )	return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.HPixelsPerMeter, f ) )	return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.VPixelsPerMeter, f ) )	return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.ColorsUsed, f ) )		return mrc_bmpIO_ERROR;
	if ( !Writeuint32( bmp->Header.ColorsRequired, f ) )	return mrc_bmpIO_ERROR;

	return mrc_bmpOK;
}


/**************************************************************
	Reads a little-endian unsigned int from the file.
	Returns non-zero on success.
**************************************************************/
int	Readuint32( uint32* x, int32 f )
{
	uint8 little[ 4 ];	/* BMPs use 32 bit ints */

	if ( x == NULL || f == 0 )
	{
		return 0;
	}
#ifdef	 MR_OLD
	if ( mr_read(f, little, 4 ) != 4 )
#else
	if ( mrc_read(f, little, 4 ) != 4 )
#endif		
	{
		return 0;
	}

	*x = ( little[ 3 ] << 24 | little[ 2 ] << 16 | little[ 1 ] << 8 | little[ 0 ] );

	return 1;
}


/**************************************************************
	Reads a little-endian unsigned short int from the file.
	Returns non-zero on success.
**************************************************************/
int	Readuint16( uint16 *x, int32 f )
{
	uint8 little[ 2 ];	/* BMPs use 16 bit shorts */

	if ( x == NULL || f == 0 )
	{
		return 0;
	}
#ifdef	 MR_OLD
	if ( mr_read(f, little, 2 ) != 2 )
#else
	if ( mrc_read(f, little, 2 ) != 2 )
#endif		
	{
		return 0;
	}

	*x = ( little[ 1 ] << 8 | little[ 0 ] );

	return 1;
}


/**************************************************************
	Writes a little-endian unsigned int to the file.
	Returns non-zero on success.
**************************************************************/
int	Writeuint32( uint32 x, int32 f )
{
	uint8 little[ 4 ];	/* BMPs use 32 bit ints */

	little[ 3 ] = (uint8)( ( x & 0xff000000 ) >> 24 );
	little[ 2 ] = (uint8)( ( x & 0x00ff0000 ) >> 16 );
	little[ 1 ] = (uint8)( ( x & 0x0000ff00 ) >> 8 );
	little[ 0 ] = (uint8)( ( x & 0x000000ff ) >> 0 );
#ifdef	 MR_OLD
	return ( f && mr_write(f, little, 4 ) == 4 );
#else
	return ( f && mrc_write(f, little, 4 ) == 4 );
#endif
}


/**************************************************************
	Writes a little-endian unsigned short int to the file.
	Returns non-zero on success.
**************************************************************/
int	Writeuint16( uint16 x, int32 f )
{
	uint8 little[ 2 ];	/* BMPs use 16 bit shorts */

	little[ 1 ] = (uint8)( ( x & 0xff00 ) >> 8 );
	little[ 0 ] = (uint8)( ( x & 0x00ff ) >> 0 );
#ifdef	 MR_OLD
	return ( f && mr_write(f, little, 2 ) == 2 );
#else
	return ( f && mrc_write(f, little, 2 ) == 2 );
#endif
}

#ifdef LIB_DEBUG
int32 mrc_bmpPrintInfo(BMP*hBMP)
{
	uint32 row,col,bytes_per_row,colorIndex=0;
	uint8 Old,*pixel;
//#ifdef LIB_DEBUG			
	DBG_PRINTF(("\n\n\nmrc_bmpPrintInfo"));	
	DBG_PRINTF(("hBMP:%0x,DataType:%d,Data:%0x,PaletteType:%d,Palette:%0x,MrpBuffer:%0x",hBMP,hBMP->DataType,hBMP->Data,hBMP->PaletteType,hBMP->Palette,hBMP->MrpBuffer));
	DBG_PRINTF(("Width=%d,Height=%d,BitsPerPixel=%d,ColorsUsed=%d",hBMP->Header.Width,hBMP->Header.Height,hBMP->Header.BitsPerPixel,hBMP->Header.ColorsUsed));
	bytes_per_row = hBMP->Header.ImageDataSize / hBMP->Header.Height;	
	DBG_PRINTF(("FileSize=%d,DataOffset=%d,ImageDataSize=%d,HeaderSize=%d,Magic=%0x,bytes_per_row=%d",hBMP->Header.FileSize,hBMP->Header.DataOffset,hBMP->Header.ImageDataSize,hBMP->Header.HeaderSize,hBMP->Header.Magic,bytes_per_row));
	Old=*hBMP->Data;	
	if(hBMP->Header.ColorsUsed==0)
		colorIndex=256;
	else
		colorIndex=hBMP->Header.ColorsUsed;
	for(row=0;row<hBMP->Header.Height;row++)
	{
		for(col=0;col<hBMP->Header.Width;col++)
		{
			pixel = hBMP->Data + ( ( hBMP->Header.Height - row - 1 ) * bytes_per_row + col );
			if(*pixel!=Old)
			{
				DBG_PRINTF(("Old(0,0)=%d,pixel=%0x,*pixel=%d",Old,pixel,*pixel));
				break;
			}
		}
	}
	DBG_PRINTF(("mrc_bmpPrintInfo finish!"));
	return 1;
}
#endif 

/*
将hBMP指向的windows bmp 256色或16色格式的图片，从(sx, sy)开始的
宽高为w, h的区域，绘制到图片缓存di从(x, y)开始的地方。

输入:
di               目标图片缓冲序号
buf,len        指向保存windows bmp 256色格式的图片的内存
w,h             欲绘制区域的宽高
x,y             欲绘制区域在屏幕上的起始位置
sx,sy          欲绘制区域在源图片的起始位置

返回:
      MR_SUCCESS     成功
      MR_FAILED         失败
*/
int32 mrc_bmpBufferDraw(uint16 di, BMP*hBMP, int32 x, int32 y, int32 w, int32 h, uint16 rop,int32 sx, int32 sy)
{
	mr_bitmapSt BitMapBuf;
   uint32 row=0,col=0,k=0,ColorUsed=0;
   uint16 *dest=NULL,*Palette=NULL;
   uint8 TransClorIndex,CheckClorIndex=0,r,g,b,*src;
#ifdef   LIB_DEBUG
   uint8 testflag=0;//,*src;	
#endif

	uint8*	pixel,*pixelbase;
	uint32	bytes_per_row;
     uint16* destrow,i;


	DBG_PRINTF(("mrc_bmpBufferDraw:hBMP=%0x",hBMP));  	
	mrc_bmpLAST_ERROR_CODE =mrc_bmpERROR;
	if((di>SCREENBUFINDEX))
		return MR_FAILED;
#ifdef	 MR_OLD	
	mr_bitmapGetInfo(di,&BitMapBuf);
#else
	mrc_bitmapGetInfo(di,&BitMapBuf);
#endif	

	if(x<0)
	{
		w=w+x;
		sx=sx-x;
		x=0;
	}
	if(y<0)
	{
		h=h+y;
		sy=sy-y;
		y=0;
	}	
	if((x+w)>BitMapBuf.w)
	{
		w=BitMapBuf.w-x;
	}
	if((y+h)>BitMapBuf.h)
	{
		h=BitMapBuf.h-y;
	}	
	if(hBMP==NULL||x<0||y<0||w<0||h<0||sx<0||sy<0)
		return MR_FAILED;
	if(hBMP->Header.Magic!=0x4D42||(x+w)>BitMapBuf.w||(y+h)>BitMapBuf.h||(sx+w)>mrc_bmpGetWidth(hBMP)||(sy+h)>mrc_bmpGetHeight(hBMP))
		return MR_FAILED;
	
#ifdef LIB_DEBUG		
	mrc_bmpPrintInfo(hBMP);
	DBG_PRINTF(("mrc_bmpBufferDraw:rop=%d",rop)); 
#endif

	if(hBMP[0].Header.BitsPerPixel==8)
	{
		ColorUsed=hBMP[0].Header.ColorsUsed;
		if(ColorUsed==0)
			ColorUsed=256;
		DBG_PRINTF(("mrc_bmpBufferDraw:ColorUsed=%d",ColorUsed));
		Palette=mrc_malloc(ColorUsed*sizeof(uint16));
		if(Palette==NULL)
			return MR_FAILED;
			
		for(k=0;k<ColorUsed;k++)
		{
			src = hBMP->Palette + (k<< 2);		
	       Palette[k]= MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));
		}		
	}

	DBG_PRINTF(("mrc_bmpBufferDraw:Palette[ColorUsed] is ready!"));
	switch(rop)
	{	
		case BM_COPY:
			/* Row's size is rounded up to the next multiple of 4 bytes */
			bytes_per_row = hBMP->Header.ImageDataSize / hBMP->Header.Height;
			/* Calculate the location of the relevant pixel (rows are flipped) */
	       pixelbase=hBMP->Data + ( ( hBMP->Header.Height - sy - 1 ) * bytes_per_row )+sx;			
			 if(hBMP[0].Header.BitsPerPixel==8)
			 {
				 for (k = 0; k < h; k++)
				 {
#ifdef MR_ANYKA_MOD
				    destrow = (uint16*)((uint8*)BitMapBuf.p + ((k+y)*BitMapBuf.w+x)*3);// 24bit色深		   
#else
				    destrow =BitMapBuf.p + (k+y)*BitMapBuf.w+x;// 16bit色深
#endif				 
			     //mrc_bmpGet16BitLineFrom8BitData(hBMP, k+sy, sx, w, row,Palette);
					pixel = pixelbase-k*bytes_per_row;
					/* In indexed color mode the pixel's value is an index within the palette */
					DBG_PRINTF(("mrc_bmpGet16BitLineFrom8BitData:y=%d,Palette=%0x",(k+sy),Palette));
		          for(i=0;i<w; i++)
		          	{
						*destrow++=Palette[*pixel++];
		               //destrow++;pixel++;
		           }
				 }
			 }
			 else 
			 {
				 for (k = 0; k < h; k++)
				 {
#ifdef MR_ANYKA_MOD
				    destrow = (uint16*)((uint8*)BitMapBuf.p + ((k+y)*BitMapBuf.w+x)*3);// 24bit色深		   
#else
				    destrow =BitMapBuf.p + (k+y)*BitMapBuf.w+x;// 16bit色深
#endif		
					mrc_bmpGet16BitLineFrom8BitData(hBMP, k+sy, sx, w, destrow,NULL);
				 }
			 }		
			break;
		case BM_TRANSPARENT:
			//由于我们这个函数是对同一幅图片抠去透明色，
			//因此只需要对比索引值即可。
			//下面的代码没有经过性能优化。
			mrc_bmpGetPixelIndex(hBMP,0,0,&TransClorIndex);
#ifdef LIB_DEBUG			
			DBG_PRINTF(("mrc_bmpBufferDraw:mrc_bmpGetPixelIndex hBMP=%0x,width=%d,height=%d,BitsPerPixel=%d",hBMP,hBMP->Header.Width,hBMP->Header.Height,hBMP->Header.BitsPerPixel));			
			if(mrc_bmpGetError()!=mrc_bmpOK)				
			{
				DBG_PRINTF(("mrc_bmpBufferDraw:mrc_bmpGetPixelIndex Error %0x,failed!",mrc_bmpGetError()));			
				break;
			}
			mrc_bmpGetPixelRGB(hBMP,0,0,&r,&g,&b);
			DBG_PRINTF(("BM_TRANSPARENT:dot(0,0) in Palatte[%d]=r %d,g %d,b %d",TransClorIndex,r,g,b));
#endif			
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点检查
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelIndex(hBMP,col,row,&CheckClorIndex);
#ifdef LIB_DEBUG
					if(mrc_bmpGetError()!=mrc_bmpOK)
					{
						DBG_PRINTF(("mrc_bmpBufferDraw:mrc_bmpGetPixelIndex(%d,%d) Error %0x,failed!hBMP=%d,width=%d,height=%d,BitsPerPixel=%d",col,row,mrc_bmpGetError(),hBMP,hBMP->Header.Width,hBMP->Header.Height,hBMP->Header.BitsPerPixel));			
						break;
					}			
#endif					
					if(CheckClorIndex!=TransClorIndex)
					{
						 //src=p->Palette+4*CheckClorIndex;
						 mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
						 //*dest=MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));	
						 *dest=MAKERGB(r, g,b);
#ifdef LIB_DEBUG						 
						if(mrc_bmpGetError()!=mrc_bmpOK)
						{
							DBG_PRINTF(("mrc_bmpBufferDraw:mrc_bmpGetPixelRGB failed!"));			
							break;
						}						 						 
						 if(testflag==0)
						 {
						 	testflag=1;
							DBG_PRINTF(("BM_TRANSPARENT:dot(%d,%d) in Palatte[%d] MAKERGB(%d,%d,%d)=%0x",col,row,CheckClorIndex,r,g,b,*dest));						 	
						 }
#endif						 
					}
					dest++;					
				}
				dest+=BitMapBuf.w-w;
			}			
#ifdef LIB_DEBUG					
			if(testflag==0)
			{
				DBG_PRINTF(("BM_TRANSPARENT:no other color than (0,0)"));
			}
#endif			
			break;				
		case BM_OR:			
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest|=MAKERGB(r, g,b);	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}					
			break;				
		case BM_XOR:
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest^=MAKERGB(r, g,b);	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}									
			break;				
		case BM_NOT:
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest=~(MAKERGB(r, g,b));	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}		
			
			break;
		case BM_MERGENOT:
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest|=~(MAKERGB(r, g,b));	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}	
			
			break;
		case BM_ANDNOT:
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest&=~(MAKERGB(r, g,b));	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}	
			
			break;
		case BM_AND:
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest&=MAKERGB(r, g,b);	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}	
			
			break;	
		case BM_GRAY:
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest=MAKERGB((uint32)r*3/10, (uint32)g*59/100,(uint32)b*11/100);	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}	
			
			break;
#ifndef MR_OLD
		case BM_REVERSE:
			mrc_bmpGetPixelIndex(hBMP,0,0,&TransClorIndex);
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelIndex(hBMP,col,row,&CheckClorIndex);
					if(CheckClorIndex!=TransClorIndex)
					{
						 //src=p->Palette+4*CheckClorIndex;
						 mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
						 // *dest=MAKERGB(*( src + 2 ), *( src + 1 ), *( src + 0 ));						
						 *dest=~(MAKERGB(r, g,b));						
					}
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}					
			break;
#endif	
/*
	case BM_WHITETRANSPARENT:
			//由于我们这个函数是对同一幅图片抠去透明色，
			//因此只需要对比索引值即可。
			//下面的代码没有经过性能优化。
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点检查
			for(row=sy;row<(sy+h);row++)
			{
				for(col=sx;col<(sx+w);col++)
				{
					 mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					if(r!=255||g!=0||b!=255)
					{
						 *dest=MAKERGB(r, g,b);			 
					}
					dest++;					
				}
				dest+=BitMapBuf.w-w;
			}			
#ifdef LIB_DEBUG					
			if(testflag==0)
			{
				DBG_PRINTF(("BM_TRANSPARENT:no other color than (0,0)"));
			}
#endif			
			break;	
*/								
		default:	//默认情况下逐点复制。	
			dest=BitMapBuf.p+(y*BitMapBuf.w+x);//注意，这里使用16bit的色深。
			//下面逐个点操作
			for(row=sy;row<sy+h;row++)
			{
				for(col=sx;col<sx+w;col++)
				{
					mrc_bmpGetPixelRGB(hBMP,col,row,&r,&g,&b);
					*dest=MAKERGB(r, g,b);	
					dest++;
				}
				dest+=BitMapBuf.w-w;
			}						
	}	
	mrc_bmpLAST_ERROR_CODE =mrc_bmpOK;
	if(Palette)
		mrc_free(Palette);
	return MR_SUCCESS;
}

//mrc_bmpBufferDrawEX调用mrc_bmpLoadFromBuffer后再调用mrc_bmpBufferDraw、mrc_bmpFree()，所以实际意义有限。
int32 mrc_bmpBufferDrawEX(uint16 di,uint8* buf,  int32 len, int32 x, int32 y,int32 w, int32 h,uint16 rop, int32 sx, int32 sy)
{		
		int32 ret=MR_FAILED;
		BMP*hBMP=NULL;

		mrc_bmpLAST_ERROR_CODE =mrc_bmpERROR;
		hBMP=mrc_bmpLoadFromBuffer(buf,len);
		if(hBMP)
		{
			ret=mrc_bmpBufferDraw(di,hBMP,x,y,w,h,rop,sx,sy);
			mrc_bmpFree(hBMP);
		}		
		return ret;
}


/*
	将句柄为p的256色bmp缓冲中的图片，从缓冲中图片的(sx, sy)
	开始的宽高为w, h的区域，绘制到(x, y)开始的屏幕缓冲中。

输入:
p        源图片指针
x,y              屏幕位置
rop              选择如下：
   BM_OR,         			//SRC .OR. DST*   半透明效果
   BM_XOR,        			//SRC .XOR. DST*
   BM_COPY,       		//DST = SRC*      覆盖
   BM_NOT,        			//DST = (!SRC)*
   BM_MERGENOT,   	//DST .OR. (!SRC)
   BM_ANDNOT,     		//DST .AND. (!SRC)
   BM_TRANSPARENT,//透明色不显示，图片的第一个象素（左上角坐标为(0,0)的象素）是透明色
   BM_AND,        			//DST AND SRC
   BM_GRAY,        		//灰度绘图， 相当于BM_TRANSPARENT＋灰度绘图：
                              DST灰度 ＝ 30%R + 59%G + 11%B图片序号
   BM_REVERSE     		//反向绘图，相当于BM_TRANSPARENT＋反向绘图（V1939）

sx,sy              源图片的起始位置
w,h             欲加载图片的宽高

返回:
      MR_SUCCESS     成功
      MR_FAILED         失败
*/
int32 mrc_bmpShow(BMP* hBMP,
   int16 x,
   int16 y,
   int16 w,
   int16 h,
   uint16 rop,
   int16 sx,
   int16 sy
)
{
		return mrc_bmpBufferDraw(SCREENBUFINDEX,hBMP,x,y,w,h,rop,sx,sy);
}



/*
	将缓冲中的256色bmp图片，从缓冲中的图片的(sx, sy)
	开始的宽高为w, h的区域，绘制到(x, y)开始的屏幕缓冲中。

输入:
p        源图片指针
x,y              屏幕位置
rop              选择如下：
   BM_OR,         			//SRC .OR. DST*   半透明效果
   BM_XOR,        			//SRC .XOR. DST*
   BM_COPY,       		//DST = SRC*      覆盖
   BM_NOT,        			//DST = (!SRC)*
   BM_MERGENOT,   	//DST .OR. (!SRC)
   BM_ANDNOT,     		//DST .AND. (!SRC)
   BM_TRANSPARENT,//透明色不显示，图片的第一个象素（左上角坐标为(0,0)的象素）是透明色
   BM_AND,        			//DST AND SRC
   BM_GRAY,        		//灰度绘图， 相当于BM_TRANSPARENT＋灰度绘图：
                              DST灰度 ＝ 30%R + 59%G + 11%B图片序号
   BM_REVERSE     		//反向绘图，相当于BM_TRANSPARENT＋反向绘图（V1939）

sx,sy              源图片的起始位置
w,h             欲加载图片的宽高

返回:
      MR_SUCCESS     成功
      MR_FAILED         失败
*/

//将buffer中的256色bmp图片显示出来。
//调用mrc_bmpLoadFromBuffer和mrc_bmpShow的原因是，考虑到，如果
//直接将buffer强制类型转换为BMP类型的话，则存在这样的问题，
//首先，我们会进行字节序的转换，这样将修改buffer中的BMP头的数据；
//其次，由于我们对BMP头进行了扩展，增加了mrbBuffer的指针，所以
//我们的BMP头和BMP标准头不再兼容，在调用mrc_bmpFree时将会带来麻烦。
//而调用mrc_bmpLoadFromBuffer则可以回避这个问题，代价当然是
//增加了BMP头内存空间申请释放的开销。但是在读取BMP头的开销上，是一样的。
int32 mrc_bmpShowEx(uint8 *buffer,
   int16 x,
   int16 y,
   int16 w,
   int16 h,
   uint16 rop,
   int16 sx,
   int16 sy
)
{
	BMP *hBMP;
	uint16 magic;
	uint32 FileSize;
	int32 ret;
	mrc_bufferSt bufferSt;
	bufferSt.p=buffer;
	bufferSt.pos=0;
	bufferSt.len=6;//只需要大于等于需要事先读取的字节数即可。
	
	if(!buffer)
		return MR_FAILED;

	if ( !BufferReaduint16(&magic,(int32)&bufferSt))		
		return MR_FAILED;
	if(magic!=0x4D42)
		return MR_FAILED;
	if ( !BufferReaduint32(&FileSize,(int32)&bufferSt))		
		return MR_FAILED;
	
	if(FileSize<sizeof(BMP))
		return MR_FAILED;

	hBMP=mrc_bmpLoadFromBuffer(buffer, FileSize);
	if(hBMP==NULL)
		return MR_FAILED;
	ret=mrc_bmpShow(hBMP,x,y,w,h,rop,sx,sy);
	mrc_bmpFree(hBMP);
	return ret;
}

int32 mrc_PrintScreen(char *filename)
{
	BMP *Screen=NULL;
	mr_bitmapSt bitmapInfo;
	char tempname[]="Screen.bmp";
	int32 row,col,BytesPerRow;
	uint16 *p=NULL,depth=24;
	uint8 *dest=NULL;//,r,g,b;
	
	if(!filename)
		filename = tempname;

	if(mrc_bitmapGetInfo(SCREENBUFINDEX,&bitmapInfo)==MR_FAILED)
		return MR_FAILED;
	
	Screen=mrc_bmpCreate(bitmapInfo.w,bitmapInfo.h,depth);
	if(!Screen)
		return MR_FAILED;
	
	BytesPerRow = bitmapInfo.w * (depth/8);
	BytesPerRow += ( BytesPerRow % 4 ? 4 - BytesPerRow % 4 : 0 );	
	for(row=0;row<bitmapInfo.h;row++)
	{
		p=bitmapInfo.p+row*bitmapInfo.w;
		//dest=Screen[0].Data+row*BytesPerRow;		
		dest=Screen[0].Data+(bitmapInfo.h-row)*BytesPerRow;
		if(depth==16)
		{
			mrc_memcpy(dest,(uint8 *)p,bitmapInfo.w*sizeof(uint16));
		}
		else if(depth==24)
		{
			for(col=0;col<bitmapInfo.w;col++)
			{	
/*			
				//我们手机上是使用565格式的16bit BMP。
				r=(uint8)(*p>>11);
				g=(uint8)((*p>>5)&0x3F);
				b=(uint8)(*p&0x1F);
		
				//PC上使用655格式?注意RGB的存储顺序。						
				*dest++=(b<<2);
				*dest++=(g<<2);
				*dest++=(r<<3);
*/
				*dest++= (*p&0x1f)<<3;		//蓝色，低5bit，左移3bit。
				*dest++= (*p&0x7e0)>>3;	//绿色，中6bit，右移3bit。
				*dest++= (*p&0xf800)>>8;//红色，高5bit，右移8bit。

				p++;
			}
		}
	}	

	
	mrc_bmpWriteFile(Screen,filename);
	mrc_bmpFree(Screen);
	if(mrc_bmpGetError() == mrc_bmpOK)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


/******************************************************************************
 * 绘图函数, 支持水平翻转和垂直翻转
 * rop : (BM_TRANSPARENT 或者 BM_COPY ) | (FLIP_HORI 或者 FLIP_VERT)
 * transcolor : 透明色 (16bit)
 *****************************************************************************/

int32 bitmapShowFlip(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
						   int16 sx, int16 sy, uint16 transcolor)
{
	uint16 *dstp,*srcp;
	int MaxY, MaxX;
	int MinY = MAX(0, y);
	int MinX = MAX(0, x);
	uint16 dx,dy;
	uint16 BitmapRop, BitmapFlip;
	uint16 * screenBuf;
	mr_bitmapSt screeninfo;
	int32 scrWidth, scrHeight;
	
	mrc_bitmapGetInfo(SCREENBUFINDEX,&screeninfo);
	scrWidth = screeninfo.w;//240;//screeninfo.width;
	scrHeight = screeninfo.h;//320;//screeninfo.height;
	MaxY = MIN(scrHeight, y+h);
	MaxX = MIN(scrWidth, x+w);
	screenBuf = (uint16 *)w_getScreenBuffer();
	BitmapRop = rop & 0xff;
	BitmapFlip = rop&0xff00;
	
	switch (BitmapRop) 
	{
	case BM_TRANSPARENT:
		for (dy=MinY; dy < MaxY; dy++)
		{
			dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
			if(BitmapFlip==FLIP_VERT)
			{
				srcp = p + (h - 1 - (dy - y - sy)) * mw + (MinX - x + sx);
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*srcp != transcolor)
						*dstp = *srcp;
					dstp++;
					srcp++;
				}		 
			}
			else if(BitmapFlip==FLIP_HORI)
			{
				srcp = p + (dy - y+sy) * mw + sx;
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*(srcp + w - (dx-x)-1) != transcolor)
						*dstp = *(srcp + w - (dx-x)-1);
					dstp++;
				}
				//srcp+= MaxX;
			}
			else
			{
				srcp = p + (dy - y+sy) * mw + (MinX - x + sx);
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*srcp != transcolor)
						*dstp = *srcp;
					dstp++;
					srcp++;
				}		 
			}
		}
		break;
	default:
	case BM_COPY:
		if (MaxX > MinX)
		{
			for (dy=MinY; dy < MaxY; dy++)
			{
				dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
				if(BitmapFlip==FLIP_VERT) 
				{
					srcp = p + (h - 1 - (dy - y + sy)) * mw + (MinX - x + sx);
					memcpy(dstp, srcp, (MaxX - MinX)<<1);
				}
				else if(BitmapFlip==FLIP_HORI)
				{
					srcp = p + (dy - y + sy) * mw + sx;
					for (dx = MinX; dx < MaxX; dx++)
					{
						*dstp = *(srcp + w - (dx-x)-1);
						dstp++;
					}
					//srcp+= MaxX;
				}
				else
				{
					srcp = p + (dy - y + sy) * mw + (MinX - x + sx);
					memcpy(dstp, srcp, (MaxX - MinX)<<1);
				}
			}
		}
		break;	
	}
	return MR_SUCCESS;
}

int32 mrc_bitmapShowFlip(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
						   int16 sx, int16 sy, uint16 transcolor)
{
	uint16 *dstp,*srcp;
	int MaxY, MaxX;
	int MinY = MAX(0, y);
	int MinX = MAX(0, x);
	uint16 dx,dy;
	uint16 BitmapRop, BitmapFlip;
	uint16 * screenBuf;
	mr_screeninfo  screeninfo;
	int32 scrWidth, scrHeight;

	if(NULL == p)
		return MR_FAILED;
		
	mrc_getScreenInfo(&screeninfo);
	scrWidth = screeninfo.width;
	scrHeight = screeninfo.height;
	MaxY = MIN(scrHeight, y+h);
	MaxX = MIN(scrWidth, x+w);
	screenBuf = (uint16 *)w_getScreenBuffer();
	BitmapRop = rop & 0xff;
	BitmapFlip = rop&0xff00;
	
	switch (BitmapRop) 
	{
	case BM_TRANSPARENT:
		for (dy=MinY; dy < MaxY; dy++)
		{
			dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
			switch(BitmapFlip)
			{
			case TRANS_MIRROR_ROT180:
				srcp = p + (h - 1 - (dy - y - sy)) * mw + (MinX - x + sx);
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*srcp != transcolor)
						*dstp = *srcp;
					dstp++;
					srcp++;
				}		 
				break;
			case TRANS_MIRROR:
				srcp = p + (dy - y+sy) * mw + sx;
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*(srcp + w - (dx-x)-1) != transcolor)
						*dstp = *(srcp + w - (dx-x)-1);
					dstp++;
				}
				break;
			case TRANS_ROT90:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
				
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p + (h-1+sy)*mw +(dy-MinY)+sx;
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0 && (*(srcp+mw*x-y) != transcolor))
							*dstp = *(srcp+mw*x-y);
						else if(x<0 && (*(srcp+mw*x) != transcolor))
							*dstp = *(srcp+mw*x);
						else if(y<0 && (*(srcp-y) != transcolor))
							*dstp = *(srcp-y);
						else if(*srcp != transcolor)
							*dstp = *srcp;
							
						dstp++;
						srcp = srcp - mw;
					}
				}
				break;
			case TRANS_ROT180:
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p + (h-1+sy)*mw +w-(dy-MinY)*mw-1+sx;
					
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0 && (*(srcp+x+y*mw) != transcolor))
							*dstp = *(srcp+x+y*mw);
						else if(x < 0 && (*(srcp+x) != transcolor))
							*dstp = *(srcp+x);
						else if(y < 0 && (*(srcp+y*mw) != transcolor))
							*dstp = *(srcp+y*mw);
						else if(*srcp != transcolor)
							*dstp = *srcp;
							
						dstp++;
						srcp--;
					}
				}
				break;
			case TRANS_ROT270:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
								
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);

					srcp = p +sx+sy * mw + w-(dy-MinY)-1;
					
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0 && (*(srcp-mw*x+y) != transcolor))
							*dstp = *(srcp-mw*x+y);
						else if(x<0 && (*(srcp-mw*x) != transcolor))
							*dstp = *(srcp-mw*x);
						else if(y<0 && (*(srcp+y)!= transcolor))
							*dstp = *(srcp+y);
						else if(*srcp != transcolor)
							*dstp = *srcp;

						dstp++;
						srcp = srcp + mw;
					}
				}
				break;
			case TRANS_MIRROR_ROT90:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
							
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);

					srcp = p + sx +(h+sy-1)*mw+w-dy+MinY-1;

					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0 && (*(srcp+mw*x+y) != transcolor))
							*dstp = *(srcp+mw*x+y);
						else if(x<0 && (*(srcp+mw*x) != transcolor))
							*dstp = *(srcp+mw*x);
						else if(y<0 && (*(srcp+y) != transcolor))
							*dstp = *(srcp+y);
						else if(*srcp != transcolor)
							*dstp = *srcp;

						dstp++;
						srcp = srcp - mw;
					}
				}
				break;
			case TRANS_MIRROR_ROT270:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
								
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p+(sy)*mw-MinY+sx+dy;
					//srcp = p+dy-MinY+sx;
					
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0 && (*(srcp-mw*x-y)!=transcolor))
							*dstp = *(srcp-mw*x-y);
						else if(x<0 && (*(srcp-mw*x) != transcolor))
							*dstp = *(srcp-mw*x);
						else if(y<0 && (*(srcp-y) != transcolor))
							*dstp = *(srcp-y);
						else if(*srcp != transcolor)
							*dstp = *srcp;

						dstp++;
						srcp = srcp + mw;
					}
				}
				break;
			default:
				srcp = p + (dy - y+sy) * mw + (MinX - x + sx);
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*srcp != transcolor)
						*dstp = *srcp;
					dstp++;
					srcp++;
				}	
				break;
			}
		}
		break;
	default:
	case BM_COPY:
		if (MaxX > MinX)
		{
			switch(BitmapFlip)
			{
			case TRANS_MIRROR_ROT180:
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p + (h - 1 - (dy - y + sy)) * mw + (MinX - x + sx);
					memcpy(dstp, srcp, (MaxX - MinX)<<1);
				}
				break;
			case TRANS_MIRROR:
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p + (dy - y + sy) * mw + sx;
					for (dx = MinX; dx < MaxX; dx++)
					{
						*dstp = *(srcp + w - (dx-x)-1);
						dstp++;
					}
				}
				break;
			case TRANS_ROT90:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
				
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p + (h-1+sy)*mw +(dy-MinY)+sx;
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0)
							*dstp = *(srcp+mw*x-y);
						else if(x<0)
							*dstp = *(srcp+mw*x);
						else if(y<0)
							*dstp = *(srcp-y);
						else
							*dstp = *srcp;
							
						dstp++;
						srcp = srcp - mw;
					}
				}
				break;
			case TRANS_ROT180:
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p + (h-1+sy)*mw +w-(dy-MinY)*mw-1+sx;
					
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0)
							*dstp = *(srcp+x+y*mw);
						else if(x < 0)
							*dstp = *(srcp+x);
						else if(y < 0)
							*dstp = *(srcp+y*mw);
						else
							*dstp = *srcp;
							
						dstp++;
						srcp--;
					}
				}
				break;
			case TRANS_ROT270:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
								
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);

					srcp = p +sx+sy * mw + w-(dy-MinY)-1;
					
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0)
							*dstp = *(srcp-mw*x+y);
						else if(x<0)
							*dstp = *(srcp-mw*x);
						else if(y<0)
							*dstp = *(srcp+y);
						else
							*dstp = *srcp;

						dstp++;
						srcp = srcp + mw;
					}
				}
				break;
			case TRANS_MIRROR_ROT90:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
							
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);

					srcp = p + sx +(h+sy-1)*mw+w-dy+MinY-1;

					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0)
							*dstp = *(srcp+mw*x+y);
						else if(x<0)
							*dstp = *(srcp+mw*x);
						else if(y<0)
							*dstp = *(srcp+y);
						else
							*dstp = *srcp;

						dstp++;
						srcp = srcp - mw;
					}
				}
				break;
			case TRANS_MIRROR_ROT270:
				if(w != h)
				{
					MaxY = MIN(scrHeight, y+w);
					MaxX = MIN(scrWidth, x+h);
				}	
								
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);

					srcp = p+(sy)*mw-MinY+sx+dy;
					
					for (dx = MinX; dx < MaxX; dx++)
					{
						if(x<0 && y<0)
							*dstp = *(srcp-mw*x-y);
						else if(x<0)
							*dstp = *(srcp-mw*x);
						else if(y<0)
							*dstp = *(srcp-y);
						else
							*dstp = *srcp;

						dstp++;
						srcp = srcp + mw;
					}
				}
				break;
			default:
				for (dy=MinY; dy < MaxY; dy++)
				{
					dstp = (uint16 *)(screenBuf + dy * scrWidth + MinX);
					srcp = p + (dy - y + sy) * mw + (MinX - x + sx);
					memcpy(dstp, srcp, (MaxX - MinX)<<1);
				}
				break;
			}
		}
		break;	
	}
	return MR_SUCCESS;
}

