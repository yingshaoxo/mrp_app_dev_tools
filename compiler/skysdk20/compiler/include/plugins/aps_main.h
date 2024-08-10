#ifndef _APS_MAIN_H_
#define _APS_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif


#define APS_EC_INVALID_PARAM 1
#define APS_EC_OUT_OF_MEMORY 2
#define APS_EC_IO_ERROR      3
#define APS_EC_INVALID_EXT_STATE 4



//////////////////////////////////////////////////////////////////////////////
//   NOTE:
//
//   ���𿪷������ͬ�� 
//   ���ID (extID) , ���mrp��appid�� ���mrp���ļ�·����
//   ������SDK����С��(����/�쿭)���롣
//
//    �����ֹ��ͻ��
//
//////////////////////////////////////////////////////////////////////////////


#define APS_ID_EXTID_BASE     480000

/************************************************************************
 *  ���ID �����  (0-31)
 ************************************************************************/


#define APS_EXT_ID_MAINMODULE                1                                 /* ��ģ�� */   
#define APS_EXT_ID_VERDLOAD                  2                                 /* ����   */
/* smsend.mrp  �Ѿ���ʹ�ã� ������  */     /*3*/
#define APS_EXT_ID_BROWSER                   4                                 /* ����� */
/*  ԭJavaͼ�β��  ������ */              /*5*/
/*  6  ���� */                             /*6*/
/*  7  ���� */                             /*7*/
#define APS_EXT_ID_SMCHECK		             8		                           /* �Ʒ� -- ���Ѽ��*/
#define APS_EXT_ID_SMSPAY		             9		                           /* �Ʒ� */
#define APS_EXT_ID_NETPAY		            10		                           /* �Ʒ� */
#define APS_EXT_ID_SMSNET		            11		                           /* �Ʒ� ����Ԥȡ */
/*  12 ԭ����Ӫ�����,   �����С� */      /*12*/
#define APS_EXT_ID_PASSPORT_LOCALHISTORY	13                                 /* ͨ��֤ */
#define APS_EXT_ID_PASSPORT_UI		        14                                 /* ͨ��֤ */
/*  15 ���� */                            /*15*/
#define APS_EXT_ID_SMSHIDE		            16	                               /* �Ʒ� ��̨���Ÿ��Ѳ��*/
#define APS_EXT_ID_PASSPORT_NET 	        17                                 /* ͨ��֤ */
#define APS_EXT_ID_SMSCHAR		            18		                           /* �Ʒ�:���Ÿ��Ѳ��*/
#define APS_EXT_ID_OGMCHAR		            19		                           /* �Ʒ�: ���θ��Ѳ��*/
#define APS_EXT_ID_KBCHARG		            20		                           /* �Ʒ�: K�Ҹ��Ѳ��*/
#define APS_EXT_ID_BRWCORE		            21	                               /* �Ʒ�: �����core���*/
#define APS_EXT_ID_BRWNAVI		            22		                           /* ������������*/
#define APS_EXT_ID_BRWMAIN		            23		                           /* ��������ز��*/
#define APS_EXT_ID_BRWCHLST		            24		                           /* �����Ƶ�����*/
#define APS_EXT_ID_DSMSRFM	 	            25		                           /* �Ʒ�: �ӳٶ���ȷ�϶���*/
#define APS_EXT_ID_BRWGUI		            26								   /* �����gui���*/
#define APS_EXT_ID_BRWSHELL		            27		                           /* �����shell���*/
/*   28  cplg.mrp */                      /*28*/                               /* �Ʒ�*/
/*   29  �Ʒ���Ӫ����� */                /*29*/                               /* �Ʒ� Ӫ��*/
/*   30  ���� */                          /*30*/
/*   31  ����*/                           /*31*/


#define APS_EXT_ID_APP           255


#define APS_EXT_ID_GRAPHICS      10008
#define APS_EXT_ID_RECOMMEND     10044  //10044-10047
#define APS_EXT_ID_MESSAGE	     10048  //10048-10051





// ��appģ����� mrc_exit()ʱ,����ģ�鷢��һ���˳���Ϣ��
#define APS_MAIN_CODE_EXITAPP            0  
#define APS_MAIN_CODE_RUNMRP             1

#define APS_MAIN_CODE_LOAD            32
#define APS_MAIN_CODE_UNLOAD          33


