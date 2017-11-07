#include"nfv.h"
#include "../posix/ndpi_api.h" //iphdr
#include <pcap.h>

#include "fan.h"

int main() {
	/*initialization about mqueue*/
	char proname[] = "process_3";
	setcpu(1);

	struct mq_attr attr, attr_ctrl;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MAXMSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_sdtop3;
	int mq_return = 0;
	char sdtop3[]="/sendtop3";

	mqd_sdtop3 = mq_open(sdtop3, flags, PERM, &attr);
	check_return(mqd_sdtop3, sdtop3, "mq_open");



	/*control part*/
	mqd_t mqd_ctrltop3, mqd_p3toctrl;
	char ctrltop3[] = "/ctrltop3";
	char p3toctrl[] = "/p3toctrl";
	mqd_ctrltop3 = mq_open(ctrltop3, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_ctrltop3, ctrltop3, "mq_open");
	mqd_p3toctrl = mq_open(p3toctrl, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_p3toctrl, p3toctrl, "mq_open");



	char buffer[2048];
	struct ndpi_iphdr * iph;
	long long int i = 0;


	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltop3;
	noti_tran.mqd_ptoc = mqd_p3toctrl;
	noti_tran.mqd_p[0] = mqd_sdtop3;
	noti_tran.qds = 1;
	noti_tran.i[0] = &i;
	notifysetup(&noti_tran);


//////////////////////////////////////////fw///////////////////////////
    setupDetection();    //ndpi setup
/////////////////////////////////////////////////////////////////////	

	

	writeAcl(15);
	struct timeval timestamp;
	gettimeofday( &timestamp, NULL);

	FILE *filp = NULL; 
	char fileDir[] = "/home/dpdk/github/powerSave/demo/v2.4(ids/log.txt";
	filp = fopen(fileDir,"w");

/////////////////////////////////////////////////////////////////////
	//int port = 0;

	for(i = 0;i < PACKETS*10;i++) {//PACKETS = 5000 now.
		mq_return = mq_receive(mqd_sdtop3, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
		}

		iph = (struct ndpi_iphdr *) buffer;
////////////////////////////////////////////////////////////
		int flag = 0; //1: block
		IDS(timestamp, mq_return, iph, filp, i);
		//	

		//u_int16_t proto = ProtoDtect(timestamp, mq_return, iph);
//////////////////////////////////////////////////////////////
		if((i%1000 == 0) || (i < 400)) {
			printf("i = %lld, iph->daddr = %8X, packet_length = %d \n", i, iph->daddr, mq_return);
			printf("pid = %d , working on CPU %d \n", getpid(), getcpu());
		}

		if(i%100 == 0) {
			checkqueue(mqd_sdtop3, sdtop3, &noti_tran);
		}
	}
	printf("i = %lld, iph->daddr = %8X, packet_length = %d \n", i, iph->daddr, mq_return);
	printf("%s has transfered %lld packets. \n", proname, i);

	mq_return = mq_close(mqd_sdtop3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(sdtop3);//returns 0 on success, or -1 on error.
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

