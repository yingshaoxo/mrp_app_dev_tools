
#ifndef __XMLDECODE_H__
#define __XMLDECODE_H__

#include "cxmlstring.h"

#define NODE_DOCUMENT  	0
#define NODE_ELEMENT     	1
#define NODE_COMMENT    	2
#define NODE_UNKNOWN   	3
#define NODE_TEXT            	4
#define NODE_DECLARATION 	5
#define NODE_CDATA			6	
#define NODE_TYPECOUNT     	7


#define TIXML_ENCODING_UNKNOWN 0
#define TIXML_ENCODING_UTF8 1
#define TIXML_ENCODING_LEGACY 2

#define TIXML_UTF_LEAD_0   0xef
#define TIXML_UTF_LEAD_1   0xbb
#define TIXML_UTF_LEAD_2   0xbf

typedef struct Tag_s_TitleAD
{
	int  type;
	char title_text[128];
	char title_param[128];
	
}s_TitleAD;


typedef struct tTag_TiXmlAttribute
{
    void* prev;
    void* next;
    microS name;
    microS value;
}TiXmlAttribute;

typedef struct tTag_TiXmlAttributeSet
{
    TiXmlAttribute sentinel;
}TiXmlAttributeSet;

typedef struct tTag_TiXmlNode
{
	int  nodetype;
	void * parent;
	void* firstChild;
	void* lastChild;
	void* prev;
	void* next;
	microS value;
}TiXmlNode;

typedef struct tTag_TiXmlDoc
{
	int  nodetype;
	void * parent;
	void* firstChild;
	void* lastChild;
	void* prev;
	void* next;
	microS value;
}TiXmlDoc;

typedef struct tTag_TiXmlDec
{
	int  nodetype;
	void * parent;
	void* firstChild;
	void* lastChild;
	void* prev;
	void* next;
	microS value;
	microS version;
	microS encoding;
	microS standalone;
}TiXmlDec;

typedef struct tTag_TiXmlElm
{
	int  nodetype;
	void * parent;
	void* firstChild;
	void* lastChild;
	void* prev;
	void* next;
	microS name;
	microS value;
	TiXmlAttributeSet attributeSet;
}TiXmlElm;

typedef struct tTag_TiXmlText
{
	int  nodetype;
	void * parent;
	void* firstChild;
	void* lastChild;
	void* prev;
	void* next;
	microS value;
}TiXmlText;

typedef struct tTag_TiXmlUnKnown
{
	int  nodetype;
	void * parent;
	void* firstChild;
	void* lastChild;
	void* prev;
	void* next;
	microS value;
}TiXmlUnKnown;


/***************************关于xml解析说明***************************
	只针对utf－8编码的xml数据流进行解析，解析结果即创建一棵树，各个树节点
即为对应的tag。注意，在使用完之后，必须调用docDestroy函数进行内存释放，
即docParse和docDestroy必须成对使用。
例如：
<?xml version="1.0" encoding="UTF-8" ?> 
<list>
	 <item>
		<song>等爱的玫瑰</song> 
		<singer>凤凰传奇</singer> 
		<stream_url>http://charge.sky-mobi.com:6100/A8/music/seg/music_01.mp3</stream_url> 
		<download_url>http://charge.sky-mobi.com:6100/A8/music/all/music_01.mp3</download_url> 
		<ring_id>0002</ring_id> 
		<price>2.00</price> 
		<reserved>reserved</reserved> 
	</item>

	<item>
		<song>感恩的心</song> 
		<singer>欧阳菲菲</singer> 
		<stream_url>http://charge.sky-mobi.com:6100/A8/music/seg/music_02.mp3</stream_url> 
		<download_url>http://charge.sky-mobi.com:6100/A8/music/all/music_02.mp3</download_url> 
		<ring_id>0003</ring_id> 
		<price>2.00</price> 
		<reserved>reserved</reserved> 
	</item>

	<item>
		<song>吉祥如意</song> 
		<singer>凤凰传奇</singer> 
		<stream_url>http://charge.sky-mobi.com:6100/A8/music/seg/music_03.mp3</stream_url> 
		<download_url>http://charge.sky-mobi.com:6100/A8/music/all/music_03.mp3</download_url> 
		<ring_id>0004</ring_id> 
		<price>4.00</price> 
		<reserved>reserved</reserved> 
	</item>
 </list>

  以上xml数据经过解释，会得到如下的树型结构：
				|--decNode									
				|
				|
				|
				|
				|
				|
				|
				|
				|
				|
				|
				|
	docNode-----|											
				|											
				|												|--song elementNode
				|												|--singer elementNode
				|												|--stream_url elementNode
				|						|--item elementNode---	|--download_url elementNode
				|						|						|--ring_id elementNode
				|						|						|--price elementNode
				|						|						|--reserved elementNode
				|						|						
				|						|						|--song elementNode
				|						|						|--singer elementNode
				|						|						|--stream_url elementNode
				|--list elementNode---	|--item elementNode---	|--download_url elementNode
										|						|--ring_id elementNode
										|						|--price elementNode
										|						|--reserved elementNode
										|
										|						|--song elementNode
										|						|--singer elementNode
										|						|--stream_url elementNode
										|---item elementNode---	|--download_url elementNode
																|--ring_id elementNode
																|--price elementNode
																|--reserved elementNode

  如果要获得第一个item elementNode节点的 price节点，其过程如下：
  1。调用docCreate创建TiXmlDoc节点
  2。调用docParse对xml数据进行解析，创建如上的树型结构
  3。调用docRootElement获得list elementNode节点
  4。调用getFirstChildElementNode获得第一个item elementNode
  5。调用getElementByName，将price作为name参数就能获得price elementNode
  6。使用完毕别忘了调用docDestroy进行释放
*********************************************************************/


