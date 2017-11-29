#include"nfv.h"
#include "../posix/ndpi_api.h" //iphdr
#include <pcap.h>

#include "fan.h"

int main() {
	/*initialization about mqueue*/
	char proname[] = "p5_nDPI";
	setcpu(P5_STARTING_CPU);

	struct mq_attr attr, attr_ctrl;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MAXMSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_p2top5, mqd_p5top6;
	int mq_return = 0;
	char p2top5[] = "/p2top5";
	char p5top6[] = "/p5top6";

	mqd_p2top5 = mq_open(p2top5, flags, PERM, &attr);
	check_return(mqd_p2top5, p2top5, "mq_open");

	mqd_p5top6 = mq_open(p5top6, flags, PERM, &attr);
	check_return(mqd_p5top6, p5top6, "mq_open");


	/*control part*/
	mqd_t mqd_ctrltop5, mqd_p5toctrl;
	char ctrltop5[] = "/ctrltop5";
	char p5toctrl[] = "/p5toctrl";
	mqd_ctrltop5 = mq_open(ctrltop5, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_ctrltop5, ctrltop5, "mq_open");
	mqd_p5toctrl = mq_open(p5toctrl, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_p5toctrl, p5toctrl, "mq_open");


	char buffer[2048];
	struct ndpi_iphdr * iph;
	long long int i = 0;

	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltop5;
	noti_tran.mqd_ptoc = mqd_p5toctrl;
	//noti_tran.mqd_p[0] = mqd_p2top5;
	noti_tran.qds = 1;
	noti_tran.i[0] = &i;
	notifysetup(&noti_tran);




	//int port = 0;
//////////////////////////////////////////////////////////////////////////
    setupDetection();    //ndpi setup
	struct timeval timestamp;
	gettimeofday( &timestamp, NULL);
//////////////////////////////////////////////////////////////////////


	for(i = 0;i < PACKETS*10;i++) {//PACKETS = 5000 now.
		mq_return = mq_receive(mqd_p2top5, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
		}

		iph = (struct ndpi_iphdr *) buffer;
/////////////////////////////////////////////////////////////////////////////
		u_int16_t proto = ProtoDtect(timestamp, mq_return, iph);

///////////////////////////////////////////////////////////////////////
		mq_return = mq_send(mqd_p5top6, (char *) iph, mq_return, 0);
		if(mq_return == -1) {
			printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			printstar();
			printstar();
			printstar();
		}
		if((i%1000 == 0) || (i < 400)) {
			printf("i = %lld, iph->daddr = %8X, packet_length = %d \n", i, iph->daddr, mq_return);
			printf("pid = %d , working on CPU %d \n", getpid(), getcpu());
			printf("proto : %d \n", proto);
		}
		if(i%CHECKQUEUE_FREQUENCY == 0) {
			checkqueue(mqd_p2top5, p2top5, &noti_tran);
		}

	}
	printf("i = %lld, iph->daddr = %8X, packet_length = %d \n", i, iph->daddr, mq_return);
	printf("%s has transfered %lld packets. \n", proname, i);

	//p2top5
	mq_return = mq_close(mqd_p2top5);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p2top5);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//ctrltop5
	mq_return = mq_close(mqd_ctrltop5);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctrltop5);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	//p5toctrl
	mq_return = mq_close(mqd_p5toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p5toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");


	exit(0);

}