#define APS_MAIN_CODE_BLOCK           34   //  *** block
#define APS_MAIN_CODE_UNBLOCK         35   //  *** unblock

#define APS_MAIN_CODE_REG_ENTRY       36



/******************************************************
 *  Ϊ�����°汾����ģ������ϰ汾�Ĳ��, ��ֹĳЩ�ϲ���������������CODE�����汾.
 *   
 *   �������°汾����ģ������Ȼ�������CODE
 *
 *   �������Code�Ĺ������ǵ���Ϊ:  �������plugins.lst��ѹ�������
 */
#define APS_MAIN_CODE_CHECK_EXT_VERSION   38


#define APS_MAIN_CODE_GET_EXT_VERSION     39
#define APS_MAIN_CODE_GET_EXT_PACKNAME    40
#define APS_MAIN_CODE_DUMP_LOAD           41
#define APS_MAIN_CODE_DUMP_UNLOAD         42
#define APS_MAIN_CODE_QUERY_LOAD_MODE     43
#define APS_MAIN_CODE_FWD_CALLBACK        44

#define APS_MAIN_CODE_SWITCH_CDR          45
#define APS_MAIN_CODE_RESTORE_CDR         46


#define APS_MAIN_CODE_BLOCK_EXCEPT        47    // *** block except
#define APS_MAIN_CODE_UNBLOCK_EXCEPT       48    // *** unblock except

#define APS_MAIN_CODE_QUERY_MAINEXTTYPE   49
#define APS_MAIN_CODE_GET_MAIN_VERSION    50

// ������pause/resume
#define APS_MAIN_CODE_PAUSE_REAL          51
#define APS_MAIN_CODE_RESUME_REAL         52

#define APS_MAIN_CODE_OVERRIDE_MEMFUNC   100
#define APS_MAIN_CODE_GETSMSCENTER       101

#define APS_MAIN_CODE_VM_MALLOC           180
#define APS_MAIN_CODE_VM_FREE             181
    
#define APS_MAIN_CODE_SET_UIFLAG               191
#define APS_MAIN_CODE_QUERY_NETWORK_TYPE       192
#define APS_MAIN_CODE_BLOCK_KEY_EVENT          193
#define APS_MAIN_CODE_TRANS_KEY_EVENT          194  


#define APS_MAIN_CODE_LOAD_VM				   196
#define APS_MAIN_CODE_RESET_LOAD			   197
#define APS_MAIN_CODE_RESET_UNLOAD			   198

/* ��ȡĳһ��mrp��appid��appVer��shortName */
#define APS_MAIN_CODE_GETMRPINFO               201

/* ��ȡĳһ��mrp���plugins.lst */
#define APS_MAIN_CODE_GETPLUGINSLST             202

/* ��ĳһ��MRP����ȡһ���ļ�, д��T���� */
#define APS_MAIN_CODE_RETRIEVEFILEFROMMRP            203

/* ��ĳһ��MRP����ȡһ����ѹ�����ļ�, д��T���� */
#define APS_MAIN_CODE_RETRIEVENOZIPFILEFROMMRP            204

/* ���ò����ˢ�����ȼ�*/
#define APS_MAIN_CODE_SET_REFRESHSCREEN_PRIORITY		205

/* �����ȼ�ˢ��	*/
#define APS_MAIN_CODE_REFRESHSCREEN_BY_PRIORITY		206

/* ��ͣˢ��	*/
#define APS_MAIN_CODE_REFRESHSCREEN_SUSPEND		207

/* �ָ�ˢ��	*/
#define APS_MAIN_CODE_REFRESHSCREEN_UNSUSPEND	208

/*��ѯ��ջ��Ϣ,����_rģʽ�µ���ģ����Ч*/
#define APS_MAIN_CODE_QUERYSTACK                         0xF0000000

typedef enum MainExtType
{
	MainExtType_NORMAL = 0,  /* ��ͨ����       */
	MainExtType_DUMP         /* ֧��dump������ */
}MainExtType;

/*��ѯ��ǰ����ģ������,����ֵΪMainExtType*/
#define aps_main_getMainExtType()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_QUERY_MAINEXTTYPE, 0,0,0,0,0)

/*��ѯ��ǰ��ģ��İ汾��*/
#define aps_main_getMainVersion()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_GET_MAIN_VERSION,0,0,0,0,0)

