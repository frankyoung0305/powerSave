#include"queue.h"


int main(void) {
	char proname[] = "fwd";
	setcpu(FWD_CPU);
	printf("now in %s \n", proname);


	int pktsize = sizeof(struct datamsg);
	struct mq_attr attr;
	attr.mq_maxmsg = 1000;
	attr.mq_msgsize = pktsize;
	attr.mq_flags = 0;
	int flags = O_CREAT | O_RDWR;
	
	mqd_t mqd_sendtofwd;
	mqd_t mqd_fwdtoreceive;
	int mq_return = 0;
	char sendtofwd[] = "/sendtofwd";
	char fwdtoreceive[] = "/fwdtoreceive";
	mqd_sendtofwd = mq_open(sendtofwd, flags, PERM, &attr);
	check_return(mqd_sendtofwd, proname, "mq_open /sendtofwd");
	mqd_fwdtoreceive = mq_open(fwdtoreceive, flags, PERM, &attr);
	check_return(mqd_fwdtoreceive, proname, "mq_open /fwdtoreceive");
	
	struct datamsg buffer;
	
	long long int i = 0;
	for(i = 0;i < PACKETS;i++) {
		mq_return = mq_receive(mqd_sendtofwd, (char *) &buffer, pktsize, 0);
		if(mq_return == -1) {
			printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			return -1;
		}
		mq_return = mq_send(mqd_fwdtoreceive, (char *) &buffer, pktsize, 0);
		if(mq_return == -1) {
			printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			return -1;
		}
		
		
		if(i % SHOW_FREQUENCY == 0) {
			printf("fwd has forwarded %lld packets \n", i);
		}
			
	}
	
	sleep(1);
	printf("fwd has forwarded %lld packets\n", i);
	mq_return = mq_close(mqd_sendtofwd);
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(sendtofwd);
	check_return(mq_return, proname, "mq_unlink");
	
	mq_return = mq_close(mqd_fwdtoreceive);
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(fwdtoreceive);
	check_return(mq_return, proname, "mq_unlink");
	
	return 0;
}
