#ifndef _MRC_NETPKSAFE_H_
#define _MRC_NETPKSAFE_H_
#include "mrc_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PREFIXDATA_COUNT   20

#define MAX_NICKNAME_LEN  12
#define MAX_TITLE_LEN     12
#define MAX_IDENTIFY_LEN 32
/*
  游戏角色	
*/
typedef struct 
{	
	uint32 ctrl;                            /*0: 终端控制PK结果奖励  1: 服务端控制PK结果奖励*/
	uint32 type;							/*当作为获取到的角色信息，0：普通玩家；1：官方BOSS*/
	int32  win;                             /*胜场数量*/ 
	int32  lost;                            /*败场数量*/
	int32  StrengthValue;	                /*实力指数*/
	int32 sex;								/*1表示男，2表示女,0表示性别不明*/
	int32 bitmap;							/*用于是否有某个bitmap属性，1表示有，0表示没有*/
	int32 day_win;							/*当日所赢场次*/
	int32 week_win;							/*本周所赢场次*/
	/*************** 预定单元格属性格式 ****************/
	/* MAX_PREFIXDATA_COUNT 个属性值                   
	VALUE1,VALUE2,VALUE3,...                         
	由游戏自己定义具体的单元为何种含义,如定义:               
	  prefixData[0] 存放体力                         
	  prefixData[1] 存放等级                            
	  各个单元的缺省值为0                            
	  单元格可以用于从服务端查询特定的游戏角色       */
	/***************************************************/

	int32 prefixData[MAX_PREFIXDATA_COUNT];
    
	/************** 自定义单元格的格式 *****************/ 
	/* 预定单元格只能存放int32的数值，无法存放如字符串 
	 或者字节数组这样类型的数据，customData用于存放由
	 游戏自己组织的数据格式,存放在自定义数据区的数据无
	 法用于查询。                                    */ 
	/***************************************************/
	uint8 *customData;      /*自定义数据*/            
	int32 dataLen;			/*自定义数据长度*/ 
	uint32 skyid;			/*skyid*/
	uint16 nickName[MAX_NICKNAME_LEN+1];    /*昵称*/ 
	uint16 title[MAX_TITLE_LEN+1];  		/*头衔*/
	char  identify[MAX_IDENTIFY_LEN];      /*服务器描述*/

}MR_Character;

/*角色信息列表*/
typedef struct tagCharacterList
{
	MR_Character *pCharacter;
	struct tagCharacterList *pNext;
}MR_CharacterList;


/*预设过滤条件*/
typedef enum
{
   MR_PKFilter_STRENGTH = 0,	     /*综合实力最强*/
   MR_PKFilter_SIMILAR,	             /*实力相近*/
   MR_PKFilter_FIGHTWITHME,	         /*最近挑战过自己*/
   MR_PKFilter_FEMALE,	             /*女性玩家*/
   MR_PKFilter_DAY_CHAMPION,	     /*日冠军*/
   MR_PKFilter_WEEK_CHAMPION,	     /*周冠军*/
   MR_PKFilter_CUSTOM,   	         /*自定义*/ 
   MR_PKFilter_BUDDY				/*获取好友的角色列表*/
}MR_PKFilter;

/*错误码*/
typedef enum
{
	MR_PKErrorCode_SUCCESS = 0,			/* 成功           */
	MR_PKErrorCode_UserCancel,			/* 用户取消       */
	MR_PKErrorCode_DIAL,				/* 拨号错误       */
	MR_PKErrorCode_Network,				/* 网络错误       */
	MR_PKErrorCode_InvalidateUser,		/* 无效用户       */
	MR_PKErrorCode_BlackListUser,		/* 黑名单用户     */ 
	MR_PKErrorCode_FormatError,			/*格式错误			*/
	MR_PKErrorCode_NotEnoughMem,		 /*内存不够			*/
	MR_PKErrorCode_OtherError,
	MR_PKErrorCode_ForceUpdate,			/*强制更新*/
	MR_PKErrorCode_RecommendedUpdate	/*建议更新*/	
}MR_PKErrorCode;



typedef enum
{
	MR_PKPageFlag_noPage,	//获取到的列表没有上一页，也没有下一页
	MR_PKPageFlag_next,		//获取到的列表只有上一页
	MR_PKPageFlag_previous,	//获取到的列表只有下一页
	MR_PKPageFlag_both		//获取到的列表有上一页也有下一页
}MR_PKPageFlag;

