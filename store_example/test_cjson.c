#include "stdio.h"
#include "cJSON.h"
int main(){
	int arr_size,i;
	cJSON* test_arr,arr_item;
    //��char* ģ��һ��JSON�ַ���
char* json_arr_string =\
"{,\
 \"test_1\":\"1\",\
 \"test_2\":\"2\",\
 \"test_arr\":\"[{\",\
\"{\",\
  \"test_1\":\"arr_1\",\
  \"test_2\":\"arr_2\",\
  \"test_3\":\"arr_3\",\
\"}\",\
\"{\"\
 \"test_1\":\"1\",\
 \"test_2\":\"2\",\
 \"test_3\":\"3\",\
\"}\",\
\"}]\",\
\"test_3\" :\"3\",\
}";//json_arr string;
 
//��һ�����JSON�ַ���
cJSON* cjson = cJSON_Parse(json_arr_string);
 
//�ж��Ƿ����ɹ�
if(cjson == NULL){
printf("cjson error��");
}
else{//����ɹ�����cJSON_Print��ӡ���
cJSON_Print(cjson);
}
 
//��ȡ�������
test_arr = cJSON_GetObjectltem(cjson,"test_arr");
 
//��ȡ��������������ѭ��
arr_size = cJSON_GetArraySize(test_arr);//return arr_size 2
 
//��ȡtest_arr��������ӽڵ�
arr_item = test_arr->child;//�Ӷ���
 
//ѭ����ȡ������ÿ���ֶε�ֵ��ʹ��cJSON_Print��ӡ
for(i = 0;i <=(arr_size-1)/*0*/;++i){
cJSON_Print(cJSON_GetObjectItem(arr_item,"test_1"));
cJSON_Print(cJSON_GetObjectItem(arr_item,"test_2"));
cJSON_Print(cJSON_GetObjectItem(arr_item,"test_3"));
arr_item = arr_item->next;//��һ���Ӷ���
}
 
//delete cjson ����ֻ��Ҫ�ͷ�cjson���ɣ���Ϊ�����Ķ�ָ����
cJSON_Delete(cjson);