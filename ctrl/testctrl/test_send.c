#include"../nfv.h"

int main() {
	/*initialization about mqueue*/
	char proname[] = "send_process";

	printf("test_send's pid is %d\n", getpid());


	struct mq_attr attr, *attrp;
	attrp = NULL;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;
	attrp = &attr;

	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_sdtop1;
	int mq_return = 0;
	char sdtop1[]="/sendtoreceive";

	mqd_sdtop1 = mq_open(sdtop1, flags, PERM, attrp);
	check_return(mqd_sdtop1, proname, "mq_open");


	mqd_t mqd_ctrltosd, mqd_sdtoctrl;
	char ctrltosd[] = "/controllertosend";
	char sdtoctrl[] = "/sendtocontroller";
	mqd_ctrltosd = mq_open(ctrltosd, flags_ctrl, PERM, attrp);
	check_return(mqd_ctrltosd, proname, "mq_open");
	mqd_sdtoctrl = mq_open(sdtoctrl, flags_ctrl, PERM, attrp);
	check_return(mqd_sdtoctrl, proname, "mq_open");


	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltosd;
	noti_tran.mqd_ptoc = mqd_sdtoctrl;
	noti_tran.mqd_p[0] = mqd_sdtop1;
	noti_tran.qds = 1;
	notifysetup(&noti_tran);





	struct datamsg sendbuf;
	sendbuf.a = 1;
	sendbuf.b = 2;
	sendbuf.c = 3;
	sendbuf.d = 4;
	strcpy(sendbuf.char_data, "data sent by process send.c:abcdefghijklmnopqrstuvwxyz");
	sendbuf.f1 = 1.000023;
	sendbuf.f2 = 4.000056;
	sendbuf.f3 = 7.000089;
	sendbuf.f4 = 0.000001;

	long long int i = 0;

	for(i = 0;i < PACKETS;i++) {
		mq_return = mq_send(mqd_sdtop1, (char *) &sendbuf, sizeof(struct datamsg), 0);
		if(mq_return == -1) {
			printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
		}
		if(i<400) printf("i < 400, i = %lld \n", i);
		if(i%100000 == 0) {
			printf("send has sent %lld packets \n", i);
			usleep(1000);
		}
	}
	printstar();
	printf("test_send has sent %lld packets and is closing.\n", i);
	mq_return = mq_close(mqd_sdtop1);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(sdtop1);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	printstar();
	checkcpu();

	func_quit(proname);
	exit(0);

}