/*
	联网PK回调函数原型：
		typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg,void* extraInfo);
	参数：
		resultCode：错误码，详见MR_PKErrorCode
		resultMsg：服务端返回的错误提示字符串

*/
typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg);

/*****************************************************/
/* mrc_getPkList的回调原型                           
 errCode: 错误码                                    
 lst:  PK角色列表,仅当errCode为MR_PKErrorCode_SUCCESS时有效.                     
		 lst释放说明：
			 lst是一个链表结构，最后一个节点指向NULL。应用负责释放每个节点中pCharacter所指向的内存，
			 以及pNext所指向的内存（如果pNext不为NULL）。特别注意，MR_Character中customData指针所指向的
			 内存也需由应用释放。
			 典型的释放如下：
		 void demo_freeList(MR_CharacterList* lst)
		 {
			 MR_CharacterList* pNextNode;
			 if(lst != NULL)
			 {
				 pNextNode = lst->pNext;
				 while(pNextNode)
				 {	
					 lst->pNext = pNextNode->pNext;
					 if(pNextNode->pCharacter->customData != NULL)
					 {
						 free(pNextNode->pCharacter->customData);
						 pNextNode->pCharacter->customData =NULL;
					 }
					 free(pNextNode->pCharacter);
					 pNextNode->pCharacter = NULL;
					 free(pNextNode);
					 pNextNode = pNextNode->pNext;
				 }
				 if(lst->pCharacter->customData != NULL)
				 {
					 free(lst->pCharacter->customData);
					 lst->pCharacter->customData = NULL;
					 
				 }
				 if(lst->pCharacter!=NULL)
				 {
					 free(lst->pCharacter);
					 lst->pCharacter = NULL;
				 }
				 free(lst);
				 lst = NULL;
			 }
		  }								               
*/
/*****************************************************/
typedef void (*MR_GetPKList_CB)(MR_PKErrorCode errCode,MR_CharacterList*  lst,MR_PKPageFlag pageFlag);



/****************************************************************************************/
/*    根据筛选条件得到PK列表                                                           
    输入:                                                                             
        nTop:    得到符合条件的前几个记录                                             
        filter:  预设过滤条件,当预设过滤条件不为PKFilter_CUSTOM时,customFilter无效   
        customFilter: 自定义过滤条件,仅当filter为PKFilter_CUSTOM时有效             
                           customFilter以mrc_character中prefixData为筛选单位           
						customFilter为简单规则的筛选表达式,支持下面的规则:		       					
							   C(N) = Value     等于                                            
							   C(N) <> Value    不等于                                 
							   C(N) > Value     大于                                    
							   C(N) < Value     小于                                   
							   orderby C(N)  desc(asc)   排序                          
							   desc: 降序                                               
							   asc:升序                                                 
							   and  并且                                                
							   or   或者                                               
							   (..) 逻辑判断优先级                                     
                                                                                    
							Value 为int32类型值                                        
							1<=N<MAX_PREFIXDATA_COUNT                                  
                                                                                     
			例:   C1=1000 and (C2>241 or C5<>0)  orderby C1 desc                        
			含义可以为: 体力为1000并且(等级大于241或者魔法不等于0)的记录筛选出来，      
			最后以体力的降序排列来筛选													                    
        nOffset: 选择记录集的第nOffset开始                                             
        nCount:    得到nOffset之后的nCount条记录
		flag:	在获取列表的时候是否同时也获取自身数据，如果同时获取自身数据，则回调函数的第一个结点中
				的角色信息是本地登录角色。flag为TRUE表示需要带携带自身数据，在这种情况下，获得的角色
				会比nCount多一个；flag为FALSE表示不需要携带自身数据.
        cb:      得到PK角色的回调                                                   
                                                                                      
     返回:                                                                            
                                                                                       
        MR_FAILED:    失败,参数传递错误
		MR_WAITING:	  正在下载插件
        MR_SUCCESS:   等待                                                            */
/****************************************************************************************/    
int32 mrc_netpk_getPKList(MR_PKFilter filter,
						  const char* customFilter,
						  int32 nOffset,
						  int32 nCount,
						  int32 flag,
						  MR_GetPKList_CB cb);


