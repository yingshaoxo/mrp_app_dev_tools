/** 
 ** ��ģ��
 **
 ** ����޸����ڣ�2012/2/28
 ** 
 **/

#include "ts_main.h"
#include "mrc_exb.h"
#include "debug.h"
#include "types.h"
#include "ts_aps_main.h"


#define MAINEXT_NAME		"main.ext"
#define LONGKEYPRESS_TIME	500	//������ʱ��
#define MRPDIR				"TSMain"
#define SAVEVMFILE			"vmdump0"

//EXT��־λ
#define F_EXT_CACHELOAD	0x01	//EXT���ڴ����û�����ģ�ж��EXT��ʱ�����ͷ��ڴ�
#define F_EXT_PAUSE		0x02	//EXT��ͣ��־
#define F_EXT_KEYPRESS	0x04	//EXT�����˰����¼������ڳɶԷ��������¼���
#define F_EXT_EVENTK	0x08	//EXT���������¼�
#define F_EXT_BLOCK		0x10	//EXT����״̬ ����ͣ��Ϸ�Ķ�ʱ���� ����block�����¼�.
//#define F_EXT_


#define _SET_FLAG(p, f) \
	(p->flage  |= (f))

#define _CLR_FLAG(p, f) \
	(p->flage  &= ~(f))

#define _HAS_FLAG(p, f) \
	(p->flage  & (f))


//ext���ݽṹ
typedef struct _extdata{
	int32 id;		//���ID����������Ϣʱ�ı�־
	int32 handel;	//ext�����mrc_extLoad ����ֵ
	uint8 *addres;	//ext�ڴ��ַ
	int32 len;		//ext�ֽ���
	ExtFun	fun;	//���ܺ���
	uint8 refCount;	//���ô�����ֱ�����ô���Ϊ0�������ͷ�ext
	uint8 loadMod;	//����ģʽ
	uint8 flage;	//��־λ

	struct _extdata *next;	//��һ��ext
	struct _extdata *prev;	//��һ��ext
}EXT_DATA, *PEXT_DATA;


//////// ȫ�־�̬������ ////////////////////////////////////////////////////////////////
static int32 scnW, scnH;
//��������EXT������
static struct {
	int32 count;	//���м��ص�ext��
	PEXT_DATA pHead;	//��һ��ext
	PEXT_DATA pEnd;	//���һ��ext
}extList;
static int32 timer;	//��ʱ��
static int8 longPressKey;	//��������˳������¼���������Ӧ���������¼���-1��ʾû�м�����



////////// �ڲ��������� ////////////////////////////////////////////////////////////////
static PEXT_DATA extlistGet(int32 id);	//ͨ��id��ȡext
static void extlistAdd(PEXT_DATA p);	//����ڵ�
static void extlistRemove(PEXT_DATA p); //�Ƴ�extlist�ڵ�
//�����û�����ģ��
static void loadMainext(void);




/////// �ڲ����� ///////////////////////////////////////////////////////////////////
//ͨ��ID��ȡext�ڵ㣬ʧ�ܷ���NULL
static PEXT_DATA extlistGet(int32 id){
	PEXT_DATA p;

	if(extList.count <= 0)
		return NULL;

	p = extList.pHead;
	while(p){
		if(p->id == id)
			return p;
		p = p->next;	//��һ��
	}

	return NULL;
}

//����ڵ㣨ע�⣺�ڱ�ͷ�壬�����ص�ext�����ϣ�
static void extlistAdd(PEXT_DATA p){
	if(extList.count == 0){
		extList.pEnd = extList.pHead = p;
	}else{
		p->next = extList.pHead;
		extList.pHead->prev = p;
		extList.pHead = p;
	}

	extList.count++;
}

//�Ƴ�extlist�ڵ�
static void extlistRemove(PEXT_DATA p){
	//ɾ���ڵ�
	if(p->prev == NULL){	//��ͷ
		if(p->next == NULL){	//ֻ��һ���ڵ�����
			extList.pHead = NULL;
		}else{
			p->next->prev = NULL;
			extList.pHead = p->next;
		}
	}else if(p->next == NULL){
		p->prev->next = NULL;
	}else{
		p->prev->next = p->next;
		p->next->prev = p->prev;
	}
	extList.count--;
	mrc_free(p);
}


