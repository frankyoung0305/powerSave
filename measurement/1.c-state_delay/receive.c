#include"queue.h"


int main(void) {
	char proname[] = "receive";
	setcpu(RECEIVE_CPU);
	printf("now in %s \n", proname);


	int pktsize = sizeof(struct datamsg);
	struct mq_attr attr;
	attr.mq_maxmsg = 1000;
	attr.mq_msgsize = pktsize;
	attr.mq_flags = 0;
	int flags = O_CREAT | O_RDWR;
	
	mqd_t mqd_fwdtoreceive;
	int mq_return = 0;
	char fwdtoreceive[] = "/fwdtoreceive";
	mqd_fwdtoreceive = mq_open(fwdtoreceive, flags, PERM, &attr);
	check_return(mqd_fwdtoreceive, proname, "mq_open /fwdtoreceive");
	
	struct datamsg buffer;
	struct timeval end;
	
	long long int i = 0;
	long long int sum = 0;
	for(i = 0;i < PACKETS;i++) {
		mq_return = mq_receive(mqd_fwdtoreceive, (char *) &buffer, pktsize, 0);
		gettimeofday(&end, 0);
		if(mq_return == -1) {
			printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			return -1;
		}
		sum += ((end.tv_sec - buffer.time.tv_sec) * 1000000 + (end.tv_usec - buffer.time.tv_usec));
		
		
		
		if(i % SHOW_FREQUENCY == 0) {
			printf("receive has received %lld packets \n", i);
		}
			
	}
	
	sleep(1);
	sum = sum / PACKETS;
	printf("receive has received %lld packets\n", i);
	printf("average time is %lld us \n", sum);
	mq_return = mq_close(mqd_fwdtoreceive);
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(fwdtoreceive);
	check_return(mq_return, proname, "mq_unlink");
	
	return 0;
}