/*
	说明：
		在获取列表的同时上传自身属性
	函数原型：
	int32 mrc_netpk_getPKListWithAppdata(MR_PKFilter filter,
										const char* customFilter,
										int32 nOffset,
										int32 nCount,
										int32 flag,
										MR_Character* owner,
										uint16* feed_desc,
										int32 propNum,
										uint16** propStr,
										MR_GetPKList_CB cb);
	参数：
		输入：
			filter：预定义的筛选条件，详见：MR_PKFilter
			customFilter：只有在filter为MR_PKFilter_CUSTOM时该参数有效。
			nOffset：查询到的结果中的偏移量
			nCount：总共要获取的总条数
			flag：	在获取列表的时候是否同时也获取自身数据，如果同时获取自身数据，则回调函数的第一个结点中
					的角色信息是本地登录角色。flag为TRUE表示需要带携带自身数据，在这种情况下，获得的角色
					会比nCount多一个；flag为FALSE表示不需要携带自身数据.
			owner：本地角色信息
			feed_desc：欲上传的字符串，utf16be编码
			propNum：上传的自定义字符串的条数，最大5条，将在wap短显示
			propStr：上传的字符串数组
			cb：得到PK角色的回调
		输出：	MR_SUCCESS: 正在进行联网操作
				MR_WAITING: 正在下载云存储插件
				MR_FAILED:	失败
*/
int32 mrc_netpk_getPKListWithAppdata(MR_PKFilter filter,
									 const char* customFilter,
									 int32 nOffset,
									 int32 nCount,
									 int32 flag,
									 MR_Character* owner,
									 uint16* feed_desc,
									 int32 propNum,
									 uint16** propStr,
									 MR_GetPKList_CB cb);



