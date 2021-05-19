/*
 * aec_api.c
 *
 *  Created on: 2014/2/13
 *      Author: mtk04880
 */
#include <linux/init.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
#include <linux/sched.h>
#endif
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include "aec_api.h"
#include "aec_inc/aec_util.h"
#include <sound/soc/mtk/mtk_audio_device.h>
#if defined(CONFIG_RALINK_I2S)
#include "../i2s_ctrl.h"
#endif


/**********************************
 *
 * Macro Definition
 *
 * ********************************
 */
#define AEC_API_MOD_VERSION "0.2"
#define AEC_TASK_NAME "aecTask"
#define AEC_CH_NUM		(1)
#define SLEEP_20MS (HZ/50)
#define MAX_CMD_STR_LEN 32

/* default value */
#define PCM_SAMPLE_SIZE (80) /*units: bytes*/
#define SAMPLE_BYTES (2) /* 2 bytes/sample = 16 bits*/
#define EC_SAMPLE_NUM (160) /*EC request the buffer size is 2*80sample */
#define EC_BUF_SIZE (EC_SAMPLE_NUM*SAMPLE_BYTES) /*EC request the buffer size is 2*80sample */
#define RTD_OFFSET (320) /*units : bytes*/ //jf_voip_130524  can set be no more than 320, max 320 (test results)

#define FEQ_SIZE (I2S_PAGE_SIZE*3) /* unit: bytes */
#define NEQ_SIZE (I2S_PAGE_SIZE*3) /* unit: bytes */
#define ECQ_SIZE (I2S_PAGE_SIZE*3) /* unit: bytes */
#define EC_TAIL_LENGTH_16MS (61)
#define EC_TAIL_LENGTH_32MS (65597)
#define EC_TAIL_LENGTH_48MS (131101) // 131101
#define EC_TAIL_LENGTH_128MS (196669)

/*BLI setting*/
#define DEF_STREAM_SR (44100)
#define EC_SR	  	  (8000)



/**********************************
 *
 * Data Structure
 *
 * ********************************
 */
typedef struct {
	int  enh_par[28];
	short aec_tail_length; //jf_voip_130524
	short aec_wait_ms_for_tail; //jf_voip_130524
	void *sys_ptr;
} aec_ctrl_struct;

typedef enum dbgPrint{
	DBGPRINT_NONE = 0,
	DBGPRINT_MSG,
	DBGPRINT_ERR,
	DBGPRINT_WARN,
	DBGPRINT_INFO,
	DBGPRINT_MAX
}dbgPrint_e;

typedef struct _aecCtrlBlk_s{
	aec_ctrl_struct aec_ctrl;
	int* aecCtrl_p;
	short* feBuf;
	short neBuf[160];
	/* debug count */
	unsigned long ulcount;
	unsigned long dlCount;
	unsigned long resynCount;
	/* enable runtime */
	int enable;
	/*BLI parameters*/
	/*BLI Handle*/
	BLI_HANDLE*		bli_handle;
	/*Steam SR*/
	int streamSR;
	char* bli_cod_work_buf;
	int bli_buf_size;
} _aecCtrlBlk_t;

typedef struct syncQ_s {
	int cid;
	unsigned int feqInIdx; /*cycq_in*/
	unsigned int feqOutIdx;/*cycq_out*/
	unsigned int neqInIdx;/*cycq_ecqIn*/
	unsigned int neqOutIdx;/*cycq_ecqOut*/
	unsigned int ecInIdx;/*cycq_ecqIn2*/
	unsigned int ecOutIdx;/*ecIdx*/
	unsigned int runECTimer;/*resync_count*/
	unsigned int syncChked;/*ecStart2*/
	unsigned int ecStartInd;/*resync_count*/
	char feQueue[FEQ_SIZE];
	char neQueue[NEQ_SIZE];
	char ecQueue[ECQ_SIZE];
} syncQ_t;

typedef struct procCmds_s
{
	char cmd[20];
	read_proc_t *read_proc;
	write_proc_t *write_proc;
}procCmd_t;



/**********************************
 *
 * Function Definition
 *
 * ********************************
 */
#define aecLog(level, fmt,val...) {if(level <= curDbgLv){printk("[%s:L %d]", __func__, __LINE__);printk(fmt, ##val);}}
#define OS_MALLOC(x)					kmalloc(x, GFP_KERNEL)
#define OS_MALLOC_ATOMIC(x)			kmalloc(x, GFP_ATOMIC)
#define OS_MFREE(x)					kfree(x)

/*Internal Function */
static int aecReinit(_aecCtrlBlk_t *aecCtrlBlk_p,int cid);
static int aecInit(void);
static int aecSyncObjInit(int cid);
static int aecEcTailLenSet(int cid, int ectail_len);
static int aecEctailLenRead(char *buffer, char **start, off_t offset, int length, int *eof, void *data);
static int aecEctailLenWrite(struct file *file, const char *buffer, unsigned long count, void *data);
static int aecECTailLenGet(int cid);

