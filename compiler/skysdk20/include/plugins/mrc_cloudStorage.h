#ifndef __CLOUD_STORAGE_H__
#define __CLOUD_STORAGE_H__

#include "mrc_base.h"

typedef enum
{
	MR_CLOUD_SUCCESS		 = 0,	//成功
	MR_CLOUD_INVAILD_PARAM	 = 1,	//参数错误
	MR_CLOUD_BLACK_LIST		 = 2,	//黑名单用户
	MR_CLOUD_FORCE_UPDATE	 = 3,	//强制更新
	MR_CLOUD_SUGG_UPDATE	 = 4,	//建议更新
	MR_CLOUD_INVAILD_USER	 = 5,	//错误的用户名或密码
	MR_CLOUD_SERVER_BUSY	 = 6,	//服务器忙
	MR_CLOUD_OTHER_FAILURE	 = 7	//其他错误,一般由本地错误引起,如联网失败等
}MR_cloudStorage_errcode_E;

typedef enum
{
	MR_SQLTYPE_BYTEA	= 2,	//bytea			多字节
	MR_SQLTYPE_BOOLEAN	= 1,	//boolean		单字节
	MR_SQLTYPE_INTERGER	= 4,	//interger		四字节
	MR_SQLTYPE_BIGINT	= 0,	//bigint		八字节
	MR_SQLTYPE_TIMESTP	= 5,	//timestamp		八字节
	MR_SQLTYPE_VCHAR	= 3,	//char varying	不定长
	MR_SQLTYPE_UNIVCHAR = 6		//unicode varChar不定长
}MR_sqlType_E;

typedef enum
{
	MR_CLOUD_RESULT_TYPE_MODIFY = 1,	//结果集类型：修改
	MR_CLOUD_RESULT_TYPE_QUERY  = 2		//结果集类型：查询
}MR_cloudStorage_RsType_E;

typedef struct  
{
	int32 responseCode;			//服务器返回的code
	uint16* responseMsg;		//执行结果的错误提示utf-16BE编码（非必须，可能是NULL）
	int32 resultSetNum;			//执行结果的结果集的g个数（非必须，如果是查询这个信息就没用了）
}MR_SQL_EXE_INFO_T;

typedef struct  
{
	MR_cloudStorage_RsType_E rsType;	//执行结果的状态码，取值为MR_cloudStorage_errcode_E枚举中的数据
	int32 resultCode;
	uint16* errorMsg;
	int32 columnCount;
	int32 rowCount;
}MR_SQL_RS_INFO_T;

typedef struct  
{
	int16 year;
	int16 month;
	int16 day;
	int16 hour;
	int16 minute;
	int16 seconds;
	int16 milliSecond;
}MR_SQL_DATE_T;

/*执行mrc_sql_execute后返回结果的回调函数*/
typedef void (*MR_SQL_EXE_CB)(int32 exeResult, MR_SQL_EXE_INFO_T mr_sqlExeInfo);	//执行SQL语句完成后的回调函数

typedef void (*MR_Complete_CB)(int32 dlResult);


/************************************************************************/
/*
	功能：初始化云存储
	输入：
		cb：下载云插件的回调
	返回：
		MR_OPENSKY_WAITING：版本正在更新，需要等待回调
		MR_OPENSKY_SUCCESS：加载成功，没有回调
		MR_OPENSKY_FAILED：加载失败，没有回调
		MR_OPENSKY_PARAM_ERROR：参数错误，没有回调
*/
/************************************************************************/
int32 mrc_sql_start(MR_Complete_CB cb);

/************************************************************************/
/*
	设置云存储密钥，最大不能超过64个ASCII字符
	输入：
		secretStr：密钥字符串，由斯凯提供
	返回：
		void
*/
/************************************************************************/
void mrc_sql_setSecretStr(char* secretStr);

