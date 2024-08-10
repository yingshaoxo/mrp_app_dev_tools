#ifndef __MRC_PROPSTORE_H__
#define __MRC_PROPSTORE_H__

#include "mrc_base.h"
#include "mrc_cloudStorage.h"

#define WAP_PAGE_INDEX		1
#define WAP_PAGE_ADD_PACK	2

typedef enum{
	MR_PROPSTORE_FAILED  = -1,	//失败
	MR_PROPSTORE_SUCCESS = 0,	//成功
	MR_PROPSTORE_EXPIRED,		//商品过期
	MR_PROPSTORE_INVAILD,		//商品不存在
	MR_PROPSTORE_LIMIT,			//商品超过数量
	MR_PROPSTORE_INVAILD_PARAM,	//参数错误
	MR_PROPSTORE_BLACK_LIST	,	//黑名单用户
	MR_PROPSTORE_FORCE_UPDATE,	//强制更新
	MR_PROPSTORE_SUGG_UPDATE,	//建议更新
	MR_PROPSTORE_INVAILD_USER,	//错误的用户名或密码
	MR_PROPSTORE_SERVER_BUSY,	//服务器忙
	MR_PROPSTORE_OTHER_FAILURE	//其他错误,一般由本地错误引起,如联网失败等
}MR_ResultCode_E;

typedef void (*MR_PROPSTORE_WAP_CB)(int32 resultCode);

typedef void (*MR_PROPSTORE_CB)(MR_ResultCode_E resultCode, int32 packVolumn, int32 itemCount, uint16* serverMsg);

typedef void (*MR_PROPSTORE_DEL_CB)(MR_ResultCode_E resultCode, int8* exeResult, uint16* serverMsg);

/********************************************************
					道具相关接口
********************************************************/
/*
	函数名：mrc_propStore_queryUserPack
	功能：  查询用户背包，用于返回当前活跃用户背包内的所有道具
	输入：
			columnName 配置返回的列名，可选字段：specid,descid,remain,period,num,attr 中间以逗号隔开
			queryCb    查询回调
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result	   执行结果 成功：MR_SUCCESS 失败：MR_FAILED
			resultCode 无效
			packVolumn 背包总容量
			itemCount  商品数量
			serverMsg  服务器端返回的消息，可能为NULL		
*/
int32 mrc_propStore_queryUserPack(char* columnName, MR_PROPSTORE_CB queryCb);

/*
	函数名：mrc_propStore_consumeGoods
	功能：	消耗消耗类商品
	输入：
			id		   消耗商品的ID号
			num		   消耗的数量
			queryCb    查询回调
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result	   执行结果 成功：MR_SUCCESS 失败：MR_FAILED
			resultCode 见MR_ResultCode_E定义
			packVolumn 无效
			itemCount  无效
			serverMsg  服务器端返回的消息，可能为NULL
*/
int32 mrc_propStore_consumeGoods(int32 id, int32 num, MR_PROPSTORE_CB queryCb);

/*
	函数名：mrc_propStore_deleteGoods
	功能：	删除商品，同时用于转化服务器端背包到本地，使用非消耗类道具，可同时进行多个删除，以数组的形式传入
	输入：
			idArray		删除商品的ID号数组
			itemCount	商品个数
			queryCb     查询回调
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result		执行结果
			exeResult   包含成功信息的数组，与idArray一一对应，成功为MR_SUCCESS，不成功为MR_FAILED
			serverMsg	服务器返回的信息
*/
int32 mrc_propStore_deleteGoods(int32* idArray, int32 itemCount, MR_PROPSTORE_DEL_CB queryCb);

/*
	函数名：mrc_propStore_getGoods
	功能：	根据key获取商品
	输入：
			getKey		获得商品的key，类似于一个宝箱ID，boss的ID等
			columnName	配置返回的列名，可选字段：specid,descid,remain,period,num,attr 中间以逗号隔开
			queryCb     查询回调
			isGetPack	是返回获得道具后的整个背包还是仅返回此次获得的道具，0返回获得的道具，1返回整个背包
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result	   执行结果 成功：MR_SUCCESS 失败：MR_FAILED
			resultCode 见MR_ResultCode_E定义
			packVolumn isGetPack = 0 时无效，isGetPack = 1 时返回背包容量
			itemCount  获得的商品数
			serverMsg  服务器端返回的消息，可能为NULL		
*/
int32 mrc_propStore_getGoods(int32 getKey, char* columnName, MR_PROPSTORE_CB queryCb, int32 isGetPack);

/*
	函数名：mrc_propStore_enterWap
	功能：调用浏览器，进入wap页面，在线查看用户背包等信息，并购买可在线道具
	输入：
			page	需要打开的页面取值如下：
					取值为：WAP_PAGE_INDEX		打开用户在线背包首页
					取值为：WAP_PAGE_ADD_PACK	打开背包扩容页面
			wapCb	调用浏览器的回调函数
					回调中传回的resultCode参数取值为：
							MR_SUCCESS			调用浏览器成功
							MR_FAILED			调用失败
*/
int32 mrc_propStore_enterWap(int32 page, MR_PROPSTORE_WAP_CB wapCb);

/**
 * 上传道具回调
 * resultCode:上传结果
 *      MR_FAILED 失败
 *      MR_SUCCESS 成功
 *      其他:
 *          2:商品过期
 *          3:商品不存在
 *          4:商品超过数量
 *          5:上传道具版本错误
 *          6:上传道具属性可疑，道具入库，标识为可疑属性
 *          7:上传道具属性数目不匹配
 *          8:非法道具上传成功，道具入库，标识为非法版本上传
 *          9:可疑道具上传成功，道具入库，标识为可疑版本上传，3次可疑版本上传后，则认为是一次非法上传
 * ver:下一次上传服务端期望的版本号，当resultCode==MR_FAILED时，此值不确定
 * 注意：失败时请至少重新再请求一次
 **/
typedef void (*mrc_propStore_uploadItem_cb)(int32 resultCode, int32 ver);
/**
 * 上传道具前置-设置道具属性
 * order:属性编号
 * value:属性值
 * 以上参数由应用与服务端共周商议, 本插件只做一个转发
 * 注意：属性设置在调用 mrc_propStore_uploadItem 后将被清空，如需再上传，需要重新调用本接口
 **/
int32 mrc_propStore_uploadItem_setItemProperty(int32 order, int32 value);
/**
 * 上传道具，使用本接品前请先登录通行证
 * ver:道具版本号
 * cpGoodsId:道具ID(由CP自行定义)，道具的属性请参见 mrc_propStore_uploadItem_setItemProperty 接口
 * num:道具数量
 * cb:回调函数，不能为NULL
 * 注意：在调用本函数前，应用应当暂停自己的逻辑（特别是必须要停止界面刷新），直到cb回调通知结果才能继续应用逻辑
 **/
int32 mrc_propStore_uploadItem(int32 ver, int32 cpGoodsId, int32 num, mrc_propStore_uploadItem_cb cb);

/********************************************************
				解析接口，解析类型不限
********************************************************/

/*
	函数名：mrc_propStore_resolve
	功能：  数据解析器
	输入：
			row		解析数据所在行
			column	解析数据所在列
			pData	解析后的数据
			len		解析获得的数据的长度 0则说明数据库中对应为NULL
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
*/
int32 mrc_propStore_resolve(int32 row, int32 column, void* pData, int32* len);

/********************************************************
				释放资源接口
********************************************************/
void mrc_propStore_release(void);

#endif