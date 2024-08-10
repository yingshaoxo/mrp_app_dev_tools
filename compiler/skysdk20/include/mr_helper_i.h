

#define memzero(s, n)     memset ((void *)(s), 0, (n))
#if 1
#define  MRDBGPRINTF       mr_printf           
#else
#define  MRDBGPRINTF       mrc_printf           
#endif



