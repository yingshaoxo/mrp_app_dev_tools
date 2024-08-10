#ifndef CXMLENCODE_H
#define CXMLENCODE_H

#include "cxmldecode.h"


/********************************************************************
Function:  xmlWriteDeclation
Description:  组装Declation tag
Input:  
	version：xml版本号字符串
	encoding：编码方式字符串，这里应该为“UTF-8"
	standalone:这里应该为"YES"
Output:  
	p:组装后的字符串指针
Return:  
	指向组装后字符串的结尾
Notice:  
	
*********************************************************************/
char* xmlWriteDeclation(char* p,char* version,char* encoding,char* standalone);


/********************************************************************
Function: xmlWriteDeclationNode 
Description: 根据TiXmlDec内容，组装 Declation tag
Input:  
	node：TiXmlDec指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
*********************************************************************/
char* xmlWriteDeclationNode(char* p,TiXmlDec* node);


/********************************************************************
Function: xmlWriteCommen 
Description: 根据comment内容，组装 comment tag
Input:  
	comment：字符串指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
*********************************************************************/
char* xmlWriteCommen(char*p,char* comment);


/********************************************************************
Function: xmlWriteCommenNode 
Description: 根据comment节点，组装 comment tag
Input:  
	comment：TiXmlText节点指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
*********************************************************************/
char* xmlWriteCommenNode(char* p,TiXmlText* comment);


/********************************************************************
Function: xmlWriteCdata 
Description: 根据cdata内容，组装 cdata tag
Input:  
	cdata：字符串指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
*********************************************************************/
char* xmlWriteCdata(char* p,char* cdata);


/********************************************************************
Function: xmlWriteCdataNode 
Description: 根据cdata节点，组装 cdata tag
Input:  
	cdata：TiXmlText节点指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
*********************************************************************/
char* xmlWriteCdataNode(char* p,TiXmlText* cdata);


/********************************************************************
Function: xmlWriteNullElement 
Description: 组装空element tag，即只有属性和名字，没有内容的element
Input:  
	att：该element属性节点指针
	name：该element名称
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
*********************************************************************/
char* xmlWriteNullElement(char* p,TiXmlAttribute* att,char* name);


/********************************************************************
Function: xmlWriteElementBgnTag 
Description: 组装element tag开始标志、属性及名称，没有结束标志
Input:  
	elm：element节点指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
	将element结束标志分开，是为了实现该element节点又包含子element节点
*********************************************************************/
char* xmlWriteElementBgnTag(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteElementEndTag 
Description: 组装element tag结束标志
Input:  
	elm：element节点指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
	将element结束标志分开，是为了实现该element节点又包含子element节点
*********************************************************************/
char* xmlWriteElementEndTag(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteOneElement 
Description: 组装一个完整的element tag
Input:  
	elm：element节点指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
	组装一个完整的elementtag
*********************************************************************/
char* xmlWriteOneElement(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteOneElementWithoutAtt 
Description: 组装一个无属性的完整的element tag
Input:  
	name：element名称
	content：element值
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  

*********************************************************************/
char* xmlWriteOneElementWithoutAtt(char* p,char* name,char* content);


/********************************************************************
Function: xmlWriteAllElement 
Description: 组装一个完整的element树
Input:  
	elm：element节点指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
	将该element节点及其子孙element节点组装
*********************************************************************/
char* xmlWriteAllElement(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteDoc 
Description: 生成一个完整的xml形式的字符串
Input:  
	doc：TiXmlDoc节点指针
Output: 
	p:组装后的字符串指针 
Return:
	指向组装后字符串的结尾  
Notice:  
	根据doc指针，将该树型的所有子节点组装成xml形式的字符串
*********************************************************************/
char* xmlWriteDoc(char*p, TiXmlDoc* doc);
#endif
