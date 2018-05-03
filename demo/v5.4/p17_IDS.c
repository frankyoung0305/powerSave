#include "nfv.h"
#include "fan.h"
#include "../posix/ndpi_api.h" //iphdr
#include <pcap.h>

int main() {
	/*initialization about mqueue*/
	char proname[] = "p17_IDS";
	setcpu(P17_STARTING_CPU);

	struct mq_attr attr, attr_ctrl;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MAXMSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_p16top17, mqd_p17top20, mqd_p17top19;
	int mq_return = 0;
	char p16top17[] = "/p16top17";
	char p17top20[] = "/p17top20";
	char p17top19[] = "/p17top19";
	
	mqd_p16top17 = mq_open(p16top17, flags, PERM, &attr);
	check_return(mqd_p16top17, p16top17, "mq_open");

	mqd_p17top20 = mq_open(p17top20, flags, PERM, &attr);
	check_return(mqd_p16top17, p17top20, "mq_open");

	mqd_p17top19 = mq_open(p17top19, flags, PERM, &attr);
	check_return(mqd_p16top17, p17top19, "mq_open");


	/*control part*/
	mqd_t mqd_ctrltop17, mqd_p17toctrl;
	char ctrltop17[] = "/ctrltop17";
	char p17toctrl[] = "/p17toctrl";
	mqd_ctrltop17 = mq_open(ctrltop17, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_ctrltop17, ctrltop17, "mq_open");
	mqd_p17toctrl = mq_open(p17toctrl, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_p17toctrl, p17toctrl, "mq_open");


	char buffer[2048];
	struct ndpi_iphdr * iph;
	long long int i = 0;

	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltop17;
	noti_tran.mqd_ptoc = mqd_p17toctrl;
	//noti_tran.mqd_p[0] = mqd_p16top17;
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
	char fileDir[] = "./log_IDS17.txt";
	filp = fopen(fileDir,"w");

/////////////////////////////////////////////////////////////////////


	//int port = 0;

	for(i = 0;i < PACKETS*10;i += 2) {
		mq_return = mq_receive(mqd_p16top17, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:%s receive %lld times fails:%s, errno = %d \n", proname, p16top17, i, strerror(errno), errno);
			return -1;
		}
		iph = (struct ndpi_iphdr *) buffer;
		if((i%SHOW_FREQUENCY == 0) || (i < SHOW_THRESHOLD)) {
			printf("%s:%s i = %lld, packet length = %d, iph->daddr = %8X, pid = %d , working on CPU %d \n", proname, p16top17, i, mq_return, iph->daddr, getpid(), getcpu());
		}
		if(i%CHECKQUEUE_FREQUENCY == 0) {
			checkqueue(mqd_p16top17, p16top17, &noti_tran);
		}

		/////////////////////////////////////IDS
		IDS(timestamp, mq_return, iph, filp, i);
		mq_return = mq_send(mqd_p17top20, (char *) iph, mq_return, 0);
		if(mq_return == -1) {
			printf("%s:%s send %lld times fails:%s, errno = %d \n", proname, p17top20, i, strerror(errno), errno);
			return -1;
		}		

		mq_return = mq_receive(mqd_p16top17, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:%s receive %lld times fails:%s, errno = %d \n", proname, p16top17, i, strerror(errno), errno);
			return -1;
		}
		iph = (struct ndpi_iphdr *) buffer;
		if((i%SHOW_FREQUENCY == 0) || (i < SHOW_THRESHOLD)) {
			printf("%s:%s i = %lld, packet length = %d, iph->daddr = %8X, pid = %d , working on CPU %d \n", proname, p16top17, i, mq_return, iph->daddr, getpid(), getcpu());
		}
		if(i%CHECKQUEUE_FREQUENCY == 0) {
			checkqueue(mqd_p16top17, p16top17, &noti_tran);
		}
		
		/////////////////////////////////////IDS
		IDS(timestamp, mq_return, iph, filp, i);
		mq_return = mq_send(mqd_p17top19, (char *) iph, mq_return, 0);
		if(mq_return == -1) {
			printf("%s:%s send %lld times fails:%s, errno = %d \n", proname, p17top19, i, strerror(errno), errno);
			return -1;
		}	


		

	}
	printf("%s has transfered %lld packets. \n", proname, i);

	//p16top17
	mq_return = mq_close(mqd_p16top17);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p16top17);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//p17top20
	mq_return = mq_close(mqd_p17top20);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p17top20);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//p17top19
	mq_return = mq_close(mqd_p17top19);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p17top19);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//ctrltop17
	mq_return = mq_close(mqd_ctrltop17);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctrltop17);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	//p17toctrl
	mq_return = mq_close(mqd_p17toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p17toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");


	exit(0);

}

