#ifndef _MRC_PB_H_
#define _MRC_PB_H_

#define MR_PHB_SUCCESS 				    (1000)
#define MR_PHB_ERROR 					(1001)
#define MR_PHB_READY 					(1002)
#define MR_PHB_NOT_SUPPORT 				(1003)
#define MR_PHB_NOT_READY 				(1004)
#define MR_PHB_NOT_FOUND 				(1005)
#define MR_PHB_STORAGE_FULL 			(1006) 
#define MR_PHB_NUMBER_TOO_LONG 			(1007)
#define MR_PHB_OUT_OF_INDEX 			(1008)
#define MR_PHB_MAX_NAME 				(36*2) 
#define MR_PHB_MAX_NUMBER 				(48*2)
#define MR_PHB_MAX_SEARCH_PATTERN 		(60*2)


enum PB_SEARCH_TYPE{
	TYPE_SEARCH_NONE,
	TYPE_SEARCH_NAME,
	TYPE_SEARCH_NUM
	};
enum PB_TYPE{
	TYPE_NONE,
	TYPE_PB_SIM,
	TYPE_PB_PHONE,
	TYPE_PB_ALL
};

typedef struct 
{
	char name[MR_PHB_MAX_NAME];
	char number[MR_PHB_MAX_NUMBER];
	char homeNumber[MR_PHB_MAX_NUMBER];
	char officeNumber[MR_PHB_MAX_NUMBER];
	char faxNumber[MR_PHB_MAX_NUMBER];
	char emailAddress[MR_PHB_MAX_NUMBER];
}T_PHB_ENTRY;
		
typedef struct
{
    char pattern[MR_PHB_MAX_SEARCH_PATTERN]; /*�����������unicode*/
    uint16 search_type;  /*�������ͣ�1��ͨ�����ƣ�2��ͨ������*/
    uint16 storage;       /*������*/
}T_PHB_SEARCH_REQ;

typedef struct
{
	uint16 index;                   /*���б��е�λ�ã���ʼ�� 0*/
	uint16 store_index;              /*��sim�������ֻ��������λ��*/
	T_PHB_ENTRY phb_entry; /*������Ϣ*/
}T_PHB_SEARCH_RSP;

typedef struct 
{
     uint16 storage;//��Ϣ�洢��λ�ã�sim���������ֻ�
     uint16 index; //Ҫ�޸ĵ���Ŀ���б��е�������0��ʼ��������� 0xfffff��ʾ����һ����Ŀ
     T_PHB_ENTRY entry; //�������Ϣ
}T_PHB_SET_ENTRY_REQ;

typedef struct
{
     uint16 index;       //��Ŀ���б��е�����
     uint16 storage;    //��Ŀ�����λ�á�
}T_PHB_GET_ENTRY_REQ;

typedef struct
{
	 T_PHB_ENTRY entry;//����Ŀ�ľ�����Ϣ
}T_PHB_GET_ENTRY_RSP;

typedef struct 
{
	int32 storage; /*1��sim���绰����2���ֻ��绰����3��ȫ��*/
}T_PHB_GET_COUNT_REQ;

typedef struct
{
	int32 count;/*��¼�ĸ���*/
}T_PHB_GET_COUNT_RSP;

/**************************************************************************************
�绰�������¼�˵��
1��	�绰���Ĳ����¼�����mr_event����ʽ֪ͨ�ϲ�Ӧ�á�
2��	Mr_event�����������ľ���˵�����£�
    Mr_event(p1,p2,p3);
			 p1 = 14
			 p2 = 1����ȡ��Ŀδʹ��
			 p3 = MR_FAILED ����ʧ��
			      MR_SUCCESS �����ɹ�
***************************************************************************************/