////// ��ģ�鹦�ܺ���ʵ�� ///////////////////////////////////////////////////////
void Zhu_getScnSize1(int* w, int* h){
	if(w) *w = (int)scnW;
	if(h) *h = (int)scnH;
}

//��ȡ�������ģʽ
EXT_LOADMOD Zhu_getExtLoadMod1(int32 id){
	PEXT_DATA p = extlistGet(id);

	if(!p) return ZHU_EXTLOAD_NULL;

	return p->loadMod;
}

//��ext�ö�����ϵ�ext���Ƚ��հ����¼�
int32 Zhu_bringExtToTop1(int32 id){
	PEXT_DATA p = extlistGet(id);

	if(!p) return MR_FAILED;

	if(extList.count == 0) return MR_SUCCESS;	//ֻ��һ��

	if(extList.pHead == p) return MR_SUCCESS;	//�Ѿ������

	//ջ���� ջβ
	if(p->next == NULL){
		p->prev->next = NULL;
		p->prev = NULL;
		p->next = extList.pHead;
		extList.pHead->prev = p;
		extList.pHead = p;
	}else{	//ջ�м�
		p->prev->next = p->next;
		p->next->prev = p->prev;
		p->prev = NULL;
		p->next = extList.pHead;
		extList.pHead->prev = p;
		extList.pHead = p;
	}

	return MR_SUCCESS;
}

//��ĳ��·���µ�MRP�ڼ���EXT��id��ext����mrp·��������ģ������ڣ�
#ifdef SDK_MOD
int32 Zhu_loadExtPathEx1( int32 id, 
					   const char *extname, const char *mrpPath, 
					   PZHU_MEMINFO mem,
					   mrcEnterFuncSt *entryfun)
{
	uint8* buf = NULL;
	int32 len = 0;
	PEXT_DATA p;

	//����ǲ����Ѿ�������
	p = extlistGet(id);
	if(p && ZHU_EXTLOAD_NULL != p->loadMod){
		//������ô���
		p->refCount++;

		Zhu_bringExtToTop1(p->id);	//�ö�

		return MR_SUCCESS;
	}

	buf = malloc(64);
	len = 64;

	p = malloc(sizeof(EXT_DATA));
	memset(p, 0, sizeof(EXT_DATA));
	p->addres = buf;
	p->len = len;
	p->id = id;

	p->handel = mrc_extLoad(buf, len, 
		entryfun->mrc_init_f, 
		entryfun->mrc_event_f, 
		entryfun->mrc_pause_f, 
		entryfun->mrc_resume_f, 
		entryfun->mrc_exitApp_f);

	//��ӵ�����
	extlistAdd(p);
	//������ô���
	p->refCount++;
	p->loadMod = ZHU_EXTLOAD_NORMAL;	//����ģʽ
	if(mem)
		p->flage |= F_EXT_CACHELOAD;	//�û��ڴ��־

	mrc_extInit(p->handel);

	return MR_SUCCESS;
}
#else
int32 Zhu_loadExtPathEx1( int32 id, 
					   const char *extname, const char *mrpPath ,
					   PZHU_MEMINFO mem
					   )
{
	uint8* buf = NULL;
	int32 len = 0;
	PEXT_DATA p;
	int32 ret;

	//����ǲ����Ѿ�������
	p = extlistGet(id);
	if(p && ZHU_EXTLOAD_NULL != p->loadMod){
		//FPRINTF("Zhu_loadExtPath1: id %d hasload!", id);

		//������ô���
		p->refCount++;

		Zhu_bringExtToTop1(p->id);	//�ö�

		return MR_SUCCESS;
	}

	if(mem)
		buf = mem->addr, len = mem->len;
	ret = mrc_readFileFromMrpExA((PSTR)mrpPath, extname, &buf, &len, 3);
	if(ret != MR_SUCCESS || !buf){
		if(buf && !mem)
			mrc_freeFileData((void*)buf, len);
		ZHU_LOG("Zhu_loadExtPath1: mrc_readFileFromMrpExA fail!");
		return MR_FAILED;
	}

	p = malloc(sizeof(EXT_DATA));
	memset(p, 0, sizeof(EXT_DATA));
	p->addres = buf;
	p->len = len;
	p->id = id;
#ifdef SDK_MOD
	p->handel = mrc_extLoad(buf, len, 
		entryfun->mrc_init_f, 
		entryfun->mrc_event_f, 
		entryfun->mrc_pause_f, 
		entryfun->mrc_resume_f, 
		entryfun->mrc_exitApp_f);
#else
	p->handel = mrc_extLoad(buf, len);
#endif

	//��ӵ�����
	extlistAdd(p);
	//������ô���
	p->refCount++;
	p->loadMod = ZHU_EXTLOAD_NORMAL;	//����ģʽ
	if(mem)
		p->flage |= F_EXT_CACHELOAD;	//�û��ڴ��־

	mrc_extInit(p->handel);
	mrc_extSetMode(p->handel, 1);

	return MR_SUCCESS;
}
#endif

