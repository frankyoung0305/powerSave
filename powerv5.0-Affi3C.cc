#include <unistd.h>  
#include <stdio.h>
#include <sched.h>  // cpu affinity
#include <sys/msg.h> //key_t,ftok,msgget,msgsnd,IPC_CREAT 等相关声明都在这里面定义
#include <string.h> //strncmp,strcmp,strlen 的函数声明在这个头文件里
#define BUFSZ 2048
#define MAXLEN 1500  //最大caplen
#define REPS 10000000  //发包数

#include "pcap.h" //pcapreader

#include <sys/time.h> //time stamp

#include <errno.h> //errno

#include<malloc.h> //malloc

#include<stdlib.h>  //call shell cmd in c

cpu_set_t  mask;

int assignToThisCore(int core_id)
{
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);
    return 0;
}//  set cpu affinity

int timevalSubtract(struct timeval* result, struct timeval* x, struct timeval* y)
{
    int nsec;
    
    if ( x->tv_sec>y->tv_sec )
        return -1;
    
    if ( (x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec) )
        return -1;
    
    result->tv_sec = ( y->tv_sec-x->tv_sec );
    result->tv_usec = ( y->tv_usec-x->tv_usec );
    
    if (result->tv_usec<0)
    {
        result->tv_sec--;
        result->tv_usec+=1000000;
    }
    
    return 0;
} //计算时间差

int StrCopyEx (void* pstFrom ,void* pstTo)
{
	if (pstFrom == NULL || pstTo == NULL)
	return -1 ;
	unsigned char* pstF = (unsigned char*) pstFrom ;
	unsigned char* pstT = (unsigned char*) pstTo ;
	while (*pstT++ = *pstF++) ;
	return 0 ;
}//unsigned char 数组copy (写数据包)	
typedef struct datamessage //此结构体用于存放消息，从中可以看到消息的两个字段
{
    int serv; //服务链标号，以整型值进行标示
    bpf_u_int32 caplen; //pcap pkt length
    //struct timeval timestamp;
    u_char pktdata[MAXLEN]; //pkt内容
}DMSG; //取了一个别名

typedef struct ctlmessage //此结构体用于存放消息，从中可以看到消息的两个字段
{
    int serv; //服务类型，以整型值进行标示  0:请求 1:应答 2:命令
    int cpu; //设置cpu亲和
    //struct timeval timestamp;
    long unsigned int queuelen; //队列长度
}CMSG; //取了一个别名

int initQueue(int* qid, key_t* key, int arg){
		int res = -1;
		if(-1 == (*key=ftok("/",arg))) // 通过ftok获取一个key，两个进程可以通过这个key来获取队列信息 fname = "/", keep fname 
	    {
		perror("ftok");
		return res;
	    }

	    if(-1==(*qid=msgget(*key,IPC_CREAT|0600))) //创建一个消息队列1，将id存到qid中 
	    {
		perror("msgget");
		return res;
	    }
	  
	    printf("open queue :%d\n",*qid); //将qid进行显示
		return 0;
}

int sndDMsg(int qid, DMSG* dmsg, bpf_u_int32 caplen){
	int res = -1;
	if (0 > msgsnd(qid,dmsg,caplen,0)) //发送信息 to queue A 
	{
		perror("msgsnd");
		return res;
	}
	//printf("A");
	return 0;
	
}

int delQueue(int qid){
	int res = -1;
	if( 0 > msgctl(qid,IPC_RMID,NULL) ) //将队列删除，如果不删除，在进程退出后，消息将依旧保留在内核中，直到重启系统，消息的持久性，界于进程与磁盘之间
	{
	   perror("msgctl");
	   return res;
	}
}

int getPkt(pcap_t* p,struct pcap_pkthdr** pkt_header,const u_char** pkt_data){
	if(pcap_next_ex(p, pkt_header, pkt_data) != 1){
		pcap_close(p);
		char errbuf[100];  //error buf for pcapReader
		p = pcap_open_offline("/tmp/traffic_sample.pcap", errbuf);  //head
		pcap_next_ex(p, pkt_header, pkt_data);
	}
	return 0;
}  //获取下一个数据包

int getQueueStat(int msgqid, struct msqid_ds* statbuf){
	//printf("run msgqid\n");

	msgctl(msgqid, IPC_STAT, statbuf);
	//printf("errno = %d\n", errno); // 
	//printf("Done.\n");

}  //update status buffer