/*External Function for modules calling AEC*/
int AECQueryConfig(aecCtrlBlk_t* aecCtrlBlk_p);
int AECReInit(aecCtrlBlk_t* aecCtrlBlk_p);
int AECFeEnq(int cid,void* buf,int size);
int AECECDeq(int cid,void* buf,int size);
int AECNeEnq(int cid,void* buf,int size);

/*AEC / BLI function embedded in lib*/
extern int  ENH_API_Get_Memory(aec_ctrl_struct *aec_ctrl);
extern int  ENH_API_Alloc(aec_ctrl_struct *aec_ctrl, int *aec_mem_ptr);
extern int  ENH_API_Init_AEC(aec_ctrl_struct *aec_ctrl);
extern int  ENH_API_Free_AEC(aec_ctrl_struct *aec_ctrl);
// Operation Functions
extern void ENH_API_Run_Aec_UL(aec_ctrl_struct *aec_ctrl, short *ne_sp , short *fe_sp);
extern void ENH_API_Run_Aec_DL(aec_ctrl_struct *aec_ctrl, short *fe_sp);

/**********************************
 *
 * Global Variable
 *
 * ********************************
 */

static int curDbgLv = DBGPRINT_MAX;
static int streamSR = DEF_STREAM_SR;
#if CONFIG_PROC_FS
struct proc_dir_entry *aec_dir=0;
struct proc_dir_entry *temp_dir=0;
#endif

static struct task_struct * aec_task;
static aecCtrlBlk_t g_aecCtrlBlk[AEC_CH_NUM];
static _aecCtrlBlk_t _g_aecCtrlBlk[AEC_CH_NUM];
static syncQ_t sync_qObj[AEC_CH_NUM];
//TODO: static unsigned short memPool[AEC_CH_NUM][];



static procCmd_t procAECCmds[] =
{
	{	"ectail_len", aecEctailLenRead, aecEctailLenWrite},
	{	"0",0, 0},
};

static const aecCtrlBlk_t defAecCtrlBlk = {
		.cid = 0,
		.ecTailLen = EC_TAIL_LENGTH_48MS,
		.ecEnable = 1,
		.streamSR = DEF_STREAM_SR,
		.reserved = 0,
};
extern aecFuncTbl_t *aecFuncP;

/**********************************
 *
 * Function Declaration
 *
 * ********************************
 */
#if CONFIG_PROC_FS
static char *aecProcGetNextCommand(char *pCmdStr, char **pCmd)
{
	while((*pCmdStr != '\0') && (*pCmdStr == ' '))
	pCmdStr++;

	if(*pCmdStr != '\0')
	{
		*pCmd = pCmdStr;

		while((*pCmdStr != '\0') && (*pCmdStr != ' '))
		pCmdStr++;

		if(*pCmdStr == ' ')
		{
			*pCmdStr = '\0';
			pCmdStr++;
		}
	}
	else
	{
		*pCmd = 0;
	}

	return pCmdStr;
}

static int aec_atoi(const char *num_str)
{
	int val = 0;

	while(1)
	{
		switch(*num_str)
		{
			case '0'...'9':
			val = (10 * val) + (*num_str - '0');
			break;

			default:
			return val;
		}

		num_str++;
	}
}

static int aecEctailLenWrite(struct file *file, const char *buffer, unsigned long count, void *data)
{
	/**
	 * input
	 * @param cid
	 * @param ectail_len
	 */
	char cmdStr[MAX_CMD_STR_LEN + 1];
	char *pCmdStr, *pcid, *pectail_len;
	int strLen, cid, ectail_len;

	strLen = (count > MAX_CMD_STR_LEN) ? MAX_CMD_STR_LEN : (count - 1);

	copy_from_user(cmdStr, buffer, strLen);
	cmdStr[strLen] = 0;

	pCmdStr = &cmdStr[0];

	pCmdStr = aecProcGetNextCommand(pCmdStr, &pcid);
	if(pcid==0)
	{
		aecLog(DBGPRINT_MSG, "Usage: echo \"<cid> <ectail_len>\" > /proc/aec/ectail_len\r\n");
		aecLog(DBGPRINT_MSG, "ectail_len:\n");
		//DBGLOG1(M_aec, LOGT_INFO, "\taec_24MS = %d\n", EC_TAIL_LENGTH_24MS);
		aecLog(DBGPRINT_MSG, "\taec_16MS = %d\n", EC_TAIL_LENGTH_16MS);
		aecLog(DBGPRINT_MSG, "\taec_32MS = %d\n", EC_TAIL_LENGTH_32MS);
		aecLog(DBGPRINT_MSG, "\taec_48MS = %d\n", EC_TAIL_LENGTH_48MS);
		aecLog(DBGPRINT_MSG, "\taec_128MS = %d\n", EC_TAIL_LENGTH_128MS);
		return count;
	}
	cid = aec_atoi(pcid);

	pCmdStr = aecProcGetNextCommand(pCmdStr, &pectail_len);
	if(pectail_len==0)
	{
		aecLog(DBGPRINT_MSG, "Usage: echo \"<cid> <ectail_len>\" > /proc/aec/ectail_len\r\n");
		aecLog(DBGPRINT_MSG, "ectail_len:\n");
		//DBGLOG1(M_aec, LOGT_INFO, "\taec_24MS = %d\n", EC_TAIL_LENGTH_24MS);
		aecLog(DBGPRINT_MSG, "\taec_16MS = %d\n", EC_TAIL_LENGTH_16MS);
		aecLog(DBGPRINT_MSG, "\taec_32MS = %d\n", EC_TAIL_LENGTH_32MS);
		aecLog(DBGPRINT_MSG, "\taec_48MS = %d\n", EC_TAIL_LENGTH_48MS);
		aecLog(DBGPRINT_MSG, "\taec_128MS = %d\n", EC_TAIL_LENGTH_128MS);

		return count;
	}
	ectail_len = aec_atoi(pectail_len);

	aecEcTailLenSet(cid, ectail_len);

	aecLog(DBGPRINT_MSG, "cid = %d ecTail_len = %d ms\r\n", cid, aecECTailLenGet(cid));

	return (count);
}

