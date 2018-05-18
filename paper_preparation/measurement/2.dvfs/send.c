#include"queue.h"


int main(void) {
	char proname[] = "send";
	//struct timeval start;
	setcpu(SEND_CPU);
	printf("now in %s \n", proname);


	int pktsize = sizeof(struct datamsg);
	struct mq_attr attr;
	attr.mq_maxmsg = 1000;
	attr.mq_msgsize = pktsize;
	attr.mq_flags = 0;
	int flags = O_CREAT | O_RDWR;
	
	mqd_t mqd_sendtofwd;
	int mq_return = 0;
	char sendtofwd[] = "/sendtofwd";
	mqd_sendtofwd = mq_open(sendtofwd, flags, PERM, &attr);
	check_return(mqd_sendtofwd, proname, "mq_open /sendtofwd");
	
	struct datamsg buffer;
	
	long long int i = 0;
	for(i = 0;i < PACKETS;i++) {
		buffer.i = i;
		gettimeofday(&(buffer.time), 0);
		mq_return = mq_send(mqd_sendtofwd, (char *) &buffer, pktsize, 0);
		if(mq_return == -1) {
			printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			return -1;
		}
		usleep(PACKET_INTERVAL);
		/*if(i % USLEEP_FREQUENCY == 0) {
                        usleep(USLEEP_TIME);
                }*/
                #ifdef PRINTMODE
		if(i % SHOW_FREQUENCY == 0) {
			printf("send has sent %lld packets \n", i);
		}
		#endif
	}
	
	sleep(1);
	printf("send has sent %lld packets\n", i);
	mq_return = mq_close(mqd_sendtofwd);
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(sendtofwd);
	check_return(mq_return, proname, "mq_unlink");
	
	return 0;
}
