#include"nfv.h"
#include "../posix/ndpi_api.h" //iphdr
#include <pcap.h>

#include "fan.h"

int main() {
	/*initialization about mqueue*/
	char proname[] = "p6_FW";
	setcpu(P6_STARTING_CPU);

	struct mq_attr attr, attr_ctrl, q_attr;
	attr.mq_maxmsg = MAXMSG;
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MAXMSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_p5top6, mqd_p4top6, mqd_p6top8;
	int mq_return = 0;
	char p5top6[] = "/p5top6";
	char p4top6[] = "/p4top6";
	char p6top8[] = "/p6top8";

	mqd_p5top6 = mq_open(p5top6, flags, PERM, &attr);
	check_return(mqd_p5top6, p5top6, "mq_open");
	
	mqd_p4top6 = mq_open(p4top6, flags, PERM, &attr);
	check_return(mqd_p4top6, p4top6, "mq_open");

	mqd_p6top8 = mq_open(p6top8, flags, PERM, &attr);
	check_return(mqd_p6top8, p6top8, "mq_open");

	/*control part*/
	mqd_t mqd_ctrltop6, mqd_p6toctrl;
	char ctrltop6[] = "/ctrltop6";
	char p6toctrl[] = "/p6toctrl";
	mqd_ctrltop6 = mq_open(ctrltop6, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_ctrltop6, ctrltop6, "mq_open");
	mqd_p6toctrl = mq_open(p6toctrl, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_p6toctrl, p6toctrl, "mq_open");



	char buffer[2048];
	struct ndpi_iphdr * iph;
	long long int i = 0;
	long long int j = 0;

	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltop6;
	noti_tran.mqd_ptoc = mqd_p6toctrl;
	noti_tran.qds = 2;
	noti_tran.i[0] = &i;
	noti_tran.i[1] = &j;
	notifysetup(&noti_tran);


//////////////////////////////////////////fw///////////////////////////
    setupDetection();    //ndpi setup

	writeAcl(50);
	struct timeval timestamp;
	gettimeofday( &timestamp, NULL);
/////////////////////////////////////////////////////////////////////

	int flag = 0; //1: block, shows the result of firewall.
	int p_count1 = 0;
	int p_count2 = 0;
	int k = 0;
	while(1) {
		//p5top6
		mq_return = mq_getattr(mqd_p5top6, &q_attr);
		if(mq_return == -1) {
			printf("something wrong happened in %s when mq_getattr p5top6. \n", proname);
		}
		p_count1 = q_attr.mq_curmsgs >= 50?50:q_attr.mq_curmsgs;
		for(k = 0;k < p_count1;k++) {
			mq_return = mq_receive(mqd_p5top6, buffer, 2048, 0);
			if(mq_return == -1) {
				printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			}
			
			iph = (struct ndpi_iphdr *) buffer;
/////////////////////////////////////////////////////////////////////////
			//FW actions
		
			fwpacket_preprocess(timestamp, mq_return, iph, &flag);
			if(flag != 0) {
				printf("i = %lld, flag: %d \n", i, flag);
			}
			else {
					mq_return = mq_send(mqd_p6top8, (char *) iph, mq_return, 0);
					if(mq_return == -1) {
						printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
						printstar();
						printstar();
						printstar();
					}
			
			}
			///////////////////////////////////
			
			if(((i + j)%100000 == 0) || ((i + j) < 400)) {
				printf("i = %lld, pid = %d , working on CPU %d \n", i, getpid(), getcpu());
			}
			if(i%CHECKQUEUE_FREQUENCY == 0) {

				checkqueue(mqd_p5top6, p5top6, &noti_tran);//check if the queue is congested and process needs to be splited.
			}
			i++;			
		}
		
		//p4top6
		mq_return = mq_getattr(mqd_p4top6, &q_attr);
		if(mq_return == -1) {
			printf("something wrong happened in %s when mq_getattr p5top6. \n", proname);
		}
		p_count2 = q_attr.mq_curmsgs >= 50?50:q_attr.mq_curmsgs;
		for(k = 0;k < p_count2;k++) {
			mq_return = mq_receive(mqd_p4top6, buffer, 2048, 0);
			if(mq_return == -1) {
				printf("%s:receive %lld times fails:%s, errno = %d \n", proname, j, strerror(errno), errno);
			}
			
			iph = (struct ndpi_iphdr *) buffer;
/////////////////////////////////////////////////////////////////////////
			//FW actions
		
			fwpacket_preprocess(timestamp, mq_return, iph, &flag);
			if(flag != 0) {
				printf("j = %lld, flag: %d \n", j, flag);
			}
			else {
					mq_return = mq_send(mqd_p6top8, (char *) iph, mq_return, 0);
					if(mq_return == -1) {
						printf("%s:send %lld times fails:%s, errno = %d \n", proname, j, strerror(errno), errno);
						printstar();
						printstar();
						printstar();
					}
			
			}
			///////////////////////////////////
			
			if(((i + j)%100000 == 0) || ((i + j) < 400)) {
				printf("j = %lld, pid = %d , working on CPU %d \n", j, getpid(), getcpu());
			}
			if(j%CHECKQUEUE_FREQUENCY == 0) {

				checkqueue(mqd_p4top6, p4top6, &noti_tran);//check if the queue is congested and process needs to be splited.
			}
			j++;
		}	
		if(p_count1 || p_count2) {
			continue;
		}
		else {//pretend the process to work when there is nothing in queue.
			usleep(1000);
		}	
		
	}
	
	
	
	


	printf("i = %lld, iph->daddr = %8X, packet_length = %d \n", i, iph->daddr, mq_return);
	printf("%s has transfered %lld packets. \n", proname, i);
	checkcpu();

	//p5top6
	mq_return = mq_close(mqd_p5top6);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p5top6);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//p4top6
	mq_return = mq_close(mqd_p4top6);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p4top6);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	
	//p6top8
	mq_return = mq_close(mqd_p6top8);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p6top8);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//ctrltop6
	mq_return = mq_close(mqd_ctrltop6);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctrltop6);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	//p6toctrl
	mq_return = mq_close(mqd_p6toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p6toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");




	exit(0);

}