static int aecEctailLenRead(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
	/**
	 * input
	 * @param cid
	 */
	int i;
	int ectail_len=0;
	char *p = buffer;

	for(i=0;i<AEC_CH_NUM;i++)
	{
		ectail_len = aecECTailLenGet(i);
		aecLog(DBGPRINT_MSG,  "cid = %d ecTail_len = %d ms\r\n", i, ectail_len);
	}

	if ((p - buffer) <= offset)
	{
		*eof = 1;
		return (0);
	}
	return (p - buffer);
}
#endif

static int aecFuncPReg(aecFuncTbl_t *o){
	if(o){
		o->AECQueryConfig = AECQueryConfig;
		o->AECReInit = AECReInit;
		o->AECFeEnq = AECFeEnq;
		o->AECECDeq = AECECDeq;
		o->AECNeEnq = AECNeEnq;
	}
	else{
		aecLog(DBGPRINT_ERR,"AEC Func Pointer NULL\n");
		return AEC_FAIL;
	}
}
static int aecECTailLenGet(int cid) {
	int tLen = 0;
	#if 1 //jf_voip_130524
	switch (_g_aecCtrlBlk[cid].aec_ctrl.enh_par[1]) {
	case EC_TAIL_LENGTH_16MS:
		tLen = 128;
		break;
	case EC_TAIL_LENGTH_32MS:
		tLen = 256;
		break;
	case EC_TAIL_LENGTH_48MS:
		tLen = 384;
		break;
	case EC_TAIL_LENGTH_128MS:
		tLen = 1024;
		break;
	default:
		break;
	}
	#else  //jf_voip_130524

	switch ((aecCtrlBlk[cid].aec_ctrl.enh_par[1]>>16) & 0x07) {  //jf_voip_130524
		case EC_TAIL_LENGTH_24MS:
			tLen = 24;
			break;
		case EC_TAIL_LENGTH_36MS:
			tLen = 36;
			break;
		case EC_TAIL_LENGTH_48MS:
			tLen = 48;
			break;
		case EC_TAIL_LENGTH_60MS:
			tLen = 60;
			break;
		case EC_TAIL_LENGTH_72MS:
			tLen = 72;
			break;
		case EC_TAIL_LENGTH_96MS:
			tLen = 96;
			break;
		case EC_TAIL_LENGTH_120MS:
			tLen = 120;
			break;

		default:
			break;
		}

	#endif //jf_voip_130524

	return tLen;
}

static int aecEcTailLenSet(int cid, int ectail_len) {
	int tLen = 0;
	switch (ectail_len) {

	case LEC_16MS: {
		tLen = EC_TAIL_LENGTH_16MS;
		break;
	}
	case LEC_32MS: {
		tLen = EC_TAIL_LENGTH_32MS;
		break;
	}
	case LEC_48MS: {
		tLen = EC_TAIL_LENGTH_48MS;
		break;
	}
	case LEC_128MS: {
		tLen = EC_TAIL_LENGTH_128MS;
		break;
	}
	default: {
		aecLog(DBGPRINT_ERR, "un-support tail length = %d\r\n",ectail_len);
		return AEC_FAIL;
	}
	}

	_g_aecCtrlBlk[cid].aec_ctrl.enh_par[1] = tLen;  //jf_voip_130524

	return AEC_SUCCESS;
}

/*EC queue get called by I2S*/
static int aec_ecQGet(int cid, short *buf, int size) {
	short *ptr;
	int i, bufIdx;
	printk("======%s======\n",__func__);
	bufIdx = sync_qObj[cid].ecOutIdx;
	ptr = (short*) &sync_qObj[cid].ecQueue[bufIdx];

	for (i = 0; i < size/sizeof(short); i++) {
		ptr = (short*) &sync_qObj[cid].ecQueue[bufIdx];
		buf[i] = *ptr;
		bufIdx = (bufIdx + 2) % ECQ_SIZE;
	}
	sync_qObj[cid].ecOutIdx = bufIdx;

	return AEC_SUCCESS;
}