/****************************************/
/*mrc_netpk_upload_appdata的回调原型 
errCode:   错误码                
respData:  从服务端反馈的数据,由应用负责释放，目前为utf16be编码的文字    
len:       respData的长度				*/  
/****************************************/
typedef void (*MR_Netpk_upload_appdata_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

/****************************************************************************************************/
/*  向联网PK服务端发送角色信息，以更新该角色信息                                                  
    如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。                       
															                                     
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                       
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"    
	在服务端会被替换成:                                                                            
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
         owner	:     需更新的角色信息															
         feed_desc:     Feed信息(UTF16BE)															
   返回:																							
          MR_FAILED:   失败，参数传递错误															
          MR_SUCCESS:  等待																		
   此函数可以在如下场合使用:                                                                      
          1. 更新当前游戏角色信息																	
		  2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_upload_appdata(MR_Character* owner,const uint16* feed_desc,MR_Netpk_upload_appdata_CB cb);


/****************************************************************************************************/
/*  向联网PK服务端发送角色信息，以更新该角色信息                                                  
	如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。该接口同时还可上传若干字符串到服务器端，
	目前可上传道具信息也可作为游戏自定义字符串（utf16-be编码）使用，该字符串属性属于本地登录的角色，如果上传的
	角色中没有本地角色那么不上传该字符串。                      
															                                     
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                       
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"    
	在服务端会被替换成:                                                                            
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
         owner	:     需更新的角色信息															
         feed_desc:     Feed信息(UTF16BE)
		 propNum：	上传道具或游戏自定义字符串的数量
		 propStr：  指向道具或游戏自定义字符串数组的指针，字符串用utf16-be编码
   返回:																							
          MR_FAILED:   失败，参数传递错误															
          MR_SUCCESS:  等待																		
   此函数可以在如下场合使用:                                                                      
          1. 更新当前游戏角色信息																	
		  2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_upload_appdataEx(MR_Character* owner,
								 const uint16* feed_desc,
								 int32 propNum,
								 uint16** propStr,
								 MR_Netpk_upload_appdata_CB cb);


/*PK结果*/
typedef struct 
{
	uint32 winner;    /*赢家SKYID*/ 
	uint32 loster;    /*输家SKYID*/ 
}MR_PKResult;


/************************************/
/*mrc_netpk_commit_pkresult的回调原型 
errCode:   错误码                
respData:  从服务端反馈的数据，由应用负责释放，目前为utf16be编码的文字     
len:       respData的长度        */  
/************************************/
typedef void (*MR_Netpk_commit_PKResult_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

/****************************************************************************************************/
/*  向联网PK服务端PK结果																		
    如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。                          
															                                      
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                            
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"     
	在服务端会被替换成:                                                                             
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
		   result	:	  包含PK中的winner以及loster的用户名，其所指向的内存由应用自己释放。									
         owner	:     需更新的角色信息															
		   count	:	  指明需要更新多少个角色信息,目前的取值应为0，1，2						
		   lstHead	:	  需要更新的角色的链表，lstHead所指向的内存由应用自己释放													
         feed_desc:     Feed信息(UTF16BE)，其所指向的内存由应用自己释放	
   返回:																						
          MR_FAILED:   失败，参数传递错误														
          MR_SUCCESS:  等待																	 
   此函数可以在如下场合使用:                                                                      
			1. PK后对觉得进行更新																	
			2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_commit_pkresult(MR_PKResult* result,
								int32 count,
								MR_Character* lstHead,
								const uint16* feed_desc,
								MR_Netpk_commit_PKResult_CB cb);




/****************************************************************************************************/
/*  向联网PK服务端PK结果																		
    如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。该接口同时还可上传若干字符串到服务器端，
	目前可上传道具信息也可作为游戏自定义字符串（utf16-be编码）使用，该字符串属性属于本地登录的角色，如果上传的
	角色中没有本地角色那么不上传该字符串。
															                                      
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                            
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"     
	在服务端会被替换成:                                                                             
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
		   result	:	  包含PK中的winner以及loster的用户名，其所指向的内存由应用自己释放。									
         owner	:     需更新的角色信息															
		   count	:	  指明需要更新多少个角色信息,目前的取值应为0，1，2						
		   lstHead	:	  需要更新的角色的链表，lstHead所指向的内存由应用自己释放													
         feed_desc:     Feed信息(UTF16BE)，其所指向的内存由应用自己释放				
		 propNum：	上传道具或游戏自定义字符串的数量
		 propStr：  指向道具或游戏自定义字符串数组的指针，字符串用utf16-be编码
   返回:																						
          MR_FAILED:   失败，参数传递错误														
          MR_SUCCESS:  等待																	 
   此函数可以在如下场合使用:                                                                      
          1. PK后对觉得进行更新																	
			2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_commit_pkresultEx(MR_PKResult* result,
								  int32 count,
								  MR_Character* lstHead,
								  const uint16* feed_desc,
								  int32 propNum,
								  uint16** propStr,
								  MR_Netpk_commit_PKResult_CB cb);


/****************************************************************************************************/
/*	mrc_netpk_openWap的回调原型																		
	result:																							
		MR_SUCCESS:浏览器成功运行后退出																
		MR_FAILED:浏览器未能成功运行																*/
/****************************************************************************************************/
typedef void (*MR_Netpk_openWap_CB)(int32 result);


/****************************************************************************************************/
/*  访问当前游戏的联网PK专区																	
	输入:无																							
	输出:                                                                                           
      MR_SUCCESS:等待                                                                                                        
		MR_FAILED: 失败，参数传递错误 																
此函数可以在如下场合使用:																			                                                                     
          1. 用户访问wap端的当前游戏的联网PK专区													
			2. 查看各类feed信息																		*/		
/****************************************************************************************************/ 
int32 mrc_netpk_openWap(MR_Netpk_openWap_CB cb);


/********************************************************************/
/*
	获取当前可用公告条数
	输入：无
	输出：
		返回当前可用的公告条数，暂不限制公告条数
	此函数用于如下场合：
		1.在获取公告消息前获取公告条数
*/
/********************************************************************/
int32 mrc_netpk_getNoticeNum(void);


/********************************************************************************************/
/*	获取当前可用的第index条公告（index从0开始取，取值范围在（0，mrc_netpk_getNoticeNum-1）之间）
	输入：无
	输出：
		NULL：无公告可用或index出错
		字符串：utf16-be编码，以\x00\x00结尾。
此函数可以用在如下场合：
	1.用户获取当前可用的公告
*/
/******************************************************************************************/
uint16* mrc_netpk_getNotice(int32 index);


/********************************************************************************************/
/*
	用于确保联网PK插件在plugins目录下：云存储插件
	输入：mr_safeNetpk_cb
	输出：
		MR_SUCCESS:通过了本地插件版本检查
		MR_WAITING:正在下载云存储插件
		MR_FAILED:失败
*/
/******************************************************************************************/
int32 mrc_netpk_initVersion(mr_safeNetpk_cb cb);

/********************************************************************************************/
/*
	在下载完成自身的游戏后，将从原游戏路径下加载同名mrp，使原游戏重启。
	输入：无
	输出：
		MR_FAILED:下载的游戏路径不在原游戏路径下
		MR_SUCCESS:正在加载游戏
	此函数可以用在如下场合：
		1.在游戏自更新之后，对游戏进行重启
*/
/******************************************************************************************/
int32 mrc_netpk_restartSelf(void);

/*
	释放公告所占的内存空间
*/
int32 mrc_netpk_releaseRes(void);

#ifdef __cplusplus
}
#endif

#endif