static PEXT_DATA gunloadext;	//���ӳ�ж�ص�EXT�ڵ�
static void unloadExtCb(int32 data){
	if(gunloadext && data>0){
		mrc_extFree(gunloadext->handel);	//ע���ͷŵ��� handel���ȵ��ã�
		if(!(gunloadext->flage|F_EXT_CACHELOAD))
			mrc_freeFileData(gunloadext->addres, gunloadext->len);//�ͷ��ڴ�
		extlistRemove(gunloadext);

		gunloadext = NULL;
		mrc_timerDelete(data);
	}
}

int32 Zhu_unloadExt1(int32 id, int32 delay){
	PEXT_DATA p = extlistGet(id);
	
	if(!p) return MR_FAILED;

	//���ô�������
	p->refCount--;

	if(p->refCount == 0) 
	{	
		if(delay){
			int32 t = mrc_timerCreate();

			gunloadext = p;
			mrc_timerStart(t, 1000, t, unloadExtCb, 0);
		}else{
			mrc_extFree(p->handel);	//ע���ͷŵ��� handel���ȵ��ã�
			if(!(p->flage|F_EXT_CACHELOAD))
				mrc_freeFileData(p->addres, p->len);//�ͷ��ڴ�
			extlistRemove(p);
			gunloadext = NULL;
		}		
	}else{//���������ط������˸ò�������β���ж��
		return MR_IGNORE;
	}

	return MR_SUCCESS;
}

int32 Zhu_resumeExt1(int32 id){
	PEXT_DATA p = extlistGet(id);

	if(!p) return MR_FAILED;

	_CLR_FLAG(p, F_EXT_PAUSE);
	return mrc_extResume(p->handel);
}

int32 Zhu_pauseExt1(int32 id){
	PEXT_DATA p = extlistGet(id);

	if(!p) return MR_FAILED;

	_SET_FLAG(p, F_EXT_PAUSE);
	return mrc_extPause(p->handel);
}

//ע�ắ��
void Zhu_regExtFun1(int32 id, ExtFun fun){
	PEXT_DATA p = extlistGet(id);
	if(p)
		p->fun = fun;
}

//ˢ����Ļ ����
void Zhu_refScn1(int32 x, int32 y, int w, int32 h){
	mrc_refreshScreen((int16)x, (int16)y, (uint16)w, (uint16)h);
}
//ˢ����Ļ ����
void Zhu_refScnRect1(mr_screenRectSt *r){
	mrc_refreshScreen(r->x, r->y, r->w, r->h);
}
//ˢ����Ļ ȫ��
void Zhu_refScnFull1(void){
	mrc_refreshScreen(0,0,(uint16)scnW, (uint16)scnH);
}

//���ò��ģʽ
void Zhu_setExtMod1(int32 id, ZHU_EXTMOD mod){
	PEXT_DATA p = extlistGet(id);
	if(p)
		mrc_extSetMode(p->handel, mod);
}

//���ø�EXT���ǽ��ܰ����¼�
void Zhu_setExtAlwaysHandelEvents1(int32 id, int always){
	PEXT_DATA p = extlistGet(id);
	if(p){
		if(always) _SET_FLAG(p, F_EXT_EVENTK);
		else _CLR_FLAG(p, F_EXT_EVENTK);
	}
}

void Zhu_blockExt1(int32  id){
	PEXT_DATA p = extlistGet(id);
	if(p){
		mrc_extBlock(p->handel);
		_SET_FLAG(p, F_EXT_BLOCK);
	}
}

void Zhu_unBlockExt1(int32 id){
	PEXT_DATA p = extlistGet(id);
	if(p){
		mrc_extUnblock(p->handel);
		_CLR_FLAG(p, F_EXT_BLOCK);
	}
}