/************************************************************************/
/*函数名：mrc_sql_appendStatement
	功能：设置好将要执行的sql语句，可以append多次，以打包多句sql语句，注意参数一般也需要设置多次
	format说明：%A MR_SQLTYPE_BYTEA 注意!!!这个参数需对应两个变量，一个是bytea的首地址，另一个是bytea的长度!!!
					例："...%A...", ...,byteaAddr,byteaSize,... 也就是在byteaAddr额外附加上bytea的长度，
					这个长度变量不需要任何%对应，其他格式无特殊要求，一一对应即可
				%O MR_SQLTYPE_BOOLEAN
				%I MR_SQLTYPE_INTERGER
				%B MR_SQLTYPE_BIGINT
				%T MR_SQLTYPE_TIMESTP
				%V MR_SQLTYPE_VCHAR
				%U MR_SQLTYPE_UNIVCHAR
	其他注意事项：数据传入时需要以变量形式传入，例如在传bigInt类型时，如果直接写常数4，则编译器默认将其当int32处理，
				  导致后续参数堆栈数据读取错误，但传入变量可避免此问题
	输入：
		  sqlStatement 需要打包的sql语句
		  paramFormat 参数格式字符串
		  ...		  附带的参数	
	输出：MR_OPENSKY_RETCODE 枚举:
				
*/

/*
	设置欲执行的SQL语句（可同时设置多条）
	输入：
		sqlStatement：SQL语句
		paramFormat：SQL语句带的参数格式化字符串，与printf类似
		...：与paramFormat配套使用的参数
			说明：paramFormat当前有支持多种格式化字符串：
				%A：数组类型，其后参数必须带一个数组首地址和数组长度
				%O：布尔型，其后参数必须填一个0或1
				%I：整型，其后参数必须填一个int32
				%T：时间型，其后参数必须填一个时间
				%V：字符串，其后参数必须填一个字符串
				%U：UNICODE字符串，其后参数必须填一个Unicode字符串
			注意：
				数据传入时需要以变量形式传入，例如在传bigInt类型时，如果直接写常数4，则编译器默认将其当int32处理，
				导致后续参数堆栈数据读取错误，但传入变量可避免此问题
			例：
				mrc_sql_appendStatement("select * from T where id=?", "%I", 100); //执行select * from T where id=100
	返回：
		MR_OPENSKY_RETCODE枚举:
			MR_OPENSKY_SUCCESS：执行成功
			MR_OPENSKY_FAILED：执行失败
			MR_OPENSKY_PARAM_ERROR：传入参数错误
			MR_OPENSKY_MALLOC_FAILED: 分配内存失败
			MR_OPENSKY_STORAGE_REJECT : 有语句正在执行，所以拒绝执行当前操作
			(该函数返回值只需要判断是否是MR_OPENSKY_SUCCESS即可，其它返回值都是FAILED类型的细分)
*/
/************************************************************************/
int32 mrc_sql_appendStatement(const char* sqlStatement, const char* paramFormat, ...);

/************************************************************************/
/*
	执行设置好的SQL语句
	输入：
		query_cb：执行后的结果回调函数
		skyid：若需要skyid则填写，若不需要则填0
	返回：
		MR_OPENSKY_WAITING：等待执行结果回调
		其他(MR_OPENSKY_FAILED / MR_OPENSKY_PARAM_ERROR / MR_OPENSKY_STORAGE_REJECT)：失败
*/
/************************************************************************/
int32 mrc_sql_execute(MR_SQL_EXE_CB query_cb, int32 skyid);

/************************************************************************/
/*
	获取SQL结果集的相关信息
	输入：
		resultSetIndex：结果集索引号，从0开始
	输出：
		mr_sqlRsInfo：结果集信息MR_SQL_RS_INFO_T
	返回：
		MR_OPENSKY_SUCCESS：执行成功
		MR_OPENSKY_FAILED：执行失败
*/
/************************************************************************/
int32 mrc_sql_getResultSetInfo(int32 resultSetIndex, MR_SQL_RS_INFO_T* mr_sqlRsInfo);

/************************************************************************/
/*
	解析SQL结果集
	输入：
		resultSetIndex：结果集索引号，从0开始
		row：目标数据的所在行，从0开始
		column：目标数据的所在列，从0开始
	输出：
		pData：目标数据
			注意：目标数据类型必须与SQL表中的完全对应，否则可能出现不可预料的结果。
				  例如，SQL表中为int32时，void*必须为int32*。
		len：数据占用的长度，以字节为单位。int32占4个字节
	返回：
		MR_OPENSKY_SUCCESS：执行成功
		MR_OPENSKY_FAILED：执行失败
*/
/************************************************************************/
int32 mrc_sql_resolveRS(int32 resultSetIndex, int32 row, int32 column, void* pData, int32* len);