/*EC queue put called by AEC*/
static int aecECQPut(int cid, short* buf, int size) {
	short *ptr;
	int i, bufIdx ,bli_buf_size ,outputSize;
	short *bliConvertBuf;
	i = bufIdx = bli_buf_size = 0;
	outputSize = size*_g_aecCtrlBlk[i].streamSR/EC_SR;
	printk("======%s======\n",__func__);
	bliConvertBuf = OS_MALLOC(I2S_PAGE_SIZE);

	if(bliConvertBuf){
		/*BLI will be called to do downsampling*/
		/* ask working memory size */
		if(_g_aecCtrlBlk[i].bli_cod_work_buf)
			OS_MFREE(_g_aecCtrlBlk[i].bli_cod_work_buf);
		BLI_GetMemSize(EC_SR , 2,_g_aecCtrlBlk[i].streamSR , 1, &bli_buf_size);
		_g_aecCtrlBlk[i].bli_buf_size = bli_buf_size;
		_g_aecCtrlBlk[i].bli_cod_work_buf = (unsigned char *)OS_MALLOC(bli_buf_size);
		memset(_g_aecCtrlBlk[i].bli_cod_work_buf, 0, bli_buf_size);
		_g_aecCtrlBlk[i].bli_handle = \
				BLI_Open(EC_SR , 1,_g_aecCtrlBlk[i].streamSR, 2, (char*)_g_aecCtrlBlk[i].bli_cod_work_buf);
		BLI_Convert(_g_aecCtrlBlk[cid].bli_handle, (short *)buf, &size, bliConvertBuf, &outputSize);
		printk("%s:out size:%d \n",__func__,outputSize);
	}
	else{
		return AEC_FAIL;
	}

	bufIdx = sync_qObj[cid].ecInIdx;
	ptr = (short*) &sync_qObj[cid].ecQueue[bufIdx];
	for (i = 0; i < outputSize/sizeof(short); i++) {
		//printk("(%x) bufidx:%d\n",(unsigned int)ptr,bufIdx);
		ptr = (short*) &sync_qObj[cid].ecQueue[bufIdx];
		*ptr = (short) bliConvertBuf[i];
		bufIdx = (bufIdx + 2) % ECQ_SIZE;
	}
	OS_MFREE(bliConvertBuf);
	printk("<<<<<<<<%s>>>>\n",__func__);
	sync_qObj[cid].ecInIdx = (sync_qObj[cid].ecInIdx+outputSize)%ECQ_SIZE;
	return AEC_SUCCESS;
}

/*NE queue put called by I2S*/
char tmpBuff[I2S_PAGE_SIZE];
static int aecNeQPut(int cid, short *buf, int size) {
	short *ptr;
	int i, bufIdx,bli_buf_size,outputSize;
	short *bliConvertBuf;
	i = bufIdx = bli_buf_size = 0;
	printk("======%s======\n",__func__);
	outputSize = size*EC_SR/_g_aecCtrlBlk[i].streamSR;
	bliConvertBuf = (short *)tmpBuff;//kmalloc(I2S_PAGE_SIZE,GFP_KERNEL);

	if(bliConvertBuf){
		/*BLI will be called to do downsampling*/
		/* ask working memory size */
		if(_g_aecCtrlBlk[i].bli_cod_work_buf)
			OS_MFREE(_g_aecCtrlBlk[i].bli_cod_work_buf);
		BLI_GetMemSize(_g_aecCtrlBlk[i].streamSR , 2,EC_SR , 1, &bli_buf_size);
		_g_aecCtrlBlk[i].bli_buf_size = bli_buf_size;
		_g_aecCtrlBlk[i].bli_cod_work_buf = (unsigned char *)OS_MALLOC(bli_buf_size);
		memset(_g_aecCtrlBlk[i].bli_cod_work_buf, 0, bli_buf_size);
		_g_aecCtrlBlk[i].bli_handle = \
				BLI_Open( _g_aecCtrlBlk[i].streamSR, 2,EC_SR, 1, (char*)_g_aecCtrlBlk[i].bli_cod_work_buf);
		BLI_Convert(_g_aecCtrlBlk[cid].bli_handle, (short *)buf, &size, bliConvertBuf, &outputSize);
	}
	else{
		return AEC_FAIL;
	}

	bufIdx = sync_qObj[cid].neqInIdx;
	printk("%s:bufidx:%d out size:%d\n",__func__,bufIdx,outputSize);
	for (i = 0; i < outputSize/sizeof(short); i++) {
		ptr = (short*) &sync_qObj[cid].neQueue[bufIdx];
		//printk("%s ptr:%x bufidx:%d\n",__func__,(unsigned int)ptr,bufIdx);
		*ptr = (short) bliConvertBuf[i];
		bufIdx = (bufIdx + 2) % NEQ_SIZE;
	}
	//OS_MFREE(bliConvertBuf);
	//sync_qObj[cid].neqInIdx = (sync_qObj[cid].neqInIdx + size * 2) % NEQ_SIZE;
	sync_qObj[cid].neqInIdx = (sync_qObj[cid].neqInIdx + outputSize) % NEQ_SIZE;  //jf_voip
	printk("<<<<<<<<%s>>>>\n",__func__);
	return AEC_SUCCESS;
}

