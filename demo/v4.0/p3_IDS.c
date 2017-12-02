#include "nfv.h"
#include "fan.h"
#include "../posix/ndpi_api.h" //iphdr
#include <pcap.h>

int main() {
	/*initialization about mqueue*/
	char proname[] = "p3_IDS";
	setcpu(P3_STARTING_CPU);

	struct mq_attr attr, attr_ctrl;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MAXMSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_p1top3, mqd_p3top4, mqd_p3top7;
	int mq_return = 0;
	char p1top3[] = "/p1top3";
	char p3top4[] = "/p3top4";
	char p3top7[] = "/p3top7";
	
	mqd_p1top3 = mq_open(p1top3, flags, PERM, &attr);
	check_return(mqd_p1top3, p1top3, "mq_open");

	mqd_p3top4 = mq_open(p3top4, flags, PERM, &attr);
	check_return(mqd_p1top3, p3top4, "mq_open");

	mqd_p3top7 = mq_open(p3top7, flags, PERM, &attr);
	check_return(mqd_p1top3, p3top7, "mq_open");


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
	//noti_tran.mqd_p[0] = mqd_p1top3;
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
	char fileDir[] = "./log_IDS2.txt";
	filp = fopen(fileDir,"w");

/////////////////////////////////////////////////////////////////////


	//int port = 0;

	for(i = 0;i < PACKETS*10;i++) {//PACKETS = 5000 now.
		mq_return = mq_receive(mqd_p1top3, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:%s receive %lld times fails:%s, errno = %d \n", proname, p1top3, i, strerror(errno), errno);
		}
		iph = (struct ndpi_iphdr *) buffer;
		if((i%SHOW_FREQUENCY == 0) || (i < SHOW_THRESHOLD)) {
			printf("i = %lld, packet length = %d, iph->daddr = %8X \n", i, mq_return, iph->daddr);
			printf("pid = %d , working on CPU %d \n", getpid(), getcpu());
		}
		if(i%CHECKQUEUE_FREQUENCY == 0) {
			checkqueue(mqd_p1top3, p1top3, &noti_tran);
		}
		
		//////////////////////////////////////IDS
		IDS(timestamp, mq_return, iph, filp, i);
		mq_return = mq_send(mqd_p3top4, (char *) iph, mq_return, 0);
		if(mq_return == -1) {
			printf("%s:%s send %lld times fails:%s, errno = %d \n", proname, p3top4, i, strerror(errno), errno);
			return -1;
		}
		
		i++;
		
		mq_return = mq_receive(mqd_p1top3, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:%s receive %lld times fails:%s, errno = %d \n", proname, p1top3, i, strerror(errno), errno);
			return -1;
		}
		iph = (struct ndpi_iphdr *) buffer;
		if((i%SHOW_FREQUENCY == 0) || (i < SHOW_THRESHOLD)) {
			printf("i = %lld, packet length = %d, iph->daddr = %8X \n", i, mq_return, iph->daddr);
			printf("pid = %d , working on CPU %d \n", getpid(), getcpu());
		}
		if(i%CHECKQUEUE_FREQUENCY == 0) {
			checkqueue(mqd_p1top3, p1top3, &noti_tran);
		}
		
		//////////////////////////////////////IDS
		IDS(timestamp, mq_return, iph, filp, i);
		mq_return = mq_send(mqd_p3top7, (char *) iph, mq_return, 0);
		if(mq_return == -1) {
			printf("%s:%s send %lld times fails:%s, errno = %d \n", proname, p3top7, i, strerror(errno), errno);
			return -1;
		}
				
		

		

	}
	printf("%s has transfered %lld packets. \n", proname, i);

	//p1top3
	mq_return = mq_close(mqd_p1top3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p1top3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//p3top4
	mq_return = mq_close(mqd_p3top4);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p3top4);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//p3top7
	mq_return = mq_close(mqd_p3top7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p3top7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//ctrltop3
	mq_return = mq_close(mqd_ctrltop3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctrltop3);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	//p3toctrl
	mq_return = mq_close(mqd_p3toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p3toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");


	exit(0);

}