/*

����:
	T_PHB_SEARCH_REQ 
					char pattern[MR_PHB_MAX_SEARCH_PATTERN]; �����������unicode
    				unsigned short int search_type;  �������ͣ�TYPE_SEARCH_NAME��TYPE_SEARCH_NUM
    				unsigned short int storage;      ������
���:
	T_PHB_SEARCH_RSP 
					unsigned short int index;                   ���б��е�λ�ã���ʼ�� 0
	              	unsigned short int store_index;             ��sim�������ֻ��������λ��,ȡֵΪTYPE_PB_SIM, TYPE_PB_PHONE,TYPE_PB_ALL
	              	T_PHB_ENTRY phb_entry;       ������Ϣ

����:
			 MR_PHB_NOT_READY 	 //�绰����ʼ����
             MR_PHB_NOT_FOUND    //2�������1����������Ϊ�գ�2��δ�ҵ�
             MR_PHB_SUCCESS      //�����ɹ�

*/
int32 mrc_searchPBRecord(T_PHB_SEARCH_REQ *t_req,T_PHB_SEARCH_RSP *t_rsp);



/*
����:
	T_PHB_GET_ENTRY_REQ 
					unsigned short int 	index;       //��Ŀ���б��е���������0��ʼ
     				unsigned short int  	storage;     //��Ŀ�����λ�ã�ȡֵΪTYPE_PB_SIM, TYPE_PB_PHONE,TYPE_PB_ALL

���:
	T_PHB_GET_ENTRY_RSP
					T_PHB_ENTRY entry;//����Ŀ�ľ�����Ϣ

����:
			MR_PHB_OUT_OF_INDEX������Խ��
            MR_PHB_SUCCESS       ��ȡ�ɹ�
			MR_PHB_NOT_READY     �绰����ʼ����
��ע:���������һ���첽�Ĺ��̣������Ľṹ��ͨ��mr_event����ʽ���͡� ���忴ǰ�벿���¼�˵����
*/

int32 mrc_readPBRecord(T_PHB_GET_ENTRY_REQ *t_req, T_PHB_GET_ENTRY_RSP *t_rsp);

/*
����:
	T_PHB_GET_COUNT_REQ
					long int storage;  TYPE_PB_SIM, TYPE_PB_PHONE,TYPE_PB_ALL

���:
	T_PHB_GET_COUNT_RSP
					long int count;��¼�ĸ���

����:
			 MR_PHB_SUCCESS      ��ü�¼�ɹ�
             MR_PHB_NOT_READY    �绰����ʼ����
             MR_PHB_NOT_SUPPORT  �����������

*/
int32 mrc_getPBRecordCount(T_PHB_GET_COUNT_REQ *t_req, T_PHB_GET_COUNT_RSP *t_rsp);

/*
����:
		 	void

���:	
			void

����:
			 MR_PHB_NOT_READY ��1004����ʼ����
      		 MR_PHB_READY      ��1002�����Խ��в���
*/
int32 mrc_getPBState(void);

/*
����:
			void

���:
			void

����:
			 MR_SUCCESS �������������ɹ�
             MR_FAILED  �������������ʧ��
             MR_IGNORE  ����֧�ָù���
*/
int32 mrc_createPBObj(void);

/*
����:
			void

���:
			void

����:
			 MR_SUCCESS ���ɹ�����
             MR_FAILED  ������ʧ��
             MR_IGNORE  ����֧�ָù���

*/
int32 mrc_destoryPBObj(void);

/*
����:
		

���:
		T_PHB_ENTRY
				char	name[MR_PHB_MAX_NAME];          //����
	          	char	number[MR_PHB_MAX_NUMBER];		//����
	          	char	homeNumber[MR_PHB_MAX_NUMBER];	//��ͥ����
	          	char	officeNumber[MR_PHB_MAX_NUMBER];//�칫����
	          	char	faxNumber[MR_PHB_MAX_NUMBER];	//�������
	          	char	emailAddress[MR_PHB_MAX_NUMBER];//�ʼ���ַ
				

����:
		MR_SUCCESS �������Ϣ�ɹ�
        MR_FAILED  �������Ϣʧ��
        MR_IGNORE  ����֧�ָù���
*/
int32 mrc_getPBInfo(T_PHB_ENTRY *t_recInfo);

#endif
