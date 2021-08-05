
#include <pspthreadman.h>
#include <pspsysmem.h>

PSP_MODULE_INFO("nkThread", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define MAX_THREAD	64

typedef struct THREAD{
	int count;
	SceUID pThread;
} THREAD;

int count_Start;
SceUID Thread_Start[MAX_THREAD];
THREAD bufNow;

/***************************************

モジュールスタート

****************************************/
int module_start (SceSize args, void *argp)
{

	count_Start = 0;

	//開始時のスレッド一覧の取得
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, Thread_Start, MAX_THREAD, &count_Start);

	bufNow.count = 0;
	bufNow.pThread = 0;

	return 0;
}


/***************************************

スレッドを一時停止

****************************************/
int nkThreadSuspend(SceUID thId){

	int i, j;
	SceUID myThread, *Thread_Now;

	if(bufNow.pThread != 0) return 1;

	bufNow.pThread = sceKernelAllocPartitionMemory(1, "th", 0, MAX_THREAD*sizeof(SceUID), NULL);
	if(bufNow.pThread < 0){
		bufNow.count = 0;
		bufNow.pThread = 0;
		return 1;
	}
	Thread_Now = (SceUID*)sceKernelGetBlockHeadAddr(bufNow.pThread);

	//現在のスレッド一覧の取得
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, Thread_Now, MAX_THREAD, &(bufNow.count));
	myThread = sceKernelGetThreadId();

	for(i = 0; i < bufNow.count; i++){
		unsigned char match = 0;
		SceUID tmp_thid = Thread_Now[i];
		
		for(j = 0; j < count_Start; j++){
			if((tmp_thid == Thread_Start[j]) || (tmp_thid == thId) || (tmp_thid == myThread)){
				match = 1;
				j = count_Start;
			}
		}
	//	if(count_Start == 0) match = 1;
		if(match == 0){
			sceKernelSuspendThread(tmp_thid);
		}
	}

	return 0;
}

/***************************************

スレッドを再開

****************************************/
int nkThreadResume(SceUID thId){

	int i, j;
	SceUID myThread, *Thread_Now;

	if(bufNow.pThread == 0) return 1;

	Thread_Now = (SceUID*)sceKernelGetBlockHeadAddr(bufNow.pThread);
	myThread = sceKernelGetThreadId();
	

	for(i = 0; i < bufNow.count; i++){
		unsigned char match = 0;
		SceUID tmp_thid = Thread_Now[i];
		
		for(j = 0; j < count_Start; j++){
			if((tmp_thid == Thread_Start[j]) || (tmp_thid == thId) || (tmp_thid == myThread)){
				match = 1;
				j = count_Start;
			}
		}
	//	if(bufcount_Start == 0) match = 1;
		if(match == 0){
			sceKernelResumeThread(tmp_thid);
		}
	}
	
	sceKernelFreePartitionMemory(bufNow.pThread);
	bufNow.count = 0;
	bufNow.pThread = 0;

	return 0;
}

/***************************************

モジュールストップ

****************************************/
int module_stop (SceSize args, void *argp)
{
//	sceKernelFreePartitionMemory(bufNow.pThread);

	return 0;
}