#ifdef SDK_MOD
	typedef int32 (*mrc_init_t)(void);
	typedef int32 (*mrc_event_t)(int32 code, int32 param0, int32 param1);
	typedef int32 (*mrc_pause_t)(void);
	typedef int32 (*mrc_resume_t)(void);
	typedef int32 (*mrc_exitApp_t)(void);
	
	typedef struct
	{
		mrc_init_t		mrc_init_f;
		mrc_event_t		mrc_event_f;
		mrc_pause_t		mrc_pause_f;
		mrc_resume_t	mrc_resume_f;
		mrc_exitApp_t	mrc_exitApp_f;
	}mrcEnterFuncSt;
#endif

/*
 *   aps_main_loadExt
 *
 *   aps_main_loadExtAddr  ����ָ�����صĵ�ַ����Ҫ�ǿ���ʹ��ͼ�μ��ٹ��ܡ�
 */
#ifndef SDK_MOD
#define  aps_main_loadExt(extID,mrpname,mrpfolder,extname) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),0)

/*Ԥ���ز������*/
typedef struct 
{
	void* addr;  /*Ԥ�����ڴ��ַ */
	int32 len;   /*���ṩ�����ڼ��ص��ڴ�ߴ�, ==0 ��ʾ��ָ������,��ʹ�ò��ʵ�ʴ�С�ڴ�*/  
}APS_PreLoadParam;

#define  aps_main_loadExtAddr(extID,mrpname,mrpfolder,extname,preLoadParam) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)(preLoadParam))


#else   
#define  aps_main_loadExt(extID,mrpname,mrpfolder,extname,enterFunc) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)enterFunc)
#endif

#ifndef SDK_MOD
#define aps_main_loadExtVm(extID,mrpname,mrpfolder,extname) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD_VM, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),0)
#else
#define aps_main_loadExtVm(extID,mrpname,mrpfolder,extname,enterFunc) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD_VM, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)enterFunc)
#endif

#define aps_main_loadExtRest(extID,mrpname,mrpfolder,extname) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_RESET_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),0)

typedef void (*loadExt_dump_cb)(int32 result);
#ifndef SDK_MOD
/*
  aps_main_loadExt_dump
  �����ӳټ���extIDָ���Ĳ��,���ҳ��Ը���Game Ext�Ĵ������Ϊ���ʹ�õĴ�����ڴ棬���Game Ext�Ĵ���β��������ɲ���Ĵ���Σ���
  ��Ϊ��aps_main_loadExtһ�£������µ��ڴ������ء�

  aps_main_loadExt_dump�Ĳ����õȴ������ߵĵ���ջ�˳�֮���������ִ�У����ԣ��ڵ����߶���aps_main_loadExt_dump�����к���(���ε���ջ֮�ڲ���)
  ������ģ������в���,�������Ӻ�aps_main_loadExt_dump����ִ��֮����ܵõ�ִ�С�
*/
#define aps_main_loadExt_dump(extID,mrpname,mrpfolder,extname,cb) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_DUMP_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)cb)
#else
#define aps_main_loadExt_dump(extID,mrpname,mrpfolder,extname,enterFunc) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_DUMP_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)enterFunc)
#endif



#define APS_EXT_LOADMODE_NOTFOUND     0   /*���δװ��*/          
#define APS_EXT_LOADMODE_NORMAL       1   /*���������װ��*/
#define APS_EXT_LOADMODE_DUMP         2   /*����Ѹ���Game Ext����εķ�ʽװ��*/
#define APS_EXT_LOADMODE_VM			  3   /*����Ѽ��ص�VM�ڴ���*/
#define APS_EXT_LOADMODE_RESET		  4	  /*�����ͨ��reset��ʽ����*/	

/* ��ѯ�����װ��ģʽ */
#define aps_main_query_loadMode(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_QUERY_LOAD_MODE,(extID),0,0,0,0)

/*isDelayUnload  0: ����ж�� 1: �ӳ�ж��*/
#define  aps_main_unloadExt(extID,isDelayUnload) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_UNLOAD,(int32)(extID),(int32)isDelayUnload,0,0,0)

/*ж����Dump��ʽ���صĲ��*/
#define aps_main_unloadExt_dump(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_DUMP_UNLOAD,(int32)(extID),0,0,0,0)