/********************************************************************
Function: docCreate(void) 
Description: 创建 TiXmlDoc节点
Input:  
Output:  
Return:  TiXmlDoc节点
Notice:  创建分析xml文件的根节点
*********************************************************************/
TiXmlDoc* docCreate(void);


/********************************************************************
Function:  docDestroy
Description: 释放 TiXmlDoc及其子孙节点
Input: 
	doc:根节点指针；
Output:  
Return: 
	0：未成功释放
	1：成功释放
Notice:  在两次parse xml文件之间，需要调用此函数以释放第一次创建的树节点
*********************************************************************/
int docDestroy(TiXmlDoc* doc);


/********************************************************************
Function:  docParse
Description:  将xml文件内容进行解析，构建xml tag树
Input:  
	docNode：根节点
	p：指向xml内容的指针
	encoding：编码方式，这里只针对utf-8的编码方式
Output:  
	以docNode为根节点的xml tag树
Return:  
Notice:  
*********************************************************************/
char* docParse(TiXmlDoc* docNode, char* p, int encoding);


/********************************************************************
Function:  getDecNode
Description:  获得xml declaration节点
Input:  
	node：指向TiXmlDoc的根结点
Output:  
Return:  
	NULL：未找到TiXmlDec节点
	找到则返回指向TiXmlDec节点的指针
Notice:  
*********************************************************************/
TiXmlDec * getDecNode(TiXmlDoc* node);

/********************************************************************
Function:  getDecVersion
Description:  获得xml文件版本
Input:  
	node：指向TiXmlDec节点的指针
Output: 
	
Return:  
	指向版本号的字符串，如“1.0”等 
Notice:  
*********************************************************************/


/********************************************************************
Function: getDecVersion 
Description:获得xml文件的版本号  
Input: 
	node： 指向TiXmlDec节点的指针
Output:  
Return:
	获得的版本号字符串  
Notice:  
*********************************************************************/
char *getDecVersion(TiXmlDec * node);



/********************************************************************
Function:  getDectEncoding
Description:  获得xml文件的编码方式
Input: 
	node： 指向TiXmlDec节点的指针 
Output:  
Return: 
	获得编码方式的字符串如“utf－8” 
Notice:  
	我们这里要求传过来的xml编码为utf－8
*********************************************************************/
char *getDectEncoding(TiXmlDec * node);


/********************************************************************
Function: getDecStandalone 
Description: 获得 standalone属性值
Input: 
	node： 指向TiXmlDec节点的指针  
Output:  
Return:
	“yes”或者“NO”的字符串指针  
Notice:  
	我们用到的standalone一般都是"yes"
*********************************************************************/
char *getDecStandalone(TiXmlDec * node);


/********************************************************************
Function: getFstChildCommentNode 
Description: 获得第一个注释类型的子节点 
Input: 
	doc：TiXmlNode 节点
Output:  
Return:  
	TiXmlText节点指针
Notice:  
*********************************************************************/
TiXmlText* getFstChildCommentNode(TiXmlNode* doc);