/*NE queue get called by AEC*/
static int aecNeQGet(int cid, short* buf, int size) {
	short *ptr;
	int i, bufIdx;
	i = bufIdx = 0;
	/*mtk04880: for recording the EC write back idx*/
	//sync_qObj[cid].ecInIdx = sync_qObj[cid].neqOutIdx;
	printk("======%s -======\n",__func__);
	bufIdx = sync_qObj[cid].neqOutIdx;
	for (i = 0; i < (size/sizeof(short));i++){
		ptr = (short*) &sync_qObj[cid].neQueue[bufIdx];
		buf[i] = *ptr;
		bufIdx = (bufIdx + 2) % NEQ_SIZE;
	}
	printk("<<<<<<%s>>>>>>\n",__func__);
	//*buf = ptr;
	//sync_qObj[cid].neqOutIdx = (sync_qObj[cid].neqOutIdx + size*2)%SYNDLY_BUF_SIZE;
	sync_qObj[cid].neqOutIdx = bufIdx;

	return AEC_SUCCESS;
}

/*FE queue put called by I2S*/
static int aecFeQPut(int cid, short *buf, int size) {
	short *ptr = NULL;
	int i, bufIdx, outputSize,bli_buf_size;
	short *bliConvertBuf = NULL;
	i = bufIdx = bli_buf_size= 0;
	outputSize = size*EC_SR/_g_aecCtrlBlk[i].streamSR;
	printk("======%s======\n",__func__);
	bliConvertBuf = kmalloc(I2S_PAGE_SIZE,GFP_KERNEL);

	if(bliConvertBuf){
		/*BLI will be called to do downsampling*/
		if(_g_aecCtrlBlk[i].bli_cod_work_buf){
			OS_MFREE(_g_aecCtrlBlk[i].bli_cod_work_buf);
		}
		BLI_GetMemSize(_g_aecCtrlBlk[i].streamSR , 2,EC_SR, 2, &bli_buf_size);
		_g_aecCtrlBlk[i].bli_buf_size = bli_buf_size;
		/* ask working memory size */
		_g_aecCtrlBlk[i].bli_cod_work_buf = (unsigned char *)OS_MALLOC(bli_buf_size);
		memset(_g_aecCtrlBlk[i].bli_cod_work_buf, 0, bli_buf_size);
		_g_aecCtrlBlk[i].bli_handle = \
				BLI_Open(_g_aecCtrlBlk[i].streamSR, 2,EC_SR, 1, (char*)_g_aecCtrlBlk[i].bli_cod_work_buf);
		BLI_Convert(_g_aecCtrlBlk[cid].bli_handle, (short *)buf, &size, bliConvertBuf, &outputSize);
		bufIdx = sync_qObj[cid].feqInIdx;
		//printk("%s:out size:%d bufidx:%d\n",__func__,outputSize,bufIdx);
		ptr = (short*) &sync_qObj[cid].feQueue[bufIdx];

		for (i = 0; i < outputSize/sizeof(short); i++) {

			//printk("(%x) bufidx:%d\n",(unsigned int)ptr,bufIdx);
			ptr = (short*) &sync_qObj[cid].feQueue[bufIdx];
			*ptr = (short)bliConvertBuf[i];
			bufIdx = (bufIdx + 2) % FEQ_SIZE;

			//ptr[i] = 0x0;//far end mute
		}
		//printk("2%s:out size:%d\n",__func__,outputSize);
		OS_MFREE(bliConvertBuf);
		printk("<<<<<<<<%s>>>>\n",__func__);
		//sync_qObj[cid].feqInIdx = (sync_qObj[cid].feqInIdx + size * 2) % FEQ_SIZE;
		sync_qObj[cid].feqInIdx = (sync_qObj[cid].feqInIdx + outputSize) % FEQ_SIZE; //jf_voip

		return AEC_SUCCESS;
	}
	else{
		return AEC_FAIL;
	}
}

/*FE queue get called by AEC*/
static int aec_feQGet(int cid, short **buf, int size) {

	short *ptr;
	int bufIdx;
	printk("======%s======\n",__func__);
	bufIdx = sync_qObj[cid].feqOutIdx;
	ptr = (short*) &sync_qObj[cid].feQueue[bufIdx];

	*buf = (short*) ptr;
	//sync_qObj[cid].feqOutIdx = (sync_qObj[cid].feqOutIdx + size * 2) % FEQ_SIZE;
	sync_qObj[cid].feqOutIdx = (sync_qObj[cid].feqOutIdx + size) % FEQ_SIZE;
	printk("<<<<<<<<%s>>>>\n",__func__);
	return AEC_SUCCESS;
}