/*************************************************
 *  ��һ��API ���� block/unblock ���
 *     block/unblock ����ͣ��Ϸ�Ķ�ʱ���� ����block�����¼�.
 *     
 *     ��ģ����ģʽ�£� ��ʱ�����ܱ�ֹͣ�� ��ҪӦ���Լ���ͣ�� 
 *************************************************/

#define aps_main_blockExt(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_BLOCK,(int32)(extID),0,0,0,0)

#define aps_main_unblockExt(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_UNBLOCK,(int32)(extID),0,0,0,0)

/*��ͣ�����������������extID֮��*/
#define aps_main_blockExt_except(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_BLOCK_EXCEPT,(int32)(extID),0,0,0,0)

/*�ָ������������������extID֮�� */
#define aps_main_unblockExt_except(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_UNBLOCK_EXCEPT,(int32)(extID),0,0,0,0)

/*************************************************
 *  ��һ��API ���� pause/resume ���
 *  pause/resume����ͣ��Ϸ�Ķ�ʱ���� ������֯�����¼���
 *  ** ��Ӱ��Ĳ������յ� resmue/pause �¼�
 * 
 *
 *    flags :
 *          0x1  :  ��Ӱ���Ŀ�������� = ���еĲ���ļ��� - (extID1, extID2)
 *          
 *          0x2  :  ��Ӱ���Ŀ�������� = ���еĲ���ļ��� - (extID1, extID2) - VM��פ���
 *
 *
 **************************************************/

#define aps_main_pauseExtReal(flags,extID1,extID2) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_PAUSE_REAL,(int32)(flags),extID1,extID2,0,0)

#define aps_main_resumeExtReal(flags,extID1,extID2) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RESUME_REAL,(int32)(flags),extID1,extID2,0,0)


/*�������ں���ԭ��*/
/* p0:  ��Ϣ�� */
/* p1~p5: ���� */
typedef int32 (*ExtEntry)(int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

/*ע��������ں���*/
#define aps_main_regEntry(selfExtid, entryAddr) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_REG_ENTRY,(int32)(selfExtid),(int32)(entryAddr),0,0,0)


/* ��ȡĳһ��mrp��appid��appVer��shortName */
#define aps_main_getMrpInfo(packName,appID,appVer,shortName) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_GETMRPINFO, (int32)packName, (int32)appID, (int32)appVer,(int32)shortName,0)

/* ��ȡĳһ��mrp���plugins.lst */
#define aps_main_getAppVer_from_Pluginslst(mrpName,appID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_GETPLUGINSLST, (int32)mrpName,appID,0,0,0)

/* ��ĳһ��MRP����ȡһ���ļ�, д��T���� */
#define aps_main_retrieveFileFromMrp(srcMrpName,srcFileName, dstFolder, dstFileName) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_RETRIEVEFILEFROMMRP, (int32)srcMrpName, (int32)srcFileName, (int32)dstFolder, (int32)dstFileName, 0)

#if 0 /*�˹���Ŀǰ�ر�״̬*/
/* ��ĳһ��MRP����ȡһ��ѹ�����ļ�, д��T���ϣ�����Ҫ�ķѽ�ѹ�����ڴ� */
#define aps_main_retrieveNozipFileFromMrp(srcMrpName,srcFileName,dstFolder,dstFileName) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_RETRIEVENOZIPFILEFROMMRP, (int32)srcMrpName, (int32)srcFileName, (int32)dstFolder, (int32)dstFileName, 0)
#endif

#define aps_main_getExtVersion(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_GET_EXT_VERSION, (extID),0,0,0,0)

#define aps_main_getExtPackName(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_GET_EXT_PACKNAME, (extID), 0,0,0,0)

#define aps_main_runMrpLater(mrp_name,parameter) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RUNMRP,(int32)(mrp_name),(int32)(parameter),0,0,0)

#define aps_main_switchCDR(oldDirBuffer) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_SWITCH_CDR,(int32)(oldDirBuffer),0,0,0,0)

#define aps_main_restoreCDR(oldDirBuffer) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RESTORE_CDR,(int32)(oldDirBuffer),0,0,0,0)

#define aps_main_getSMSCenter(smsCenter) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_GETSMSCENTER,(int32)(smsCenter),0,0,0,0)

#define aps_main_setUIFlag(onOffFlag) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_SET_UIFLAG,(int32)(onOffFlag),0,0,0,0)

#define aps_main_blockKeyEvent(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_BLOCK_KEY_EVENT,(int32)(extID),1,0,0,0)

#define aps_main_unblockKeyEvent(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_BLOCK_KEY_EVENT,(int32)(extID),0,0,0,0)

#define aps_main_OnExtKeyEvent(extID,code,param0,param1) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_TRANS_KEY_EVENT,(int32)(extID),code,param0,param1,0)									

			
typedef struct _tagCBParam
{
	int32 p0;
	int32 p1; 
	int32 p2; 
	int32 p3; 
	int32 p4; 
	int32 p5;
}APS_CBParam;



/*************************************************************
  aps_main_fwd_cb����ί����ģ���첽�ص��ûص�����,�ڵ��ô˺���ǰ
  Ӧ����aps_main_unloadExt_dump
  func:   mpsFpFuncType                                    
  param:  APS_CBParam*    ��Ҫָ��һ�鶯̬�ڴ�,�ڻص�ִ�к�����
                          �ͷ�
*************************************************************/
#define aps_main_fwd_cb(func,param) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_FWD_CALLBACK,(int32)func,(int32)param,0,0,0)


/***********************************************************************
aps_vm_malloc��aps_vm_free����������ͷŲ���DUMPװ���ڴ�������Ծɰ�ȫ
���ڴ�ռ䡣

��A������ͷ��Լ�֮ǰ����ʹ��aps_vm_malloc������һ���ڴ棬���������Ҫ��
Game.Ext����Ϣ����ʹ�����ͷ�֮���Game.Ext��˵������ڴ��Ծ��ǰ�ȫ�ġ�

���Ա���������aps_vm_malloc���ڴ�ǳ�����,ֻ����������С����ڴ档
*************************************************************************/
#define aps_vm_malloc(size)\
	(void*)mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_VM_MALLOC,(int32)(size),0,0,0,0)