/********************************************************************
Function: getCommentContent 
Description: 获得注释节点的内容 
Input:  
	node：注释节点
Output:  
Return:
	注释节点内容指针  
Notice:  
*********************************************************************/
char* getCommentContent(TiXmlText* node);


/********************************************************************
Function:  getFstChildCDATANode
Description:  获得cdata节点
Input:  
	doc：指向TiXmlNode的指针
Output:  
Return: 
	获得的 cdata节点
Notice:  
*********************************************************************/
TiXmlText* getFstChildCDATANode(TiXmlNode* doc);


/********************************************************************
Function: getCDATAContent 
Description:  获得cdata节点的内容
Input:  
	node：cdata节点指针
Output:  
Return:  
	指向cdata节点的字符串指针
Notice:  
*********************************************************************/
char* getCDATAContent(TiXmlText* node);
//char* elmGetText(TiXmlElm* elm);

/********************************************************************
Function: elmFstAttr 
Description: 获得element节点第一个属性 
Input:  
	elm：element节点指针
Output:  
Return:  
	该element节点的第一个属性
Notice:  
	一个element节点可以有很多属性
*********************************************************************/
TiXmlAttribute * elmFstAttr(TiXmlElm* elm);


/********************************************************************
Function: elmNextAttr 
Description: 获得element下一个属性
Input:  
	att：element节点属性指针
Output:  
Return:  
	该element下一个属性
Notice:  
	一个element节点可以有很多属性
*********************************************************************/
//TiXmlAttribute * elmNextAttr(TiXmlAttribute* att);


/********************************************************************
Function: elmLstAttr 
Description: 获得element节点最后一个属性 
Input:  
	elm：element节点指针
Output:  
Return:  
	该element节点的最后一个属性
Notice:  
	一个element节点可以有很多属性
*********************************************************************/
TiXmlAttribute * elmLstAttr(TiXmlElm* elm);


/********************************************************************
Function:  lastCld
Description: 获得一个节点下的最后一个子节点 
Input:  
Output:  
Return:  
	最后一个子节点
Notice:  
*********************************************************************/
TiXmlNode* lastCld(TiXmlNode* node);


/********************************************************************
Function:  firstCld
Description: 获得一个节点下的第一个子节点 
Input:  
Output:  
Return:  
	第一个子节点
Notice:  
*********************************************************************/
TiXmlNode* firstCld(TiXmlNode* node);


/********************************************************************
Function:  getFirstChildElementNode
Description:  获得传入节点下第一个element子节点
Input:  
Output:  
Return:  
	第一个element子节点
Notice:  
*********************************************************************/
TiXmlElm* getFirstChildElementNode(TiXmlNode* node);


/********************************************************************
Function:  getElementByName
Description:  根据element节点名字，找出element节点
Input:  
Output:  
Return: 
	 element节点
Notice:  
	只查找node节点的子节点匹配name的element节点
*********************************************************************/
TiXmlElm* getElementByName(TiXmlNode* node,char* name);


/********************************************************************
Function:  getElementContent
Description:  获得element节点的名字和内容
Input: 
	elm：element节点指针 
Output: 
	name：element节点名字
	text：element节点内容
Return:  
Notice:  
*********************************************************************/
int getElementContent(TiXmlElm* elm,char* name, char* text);


/********************************************************************
Function: getNextElement 
Description:  获得docNode节点的下一个element节点
Input:  
Output:  
Return: 
	 下一个element节点，如果没有，返回NULL
Notice:  
*********************************************************************/
TiXmlElm* getNextElement(TiXmlNode* Node);


/********************************************************************
Function: docRootElement 
Description:  获得docNode子节点的第一个element节点
Input:  
Output:  
Return: 
	根节点下第一个element节点 
Notice:  
*********************************************************************/
TiXmlElm* docRootElement(TiXmlDoc* docNode);


/********************************************************************
Function: LinkEndChild 
Description: 将节点链接到父节点最后一个子节点后 
Input:  
	parentNode：父节点
	node：子节点
Output:  
Return:  
	指向node的指针
Notice:  
*********************************************************************/
TiXmlNode* LinkEndChild(TiXmlNode* parentNode, TiXmlNode* node );


#endif

