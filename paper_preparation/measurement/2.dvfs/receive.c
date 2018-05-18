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
	long long int time[PACKETS];
	
	for(i = 0;i < PACKETS;i++) {
		mq_return = mq_receive(mqd_fwdtoreceive, (char *) &buffer, pktsize, 0);
		gettimeofday(&end, 0);
		if(mq_return == -1) {
			printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			return -1;
		}
		#ifdef PRINTMODE
		printf("i = %lld time = %ld \n", i, (end.tv_sec - buffer.time.tv_sec) * 1000000 + (end.tv_usec - buffer.time.tv_usec));
		#endif
		time[i] = ((end.tv_sec - buffer.time.tv_sec) * 1000000 + (end.tv_usec - buffer.time.tv_usec));
		sum += time[i];
		
		
		
/*		if(i % SHOW_FREQUENCY == 0) {
			printf("receive has received %lld packets \n", i);
		}
*/			
	}
	
	usleep(100000);
	for(i = 0;i < PACKETS;i++) {
		printf("i = %lld, time = %lld \n", i, time[i]);
	}
	double average = 0;
	average = (double) sum / (double) PACKETS;
	printf("receive has received %lld packets\n", i);
	printf("PACKET INTERVAL is %dus\n", PACKET_INTERVAL);
	printf("average time is %f us \n", average);
	mq_return = mq_close(mqd_fwdtoreceive);
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(fwdtoreceive);
	check_return(mq_return, proname, "mq_unlink");
	
	return 0;
}
