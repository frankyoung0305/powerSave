#include"queue.h"


int main(void) {
	char proname[] = "send";
	struct timeval start;
	struct timeval end;
	setcpu(SEND_CPU);
	printf("now in %s \n", proname);


	int pktsize = sizeof(struct datamsg);
	struct mq_attr attr;
	attr.mq_maxmsg = 1000;
	attr.mq_msgsize = pktsize;
	attr.mq_flags = 0;
	int flags = O_CREAT | O_RDWR;
	
	mqd_t mqd_sendtoreceive;
	int mq_return = 0;
	char sendtoreceive[] = "/sendtoreceive";
	mqd_sendtoreceive = mq_open(sendtoreceive, flags, PERM, &attr);
	check_return(mqd_sendtoreceive, proname, "mq_open /sendtoreceive");
	
	struct datamsg buffer;
	gettimeofday(&start, 0);
	long long int i = 0;
	for(i = 0;i < PACKETS;i++) {
		buffer.i = i;
		gettimeofday(&(buffer.time), 0);
		mq_return = mq_send(mqd_sendtoreceive, (char *) &buffer, pktsize, 0);
		if(mq_return == -1) {
			printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			return -1;
		}
		if(i % SHOW_FREQUENCY == 0) {
			printf("send has sent %lld packets \n", i);
		}
		/*if(i % USLEEP_FREQUENCY == 0) {
			usleep(USLEEP_TIME);
		}*/
		usleep(10);
	
	}
	gettimeofday(&end, 0);
	
	usleep(100000);
	printf("send has sent %lld packets\n", i);
	printf("send has gone through %ld us \n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));
	mq_return = mq_close(mqd_sendtoreceive);
	check_return(mq_return, proname, "mq_close sendtoreceive");
	mq_return = mq_unlink(sendtoreceive);
	check_return(mq_return, proname, "mq_unlink sendtoreceive");
	
	return 0;
}