int anaQueueStat(int msgqid, struct msqid_ds* statbuf, float* maxpct){
	long unsigned int queuelen = statbuf->msg_qnum;//现有包个数
	long unsigned int queuebytes =  statbuf->msg_cbytes;//现有字节数
	long unsigned int maxbytes =  statbuf->msg_qbytes;//最大字节数
	//printf("got.\n");
	float pct = ((float)queuebytes / (float)maxbytes)* 100;//占比

	if(pct > *maxpct){
		*maxpct = pct;
		printf("maxpct of queue %d : %f\n", msgqid, *maxpct);
		//printf("maxqlen of queue %d : %ld\n", msgqid, maxbytes);
	}
	//printf("Stat of queue %d : Len: %lu , Bytes: %lu, MaxBytes: %lu, Percentage: %f \n", msgqid, queuelen, queuebytes, maxbytes, pct);
}  //analyse


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////main///////////////////////////////////////////////////////////
   
int main ()   
{   
	system("cp ~/Desktop/IPC/fork/traffic_sample.pcap /tmp/");   //转移到tmpfs

    pid_t fpid; //fpid表示fork函数返回的值 
    //printf("pid:  %4d \n",getpid());  //main function pid 
    //int count=0;  
    fpid=fork();   
    if (fpid < 0)   
        printf("error in fork controller");   
    else if (fpid == 0) {  //----------------------------------------------------controller 
		
    }  
    else {                                               //----------------------------------------------收包进程 父进程 fpid ！= 0
		fpid=fork();       //---------------fork workers and producer
        if (fpid < 0)   
        	printf("error in fork producer");
		else if (fpid == 0) {    //------------------子进程处理 producer
			int core = 0;									//-------------------------------发包进程 子进程 fpid == 0
			assignToThisCore(core); 					//----------- affinity-------------------分配到0号逻辑核
		    printf("process pid:  %4d forwarding packets, running on core %d. \n",getpid(), core);  //print 


			int res=0,qidA=0, qidB=0;
			key_t keyA=IPC_PRIVATE; //IPC_PRIVATE 就是0   key for queue 1
			key_t keyB=IPC_PRIVATE; //IPC_PRIVATE 就是0   key for queue 2
			DMSG dmsg;
			struct msqid_ds* statbufA =(struct msqid_ds *)malloc(sizeof(struct msqid_ds)); //queue stat for queue 1
			struct msqid_ds* statbufB =(struct msqid_ds *)malloc(sizeof(struct msqid_ds)); //queue stat for queue 2
			float maxpctA, maxpctB = 0;


			long long int len=0; //赋初值是一个好习惯 (in byte
			int counter=0;    
			struct timeval startTime, stopTime,diff;

			char errbuf[100];  //error buf for pcapReader

			pcap_t *pfile = pcap_open_offline("/tmp/traffic_sample.pcap", errbuf);  //head
			if (NULL == pfile) {
			printf("%s\n", errbuf);
			return -1;
			} 
			//printf("file opened\n");

			struct pcap_pkthdr *pkthdr = 0;
			const u_char *pktdata = 0;
		  
			initQueue(&qidA,&keyA,18);  //-----------------init queue 1
			initQueue(&qidB,&keyB,19);  //-----------------init queue 2

			gettimeofday( &startTime, NULL);  //start time
			//printf("time stamped\n");
			for(int i = 0; i<REPS; i++ )

			{
				getPkt(pfile, &pkthdr, &pktdata);  //获取下一个数据包
				//if(NULL == (fgets(msg.msg_text,BUFSZ,stdin))) //从标准输入中获取信息放到 msg.msg_text 中
				//{
				//    perror("fgets");
				//    return res;
				//}
				//pcap_next_ex(pfile, &pkthdr, &pktdata);  //read
				//printf("pkt got, write:\n");
				//for (bpf_u_int32 i = 0; i < pkthdr->caplen; ++i) {
				//    msg.pktdata[i] = pktdata[i];
					//}
					//for (bpf_u_int32 i = 0; i < pkthdr->caplen; ++i) {
				//	if (0 < i && 0 == i % 16) printf("\n");
				//	printf("%2x ", pktdata[i]);
					//}
				//msg.pktdata = (char *)pktdata;
				StrCopyEx((void*)pktdata, dmsg.pktdata);  //copy u_char
				//printf("wrote\n");
				//msg.msg_type=getpid(); //将消息的类型设置为本进程的ID
				dmsg.caplen = pkthdr->caplen;  //caplen
			   
				len += pkthdr->caplen; //accumulate
			
				if (counter % 2 == 0){ //------偶数包发往queue A
					sndDMsg(qidA, &dmsg, pkthdr->caplen);
				}

				else{
					sndDMsg(qidB, &dmsg, pkthdr->caplen);
				}
				counter++;
				//printf("send %d pkt. length:%d\n", counter, pkthdr->caplen);
				if (counter%100==0){
				//printf("getting queue stat...\n");
					getQueueStat(qidA, statbufA);
					anaQueueStat(qidA, statbufA, &maxpctA);				
					getQueueStat(qidB, statbufB);
					anaQueueStat(qidB, statbufB, &maxpctB);
				}

			}
			gettimeofday( &stopTime, NULL);  //start time 


			   
			res=0;
			pcap_close(pfile);
			printf("Done.\n");
			timevalSubtract(&diff,&startTime,&stopTime);
			double totalLen = (double)len / 1024 / 1024;
			double bandWidth = ((double)len / (diff.tv_sec * 1000000 + diff.tv_usec)) / 1024 / 1024 *1000000;
			double pps = ((double)counter/(diff.tv_sec * 1000000 + diff.tv_usec)) * 1000000;
			printf("time-used:%ld s:%ld us\ntotal length: %lfMB\npks:%d\npps:%lf\nbandwidth:%lf Mbyte/s ", diff.tv_sec, diff.tv_usec, totalLen, counter, pps, bandWidth);
		

			delQueue(qidA); //release queue A
			delQueue(qidB); //release queue B
			return res;			
		}
		
		else {    //fork workers 在父进程
			fpid=fork();       //---------------fork出两个worker
		    if (fpid < 0)   
		    	printf("error in fork workers");   
			else if (fpid == 0) {    //------------------work A 处理 queue 1
				int core = 1;                                //--------affinity----------------------分配到1号逻辑核
				assignToThisCore(core);
				printf("process pid:  %4d receiving packets, running on core %d. \n",getpid(), core);  //print  
				int res=0,qidA=0;
				key_t keyA=IPC_PRIVATE;  //IPC_PRIVATE 就是0
				DMSG dmsg;

				int counter=0;
			  
				initQueue(&qidA,&keyA,18);  //-----------------queue 1
			  
				do
				{
					memset(dmsg.pktdata,0,BUFSZ); //将msg.pktdata的内容清零
					if(0 > msgrcv(qidA,&dmsg,BUFSZ,0,0)) //从消息队列中获取信息并且存到msg中
				{
					perror("msgrcv");
					return res;
				} 
				counter++;
		//    	for (bpf_u_int32 i = 0; i < msg.caplen; ++i) {
		//        	if (0 < i && 0 == i % 16) printf("\n");
		//        	printf("%2x ", msg.pktdata[i]);
		//    	}
				//printf("rcved the message from process %ld , index: %d\n",msg.msg_type,counter); //将信息内容在终端进行打印
				//if(counter % 100000 == 0 ){
				//    for(int i = 0; i<1000000000; i++){}  //delay
				//}
				//for(int i = 0; i<10000; i++){}  //delay
				}while(1); //
			  

				delQueue(qidA);  //release queue A

				res=0;
				return res;        
			}
			else {   //------------------work B 处理 queue 2
				int core = 2;
				assignToThisCore(core);              //--------affinity----------------------分配到2号逻辑核
				printf("process pid:  %4d receiving packets, running on core %d. \n",getpid(), core);  //print  
				int res=0,qidB=0;
				key_t keyB=IPC_PRIVATE;  //IPC_PRIVATE 就是0
				DMSG dmsg;

				int counter=0;
			  
				initQueue(&qidB,&keyB,19);  //-----------------queue 1
				do
				{
					memset(dmsg.pktdata,0,BUFSZ); //将msg.pktdata的内容清零
					if(0 > msgrcv(qidB,&dmsg,BUFSZ,0,0)) //从消息队列中获取信息并且存到msg中
				{
					perror("msgrcv");
					return res;
				} 
				counter++;
		//    	for (bpf_u_int32 i = 0; i < msg.caplen; ++i) {
		//        	if (0 < i && 0 == i % 16) printf("\n");
		//        	printf("%2x ", msg.pktdata[i]);
		//    	}
				//printf("rcved the message from process %ld , index: %d\n",msg.msg_type,counter); //将信息内容在终端进行打印
				//if(counter % 100000 == 0 ){
				//    for(int i = 0; i<1000000000; i++){}  //delay
				//}
				//for(int i = 0; i<10000; i++){}  //delay
				}while(1); //
			  

				delQueue(qidB); //release queue B
				res=0;
				return res;  
			}			
		}
  
    }
    return 0;  
}  