static int aecReinit(_aecCtrlBlk_t *_aecCtrlBlk_p,int cid) {
	int aec_mem_size = 0;
	int bli_buf_size = 0;
	if (_aecCtrlBlk_p && _g_aecCtrlBlk[cid].aecCtrl_p) {
		/*Free AEC by previous Setting*/
		ENH_API_Free_AEC(&(_g_aecCtrlBlk[cid].aec_ctrl));

		/*Store new setting */
		memcpy(&_g_aecCtrlBlk[cid],_aecCtrlBlk_p,sizeof(_aecCtrlBlk_t));
		/* Get Memoery Requirement for AEC */
		aec_mem_size = ENH_API_Get_Memory(&(_aecCtrlBlk_p->aec_ctrl));
		//_aecCtrlBlk[cid].aecCtrl_p = (int*) OS_MALLOC(aec_mem_size);
		if (!_g_aecCtrlBlk[cid].aecCtrl_p) {
			aecLog(DBGPRINT_ERR,"%s init fail \n", __func__);
			_g_aecCtrlBlk[cid].enable = 0;
			return AEC_FAIL;
		}
		memset(_g_aecCtrlBlk[cid].aecCtrl_p, 0, aec_mem_size); // note the plat-form dependent users
		// Linking AEC Memory //
		ENH_API_Alloc(&(_g_aecCtrlBlk[cid].aec_ctrl),
				_g_aecCtrlBlk[cid].aecCtrl_p);

		// Initialize AEC //
		ENH_API_Init_AEC(&(_g_aecCtrlBlk[cid].aec_ctrl));
		/*Reset FE & NE queue*/
		aecSyncObjInit(cid);
		/* ask working memory size */
		if(_g_aecCtrlBlk[cid].bli_cod_work_buf)
			OS_MFREE(_g_aecCtrlBlk[cid].bli_cod_work_buf);
		BLI_GetMemSize( EC_SR, 2, DEF_STREAM_SR, 2, &bli_buf_size);
		_g_aecCtrlBlk[cid].bli_buf_size = bli_buf_size;
		_g_aecCtrlBlk[cid].bli_cod_work_buf = (unsigned char *)OS_MALLOC(bli_buf_size);
		memset(_g_aecCtrlBlk[cid].bli_cod_work_buf, 0, bli_buf_size);
		_g_aecCtrlBlk[cid].bli_handle = \
				BLI_Open(EC_SR, 1, DEF_STREAM_SR, 2, (char*)_g_aecCtrlBlk[cid].bli_cod_work_buf);
	}
	aecLog(DBGPRINT_INFO, "%s: Reinit success \n", __func__);
	return AEC_SUCCESS;
}

static int aecInternCtrlBlkSet(aecCtrlBlk_t* aecCtrlBlk_p,_aecCtrlBlk_t* _aecCtrlBlk_p) {
	unsigned int ecTailLen = 0;
	if((!_aecCtrlBlk_p) || (aecCtrlBlk_p))
		return AEC_FAIL;
	if(aecCtrlBlk_p->cid > AEC_CH_NUM)
		return AEC_FAIL;

	switch(aecCtrlBlk_p->ecTailLen){
	case LEC_16MS:
		ecTailLen = EC_TAIL_LENGTH_16MS;
		break;
	case LEC_32MS:
		ecTailLen = EC_TAIL_LENGTH_32MS;
		break;
	case LEC_48MS:
		ecTailLen = EC_TAIL_LENGTH_48MS;
		break;
	case LEC_128MS:
		ecTailLen = EC_TAIL_LENGTH_128MS;
		break;
	default:
		aecLog(DBGPRINT_ERR,"unsupported ecTailLen:%d \n",aecCtrlBlk_p->ecTailLen);
		return AEC_FAIL;
	}
	_aecCtrlBlk_p->aec_ctrl.enh_par[1]= ecTailLen;
	_aecCtrlBlk_p->enable =  aecCtrlBlk_p->ecEnable;
	_aecCtrlBlk_p->streamSR = aecCtrlBlk_p->streamSR;
	_aecCtrlBlk_p->aec_ctrl.enh_par[0] = 144;
	_aecCtrlBlk_p->aec_ctrl.enh_par[2] = 6212;
	_aecCtrlBlk_p->aec_ctrl.enh_par[11] = 8200;
	return AEC_SUCCESS;
}