int32 Zhu_getExtHandel1(int32 id){
	PEXT_DATA p = extlistGet(id);
	if(p)
		return p->handel;
	return -1;
}

int32 Zhu_getExtId1(int32 handel){
	PEXT_DATA p = extList.pHead;
	while(p){
		if(p->handel == handel)
			return p->id;
	}
	return -1;
}

//�ͷ� MRP��mrpName�������ļ���fileName������toDir���ļ��У��Ƿ��ѹ��unGZip��
//dstFileName ��ΪNULL ��ָ���ļ���������Ϊԭ�ļ��� fileName
int32 releaseFileFromMrp(PSTR mrpName, PCSTR fileName, PCSTR toDir, PCSTR dstFileName, BOOL unGZip){
	int32 len = 0, ret = MR_FAILED;
	PBYTE buf = NULL;

	if(unGZip){
		ret = mrc_readFileFromMrpExA(mrpName, fileName, &buf, &len, 0);
	}else{
		return MR_FAILED;	//δʵ�֡�����
	}

	if(buf){
		char name[128] = {0};
		int32 fd = 0;

		sprintf(name, "%s/%s", toDir, dstFileName? dstFileName : fileName);
		fd = mrc_open(name, MR_FILE_CREATE|MR_FILE_RDWR);
		if(fd){
			mrc_write(fd, buf, (uint32)len);
			mrc_close(fd);
		}
		mrc_freeFileData(buf, len);
	}

	return MR_SUCCESS;
}

//����Mythroad��Ŀ¼
int32 SetMthdRoot( const char* strDisk ) {
	uint8 *output = NULL;
	int32 output_len = 0;

#ifdef WIN32
	return MR_FAILED;
#endif

	return mrc_platEx(1204, (uint8*)strDisk, mrc_strlen(strDisk), &output, &output_len, NULL);
}

//////// ��ܺ����� //////////////////////////////////////////////////////////////////
int32 mrc_init(void)
{
	//�����û��SAVEVM
	//if(MR_IS_FILE == mrc_fileState(SAVEVMFILE)){
	//	mrc_loadVM();
	//	return MR_SUCCESS;
	//}

	mrc_extInitMainModule();
	mrc_extSetMode(mrc_extHandle, 0);

	//��Ļ��Ϣ��ʼ��
	{
		mr_screeninfo info;
		mrc_getScreenInfo(&info);
		scnW = info.width, scnH = info.height;
	}
	timer = mrc_timerCreate();
	longPressKey = -1;

	//ext���������ʼ��
	{
		memset(&extList, 0, sizeof(extList));
	}

	//������ģ��
	loadMainext();

	return MR_SUCCESS;
}

//�������ص�
static void Zhu_longKeyPress(int32 key)
{
	switch(key)
	{
		case MR_KEY_SOFTRIGHT:	//���������������ext�����˵����ر�/��С����
			{
				longPressKey = (int8)key;
				break;
			}		
	}
}

int32 mrc_event(int32 code, int32 param0, int32 param1){
	//���س�������ĵ����¼�
	if(longPressKey!=-1 && code == MR_KEY_RELEASE){
		longPressKey = -1;
		return MR_SUCCESS;
	}

	//��ext���¼�
	if(extList.count > 0){
		PEXT_DATA p = extList.pHead;

		while(p) {
			if(!_HAS_FLAG(p, F_EXT_PAUSE) //��EXTû����ͣ
				&& MR_SUCCESS == mrc_extEvent(p->handel, code, param0, param1)) {
				//if(code == MR_KEY_PRESS)	//��¼�ĸ�EXT�����˰����¼�
				//	_SET_FLAG(p, F_EXT_KEYPRESS);
				break;
			}
			p = p->next;
		}
		p = p->next;

		//����ʣ��
		while(p){
			if(_HAS_FLAG(p, F_EXT_EVENTK))	//���ǽ����¼�
				mrc_extEvent(p->handel, code, param0, param1);
			p = p->next;
		}
	}

	switch(code)
	{
	case MR_KEY_PRESS:
		{
			//������������ʱ��
			mrc_timerStart(timer, LONGKEYPRESS_TIME, param0, Zhu_longKeyPress, 0);			

			break;
		}

	case MR_KEY_RELEASE:
		//�������ж�
		{
			longPressKey = -1;
			mrc_timerStop(timer);
			break;
		}
	}

	return MR_SUCCESS;
}

