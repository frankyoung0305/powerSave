#include"nfv.h"
#include <stdlib.h> 
#include <stdio.h>
#include "../posix/ndpi_api.h" //iphdr
#include <string.h> 
#include <pcap.h>

int main() {
	/*initialization about mqueue*/
	char proname[] = "process_3";

	struct mq_attr attr, attr_ctrl;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_p1top3;
	int mq_return = 0;
	char p1top3[]="/l3top3";

	mqd_p1top3 = mq_open(p1top3, flags, PERM, &attr);
	check_return(mqd_p1top3, proname, "mq_open");



	/*control part*/
	mqd_t mqd_ctrltop3, mqd_p3toctrl;
	char ctrltop3[] = "/ctrltop3";
	char p3toctrl[] = "/p3toctrl";
	mqd_ctrltop3 = mq_open(ctrltop3, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_ctrltop3, proname, "mq_open");
	mqd_p3toctrl = mq_open(p3toctrl, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_p3toctrl, proname, "mq_open");


	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltop3;
	noti_tran.mqd_ptoc = mqd_p3toctrl;
	noti_tran.mqd_p[0] = mqd_p1top3;
	noti_tran.qds = 1;
	notifysetup(&noti_tran);







	char buffer[2048];
	struct ndpi_iphdr * iph;
	long long int i = 0;



	int port = 0;

	for(i = 0;i < PACKETS*10;i++) {//PACKETS = 5000 now.
		mq_return = mq_receive(mqd_p1top3, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
		}

		iph = (struct ndpi_iphdr *) buffer;

		if((i%100 == 0) || (i < 400)) {
			printf("i = %lld, iph->daddr = %8X, packet_length = %d \n", i, iph->daddr, mq_return);
		}

	}
	printf("i = %lld, iph->daddr = %8X, packet_length = %d \n", i, iph->daddr, mq_return);
	printf("%s has transfered %lld packets. \n", proname, i);

	mq_return = mq_close(mqd_p1top3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p1top3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");


	mq_return = mq_close(mqd_ctrltop3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctrltop3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	mq_return = mq_close(mqd_p3toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p3toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");




	exit(0);

}

