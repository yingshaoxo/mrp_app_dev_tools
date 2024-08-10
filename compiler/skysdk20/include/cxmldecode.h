
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


/***************************����xml����˵��***************************
	ֻ���utf��8�����xml���������н������������������һ�������������ڵ�
��Ϊ��Ӧ��tag��ע�⣬��ʹ����֮�󣬱������docDestroy���������ڴ��ͷţ�
��docParse��docDestroy����ɶ�ʹ�á�
���磺
<?xml version="1.0" encoding="UTF-8" ?> 
<list>
	 <item>
		<song>�Ȱ���õ��</song> 
		<singer>��˴���</singer> 
		<stream_url>http://charge.sky-mobi.com:6100/A8/music/seg/music_01.mp3</stream_url> 
		<download_url>http://charge.sky-mobi.com:6100/A8/music/all/music_01.mp3</download_url> 
		<ring_id>0002</ring_id> 
		<price>2.00</price> 
		<reserved>reserved</reserved> 
	</item>

	<item>
		<song>�ж�����</song> 
		<singer>ŷ���Ʒ�</singer> 
		<stream_url>http://charge.sky-mobi.com:6100/A8/music/seg/music_02.mp3</stream_url> 
		<download_url>http://charge.sky-mobi.com:6100/A8/music/all/music_02.mp3</download_url> 
		<ring_id>0003</ring_id> 
		<price>2.00</price> 
		<reserved>reserved</reserved> 
	</item>

	<item>
		<song>��������</song> 
		<singer>��˴���</singer> 
		<stream_url>http://charge.sky-mobi.com:6100/A8/music/seg/music_03.mp3</stream_url> 
		<download_url>http://charge.sky-mobi.com:6100/A8/music/all/music_03.mp3</download_url> 
		<ring_id>0004</ring_id> 
		<price>4.00</price> 
		<reserved>reserved</reserved> 
	</item>
 </list>

  ����xml���ݾ������ͣ���õ����µ����ͽṹ��
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

  ���Ҫ��õ�һ��item elementNode�ڵ�� price�ڵ㣬��������£�
  1������docCreate����TiXmlDoc�ڵ�
  2������docParse��xml���ݽ��н������������ϵ����ͽṹ
  3������docRootElement���list elementNode�ڵ�
  4������getFirstChildElementNode��õ�һ��item elementNode
  5������getElementByName����price��Ϊname�������ܻ��price elementNode
  6��ʹ����ϱ����˵���docDestroy�����ͷ�
*********************************************************************/


/********************************************************************
Function: docCreate(void) 
Description: ���� TiXmlDoc�ڵ�
Input:  
Output:  
Return:  TiXmlDoc�ڵ�
Notice:  ��������xml�ļ��ĸ��ڵ�
*********************************************************************/
TiXmlDoc* docCreate(void);


/********************************************************************
Function:  docDestroy
Description: �ͷ� TiXmlDoc��������ڵ�
Input: 
	doc:���ڵ�ָ�룻
Output:  
Return: 
	0��δ�ɹ��ͷ�
	1���ɹ��ͷ�
Notice:  ������parse xml�ļ�֮�䣬��Ҫ���ô˺������ͷŵ�һ�δ��������ڵ�
*********************************************************************/
int docDestroy(TiXmlDoc* doc);


/********************************************************************
Function:  docParse
Description:  ��xml�ļ����ݽ��н���������xml tag��
Input:  
	docNode�����ڵ�
	p��ָ��xml���ݵ�ָ��
	encoding�����뷽ʽ������ֻ���utf-8�ı��뷽ʽ
Output:  
	��docNodeΪ���ڵ��xml tag��
Return:  
Notice:  
*********************************************************************/
char* docParse(TiXmlDoc* docNode, char* p, int encoding);


/********************************************************************
Function:  getDecNode
Description:  ���xml declaration�ڵ�
Input:  
	node��ָ��TiXmlDoc�ĸ����
Output:  
Return:  
	NULL��δ�ҵ�TiXmlDec�ڵ�
	�ҵ��򷵻�ָ��TiXmlDec�ڵ��ָ��
Notice:  
*********************************************************************/
TiXmlDec * getDecNode(TiXmlDoc* node);

/********************************************************************
Function:  getDecVersion
Description:  ���xml�ļ��汾
Input:  
	node��ָ��TiXmlDec�ڵ��ָ��
Output: 
	
Return:  
	ָ��汾�ŵ��ַ������硰1.0���� 
Notice:  
*********************************************************************/


/********************************************************************
Function: getDecVersion 
Description:���xml�ļ��İ汾��  
Input: 
	node�� ָ��TiXmlDec�ڵ��ָ��
Output:  
Return:
	��õİ汾���ַ���  
Notice:  
*********************************************************************/
char *getDecVersion(TiXmlDec * node);



/********************************************************************
Function:  getDectEncoding
Description:  ���xml�ļ��ı��뷽ʽ
Input: 
	node�� ָ��TiXmlDec�ڵ��ָ�� 
Output:  
Return: 
	��ñ��뷽ʽ���ַ����硰utf��8�� 
Notice:  
	��������Ҫ�󴫹�����xml����Ϊutf��8
*********************************************************************/
char *getDectEncoding(TiXmlDec * node);