//�����Ϣ ����ֵ������Ϊ��ģ��mrc_extSendAppEvent�����ķ���ֵ
int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1){
	return MR_SUCCESS;
}

//�����Ϣ��չ ����ֵ������Ϊ��ģ��mrc_extSendAppEvent�����ķ���ֵ
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5){
	int32 extid = code;
	ZHU_FUNID funid = p0;

	FPRINTF("��ģ���յ���Ϣ extid=%d funid=%d p1=%d p2=%d p3=%d p4=%d p5=%d", 
		code, p0, p1, p2, p3, p4, p5);

	if(extid == ZHU_EXTID_CFUNCTION)
	{
		switch(funid)
		{
//---- ����˹����ģ�� ------------------------------------------------
		case APS_MAIN_CODE_LOAD: case APS_MAIN_CODE_LOAD_VM:
		case APS_MAIN_CODE_RESET_LOAD: case APS_MAIN_CODE_DUMP_LOAD:
			{
				char buf[128] = {0};

				if(p2 && p3) {
					sprintf(buf, "%s/%s", (PSTR)p2, (PSTR)p2);
				}
#ifdef SDK_MOD
					Zhu_loadExtPathEx1(p1, (PCSTR)p4, NULL, NULL, (mrcEnterFuncSt*)p5);
#else
					Zhu_loadExtPathEx1(p1, (PCSTR)p4, NULL, (PZHU_MEMINFO)p5);
#endif
			}
			break;


//---- ����˹����ģ�� ------------------------------------------------

		case ZHU_FUNID_EXIT:
			mrc_exit();
			break;

			//һ��Ҫע�⣬��ģ��id����ģ��id��extid��������ģ���id
		case ZHU_FUNID_GET_SCNSIZE:
			Zhu_getScnSize1((int*)p1, (int*)p2);
			return MR_SUCCESS;

#ifdef SDK_MOD
		case ZHU_FUNID_LOADEXT:
			return Zhu_loadExtPathEx1(p1, (PCSTR)p2, (PCSTR)p3, (PZHU_MEMINFO)p4, (mrcEnterFuncSt*)p5);		
#else
		case ZHU_FUNID_LOADEXT:
			return Zhu_loadExtPathEx1(p1, (PCSTR)p2, (PCSTR)p3, (PZHU_MEMINFO)p4);	//ע��P1
#endif

		case ZHU_FUNID_REGFUN:
		case APS_MAIN_CODE_REG_ENTRY:
			Zhu_regExtFun1(p1, (ExtFun)p2);
			return MR_SUCCESS;

		case ZHU_FUNID_REFSCN:
			Zhu_refScn1(p1, p2, p3, p4);
			return MR_SUCCESS;

		case ZHU_FUNID_REFSCN_RECT:
			Zhu_refScnRect1((mr_screenRectSt*)p1);
			return MR_SUCCESS;

		case ZHU_FUNID_REFSCN_FULL:
			Zhu_refScnFull1();
			return MR_SUCCESS;

		case ZHU_FUNID_GET_EXTLOADMOD: 
		case APS_MAIN_CODE_QUERY_LOAD_MODE:
			return Zhu_getExtLoadMod1(p1);

		case ZHU_FUNID_BRINGEXTTOTOP:
			return Zhu_bringExtToTop1(p1);

		case ZHU_FUNID_UNLOADEXT:
		case APS_MAIN_CODE_UNLOAD: 
		case APS_MAIN_CODE_DUMP_UNLOAD:	//DUMP
			return Zhu_unloadExt1(p1, p2);

		case ZHU_FUNID_EXTPAUSE:
			return Zhu_pauseExt1(p1);

		case ZHU_FUNID_EXTRESUME:
			return Zhu_resumeExt1(p1);

		case ZHU_FUNID_SAVEVM:
			return mrc_saveVM(SAVEVMFILE);
		case ZHU_FUNID_LOADVM:
			return mrc_loadVM();

		case ZHU_FUNID_SETMOD:
			Zhu_setExtMod1(p1, (ZHU_EXTMOD)p2);
			break;

		case ZHU_FUNID_SETEXTALWAYSHANDELEVENTS:
			Zhu_setExtAlwaysHandelEvents1(p1, (int)p2);
			break;

		case ZHU_FUNID_BLOCKEXT:
		case APS_MAIN_CODE_BLOCK:
			Zhu_blockExt1(p1);
			break;

		case ZHU_FUNID_UNBLOCKEXT:
		case APS_MAIN_CODE_UNBLOCK:
			Zhu_unBlockExt1(p1);
			break;

		case ZHU_FUNID_GETHANDEL:
			return Zhu_getExtHandel1(p1);

		case ZHU_FUNID_GETID:
			return Zhu_getExtId1(p1);

		case ZHU_FUNID_GETMRPINFO:
		case APS_MAIN_CODE_GETMRPINFO:
			return mrc_getAppInfoEx((PCSTR)p1, (int32*)p2, (int32*)p3, (char*)p4);

		case ZHU_FUNID_RETRIEVEFILEFROMMRP:
		case APS_MAIN_CODE_RETRIEVEFILEFROMMRP:
			return releaseFileFromMrp((PSTR)p1, (PCSTR)p2, (PSTR)p3, (PCSTR)p4, TRUE);

		case ZHU_FUNID_SWITCH_CDR:
		case APS_MAIN_CODE_SWITCH_CDR:
			return SetMthdRoot((PCSTR)p1);

		case ZHU_FUNID_RESTORE_CDR:
		case APS_MAIN_CODE_RESTORE_CDR:
			return SetMthdRoot("Z");

		case ZHU_FUNID_TRANS_KEY_EVENT:
		case APS_MAIN_CODE_TRANS_KEY_EVENT:
			{
				PEXT_DATA p = extlistGet(extid);

				if(p) 
					return mrc_extEvent(p1, p2, p3, p4);

				break;
			}
		}
	}else{
		PEXT_DATA p = extlistGet(extid);

		FPRINTF("��ģ��ת����Ϣ extid=%d funid=%d p1=%d p2=%d p3=%d p4=%d p5=%d", 
			code, p0, p1, p2, p3, p4, p5);

		if(!p) {
			ZHU_LOG("extlistGet null!");
			return MR_FAILED;
		}

		if(p->fun){
			return mrc_extMpsFpCall(p->handel, p->fun, p0, p1, p2, p3, p4, p5);
			//p->fun(p0, p1, p2, p3, p4, p5); //ֱ�ӵ�������ַ��������
			//return MR_SUCCESS;
		}
	}

	return MR_SUCCESS;
}

