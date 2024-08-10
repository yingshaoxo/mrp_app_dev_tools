#ifndef MRC_PLATREQ_H
#define MRC_PLATREQ_H


#define MRC_PLATFROM_REQUEST_TYPE_SCORE   0x1   // ���ӵ���Ϸ�ɼ�����ҳ�档
#define MRC_PLATFROM_REQUEST_TYPE_REMARK  0x2   // ��ת����Ϸ���۵�ҳ�档
#define MRC_PLATFROM_REQUEST_TYPE_CPZONE  0x3   // ��ת��CPר�� ������֧�֣�
#define MRC_PLATFROM_REQUEST_TYPE_GUIDES  0x4	// ��ת����Ϸ����


/*
 * ����Ҫ�첽���̵������֪ͨӦ�ý����
 */
typedef void (*mrc_platreq_cb_t)(int32 result/*ErrorCode*/, int32 var1, int32 var2, int32 var3, int32 var4, int32 var5);

/*
 *  ������ mrc_platformRequest
 *
 * Parameters :
 *        reqType : ��������ͣ�Ŀǰ����֧��  �ɼ����У���Ϸ���ۣ�cpר��������֧�֣���
 *     
 *        resultCB: ���첽�������֪ͨ����ġ�
 *
 *        hintStr:       (const char*),  �����Ҫ���������ع��ܲ������ʾ���û�����ʾ� ����ΪNULL��  utf16be encoded.
 *
 *        MRC_PLATFROM_REQUEST_TYPE_SCORE:
 *                 var1 : int32, ��Ϸ�Ļ��֡�
 * 
 * Returns : 
 *       MR_SUCCESS:  �ɹ��� Ӧ�ò���Ҫ��ʲô�� 
 *                    ��ͨ��resultCB��֪ͨӦ��ʵ�ʵĽ����Ŀǰ���ܵ�result�У�
 *
 *      ********************************************************************************************************
 *              NOTE :  �������ֵΪMR_SUCCESS, ��ôӦ����Ҫֹͣ�Լ���ˢ����������ܺͻ�����ز����ͻ��
 *      ********************************************************************************************************                               
 *                        
 *      
 *       ELSE:   ʧ�ܣ� Ӧ����Ҫ��һ��UI��ʾ�û��� ��ʾ���ݿ���Ϊ�� ������ƽ̨���ܳ���������룺%x��
 *       
 * �첽�ص����
 *    result: 
 *		MR_SUCCESS:������ɹ����к��˳�
 *		MR_FAILED:�����δ�ܳɹ�����
 *
 */

extern int32 mrc_platformRequest(int32 reqType, mrc_platreq_cb_t resultCB, const char* hintStr, int32 var1, int32 var2, int32 var3);


/* ����mrc_startBrowser
 * Parameters:
 *    url1:��ѡ���ص�url������ΪNULL
 *    url2:��url1����ʧ��ʱ����url2������ΪNULL
 *    resultCB:���첽�������֪ͨ���
 *    hintStr:       (const char*),  �����Ҫ���������ع��ܲ������ʾ���û�����ʾ� ����ΪNULL��  utf16be encoded.
 *
 * Return:
 *       MR_SUCCESS:  �ɹ��� Ӧ�ò���Ҫ��ʲô�� 
 *                    ��ͨ��resultCB��֪ͨӦ��ʵ�ʵĽ����
 *       ELSE:   ʧ�ܣ� Ӧ����Ҫ��һ��UI��ʾ�û���
 *					��ʾ���ݿ���Ϊ�� �����������ʧ�ܡ�������룺%x��
 *       
 *
 * �첽�ص����
 *    result: 
 *		MR_SUCCESS:������ɹ����к��˳�
 *		MR_FAILED:�����δ�ܳɹ�����
 */
extern int32 mrc_startBrowser(const char *url1, const char *url2, mrc_platreq_cb_t resultCB, const char* hintStr);


/*����������Ĺ���·��
*  �����Ĭ�Ϲ���·������������app��·��
*  Parameters:
*  	pszwkdir:����·��
*  ����ֵ:
*  result: 
*	   MR_SUCCESS:���óɹ�
*	   MR_FAILED:����ʧ��
*/
extern int32 mrc_setwkDir(char * pszwkdir);

#endif