/************************************************************************/
/*
	释放所有资源，并卸载插件
*/
/************************************************************************/
void mrc_sql_releaseResource(void);	//释放被占用的资源

/************************************************************************/
/*
	释放所有资源，不卸载插件
*/
/************************************************************************/
void mrc_sql_releaseResourceOnly(void);

/*********************************************************
	方便读取数据而增加的接口，以下接口可用
	通用解析函数mrc_sql_resolveRS实现相同功能
	如果习惯使用mrc_sql_resolveRS下面函数可忽略
**********************************************************/

/************************************************************************/
/*
	解析SQL结果集中的boolean数据
	输入：
		resultSetIndex：结果集索引号，从0开始
		row：目标数据的所在行，从0开始
		column：目标数据的所在列，从0开始
	输出：
		len：数据占用的长度，以字节为单位，如果长度为0可能是解析失败
	返回：
		解析后的boolean值
*/
/************************************************************************/
uint8 mrc_sql_resolve_boolen(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	解析SQL结果集中的int32数据
	输入：
		resultSetIndex：结果集索引号，从0开始
		row：目标数据的所在行，从0开始
		column：目标数据的所在列，从0开始
	输出：
		len：数据占用的长度，以字节为单位，如果长度为0可能是解析失败
	返回：
		解析后的int32值
*/
/************************************************************************/
uint32 mrc_sql_resolve_int(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	解析SQL结果集中的bigint数据
	输入：
		resultSetIndex：结果集索引号，从0开始
		row：目标数据的所在行，从0开始
		column：目标数据的所在列，从0开始
	输出：
		len：数据占用的长度，以字节为单位，如果长度为0可能是解析失败
	返回：
		解析后的bigint值
*/
/************************************************************************/
uint64 mrc_sql_resolve_bigInt(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	解析SQL结果集中的date数据
	输入：
		resultSetIndex：结果集索引号，从0开始
		row：目标数据的所在行，从0开始
		column：目标数据的所在列，从0开始
	输出：
		len：数据占用的长度，以字节为单位，如果长度为0可能是解析失败
	返回：
		解析后的MR_SQL_DATE_T值
*/
/************************************************************************/
MR_SQL_DATE_T mrc_sql_resolve_timeStamp(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	解析SQL结果集中的byte数组数据
	输入：
		resultSetIndex：结果集索引号，从0开始
		row：目标数据的所在行，从0开始
		column：目标数据的所在列，从0开始
	输出：
		len：数据占用的长度，以字节为单位，如果长度为0可能是解析失败
	返回：
		解析后的byte数据首地址指针
*/
/************************************************************************/
const uint8* mrc_sql_resolve_bytea(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	解析SQL结果集中的varchar数据
	输入：
		resultSetIndex：结果集索引号，从0开始
		row：目标数据的所在行，从0开始
		column：目标数据的所在列，从0开始
	输出：
		len：数据占用的长度，以字节为单位，如果长度为0可能是解析失败
	返回：
		解析后的char数据首地址指针
*/
/************************************************************************/
const char* mrc_sql_resolve_varChar(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	获取服务器时间的回调
	输出：
		result：是否成功
		date：时间戳，仅当result==MR_OPENSKY_SUCCESS时有效
*/
/************************************************************************/
typedef void (*MR_SQL_GETTIME_CB)(int32 result, MR_SQL_DATE_T date);

/************************************************************************/
/*
	获取服务器时间
	输入：
		cb：回调函数
	返回：
		MR_OPENSKY_RETCODE，当MR_OPENSKY_WAITING时需要等待回调
*/
/************************************************************************/
int32 mrc_sql_getServerTime(MR_SQL_GETTIME_CB cb);

/************************************************************************/
/*
	从缓存中服务器时间
	返回：
		缓存中保存的服务器时间戳，这个时间在任意一个云插件请求过程中被更新
*/
/************************************************************************/
MR_SQL_DATE_T mrc_sql_getServerTimeFromBuffer(void);

#endif