static int aecInit(void) {
	int i, aecMemSz;
	unsigned int bli_buf_size = 0;
	i = aecMemSz = 0;

	for (i = 0; i < AEC_CH_NUM; i++) {
#if 0
		_aecCtrlBlk[i].aec_ctrl.enh_par[0] = OSBN_DEFAULT_ENH0_SET; //jf_voip_130524s
		_aecCtrlBlk[i].aec_ctrl.enh_par[1] = OSBN_DEFAULT_ENH1_SET; //jf_voip_130524
		_aecCtrlBlk[i].aec_ctrl.enh_par[2] = OSBN_DEFAULT_ENH2_SET; //jf_voip_130524
		_aecCtrlBlk[i].aec_ctrl.enh_par[12] = OSBN_DEFAULT_ENH12_SET; //change from 11 to 12  //jf_voip_130524
#else
		_g_aecCtrlBlk[i].aec_ctrl.enh_par[0] = 64;
		_g_aecCtrlBlk[i].aec_ctrl.enh_par[1]=  EC_TAIL_LENGTH_48MS;
		_g_aecCtrlBlk[i].aec_ctrl.enh_par[2] = 6212;
		_g_aecCtrlBlk[i].aec_ctrl.enh_par[11] = 8200;
		_g_aecCtrlBlk[0].streamSR = streamSR;
		/*Load default setting for external ctrl block*/
		memcpy(&g_aecCtrlBlk[i],&defAecCtrlBlk,sizeof(aecCtrlBlk_t));
#endif
		// Get Memoery Requirement for AEC //
		aecMemSz = ENH_API_Get_Memory(&(_g_aecCtrlBlk[i].aec_ctrl));
		// Memory Allocation and Reset by OS //
		_g_aecCtrlBlk[i].aecCtrl_p = (int*) OS_MALLOC(aecMemSz);
		if (!_g_aecCtrlBlk[i].aecCtrl_p) {
			aecLog(DBGPRINT_ERR,"init fail \n");
			_g_aecCtrlBlk[i].enable = 0;
			return AEC_FAIL;
		}
		_g_aecCtrlBlk[i].enable = 1;
		memset(_g_aecCtrlBlk[i].aecCtrl_p, 0, aecMemSz); // note the plat-form dependent users

		// Linking AEC Memory //
		ENH_API_Alloc(&(_g_aecCtrlBlk[i].aec_ctrl), _g_aecCtrlBlk[i].aecCtrl_p);

		// Initialize AEC //
		ENH_API_Init_AEC(&(_g_aecCtrlBlk[i].aec_ctrl));
		/* initial the cyclic queue for the delay buffer */
		aecSyncObjInit(i);
		/*BLI Init*/
		/* ask working memory size */
		if(_g_aecCtrlBlk[i].bli_cod_work_buf)
			OS_MFREE(_g_aecCtrlBlk[i].bli_cod_work_buf);
		BLI_GetMemSize( EC_SR, 1, DEF_STREAM_SR, 2, &bli_buf_size);
		_g_aecCtrlBlk[i].bli_buf_size = bli_buf_size;
		_g_aecCtrlBlk[i].bli_cod_work_buf = (unsigned char *)OS_MALLOC(bli_buf_size);
		memset(_g_aecCtrlBlk[i].bli_cod_work_buf, 0, bli_buf_size);
		_g_aecCtrlBlk[i].bli_handle = \
				BLI_Open( EC_SR, 1, DEF_STREAM_SR, 2, (char*)_g_aecCtrlBlk[i].bli_cod_work_buf);

	}
	/*For Stream parameter could be loaded as AEC module loading.*/

	return AEC_SUCCESS;
}

static int aecSyncObjInit(int cid) {
	memset(&(sync_qObj[cid]), 0, sizeof(syncQ_t));
	sync_qObj[cid].neqOutIdx = RTD_OFFSET;
	sync_qObj[cid].ecOutIdx = RTD_OFFSET;
	return AEC_SUCCESS;
}

static int syncCheckFunc(int cid) {
	if ((sync_qObj[cid].feqInIdx - sync_qObj[cid].feqOutIdx) >= RTD_OFFSET) {
		//printk("first (%d)--cycq in:%d out:%d ecq in :%d out:%d \n",cid,sync_qObj[cid].feqInIdx,sync_qObj[cid].feqOutIdx,sync_qObj[cid].neqInIdx,sync_qObj[cid].neqOutIdx);
		sync_qObj[cid].syncChked = 1;
	}
	return 0;
}