int32 mrc_pause(void){
	PEXT_DATA p = extList.pHead;
	
	while(p){
		mrc_extPause(p->handel);
		p = p->next;
	}

	return MR_SUCCESS;
}

int32 mrc_resume(void){
	PEXT_DATA p = extList.pHead;
	
	while(p){
		if(!_HAS_FLAG(p, F_EXT_PAUSE))
			mrc_extResume(p->handel);
		p = p->next;
	}

	return MR_SUCCESS;
}

int32 mrc_exitApp(void){

	
	PEXT_DATA p = extList.pHead, p1;
	
	while(p){
		mrc_extFree(p->handel);	//ע���ͷŵ��� handel���ȵ��ã�
		if(! _HAS_FLAG(p, F_EXT_CACHELOAD))
			mrc_freeFileData(p->addres, p->len);//�ͷ��ڴ�
		p1 = p->next;
		mrc_free(p);
		p = p1;
	}
	
	//FPRINTF("��ģ���˳�");


	return MR_SUCCESS;
}

//�����û�����ģ��
static void loadMainext(void){
#ifdef	SDK_MOD
	{
		mrcEnterFuncSt enterFunc;

		enterFunc.mrc_init_f = mrc_mainextInit;
		enterFunc.mrc_event_f = mrc_mainextEvent;
		enterFunc.mrc_pause_f = mrc_mainextPause;
		enterFunc.mrc_resume_f = mrc_mainextResume;
		enterFunc.mrc_exitApp_f = mrc_mainextExit;	

		Zhu_loadExtPathEx1(ZHU_EXTID_MAIN, MAINEXT_NAME, NULL, NULL, &enterFunc);	
	}
#else
	Zhu_loadExtPathEx1(ZHU_EXTID_MAIN, MAINEXT_NAME, NULL, NULL);
#endif
}