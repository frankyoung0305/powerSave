#include"nfv.h"
#include "../posix/ndpi_api.h" //iphdr
#include <pcap.h>

#include "fan.h"

int main() {
	/*initialization about mqueue*/
	char proname[] = "p7_FW";
	setcpu(1);

	struct mq_attr attr, attr_ctrl, q_attr;
	attr.mq_maxmsg = MAXMSG;
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MAXMSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_p4top7, mqd_p3top7, mqd_p7top8;
	int mq_return = 0;
	char p4top7[] = "/p4top7";
	char p3top7[] = "/p3top7";
	char p7top8[] = "/p7top8";

	mqd_p4top7 = mq_open(p4top7, flags, PERM, &attr);
	check_return(mqd_p4top7, p4top7, "mq_open");
	
	mqd_p3top7 = mq_open(p3top7, flags, PERM, &attr);
	check_return(mqd_p3top7, p3top7, "mq_open");

	mqd_p7top8 = mq_open(p7top8, flags, PERM, &attr);
	check_return(mqd_p7top8, p7top8, "mq_open");

	/*control part*/
	mqd_t mqd_ctrltop7, mqd_p7toctrl;
	char ctrltop7[] = "/ctrltop7";
	char p7toctrl[] = "/p7toctrl";
	mqd_ctrltop7 = mq_open(ctrltop7, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_ctrltop7, ctrltop7, "mq_open");
	mqd_p7toctrl = mq_open(p7toctrl, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_p7toctrl, p7toctrl, "mq_open");



	char buffer[2048];
	struct ndpi_iphdr * iph;
	long long int i = 0;
	long long int j = 0;

	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltop7;
	noti_tran.mqd_ptoc = mqd_p7toctrl;
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
		//p4top7
		mq_return = mq_getattr(mqd_p4top7, &q_attr);
		if(mq_return == -1) {
			printf("something wrong happened in %s when mq_getattr p4top7. \n", proname);
		}
		p_count1 = q_attr.mq_curmsgs >= 50?50:q_attr.mq_curmsgs;
		for(k = 0;k < p_count1;k++) {
			mq_return = mq_receive(mqd_p4top7, buffer, 2048, 0);
			if(mq_return == -1) {
				printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
			}
			
			iph = (struct ndpi_iphdr *) buffer;
/////////////////////////////////////////////////////////////////////////
			//FW actions
		
			fwpacket_preprocess(timestamp, mq_return, iph, &flag);
			printf("flag: %d \n", flag);
			if(flag == 0) {
					mq_return = mq_send(mqd_p7top8, (char *) iph, mq_return, 0);
					if(mq_return == -1) {
						printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
						printstar();
						printstar();
						printstar();
					}
			
			}
			///////////////////////////////////
			
			
			if(i%100 == 0) {

				checkqueue(mqd_p4top7, p4top7, &noti_tran);//check if the queue is congested and process needs to be splited.
			}
			i++;			
		}
		
		//p3top7
		mq_return = mq_getattr(mqd_p3top7, &q_attr);
		if(mq_return == -1) {
			printf("something wrong happened in %s when mq_getattr p4top7. \n", proname);
		}
		p_count2 = q_attr.mq_curmsgs >= 50?50:q_attr.mq_curmsgs;
		for(k = 0;k < p_count2;k++) {
			mq_return = mq_receive(mqd_p3top7, buffer, 2048, 0);
			if(mq_return == -1) {
				printf("%s:receive %lld times fails:%s, errno = %d \n", proname, j, strerror(errno), errno);
			}
			
			iph = (struct ndpi_iphdr *) buffer;
/////////////////////////////////////////////////////////////////////////
			//FW actions
		
			fwpacket_preprocess(timestamp, mq_return, iph, &flag);
			printf("flag: %d \n", flag);
			if(flag == 0) {
					mq_return = mq_send(mqd_p7top8, (char *) iph, mq_return, 0);
					if(mq_return == -1) {
						printf("%s:send %lld times fails:%s, errno = %d \n", proname, j, strerror(errno), errno);
						printstar();
						printstar();
						printstar();
					}
			
			}
			///////////////////////////////////
			
			
			if(j%100 == 0) {

				checkqueue(mqd_p3top7, p3top7, &noti_tran);//check if the queue is congested and process needs to be splited.
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

	//p4top7
	mq_return = mq_close(mqd_p4top7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p4top7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//p3top7
	mq_return = mq_close(mqd_p3top7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p3top7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	
	//p7top8
	mq_return = mq_close(mqd_p7top8);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p7top8);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//ctrltop7
	mq_return = mq_close(mqd_ctrltop7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctrltop7);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	//p7toctrl
	mq_return = mq_close(mqd_p7toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p7toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");




	exit(0);

}