/********************************************************************
Function: getDecStandalone 
Description: ��� standalone����ֵ
Input: 
	node�� ָ��TiXmlDec�ڵ��ָ��  
Output:  
Return:
	��yes�����ߡ�NO�����ַ���ָ��  
Notice:  
	�����õ���standaloneһ�㶼��"yes"
*********************************************************************/
char *getDecStandalone(TiXmlDec * node);


/********************************************************************
Function: getFstChildCommentNode 
Description: ��õ�һ��ע�����͵��ӽڵ� 
Input: 
	doc��TiXmlNode �ڵ�
Output:  
Return:  
	TiXmlText�ڵ�ָ��
Notice:  
*********************************************************************/
TiXmlText* getFstChildCommentNode(TiXmlNode* doc);


/********************************************************************
Function: getCommentContent 
Description: ���ע�ͽڵ������ 
Input:  
	node��ע�ͽڵ�
Output:  
Return:
	ע�ͽڵ�����ָ��  
Notice:  
*********************************************************************/
char* getCommentContent(TiXmlText* node);


/********************************************************************
Function:  getFstChildCDATANode
Description:  ���cdata�ڵ�
Input:  
	doc��ָ��TiXmlNode��ָ��
Output:  
Return: 
	��õ� cdata�ڵ�
Notice:  
*********************************************************************/
TiXmlText* getFstChildCDATANode(TiXmlNode* doc);


/********************************************************************
Function: getCDATAContent 
Description:  ���cdata�ڵ������
Input:  
	node��cdata�ڵ�ָ��
Output:  
Return:  
	ָ��cdata�ڵ���ַ���ָ��
Notice:  
*********************************************************************/
char* getCDATAContent(TiXmlText* node);
//char* elmGetText(TiXmlElm* elm);

/********************************************************************
Function: elmFstAttr 
Description: ���element�ڵ��һ������ 
Input:  
	elm��element�ڵ�ָ��
Output:  
Return:  
	��element�ڵ�ĵ�һ������
Notice:  
	һ��element�ڵ�����кܶ�����
*********************************************************************/
TiXmlAttribute * elmFstAttr(TiXmlElm* elm);


/********************************************************************
Function: elmNextAttr 
Description: ���element��һ������
Input:  
	att��element�ڵ�����ָ��
Output:  
Return:  
	��element��һ������
Notice:  
	һ��element�ڵ�����кܶ�����
*********************************************************************/
//TiXmlAttribute * elmNextAttr(TiXmlAttribute* att);


/********************************************************************
Function: elmLstAttr 
Description: ���element�ڵ����һ������ 
Input:  
	elm��element�ڵ�ָ��
Output:  
Return:  
	��element�ڵ�����һ������
Notice:  
	һ��element�ڵ�����кܶ�����
*********************************************************************/
TiXmlAttribute * elmLstAttr(TiXmlElm* elm);


/********************************************************************
Function:  lastCld
Description: ���һ���ڵ��µ����һ���ӽڵ� 
Input:  
Output:  
Return:  
	���һ���ӽڵ�
Notice:  
*********************************************************************/
TiXmlNode* lastCld(TiXmlNode* node);


/********************************************************************
Function:  firstCld
Description: ���һ���ڵ��µĵ�һ���ӽڵ� 
Input:  
Output:  
Return:  
	��һ���ӽڵ�
Notice:  
*********************************************************************/
TiXmlNode* firstCld(TiXmlNode* node);


/********************************************************************
Function:  getFirstChildElementNode
Description:  ��ô���ڵ��µ�һ��element�ӽڵ�
Input:  
Output:  
Return:  
	��һ��element�ӽڵ�
Notice:  
*********************************************************************/
TiXmlElm* getFirstChildElementNode(TiXmlNode* node);


/********************************************************************
Function:  getElementByName
Description:  ����element�ڵ����֣��ҳ�element�ڵ�
Input:  
Output:  
Return: 
	 element�ڵ�
Notice:  
	ֻ����node�ڵ���ӽڵ�ƥ��name��element�ڵ�
*********************************************************************/
TiXmlElm* getElementByName(TiXmlNode* node,char* name);


/********************************************************************
Function:  getElementContent
Description:  ���element�ڵ�����ֺ�����
Input: 
	elm��element�ڵ�ָ�� 
Output: 
	name��element�ڵ�����
	text��element�ڵ�����
Return:  
Notice:  
*********************************************************************/
int getElementContent(TiXmlElm* elm,char* name, char* text);


/********************************************************************
Function: getNextElement 
Description:  ���docNode�ڵ����һ��element�ڵ�
Input:  
Output:  
Return: 
	 ��һ��element�ڵ㣬���û�У�����NULL
Notice:  
*********************************************************************/
TiXmlElm* getNextElement(TiXmlNode* Node);


/********************************************************************
Function: docRootElement 
Description:  ���docNode�ӽڵ�ĵ�һ��element�ڵ�
Input:  
Output:  
Return: 
	���ڵ��µ�һ��element�ڵ� 
Notice:  
*********************************************************************/
TiXmlElm* docRootElement(TiXmlDoc* docNode);


/********************************************************************
Function: LinkEndChild 
Description: ���ڵ����ӵ����ڵ����һ���ӽڵ�� 
Input:  
	parentNode�����ڵ�
	node���ӽڵ�
Output:  
Return:  
	ָ��node��ָ��
Notice:  
*********************************************************************/
TiXmlNode* LinkEndChild(TiXmlNode* parentNode, TiXmlNode* node );


#endif

