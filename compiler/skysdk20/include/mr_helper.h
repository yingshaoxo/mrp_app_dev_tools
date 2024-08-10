

#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#ifndef SDK_MOD
	typedef  unsigned long long  uint64;      /* Unsigned 64 bit value */
	typedef  long long                int64;      /* signed 64 bit value */
#else
	typedef	unsigned _int64	uint64;
	typedef	 _int64	int64;
#endif
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  long int                int32;      /* signed 32 bit value */
typedef  unsigned char      uint8;        /*Unsigned  Signed 8  bit value */
typedef  signed char          int8;        /* Signed 8  bit value */
typedef  signed short         int16;       /* Signed 16 bit value */

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef TRUE
  #define TRUE 1
#endif

#if 1

#ifdef SDK_MOD
typedef long int size_t;
#else
#define size_t int
#endif

enum {
   MR_KEY_0,               //按键 0
   MR_KEY_1,               //按键 1
   MR_KEY_2,               //按键 2
   MR_KEY_3,               //按键 3
   MR_KEY_4,               //按键 4
   MR_KEY_5,               //按键 5
   MR_KEY_6,               //按键 6
   MR_KEY_7,               //按键 7
   MR_KEY_8,               //按键 8
   MR_KEY_9,               //按键 9
   MR_KEY_STAR,            //按键 *
   MR_KEY_POUND,           //按键 #
   MR_KEY_UP,              //按键 上
   MR_KEY_DOWN,            //按键 下
   MR_KEY_LEFT,            //按键 左
   MR_KEY_RIGHT,           //按键 右
   MR_KEY_POWER,           //按键 挂机键
   MR_KEY_SOFTLEFT,        //按键 左软键
   MR_KEY_SOFTRIGHT,       //按键 右软键
   MR_KEY_SEND,            //按键 接听键
   MR_KEY_SELECT,          //按键 确认/选择（若方向键中间有确认键，建议设为该键）
   MR_KEY_VOLUME_UP,       //按键 侧键上
   MR_KEY_VOLUME_DOWN,     //按键 侧键下
   MR_KEY_NONE             //按键 保留
};

enum {
   MR_NET_ID_MOBILE,       //移动
   MR_NET_ID_CN,          // 联通gsm
   MR_NET_ID_CDMA,       //联通CDMA
   MR_NET_ID_NONE       //未插卡
};

enum {
   MR_EDIT_ANY,                  //任何字符
   MR_EDIT_NUMERIC,          // 数字
   MR_EDIT_PASSWORD       //密码，用"*"显示
};

enum {
   MR_SIM_NEW,     //新手机第一次插入SIM卡
   MR_SIM_CHANGE, //用户更换SIM卡
   MR_SIM_SAME    //未进行换卡操作
};

enum {
   MR_DIALOG_OK,                   //对话框有"确定"键。
   MR_DIALOG_OK_CANCEL,
   MR_DIALOG_CANCEL
};

enum {
   MR_DIALOG_KEY_OK,
   MR_DIALOG_KEY_CANCEL
};

enum {
   MR_KEY_PRESS,
   MR_KEY_RELEASE,
   MR_MOUSE_DOWN,
   MR_MOUSE_UP,
   MR_MENU_SELECT,
   MR_MENU_RETURN,
   MR_DIALOG_EVENT,
   MR_SMS_INDICATION,
   MR_EXIT_EVENT,
   MR_TIMER_EVENT = 1001,
   MR_NET_EVENT
};

typedef enum 
{
   MR_SOUND_MIDI,
   MR_SOUND_WAV,
   MR_SOUND_MP3,
   MR_SOUND_PCM    //8K
}MR_SOUND_TYPE;

typedef enum 
{
   MR_FONT_SMALL,
   MR_FONT_MEDIUM,
   MR_FONT_BIG
}MR_FONT_TYPE;

enum
{
   MR_SEEK_SET, 
   MR_SEEK_CUR, 
   MR_SEEK_END
};

enum
{
   MR_SOCK_STREAM,
   MR_SOCK_DGRAM
};

enum
{
   MR_IPPROTO_TCP,
   MR_IPPROTO_UDP
};

enum
{
   MR_ENCODE_ASCII,
   MR_ENCODE_UNICODE
};

#define   MR_SMS_ENCODE_FLAG   7
#define   MR_SMS_REPORT_FLAG   8
#define   MR_SMS_RESULT_FLAG   16

enum {
   MR_SOCKET_BLOCK,          //阻塞方式（同步方式）
   MR_SOCKET_NONBLOCK       //非阻塞方式（异步方式）
};

#ifdef MR_SPREADTRUM_MOD
#pragma no_check_stack
#endif

#define MR_SUCCESS  0    //成功
#define MR_FAILED   -1    //失败
#define MR_IGNORE   1     //不关心
#define MR_WAITING   2     //异步(非阻塞)模式

typedef struct
{
   uint32 width;                  //屏幕宽
   uint32 height;                 //屏幕高
   uint32 bit;                    //屏幕象素深度，单位bit
}mr_screeninfo;

typedef struct
{
   uint16 year;                 //年
   uint8  month;                //月
   uint8  day;                  //日
   uint8  hour;                 //时，24小时制
   uint8  minute;               //分
   uint8  second;               //秒
}mr_datetime;

typedef struct
{
	uint8       IMEI[16];	      //IMEI len eq 15
	uint8       IMSI[16];	      //IMSI len not more then 15
	char        manufactory[8];  //厂商名，最大7个字符，空字节填\0
	char        type[8];         //mobile type，最大7个字符，空字节填\0
	uint32      ver;            //SW ver
	uint8       spare[12];       //备用
}mr_userinfo;


typedef void (*mrc_timerCB)(int32 data);

typedef struct mrc_timerSt                /* TIMER CONTROL BLOCK      */
{
    int32 check;                           /* check this value,is the timer valid?             */
    int32 time;                           /* timeout time             */
    int32 left;                           /* time left before timeout */
    mrc_timerCB handler;                      /* event handler            */
    int32 data;
    int32 loop;
    struct mrc_timerSt *next;             /* next in active chain     */
    struct mrc_timerSt *next2;             /* next in timeout chain     */
} mrc_timerSt;




extern int32 mr_c_function_load(int32 code);
extern int32 mr_c_function_load_thumb(int32 code);


typedef int32 (*MR_C_FUNCTION)(void* P, int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len);
typedef void (*T_mr_printf)(const char *format,...);
typedef int32 (*T_mr_mem_get )(char** mem_base, uint32* mem_len);


typedef int32 (*T_mr_mem_free )(char* mem, uint32 mem_len);


typedef void (*T_mr_drawBitmap)(uint16* bmp, int16 x, int16 y, uint16 w, uint16 h);
typedef char *(*T_mr_getCharBitmap)(uint16 ch, uint16 fontSize, int *width, int *height);

typedef int32 (*T_mr_timerStart)(uint16 t);
typedef int32 (*T_mr_timerStop)(void);
typedef uint32 (*T_mr_getTime)(void);
typedef int32 (*T_mr_getDatetime)(mr_datetime* datetime);
typedef int32 (*T_mr_getUserInfo)(mr_userinfo* info);
typedef int32 (*T_mr_sleep)(uint32 ms);
typedef int32 (*T_mr_plat)(int32 code, int32 param);

typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);
typedef int32 (*T_mr_platEx)(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


typedef int32 (*T_mr_ferrno)(void);
typedef int32 (*T_mr_open)(const char* filename,  uint32 mode);
typedef int32 (*T_mr_close)(int32 f);
typedef int32 (*T_mr_info)(const char* filename);
typedef int32 (*T_mr_write)(int32 f,void *p,uint32 l);
typedef int32 (*T_mr_read)(int32 f,void *p,uint32 l);
typedef int32 (*T_mr_seek)(int32 f, int32 pos, int method);
typedef int32 (*T_mr_getLen)(const char* filename);
typedef int32 (*T_mr_remove)(const char* filename);
typedef int32 (*T_mr_rename)(const char* oldname, const char* newname);
typedef int32 (*T_mr_mkDir)(const char* name);
typedef int32 (*T_mr_rmDir)(const char* name);


typedef int32 (*T_mr_findStart)(const char* name, char* buffer, uint32 len);
typedef int32 (*T_mr_findGetNext)(int32 search_handle, char* buffer, uint32 len);
typedef int32 (*T_mr_findStop)(int32 search_handle);
typedef int32 (*T_mr_exit)(void);

typedef int32 (*T_mr_startShake)(int32 ms);
typedef int32 (*T_mr_stopShake)(void);

typedef int32 (*T_mr_playSound)(int type, const void* data, uint32 dataLen, int32 loop);
typedef int32 (*T_mr_stopSound )(int type);

typedef int32 (*T_mr_sendSms)(char* pNumber, char*pContent, int32 flags);
typedef void (*T_mr_call)(char *number);
typedef int32 (*T_mr_getNetworkID)(void);
typedef void (*T_mr_connectWAP)(char* wap);


typedef void (*T_mr_platDrawChar)(uint16 ch, int32 x, int32 y, int32 color);


typedef int32 (*T_mr_menuCreate)(const char* title, int16 num);
typedef int32 (*T_mr_menuSetItem)(int32 menu, const char *text, int32 index);
typedef int32 (*T_mr_menuShow)(int32 menu);
typedef int32 (*T_mr_menuSetFocus)(int32 menu, int32 index);
typedef int32 (*T_mr_menuRelease)(int32 menu);
typedef int32 (*T_mr_menuRefresh)(int32 menu);



typedef int32 (*T_mr_dialogCreate)(const char * title, const char * text, int32 type);
typedef int32 (*T_mr_dialogRelease)(int32 dialog);
typedef int32 (*T_mr_dialogRefresh)(int32 dialog, const char * title, const char * text, int32 type);

typedef int32 (*T_mr_textCreate)(const char * title, const char * text, int32 type);
typedef int32 (*T_mr_textRelease)(int32 text);
typedef int32 (*T_mr_textRefresh)(int32 handle, const char * title, const char * text);

typedef int32 (*T_mr_editCreate)(const char * title, const char * text, int32 type, int32 max_size);
typedef int32 (*T_mr_editRelease)(int32 edit);
typedef const char* (*T_mr_editGetText)(int32 edit);

typedef int32 (*T_mr_winCreate)(void);
typedef int32 (*T_mr_winRelease)(int32 win);

typedef int32 (*T_mr_getScreenInfo)(mr_screeninfo * screeninfo);

typedef int32 (*MR_INIT_NETWORK_CB)(int32 result);
typedef int32 (*MR_GET_HOST_CB)(int32 ip);


typedef int32 (*T_mr_initNetwork)(MR_INIT_NETWORK_CB cb, const char *mode);
typedef int32 (*T_mr_closeNetwork)(void);
typedef int32 (*T_mr_getHostByName)(const char *name, MR_GET_HOST_CB cb);
typedef int32 (*T_mr_socket)(int32 type, int32 protocol);
typedef int32 (*T_mr_connect)(int32 s, int32 ip, uint16 port, int32 type);
typedef int32 (*T_mr_closeSocket)(int32 s);
typedef int32 (*T_mr_recv)(int32 s, char *buf, int len);
typedef int32 (*T_mr_recvfrom)(int32 s, char *buf, int len, int32 *ip, uint16 *port);
typedef int32 (*T_mr_send)(int32 s, const char *buf, int len);
typedef int32 (*T_mr_sendto)(int32 s, const char *buf, int len, int32 ip, uint16 port);


typedef void* (*T_mr_malloc)(uint32 len);
typedef void  (*T_mr_free)(void* p, uint32 len);
typedef void* (*T_mr_realloc)(void* p, uint32 oldlen, uint32 len);

typedef void *(*T_memcpy)(void * s1, const void * s2, int n);
typedef void *(*T_memmove)(void * s1, const void * s2, int n);
typedef char *(*T_strcpy)(char * s1, const char * s2);
typedef char *(*T_strncpy)(char * s1, const char * s2, int n);
typedef char *(*T_strcat)(char * s1, const char * s2);
typedef char *(*T_strncat)(char * s1, const char * s2, int n);
typedef int (*T_memcmp)(const void * s1, const void * s2, int n);
typedef int (*T_strcmp)(const char * s1, const char * s2);
typedef int (*T_strncmp)(const char * s1, const char * s2, int n);
typedef int (*T_strcoll)(const char * s1, const char * s2);
typedef void *(*T_memchr)(const void * s, int c, int n);
typedef void *(*T_memset)(void * s, int c, int n);
typedef int (*T_strlen)(const char * s);
typedef char *(*T_strstr)(const char * s1, const char * s2);

typedef int (*T_sprintf)(char * s, const char * format, ...);
typedef int (*T_atoi)(const char * nptr);
typedef unsigned long int (*T_strtoul)(const char * nptr,
                                       char ** endptr, int base);
typedef int (*T_rand)(void);

typedef struct {
   uint16            w;
   uint16            h;
   uint32            buflen;
   uint32            type;
   uint16*            p;
}mr_bitmapSt;

typedef struct  {
   int16            x;
   int16            y;
   uint16            w;
   uint16            h;
   int16            x1;
   int16            y1;
   int16            x2;
   int16            y2;
   uint16            tilew;
   uint16            tileh;
}mr_tileSt;

typedef struct {
   void*            p;
   uint32            buflen;
   int32            type;
}mr_soundSt;

typedef struct {
   uint16            h;
}mr_spriteSt;

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

typedef struct md5_state_s {
    md5_word_t count[2];	/* message length in bits, lsw first */
    md5_word_t abcd[4];		/* digest buffer */
    md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

typedef struct {
   uint16*            p;
   uint16            w;
   uint16            h;
   uint16            x;
   uint16            y;
}mr_bitmapDrawSt;

typedef struct {
   int16 A;  // A, B, C, and D are fixed point values with an 8-bit integer part
   int16 B;  // and an 8-bit fractional part.
   int16 C;
   int16 D;
   uint16 rop;
} mr_transMatrixSt;

typedef struct {
   uint16            x;
   uint16            y;
   uint16            w;
   uint16            h;
}mr_screenRectSt;

typedef struct {
   uint8            r;
   uint8            g;
   uint8            b;
}mr_colourSt;

typedef struct
{
   int32   id;
   int32   ver;
   char*  sidName;
}mrc_appInfoSt;



typedef void (*T_mr_md5_init)(md5_state_t *pms);
typedef void (*T_mr_md5_append)(md5_state_t *pms, const md5_byte_t *data, int nbytes);
typedef void (*T_mr_md5_finish)(md5_state_t *pms, md5_byte_t digest[16]);

typedef  int32 (*T__mr_load_sms_cfg)(void);
typedef  int32 (*T__mr_save_sms_cfg)(int32 f);
typedef  int32 (*T__DispUpEx)(int16 x, int16 y, uint16 w, uint16 h);
typedef  void (*T__DrawPoint)(int16 x, int16 y, uint16 nativecolor);
typedef  void (*T__DrawBitmap)(uint16* p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
typedef  void (*T__DrawBitmapEx)(mr_bitmapDrawSt* srcbmp, mr_bitmapDrawSt* dstbmp, uint16 w, uint16 h, mr_transMatrixSt* pTrans, uint16 transcoler);
typedef  void (*T_DrawRect)(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
typedef  int32 (*T__DrawText)(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
typedef int (*T__BitmapCheck)(uint16*p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
typedef void * (*T__mr_readFile)(const char* filename, int *filelen, int lookfor);
typedef int (*T_mr_wstrlen)(char * txt);
typedef int32 (*T_mr_registerAPP)(uint8 *p, int32 len, int32 index);

typedef int32 (*T__mr_c_function_new)(MR_C_FUNCTION f, int32 len);


typedef int32 (*T__DrawTextEx)(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);
typedef int32 (*T__mr_EffSetCon)(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);
typedef int32 (*T__mr_TestCom)(int32 L, int input0, int input1);
typedef int32 (*T__mr_TestCom1)(int32 L, int input0, char* input1, int32 len);

/*
函数功能：将gb字符串转换为Unicode字符串，并申请一片内存保存Unicode字符串，将Unicode字符串
的指针返回。
cp: 输入的gb字符串；
err：填NULL；
size：输出的Unicode字符串长度；
返回：Unicode字符串指针*/
typedef uint16* (*T_c2u)(char *cp, int32 *err, int32 *size);

typedef int32 (*T__mr_div)(int32 a, int32 b);
typedef int32 (*T__mr_mod)(int32 a, int32 b);


typedef int32 (*T_mrp_error) (int32 L);


typedef void (*T_mrp_settop) (int32 L, int idx);


typedef struct{
   uint8**                  mr_m0_files;
   uint32*                  vm_state;
   int32*                  mr_state;
   int32*                  _bi;
   
   void**                  mr_timer_p;
   uint32*                  mr_timer_state;
   void*                  mr_timer_run_without_pause;
   
   void*                  mr_gzInBuf;
   void*                  mr_gzOutBuf;
   void*                  LG_gzinptr;
   void*                  LG_gzoutcnt;
   
   int32*                  mr_sms_cfg_need_save;
   void*                  _mr_smsSetBytes;
   void*                  _mr_smsAddNum;
   void*                  _mr_newSIMInd;
   
   void*                  _mr_isMr;


   void*                   mrp_gettop; //1937
   T_mrp_settop   mrp_settop;
   void*   mrp_pushvalue;
   void*   mrp_remove;
   void*   mrp_insert;
   void*   mrp_replace;
   
   void*   mrp_isnumber;
   void*   mrp_isstring;
   void*   mrp_iscfunction;
   void*   mrp_isuserdata;
   void*   mrp_type;
   void*   mrp_typename;
   void*   mrp_shorttypename;
   
   
   void*   mrp_equal;
   void*   mrp_rawequal;
   void*   mrp_lessthan;
   
   void*   mrp_tonumber;
   void*   mrp_toboolean;
   void*   mrp_tostring;
   void*   mrp_strlen;
   void*   mrp_tostring_t;
   void*   mrp_strlen_t;
   void*   mrp_tocfunction;
   void*   mrp_touserdata;
   void*   mrp_tothread;
   void*   mrp_topointer;
   
   void*   mrp_pushnil;
   void*   mrp_pushnumber;
   void*   mrp_pushlstring;
   void*   mrp_pushstring;
   void*   mrp_pushvfstring;
   void*   mrp_pushfstring;
   void*   mrp_pushboolean;
   void*   mrp_pushcclosure;
   
   
   void*   mrp_gettable;
   void*   mrp_rawget;
   void*   mrp_rawgeti;
   void*   mrp_newtable;
   void*   mrp_getmetatable;
   
   
   void*   mrp_settable;
   void*   mrp_rawset;
   void*   mrp_rawseti;
   
   
   void*   mrp_call;
   void*   mrp_pcall;
   void*   mrp_load;
   
   void*   mrp_getgcthreshold;
   void*   mrp_setgcthreshold;
   
   
   T_mrp_error   mrp_error;

   void*   mrp_checkstack;
   void*   mrp_newuserdata;
   void*   mrp_getfenv;
   void*   mrp_setfenv;
   void*   mrp_setmetatable;
   void*   mrp_cpcall;
   void*   mrp_next;
   void*   mrp_concat;
   void*   mrp_pushlightuserdata;
   void*   mrp_getgccount;
   void*   mrp_dump;
   void*   mrp_yield;
   void*   mrp_resume;

}mr_internal_table;


typedef int32 (*T_mr_c_gcfunction)(int32 code);

typedef struct{
   void*          reserve0;  
   void*          reserve1;  
   void*          reserve2;  
   T_mr_c_gcfunction mr_c_gcfunction;
}mr_c_port_table;


typedef struct {
  
   T_mr_malloc    mr_malloc;
   T_mr_free      mr_free;
   T_mr_realloc   mr_realloc;  
   

   T_memcpy       memcpy;
   T_memmove      memmove;
   T_strcpy       strcpy;
   T_strncpy      strncpy;
   T_strcat       strcat;
   T_strncat      strncat;
   T_memcmp       memcmp;
   T_strcmp       strcmp;
   T_strncmp      strncmp;
   T_strcoll      strcoll;
   T_memchr       memchr;
   T_memset       memset;
   T_strlen       strlen;
   T_strstr       strstr;
   T_sprintf      sprintf;
   T_atoi         atoi;
   T_strtoul      strtoul;  
   T_rand       rand; 
  
  
   void*          reserve0;  
   void*          reserve1;  
   mr_internal_table*          _mr_c_internal_table;  
   mr_c_port_table*          _mr_c_port_table;  
   T__mr_c_function_new _mr_c_function_new;  

   T_mr_printf              mr_printf;
   T_mr_mem_get             mr_mem_get ;
   T_mr_mem_free            mr_mem_free ;
   T_mr_drawBitmap          mr_drawBitmap;
   T_mr_getCharBitmap       mr_getCharBitmap;
   T_mr_timerStart          g_mr_timerStart;
   T_mr_timerStop           g_mr_timerStop;
   T_mr_getTime             mr_getTime;
   T_mr_getDatetime         mr_getDatetime;
   T_mr_getUserInfo         mr_getUserInfo;
   T_mr_sleep               mr_sleep;   
   
   T_mr_plat                mr_plat;
   T_mr_platEx              mr_platEx;
   
   T_mr_ferrno              mr_ferrno;
   T_mr_open                mr_open;
   T_mr_close               mr_close;
   T_mr_info                mr_info;
   T_mr_write               mr_write;
   T_mr_read                mr_read;
   T_mr_seek                mr_seek;
   T_mr_getLen              mr_getLen;
   T_mr_remove              mr_remove;
   T_mr_rename              mr_rename;
   T_mr_mkDir               mr_mkDir;
   T_mr_rmDir               mr_rmDir;
   T_mr_findStart           mr_findStart;
   T_mr_findGetNext         mr_findGetNext;
   T_mr_findStop            mr_findStop;    
   
   T_mr_exit                mr_exit;
   T_mr_startShake          mr_startShake;
   T_mr_stopShake           mr_stopShake;
   T_mr_playSound           mr_playSound;
   T_mr_stopSound           mr_stopSound ;  
   
   T_mr_sendSms             mr_sendSms;
   T_mr_call                mr_call;
   T_mr_getNetworkID        mr_getNetworkID;
   T_mr_connectWAP          mr_connectWAP;   
   
   T_mr_menuCreate          mr_menuCreate;
   T_mr_menuSetItem         mr_menuSetItem;
   T_mr_menuShow            mr_menuShow;
   void*                    reserve;
   T_mr_menuRelease         mr_menuRelease;
   T_mr_menuRefresh         mr_menuRefresh;
   T_mr_dialogCreate        mr_dialogCreate;
   T_mr_dialogRelease       mr_dialogRelease;
   T_mr_dialogRefresh       mr_dialogRefresh;
   T_mr_textCreate          mr_textCreate;
   T_mr_textRelease         mr_textRelease;
   T_mr_textRefresh         mr_textRefresh;
   T_mr_editCreate          mr_editCreate;
   T_mr_editRelease         mr_editRelease;
   T_mr_editGetText         mr_editGetText;
   T_mr_winCreate           mr_winCreate;
   T_mr_winRelease          mr_winRelease;
   
   T_mr_getScreenInfo       mr_getScreenInfo;
   
   T_mr_initNetwork         mr_initNetwork;
   T_mr_closeNetwork        mr_closeNetwork;
   T_mr_getHostByName       mr_getHostByName;
   T_mr_socket              mr_socket;
   T_mr_connect             mr_connect;
   T_mr_closeSocket         mr_closeSocket;
   T_mr_recv                mr_recv;
   T_mr_recvfrom            mr_recvfrom;
   T_mr_send                mr_send;
   T_mr_sendto              mr_sendto;
   
   uint16**               mr_screenBuf;
   int32*                 mr_screen_w;
   int32*                 mr_screen_h;
   int32*                 mr_screen_bit;
   mr_bitmapSt*           mr_bitmap;
   mr_tileSt*             mr_tile;
   int16**                mr_map;
   mr_soundSt*            mr_sound;
   mr_spriteSt*           mr_sprite;
   
   char*                  pack_filename;
   char*                  start_filename;
   char*                  old_pack_filename;
   char*                  old_start_filename;
   
   
   char**                 mr_ram_file;
   int32*                 mr_ram_file_len;
   
   int8*                  mr_soundOn;
   int8*                  mr_shakeOn;
   
   char**                 LG_mem_base;
   int32*                 LG_mem_len;
   char**                 LG_mem_end;
   int32*                 LG_mem_left;

   uint8*                 mr_sms_cfg_buf;
   T_mr_md5_init          mr_md5_init;
   T_mr_md5_append        mr_md5_append;
   T_mr_md5_finish        mr_md5_finish;
   T__mr_load_sms_cfg     _mr_load_sms_cfg;
   T__mr_save_sms_cfg     _mr_save_sms_cfg;
   T__DispUpEx            _DispUpEx;

   T__DrawPoint           _DrawPoint;
   T__DrawBitmap          _DrawBitmap;
   T__DrawBitmapEx        _DrawBitmapEx;
   T_DrawRect             DrawRect;
   T__DrawText            _DrawText;
   T__BitmapCheck         _BitmapCheck;
   T__mr_readFile         _mr_readFile;
   T_mr_wstrlen           mr_wstrlen;
   T_mr_registerAPP       mr_registerAPP;
   T__DrawTextEx          _DrawTextEx;  //1936
   T__mr_EffSetCon        _mr_EffSetCon;
   T__mr_TestCom          _mr_TestCom;
   T__mr_TestCom1         _mr_TestCom1;//1938
   T_c2u                  c2u;  //1939

   T__mr_div _mr_div;           //1941
   T__mr_mod _mr_mod;
   
   uint32*   LG_mem_min;
   uint32*    LG_mem_top;
   void*      mr_updcrc;    //1943
   char*       start_fileparameter;//1945
   void*       mr_sms_return_flag;//1949
   void*       mr_sms_return_val;
   void*      mr_unzip;   //1950
   mrc_timerCB*  mr_exit_cb;//1951
   int32*        mr_exit_cb_data;//1951
   char*         mr_entry;//1952,V2000-V2002不支持
   T_mr_platDrawChar   mr_platDrawChar; //1961
}mr_table;

#ifdef SDK_MOD
typedef struct _mr_c_function_st mr_c_function_st;

extern mr_table *mr_get_helper(void);
extern  mr_c_function_st *mr_get_c_function_p(void);

#define GET_HELPER()      mr_get_helper()
#define GET_C_FUNCTION_P()      mr_get_c_function_p()
#endif

#define MR_NEW_CFUNCTION(f,l)                   GET_HELPER()->_mr_c_function_new(f,l)

#if 1
#define MEMCPY       GET_HELPER()->memcpy
#define MEMMOVE      GET_HELPER()->memmove
#define MEMSET        GET_HELPER()->memset
#define MEMCMP            GET_HELPER()->memcmp
#define MEMCHR            GET_HELPER()->memchr
#define MEMSTR              GET_HELPER()->memstr
#define MEMRCHR             GET_HELPER()->memrchr
#define MEMCHREND           GET_HELPER()->memchrend
#define MEMRCHRBEGIN        GET_HELPER()->memrchrbegin
#define STRCPY            GET_HELPER()->strcpy
#define STRNCPY     GET_HELPER()->strncpy
#define STRNCMP          GET_HELPER()->strncmp
#define STRCOLL          GET_HELPER()->strcoll


#define STRICMP               GET_HELPER()->stricmp
#define STRNICMP         GET_HELPER()->strnicmp
#define STRCAT            GET_HELPER()->strcat
#define STRNCAT            GET_HELPER()->strncat
#define STRCMP               GET_HELPER()->strcmp
#define STRLEN                  GET_HELPER()->strlen
#define STRCHR               GET_HELPER()->strchr
#define STRCHREND            GET_HELPER()->strchrend
#define STRCHRSEND            GET_HELPER()->strchrsend
#define STRRCHR              GET_HELPER()->strrchr
#define STRSTR                 GET_HELPER()->strstr
#define STRISTR                GET_HELPER()->stristr
#define SPRINTF                     GET_HELPER()->sprintf
#define STRTOUL            GET_HELPER()->strtoul
#define VSPRINTF                    GET_HELPER()->vsprintf
#define ATOI                   GET_HELPER()->atoi
#define rand                        GET_HELPER()->rand
#endif

#undef MR_MALLOC
#undef MR_FREE
#undef MR_REALLOC

#define MR_MALLOC GET_HELPER()->mr_malloc
#define MR_FREE(a, b) GET_HELPER()->mr_free(a, b)
#define MR_REALLOC(b,os,s)  GET_HELPER()->mr_realloc(b,os,s)

#define  mr_printf              GET_HELPER()->mr_printf           
#if 1
#define  MRDBGPRINTF       mr_printf           
#else
#define  MRDBGPRINTF       mrc_printf           
#endif
//#define  MRDBGPRINTF                  

#define  mr_mem_get             GET_HELPER()->mr_mem_get          
#define  mr_mem_free            GET_HELPER()->mr_mem_free         
#define  mr_drawBitmap          GET_HELPER()->mr_drawBitmap       
#define  mr_getCharBitmap       GET_HELPER()->mr_getCharBitmap    
#define  mr_timerStart          GET_HELPER()->g_mr_timerStart       
#define  mr_timerStop           GET_HELPER()->g_mr_timerStop        
#define  mr_getTime             GET_HELPER()->mr_getTime          
#define  mr_getDatetime         GET_HELPER()->mr_getDatetime      
#define  mr_getUserInfo         GET_HELPER()->mr_getUserInfo      
#define  mr_sleep               GET_HELPER()->mr_sleep            

#define  mr_plat                GET_HELPER()->mr_plat             
#define  mr_platEx              GET_HELPER()->mr_platEx           

#define  mr_ferrno              GET_HELPER()->mr_ferrno           
#define  mr_open                GET_HELPER()->mr_open             
#define  mr_close               GET_HELPER()->mr_close            
#define  mr_info                GET_HELPER()->mr_info             
#define  mr_write               GET_HELPER()->mr_write            
#define  mr_read                GET_HELPER()->mr_read             
#define  mr_seek                GET_HELPER()->mr_seek             
#define  mr_getLen              GET_HELPER()->mr_getLen           
#define  mr_remove              GET_HELPER()->mr_remove           
#define  mr_rename              GET_HELPER()->mr_rename           
#define  mr_mkDir               GET_HELPER()->mr_mkDir            
#define  mr_rmDir               GET_HELPER()->mr_rmDir            
#define  mr_findStart           GET_HELPER()->mr_findStart        
#define  mr_findGetNext         GET_HELPER()->mr_findGetNext      
#define  mr_findStop            GET_HELPER()->mr_findStop         

#define  mr_exit                GET_HELPER()->mr_exit             
#define  mr_startShake          GET_HELPER()->mr_startShake       
#define  mr_stopShake           GET_HELPER()->mr_stopShake        
#define  mr_playSound           GET_HELPER()->mr_playSound        
#define  mr_stopSound           GET_HELPER()->mr_stopSound        

#define  mr_sendSms             GET_HELPER()->mr_sendSms          
#define  mr_call                GET_HELPER()->mr_call             
#define  mr_getNetworkID        GET_HELPER()->mr_getNetworkID     
#define  mr_connectWAP          GET_HELPER()->mr_connectWAP       

#define  mr_menuCreate          GET_HELPER()->mr_menuCreate       
#define  mr_menuSetItem         GET_HELPER()->mr_menuSetItem      
#define  mr_menuShow            GET_HELPER()->mr_menuShow         
#define  mr_menuRelease         GET_HELPER()->mr_menuRelease      
#define  mr_menuRefresh         GET_HELPER()->mr_menuRefresh      
#define  mr_dialogCreate        GET_HELPER()->mr_dialogCreate     
#define  mr_dialogRelease       GET_HELPER()->mr_dialogRelease    
#define  mr_dialogRefresh       GET_HELPER()->mr_dialogRefresh    
#define  mr_textCreate          GET_HELPER()->mr_textCreate       
#define  mr_textRelease         GET_HELPER()->mr_textRelease      
#define  mr_textRefresh         GET_HELPER()->mr_textRefresh      
#define  mr_editCreate          GET_HELPER()->mr_editCreate       
#define  mr_editRelease         GET_HELPER()->mr_editRelease      
#define  mr_editGetText         GET_HELPER()->mr_editGetText      
#define  mr_winCreate           GET_HELPER()->mr_winCreate        
#define  mr_winRelease          GET_HELPER()->mr_winRelease       

#define  mr_getScreenInfo       GET_HELPER()->mr_getScreenInfo    

#define  mr_initNetwork         GET_HELPER()->mr_initNetwork      
#define  mr_closeNetwork        GET_HELPER()->mr_closeNetwork     
#define  mr_getHostByName       GET_HELPER()->mr_getHostByName    
#define  mr_socket              GET_HELPER()->mr_socket           
#define  mr_connect             GET_HELPER()->mr_connect          
#define  mr_closeSocket         GET_HELPER()->mr_closeSocket      
#define  mr_recv                GET_HELPER()->mr_recv             
#define  mr_recvfrom            GET_HELPER()->mr_recvfrom         
#define  mr_send                GET_HELPER()->mr_send             
#define  mr_sendto              GET_HELPER()->mr_sendto           
#define  mr_md5_init            GET_HELPER()->mr_md5_init         
#define  mr_md5_append          GET_HELPER()->mr_md5_append       
#define  mr_md5_finish          GET_HELPER()->mr_md5_finish       
#define  _mr_load_sms_cfg       GET_HELPER()->_mr_load_sms_cfg    
#define  _mr_save_sms_cfg       GET_HELPER()->_mr_save_sms_cfg    
#define  _DispUpEx              GET_HELPER()->_DispUpEx           


#define  mr_platDrawChar              GET_HELPER()->mr_platDrawChar        


#define  _DrawPoint             GET_HELPER()->_DrawPoint          
#define  _DrawBitmap            GET_HELPER()->_DrawBitmap         
#define  _DrawBitmapEx          GET_HELPER()->_DrawBitmapEx       
#define  DrawRect               GET_HELPER()->DrawRect            
#define  _DrawText              GET_HELPER()->_DrawText           
#define  _BitmapCheck           GET_HELPER()->_BitmapCheck        
#define  MR_READFILE            GET_HELPER()->_mr_readFile        
#define  mr_wstrlen             GET_HELPER()->mr_wstrlen          
#define  mr_registerAPP         GET_HELPER()->mr_registerAPP      

#define  _DrawTextEx            GET_HELPER()->_DrawTextEx      
#define  _mr_EffSetCon          GET_HELPER()->_mr_EffSetCon      
#define  _mr_TestCom            GET_HELPER()->_mr_TestCom      
#define  _mr_TestCom1           GET_HELPER()->_mr_TestCom1      

#define  c2u           GET_HELPER()->c2u      

#define _mr_div                 GET_HELPER()->_mr_div
#define _mr_mod                 GET_HELPER()->_mr_mod

#define vm_state (*(GET_HELPER()->_mr_c_internal_table->vm_state))
#define mr_timer_state (*(GET_HELPER()->_mr_c_internal_table->mr_timer_state))
#define mr_timer_p (*(GET_HELPER()->_mr_c_internal_table->mr_timer_p))
#define mr_state (*(GET_HELPER()->_mr_c_internal_table->mr_state))

#define mr__bi (GET_HELPER()->_mr_c_internal_table->_bi)
#define bi (*(GET_HELPER()->_mr_c_internal_table->_bi))

#define mrp_error (GET_HELPER()->_mr_c_internal_table->mrp_error)
#define mrp_settop (GET_HELPER()->_mr_c_internal_table->mrp_settop)

#define mr_ram_file (*(GET_HELPER()->mr_ram_file))

#define mr_m0_files (GET_HELPER()->_mr_c_internal_table->mr_m0_files)

#define mr_ram_file_len  (*(GET_HELPER()->mr_ram_file_len))

#define LG_mem_base (*(GET_HELPER()->LG_mem_base))
#define LG_mem_end (*(GET_HELPER()->LG_mem_end))
#define LG_mem_left (*(GET_HELPER()->LG_mem_left))
#define LG_mem_len (*(GET_HELPER()->LG_mem_len))
#define LG_mem_top (*(GET_HELPER()->LG_mem_top))

#define pack_filename ((GET_HELPER()->pack_filename))
#define old_pack_filename ((GET_HELPER()->old_pack_filename))

#define start_filename ((GET_HELPER()->start_filename))
#define old_start_filename ((GET_HELPER()->old_start_filename))



#define mr_screenBuf (*(GET_HELPER()->mr_screenBuf))
#define mr_screen_w (*(GET_HELPER()->mr_screen_w))
#define mr_screen_h (*(GET_HELPER()->mr_screen_h))

#define mr_bitmap ((GET_HELPER()->mr_bitmap))
#define mr_sound ((GET_HELPER()->mr_sound))
#define mr_sprite ((GET_HELPER()->mr_sprite))
#define mr_tile ((GET_HELPER()->mr_tile))
#define mr_map ((GET_HELPER()->mr_map))
//#define mr_map ((GET_HELPER()->mr_map))

#define mr_exit_cb (*(GET_HELPER()->mr_exit_cb))
#define mr_exit_cb_data (*(GET_HELPER()->mr_exit_cb_data))

#define mr_sms_cfg_buf  ((GET_HELPER()->mr_sms_cfg_buf))

#define mr_sms_cfg_need_save (*(GET_HELPER()->_mr_c_internal_table->mr_sms_cfg_need_save))

#define start_fileparameter  ((GET_HELPER()->start_fileparameter))
#define mr_entry  ((GET_HELPER()->mr_entry))


#endif




extern int32 mrc_init(void);
extern int32 mrc_event(int32 code, int32 param0, int32 param1);
extern int32 mrc_pause(void);
extern int32 mrc_resume(void);
extern int32 mrc_exitApp(void);
extern int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1);
extern int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4,
                                int32 p5);
extern int32 mrc_eventEx(int32 code, int32 param0, int32 param1, int32 param2, int32 param3);
//extern int32 mrc_timer(void);
//extern int32 mrc_call (int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len);

typedef struct  _mr_c_event_st                       
{
   int32 code;
   int32 param0;
   int32 param1;
   int32 param2;
   int32 param3;
} mr_c_event_st;

typedef struct  _mr_c_call_st                       
{
   int32 code;
   uint8* input;
   int32 input_len;

} mr_c_call_st;



typedef struct _mrc_extChunk_st mrc_extChunk_st;

//为防止不同SDK版本编译的MRP不能兼容
//在这个结构中添加元素时需要加在最后
typedef struct  _mr_c_function_st                       
{
   uint8* start_of_ER_RW;
   uint32 ER_RW_Length;
   //uint8* old_start_of_ER_RW;
   int32 ext_type;
   mrc_extChunk_st * mrc_extChunk;

   //stack shell 2008-2-28
   int32 stack;
   //
} mr_c_function_st;



#define MR_MAX_FILENAME_SIZE		128

#define MR_MAX_FILE_SIZE		1024000

#define MR_MINIMUM_TIMER 10
#define MR_MINIMUM_TIMER_OUT 50

#define NULL 0


#define MR_SCREEN_W mr_screen_w
#define MR_SCREEN_MAX_W MR_SCREEN_W
#define MR_SCREEN_H mr_screen_h

#ifdef MR_ANYKA_MOD
#define MR_SCREEN_DEEP 3
#else
#define MR_SCREEN_DEEP 2
#endif

/*下面的MAKERGB是MR平台内部的*/
#ifdef MR_ANYKA_MOD
#define MAKERGB(r, g, b)     (uint32) ( ((uint32)r<<16) | ((uint32)g << 8) | ((uint32)b) )
#else
#define MAKERGB(r, g, b)     (uint16) ( ((uint32)(r>>3) << 11) + ((uint32)(g>>2) << 5) + ((uint32)(b>>3)) )
#endif

#ifdef MR_ANYKA_MOD
#define MR_SCREEN_CACHE_POINT(x, y)    ((uint8*)mr_screenBuf + (y * MR_SCREEN_MAX_W + x)*3)
#define MR_BITMAP_POINT(p, x, y, w)    ((uint8*)p + (y * w + x)*3)
#define MR_BITMAP_POINT_COLOUR(p)  ( ((uint32)*((uint8*)p)<<16) | ((uint32)*((uint8*)p+1)<<8) | *((uint8*)p+2) )

#else
#define MR_SCREEN_CACHE_POINT(x, y)    (mr_screenBuf + y * MR_SCREEN_MAX_W + x)
#endif


#define MR_FILE_RDONLY         1//以只读的方式打开文件。
#define MR_FILE_WRONLY        2//以只写的方式打开文件。
#define MR_FILE_RDWR             4//以读写的方式打开文件。
#define MR_FILE_CREATE          8//如果文件不存在，创建该文件。

/*
 *  zefang_wang 2010.12.21 :
 *    这个值之前定义有错误， 16 在底层的实现为 SHARE_OPEN，
 *    但是之前被定义成了   RECREATE.
 */

#define MR_FILE_SHARE_OPEN      16// 一边写， 一边读。

#define MR_IS_FILE     1      //文件
#define MR_IS_DIR      2      //目录
#define MR_IS_INVALID  8  //无效(非文件、非目录)


#define MR_FILE_HANDLE int32
#define MR_SEARCH_HANDLE int32

#define MR_SUCCESS  0    //成功
#define MR_FAILED   -1    //失败
#define MR_IGNORE   1     //不关心
#define MR_WAITING   2     //异步(非阻塞)模式

#define BITMAPMAX  30
#define SPRITEMAX  10
#define TILEMAX    3

#define SOUNDMAX   5

#define MR_SPRITE_INDEX_MASK       (0x03FF) // mask of bits used for tile index
#define MR_SPRITE_TRANSPARENT         (0x0400)

#define MR_TILE_SHIFT         (11)

#define MR_ROTATE_0         (0)
#define MR_ROTATE_90         (1)
#define MR_ROTATE_180         (2)
#define MR_ROTATE_270         (3)


enum {
   BM_OR,         //SRC .OR. DST*   半透明效果
   BM_XOR,        //SRC .XOR. DST*
   BM_COPY,       //DST = SRC*      覆盖
   BM_NOT,        //DST = (!SRC)*
   BM_MERGENOT,   //DST .OR. (!SRC)
   BM_ANDNOT,     //DST .AND. (!SRC)
   BM_TRANSPARENT, //透明色不显示，图片的第一个象素（左上角的象素）是透明色
   BM_AND,
   BM_GRAY,
   BM_REVERSE
};

enum {
   MR_FILE_STATE_OPEN,
   MR_FILE_STATE_CLOSED,
   MR_FILE_STATE_NIL
};

#define DRAW_TEXT_EX_IS_UNICODE               1
#define DRAW_TEXT_EX_IS_AUTO_NEWLINE    2

#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#ifndef ABS
   #define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif

enum {
   MR_TIMER_STATE_IDLE,
   MR_TIMER_STATE_RUNNING,
   MR_TIMER_STATE_SUSPENDED,
   MR_TIMER_STATE_ERROR
};

enum {
   MR_STATE_IDLE,
   MR_STATE_RUN,
   MR_STATE_PAUSE,
   MR_STATE_RESTART,
   MR_STATE_STOP,
   MR_STATE_ERROR
};

#define MRC_TIME_START(a) {mr_timerStart(a);mr_timer_state = MR_TIMER_STATE_RUNNING;}
#define MRC_TIME_STOP() {mr_timerStop();mr_timer_state = MR_TIMER_STATE_IDLE;}
//#define MR_TIME_STOP() {mr_timer_state = MR_TIMER_STATE_IDLE;}



#ifndef SDK_MOD
//#define GET_C_FUNCTION_P()  (*(((mr_c_function_st**)mr_c_function_load)-1))
//#define GET_HELPER() (*(((mr_table**)mr_c_function_load)-2))
#endif



extern void (*mrc_printf)(const char *format,...);

extern void *(*MR_MEMCPY)(void * s1, const void * s2, int n);
extern void *(*MR_MEMMOVE)(void * s1, const void * s2, int n);
extern char *(*MR_STRCPY)(char * s1, const char * s2);
extern char *(*MR_STRNCPY)(char * s1, const char * s2, int n);
extern char *(*MR_STRCAT)(char * s1, const char * s2);
extern char *(*MR_STRNCAT)(char * s1, const char * s2, int n);
extern int (*MR_MEMCMP)(const void * s1, const void * s2, int n);
extern int (*MR_STRCMP)(const char * s1, const char * s2);
extern int (*MR_STRNCMP)(const char * s1, const char * s2, int n);
extern int (*MR_STRCOLL)(const char * s1, const char * s2);
extern void *(*MR_MEMCHR)(const void * s, int c, int n);
extern void *(*MR_MEMSET)(void * s, int c, int n);
extern int (*MR_STRLEN)(const char * s);
extern char *(*MR_STRSTR)(const char * s1, const char * s2);
extern int (*MR_SPRINTF)(char * s, const char * format, ...);
extern int (*MR_ATOI)(const char * nptr);
extern unsigned long int (*MR_STRTOUL)(const char * nptr, char ** endptr, int base);


typedef int32 (*MR_LOAD_C_FUNCTION)(int32 code);


#ifndef MRC_PLUGIN
typedef int32 (*mrc_extMainSendAppMsg_t)(int32 extCode, int32 app, int32 code, int32 param0, int32 param1);
#else
typedef int32 (*mrc_extMainSendAppMsg_t)(int32 app, int32 code, int32 param0, 
                              int32 param1, int32 param2, int32 param3, int32 param4, int32 extCode);
#endif



#ifdef SDK_MOD
extern void* sdk_mr_c_function_table;
typedef int32 (*mrc_init_t)(void);
typedef int32 (*mrc_event_t)(int32 code, int32 param0, int32 param1);
typedef int32 (*mrc_pause_t)(void);
typedef int32 (*mrc_resume_t)(void);
typedef int32 (*mrc_exitApp_t)(void);

#endif

typedef int32 (*mpsFpFuncType)(int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

#ifdef MRC_PLUGIN
typedef int32 (*MR_C_FUNCTION_EX)(int32 p0, int32 p1, int32 p2, int32 p3,
                                   int32 p4, int32 p5, void* P, mpsFpFuncType func);
#endif

//为防止不同SDK版本编译的MRP不能兼容
//在这个结构中添加元素时需要加在最后
typedef struct  _mrc_extChunk_st
{
   int32 check;
   MR_LOAD_C_FUNCTION init_func;
   MR_C_FUNCTION event;
   uint8* code_buf;
   int32   code_len;
   uint8* var_buf;
   int32   var_len;
   mr_c_function_st* global_p_buf;
   int32   global_p_len;
   int32   timer;
   mrc_extMainSendAppMsg_t sendAppEvent;
   mr_table *extMrTable;
#ifdef MRC_PLUGIN
   MR_C_FUNCTION_EX eventEx;
#endif
   int32 isPause;/*1: pause 状态0:正常状态*/
#ifdef SDK_MOD
   mrc_init_t init_f;
   mrc_event_t event_f;
   mrc_pause_t pause_f;
   mrc_resume_t resume_f;
   mrc_exitApp_t exitApp_f;
#endif
} mrc_extChunk_st;


enum {
   MRC_EXT_INTERNAL_EVENT,
   MRC_EXT_APP_EVENT,
   MRC_EXT_MPS_EVENT
};

#ifdef MR_BIG_ENDIAN
#define ntohl(i) (((uint32)i>>24) | (((uint32)i&0xff0000)>>8) | (((uint32)i&0xff00)<<8) | ((uint32)i<<24))
#define htonl(i) (i)
#define ntohs(i) ((((uint16)i)>>8) | ((uint16)i<<8))
#else
#define ntohl(i) (i)
#define htonl(i) (((uint32)i>>24) | (((uint32)i&0xff0000)>>8) | (((uint32)i&0xff00)<<8) | ((uint32)i<<24))
#define ntohs(i) (i)
#endif




/********************************文件接口********************************/
/*
该函数用于调试使用，返回的是最后一次操作文件
失败的错误信息，返回的错误信息具体含义与平台
上使用的文件系统有关。
该函数可能被实现为总是返回MR_SUCCESS。
返回:
      最后一次操作文件失败的错误信息
*/
int32 mrc_ferrno(void);

/*
以mode方式打开文件，如果文件不存在，根据mode值
判断是否创建之。
输入参数
参数	说明
返回说明
返回状态	说明 
非 NULL	文件句柄
NULL	失败（注意，这里与其他接口不一样）

输入:
filename	文件名
mode	文件打开方式
         mode取值
                MR_FILE_RDONLY   //以只读的方式打开文件。
                MR_FILE_WRONLY   //以只写的方式打开文件。
                                    （这个方式可能被实现为与
                                    MR_FILE_RDWR相同的操作）
                MR_FILE_RDWR      //以读写的方式打开文件。
                MR_FILE_CREATE     //如果文件不存在，创建
                                    该文件，该参数不会单独出现
                                    ，只能与其他值一同出现（使
                                    用"或"运算）
               mode可能的取值：
               a、	前三个参数的其中一个
               b、	前三个参数的其中一个和MR_FILE_CREATE的"或"值
返回:
非 NULL	       文件句柄
NULL	            失败（注意，这里与其他接口不一样）
*/
int32 mrc_open(const char* filename,  uint32 mode);

/*
关闭文件。
输入:
f	文件句柄
返回:
MR_SUCCESS	成功
MR_FAILED	失败
*/
int32 mrc_close(int32 f);

/*
取得文件类型信息。
输入:
filename	文件名
返回:
      MR_IS_FILE     1//是文件
      MR_IS_DIR      2//是目录
      MR_IS_INVALID  8//文件不存在，或既不是文件也不是目录
*/
int32 mrc_fileState(const char* filename);

/*
写文件
输入:
f	文件句柄
p	待写入数据存放地址
l	待写入数据长度
返回:
      >0                   确切写入的字节数
      MR_FAILED      失败
*/
int32 mrc_write(int32 f,void *p,uint32 l);

/*
读取文件的内容到指定的缓冲。
输入:
f	文件句柄
p	文件缓存地址
l	缓冲长度
返回:
      >=0                确切读取的字节数
      MR_FAILED      失败
*/
int32 mrc_read(int32 f,void *p,uint32 l);

/*
读取文件的所有内容到申请的内存中。
输入:
filename	文件名
输出:
len           读取到的内容长度
返回:
      非NULL         指向读取到的内容的指针，该内存需要
                              调用者释放
      NULL              失败
*/
void* mrc_readAll(const char* filename, uint32 *len);


/*
从mrp中读取文件的所有内容到申请的内存中。
输入:
filename	文件名
输出:
len           读取到的内容长度
返回:
      非NULL         指向读取到的内容的指针，该内存需要
                              调用者释放
      NULL              失败
*/
void * mrc_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor);

/*
设置文件指针。
输入:
f	文件句柄
pos	文件指针位置
method	可能的取值为：
   MR_SEEK_SET, 
   MR_SEEK_CUR, 
   MR_SEEK_END
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_seek(int32 f, int32 pos, int method);

/*
取得文件长度。
输入:
filename	文件名 
返回:
      >=0   文件长度
      MR_FAILED   失败/文件不存在
*/
int32 mrc_getLen(const char* filename);

/*
删除文件。
输入:
filename	文件名
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_remove(const char* filename);

/*
文件重命名。
输入:
oldname	旧文件名
newname	新文件名
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_rename(const char* oldname, const char* newname);

/*
创建目录。
输入:
name	目录名
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_mkDir(const char* name);

/*
删除目录以及目录下面的文件和子目录。
输入:
name	目录名
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_rmDir(const char* name);

/*
准备搜索name指定的目录，当dsm调用该函数后，系统
初始化一个目录搜索，并返回第一个目录搜索的结果
，每当dsm调用一次mr_c_findGetNext函数，系统返回下一个
该目录下的文件或一级子目录名。该函数只返回查找
句柄。当name为空字符串""时（注意name指向空串，但
name不是NULL），搜索Mythroad平台引擎的文件当前目录。
如：若手机以"/dsmdata/"作为Mythroad平台引擎的文件当前
目录，当name为空字符串时，搜索目录"/dsmdata"。
例：一个目录下有文件："a.bmp"、"b.mrp"；目录"data"，则
mr_c_findStart返回查找句柄和"a.bmp"，紧接着的mr_c_findGetNext
返回"b.mrp"、"data"和MR_FAILED。
输入:
name	目录名
len	缓冲区大小
输出:
buffer  缓冲区，用于存放查找成功时第一个文件名或一级
            子目录名
返回:
      >0                  查找句柄，供mr_c_findGetNext、mr_c_findStop函数使用
      MR_FAILED      失败
*/
int32 mrc_findStart(const char* name, char* buffer, uint32 len);

/*
取得一个目录搜索的结果。将结果放置于buffer中。当
目录中的结果都遍历过后，返回NULL。
这里需要注意的是，使用mr_c_findGetNext获得的子目录不
能是"."和".."。
输入:
   search_handle	调用mr_c_findStart时返回的查找句柄
   len	缓冲区大小
输出:
   buffer  缓冲区，用于存放查找成功时文件名或一级子目录名
返回:
      MR_SUCCESS  搜索成功
      MR_FAILED   搜索结束或搜索失败
*/
int32 mrc_findGetNext(int32 search_handle, char* buffer, uint32 len);

/*
目录搜索结束。中止一个mr_c_findStart开启的目录搜索。
输入:
search_handle        调用mr_findStart时返回的查找句柄
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_findStop(int32 search_handle);



int32 mrc_isZip(uint8* inputbuf, int32 inputlen);
extern int32 mrc_unzip(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);
extern int32 mrc_unzip8(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);
extern int32 mrc_unzipFile(char* filename, int32 pos, int32 len, uint8** outputbuf, int32* outputlen);

uint32 mrc_updcrc(uint8 *s, uint32 n);

extern void* mrc_malloc(int size);
extern void mrc_free(void *address);

extern void mrc_refreshScreenReal(void);

typedef struct  _mrcMpsFpCallParamsSt                       
{
   mpsFpFuncType func;
   int32 p0;
   int32 p1;
   int32 p2;
   int32 p3;
   int32 p4;
   int32 p5;
} mrcMpsFpCallParamsSt;

typedef struct  _mrcMpsFpEventParamsSt                       
{
   //int32 p1;
   int32 p2;
   int32 p3;
   int32 p4;
   int32 p5;
} mrcMpsFpEventParamsSt;


int32 mrc_timerInit(void);


#ifdef MR_SPREADTRUM_MOD
void spreadtrum_patch_save(int32*gr9, int32*gr8);
void spreadtrum_patch_load(int32 gr9, int32 gr8);

#ifndef __thumb
#define SPREADTRUM_PATCH_SAVE \
      int32 gr8,gr9;\
      spreadtrum_patch_save(&gr9, &gr8);


#define SPREADTRUM_PATCH_LOAD \
      spreadtrum_patch_load(gr9, gr8);

#else
#define SPREADTRUM_PATCH_SAVE
#define SPREADTRUM_PATCH_LOAD
#endif

#else

#define SPREADTRUM_PATCH_SAVE
#define SPREADTRUM_PATCH_LOAD

#endif



//extern int32 mrc_initNetworkCbState;
extern MR_INIT_NETWORK_CB mrc_initNetworkCbFunc;
extern MR_GET_HOST_CB mr_getHostByNameCbFunc;

void mr_helper_set_sb(int32 sb);

#ifdef  MR_MSTAR_MOD
int32 mr_getR9(void);
#endif


#ifndef SDK_MOD
#define MRC_MALLOC(size) mrc_malloc(size)
#define MRC_FREE(p) mrc_free(p)
#else

#if 1
#define MRC_MALLOC(size) mrc_malloc(size)
#define MRC_FREE(p) mrc_free(p)
#else
void *mrc_mallocEx(int size, char* file, int line);
#define MRC_MALLOC(size) mrc_mallocEx(size, __FILE__, __LINE__)
#define MRC_FREE(p) {\
	mrc_free(p);\
	mrc_printf("free,%lu,FILE:%s,LINE:%d", p, __FILE__, __LINE__);\
}
#endif

#endif


#endif
