#ifndef CXMLENCODE_H
#define CXMLENCODE_H

#include "cxmldecode.h"


/********************************************************************
Function:  xmlWriteDeclation
Description:  ��װDeclation tag
Input:  
	version��xml�汾���ַ���
	encoding�����뷽ʽ�ַ���������Ӧ��Ϊ��UTF-8"
	standalone:����Ӧ��Ϊ"YES"
Output:  
	p:��װ����ַ���ָ��
Return:  
	ָ����װ���ַ����Ľ�β
Notice:  
	
*********************************************************************/
char* xmlWriteDeclation(char* p,char* version,char* encoding,char* standalone);


/********************************************************************
Function: xmlWriteDeclationNode 
Description: ����TiXmlDec���ݣ���װ Declation tag
Input:  
	node��TiXmlDecָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
*********************************************************************/
char* xmlWriteDeclationNode(char* p,TiXmlDec* node);


/********************************************************************
Function: xmlWriteCommen 
Description: ����comment���ݣ���װ comment tag
Input:  
	comment���ַ���ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
*********************************************************************/
char* xmlWriteCommen(char*p,char* comment);


/********************************************************************
Function: xmlWriteCommenNode 
Description: ����comment�ڵ㣬��װ comment tag
Input:  
	comment��TiXmlText�ڵ�ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
*********************************************************************/
char* xmlWriteCommenNode(char* p,TiXmlText* comment);


/********************************************************************
Function: xmlWriteCdata 
Description: ����cdata���ݣ���װ cdata tag
Input:  
	cdata���ַ���ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
*********************************************************************/
char* xmlWriteCdata(char* p,char* cdata);


/********************************************************************
Function: xmlWriteCdataNode 
Description: ����cdata�ڵ㣬��װ cdata tag
Input:  
	cdata��TiXmlText�ڵ�ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
*********************************************************************/
char* xmlWriteCdataNode(char* p,TiXmlText* cdata);


/********************************************************************
Function: xmlWriteNullElement 
Description: ��װ��element tag����ֻ�����Ժ����֣�û�����ݵ�element
Input:  
	att����element���Խڵ�ָ��
	name����element����
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
*********************************************************************/
char* xmlWriteNullElement(char* p,TiXmlAttribute* att,char* name);


/********************************************************************
Function: xmlWriteElementBgnTag 
Description: ��װelement tag��ʼ��־�����Լ����ƣ�û�н�����־
Input:  
	elm��element�ڵ�ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
	��element������־�ֿ�����Ϊ��ʵ�ָ�element�ڵ��ְ�����element�ڵ�
*********************************************************************/
char* xmlWriteElementBgnTag(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteElementEndTag 
Description: ��װelement tag������־
Input:  
	elm��element�ڵ�ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
	��element������־�ֿ�����Ϊ��ʵ�ָ�element�ڵ��ְ�����element�ڵ�
*********************************************************************/
char* xmlWriteElementEndTag(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteOneElement 
Description: ��װһ��������element tag
Input:  
	elm��element�ڵ�ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
	��װһ��������elementtag
*********************************************************************/
char* xmlWriteOneElement(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteOneElementWithoutAtt 
Description: ��װһ�������Ե�������element tag
Input:  
	name��element����
	content��elementֵ
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  

*********************************************************************/
char* xmlWriteOneElementWithoutAtt(char* p,char* name,char* content);


/********************************************************************
Function: xmlWriteAllElement 
Description: ��װһ��������element��
Input:  
	elm��element�ڵ�ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
	����element�ڵ㼰������element�ڵ���װ
*********************************************************************/
char* xmlWriteAllElement(char* p,TiXmlElm* elm);


/********************************************************************
Function: xmlWriteDoc 
Description: ����һ��������xml��ʽ���ַ���
Input:  
	doc��TiXmlDoc�ڵ�ָ��
Output: 
	p:��װ����ַ���ָ�� 
Return:
	ָ����װ���ַ����Ľ�β  
Notice:  
	����docָ�룬�������͵������ӽڵ���װ��xml��ʽ���ַ���
*********************************************************************/
char* xmlWriteDoc(char*p, TiXmlDoc* doc);
#endif