#define aps_vm_free(p)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_VM_FREE,(int32)(p),0,0,0,0)


#define APS_NETWORK_STATUS_NONE    0xFF
#define APS_NETWORK_STATUS_CMWAP    1
#define APS_NETWORK_STATUS_CMNET    2
#define APS_NETWORK_STATUS_UNIWAP   3
#define APS_NETWORK_STATUS_UNINET   4          
#define APS_NETWORK_STATUS_UNKNOW   0xF1

#define aps_query_netowrkType()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_QUERY_NETWORK_TYPE,0,0,0,0,0)

#define aps_query_stackInfo(stackTop,stackLen,vmLeft, ctrlLeft)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_QUERYSTACK,(int32)(stackTop),(int32)(stackLen),(int32)vmLeft,(int32)ctrlLeft,0)


/*
 *  ˢ�����¼����ȼ�������  - ����ͬʱ���ã�Ҳ����һ������һ����
 *  priority -- ˢ�������ȼ���  Խ�ߵ�ֵ�� ��Խ�����ˢ
 *  eventPriority -- �¼����У�  Խ�ߵ�ֵ�� ��Խ�����ܵ��¼���
 *  
 *      if (paint_func)  {   ����ˢ�� }
 *      if (eventPriority != -1 )  {�����¼����ȼ�}
 *  
 *   tgtExtID -- Ŀ������ ��ģ������ڲ����pauseʱ������ԭ����ˢ��������
 */

typedef void (*aps_asynPaint_func_t)(int32 x, int32 y, int32 w, int32 h);

#define aps_set_refreshScreen_priority(paint_func,priority, tgtExtID, eventPriority)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_SET_REFRESHSCREEN_PRIORITY,(int32)(paint_func),(int32)(priority),(int32)(tgtExtID),(int32)(eventPriority),0)
		
#define aps_refreshScreen_trigger(x,y,w,h)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_REFRESHSCREEN_BY_PRIORITY,(int32)(x),(int32)(y),(int32)(w),(int32)(h),0)

#define aps_refreshScreen_suspend()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_REFRESHSCREEN_SUSPEND,0,0,0,0,0)

#define aps_refreshScreen_unsuspend()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_REFRESHSCREEN_UNSUSPEND,0,0,0,0,0)



 
#ifdef __cplusplus
}
#endif



#endif