short tmpdata[80];
static int aecHandlTask(void *data){
        int i = 0;
        int timeout;
        static int oneShot = 0;
        wait_queue_head_t timeout_wq;
        aecLog(DBGPRINT_INFO,"thread_func %d started \n", i);

        aecFuncPReg(aecFuncP);
       // init_waitqueue_head(&timeout_wq);
        while(!kthread_should_stop()){
        	if(!oneShot){

        	}
        	for(i = 0;i< AEC_CH_NUM;i++){
				if (!sync_qObj[i].syncChked){
					syncCheckFunc(i);
					memset(tmpdata, 0, 160);
				}
				else{
					//printk("%d.(%d)--cycq in:%d out:%d ecq in :%d out:%d\n",resync_count[cid],cid,sync_qObj[cid].feqInIdx,sync_qObj[cid].feqOutIdx,sync_qObj[cid].neqInIdx,sync_qObj[cid].neqOutIdx);
					aec_feQGet(i, &(_g_aecCtrlBlk[i].feBuf), EC_BUF_SIZE);
					//aecNeQGet(i, _g_aecCtrlBlk[i].neBuf, EC_BUF_SIZE);
#ifdef NETLOG_DEBUG
					ecLoggerSend(i,FE_LOGGER,_aecCtrlBlk[i].feBuf,320);
					ecLoggerSend(i,NE_LOGGER,_aecCtrlBlk[i].neBuf,320);
#endif
					ENH_API_Run_Aec_UL(&(_g_aecCtrlBlk[i].aec_ctrl),\
							_g_aecCtrlBlk[i].neBuf, _g_aecCtrlBlk[i].feBuf);
#ifdef NETLOG_DEBUG
					ecLoggerSend(i,NEO_LOGGER,_g_aecCtrlBlk[i].neBuf,320);
#endif
					aecECQPut(i, _g_aecCtrlBlk[i].neBuf, EC_BUF_SIZE);
					sync_qObj[i].ecStartInd = 1;
					sync_qObj[i].runECTimer = 0;

					if (!sync_qObj[i].ecStartInd){
						memset(tmpdata, 0, 160);
					}
					else{
						aec_ecQGet(i, tmpdata, EC_BUF_SIZE);
					}
				}
    			set_current_state(TASK_INTERRUPTIBLE);
    			schedule_timeout(SLEEP_20MS);
        	}


        }

        return AEC_SUCCESS;
}
/**********************************
 *
 * Exported Functions
 *
 * ********************************
 */
int AECQueryConfig(aecCtrlBlk_t* aecCtrlBlk_p){

	int cid = 0;
	if(!aecCtrlBlk_p)
		return AEC_FAIL;
	if(aecCtrlBlk_p->cid > AEC_CH_NUM)
		return AEC_FAIL;

	cid = aecCtrlBlk_p->cid;
	memcpy(aecCtrlBlk_p,&g_aecCtrlBlk[cid],sizeof(aecCtrlBlk_t));
	return AEC_SUCCESS;
}

int AECReInit(aecCtrlBlk_t* aecCtrlBlk_p){
	_aecCtrlBlk_t _aecCtrlBlk;
	int cid = 0;

	if(!aecCtrlBlk_p)
		return AEC_FAIL;
	if(aecCtrlBlk_p->cid > AEC_CH_NUM)
		return AEC_FAIL;

	memset((aecCtrlBlk_t* )&_aecCtrlBlk,NULL,sizeof(_aecCtrlBlk_t));
	cid = aecCtrlBlk_p->cid;
	aecInternCtrlBlkSet(aecCtrlBlk_p,&_aecCtrlBlk);
	memcpy(aecCtrlBlk_p,&g_aecCtrlBlk[cid],sizeof(aecCtrlBlk_t));
	return aecReinit(&_aecCtrlBlk,cid);
}

int AECFeEnq(int cid,void* buf,int size){
		return aecFeQPut(cid,buf,size);
}

int AECECDeq(int cid,void* buf,int size){
	return aec_ecQGet(cid,buf,size);
}

int AECNeEnq(int cid,void* buf,int size){
	//return aecNeQPut(cid,buf,size);
	return AEC_SUCCESS;
}

int __init aec_api_mod_init(void){
	int i;

	aecLog(DBGPRINT_MSG, "init AEC with bli, %s %s \n",__DATE__, __TIME__);
#ifndef CONFIG_PROC_FS
	aec_dir = proc_mkdir("aec", NULL);
	if(aec_dir == NULL){
		/* failed when creating file */
		aecLog(DBGPRINT_ERR, "Error while creating LEC Proc directory\n");
		return AEC_FAIL;
	}
	i=0;
	while(procAECCmds[i].cmd[0]!='0'){
		temp_dir = create_proc_read_entry(procAECCmds[i].cmd, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH, aec_dir, procAECCmds[i].read_proc, NULL);
		temp_dir->write_proc = procAECCmds[i].write_proc;
		i++;
	}
#endif

	/*Reset Internal Ctrl Blk*/
	memset(_g_aecCtrlBlk,NULL,sizeof(_g_aecCtrlBlk));
	/*Reset External Ctrl Blk*/
	memset(g_aecCtrlBlk,NULL,sizeof(g_aecCtrlBlk));

	MUTX_Init(0);
	/* initial the aec core */
	aecInit();
	/*Create AEC Task to Run EC Alg*/
	aec_task = kthread_run(aecHandlTask, NULL, AEC_TASK_NAME);
	if (!IS_ERR(aec_task)){
		aecLog(DBGPRINT_INFO, "kthread_create done \n");
	}
	else{
		aecLog(DBGPRINT_ERR, "kthread_create error \n");
	}

	return AEC_SUCCESS;
}

void aec_api_mod_exit(void){

}

module_init(aec_api_mod_init);
module_exit(aec_api_mod_exit);
module_param(streamSR, int, S_IRUGO);

MODULE_DESCRIPTION("MTK SoC AEC Module");
MODULE_AUTHOR("mtk04880");
MODULE_VERSION(AEC_API_MOD_VERSION);
MODULE_LICENSE("GPL");
