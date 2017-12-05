#include"nfv.h"
#include "../posix/ndpi_api.h" //iphdr
#include <pcap.h>

#include "fan.h"

int main() {
	/*initialization about mqueue*/
	char proname[] = "p39_counter";
	setcpu(P39_STARTING_CPU);

	struct mq_attr attr, attr_ctrl, q_attr;
	attr.mq_maxmsg = MAXMSG;
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;

	attr_ctrl.mq_maxmsg = MAXMSGCTOP;
	attr_ctrl.mq_msgsize = 2048;
	attr_ctrl.mq_flags = 0;


	int flags = O_CREAT | O_RDWR;
	int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_p37top39, mqd_p38top39;
	int mq_return = 0;
	char p37top39[] = "/p37top39";
	char p38top39[] = "/p38top39";

	mqd_p37top39 = mq_open(p37top39, flags, PERM, &attr);
	check_return(mqd_p37top39, p37top39, "mq_open");
	
	mqd_p38top39 = mq_open(p38top39, flags, PERM, &attr);
	check_return(mqd_p38top39, p38top39, "mq_open");


	/*control part*/
	mqd_t mqd_ctrltop39, mqd_p39toctrl;
	char ctrltop39[] = "/ctrltop39";
	char p39toctrl[] = "/p39toctrl";
	mqd_ctrltop39 = mq_open(ctrltop39, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_ctrltop39, ctrltop39, "mq_open");
	mqd_p39toctrl = mq_open(p39toctrl, flags_ctrl, PERM, &attr_ctrl);
	check_return(mqd_p39toctrl, p39toctrl, "mq_open");



	char buffer[2048];
	//struct ndpi_iphdr * iph;
	long long int i = 0;
	long long int j = 0;

	/*pthread*/
	struct transfer noti_tran;
	noti_tran.mqd_ctop = mqd_ctrltop39;
	noti_tran.mqd_ptoc = mqd_p39toctrl;
	noti_tran.qds = 2;
	noti_tran.i[0] = &i;
	noti_tran.i[1] = &j;
	notifysetup(&noti_tran);



	int p_count1 = 0;
	int p_count2 = 0;
	int k = 0;
	while(1) {
		//p37top39
		mq_return = mq_getattr(mqd_p37top39, &q_attr);
		if(mq_return == -1) {
			printf("%s:something wrong happened when mq_getattr p37top39. \n", proname);
			return -1;
		}
		p_count1 = q_attr.mq_curmsgs >= 50?50:q_attr.mq_curmsgs;
		for(k = 0;k < p_count1;k++) {
			mq_return = mq_receive(mqd_p37top39, buffer, 2048, 0);
			if(mq_return == -1) {
				printf("%s:%s receive %lld times fails:%s, errno = %d \n", proname, p37top39, i, strerror(errno), errno);
				return -1;
			}
			if(((i + j)%SHOW_FREQUENCY == 0) || (i + j < SHOW_THRESHOLD)) {
				printf("%s:%s i = %lld, packet length = %d, pid = %d , working on CPU %d \n", proname, p37top39, i, mq_return, getpid(), getcpu());
			}
			
			//iph = (struct ndpi_iphdr *) buffer;			
			if(i%CHECKQUEUE_FREQUENCY == 0) {
				checkqueue(mqd_p37top39, p37top39, &noti_tran);//check if the queue is congested and process needs to be splited.
			}
			i++;			
		}
		
		//p38top39
		mq_return = mq_getattr(mqd_p38top39, &q_attr);
		if(mq_return == -1) {
			printf("%s:something wrong happened when mq_getattr p37top39. \n", proname);
			return -1;
		}
		p_count2 = q_attr.mq_curmsgs >= 50?50:q_attr.mq_curmsgs;
		for(k = 0;k < p_count2;k++) {
			mq_return = mq_receive(mqd_p38top39, buffer, 2048, 0);
			if(mq_return == -1) {
				printf("%s:%s receive %lld times fails:%s, errno = %d \n", proname, p38top39, j, strerror(errno), errno);
				return -1;
			}
			if(((i + j)%SHOW_FREQUENCY == 0) || (i + j) < SHOW_THRESHOLD) {
				printf("%s:%s j = %lld, packet length = %d, pid = %d , working on CPU %d \n", proname, p38top39, j, mq_return, getpid(), getcpu());
			}
					
			//iph = (struct ndpi_iphdr *) buffer;			
			if(j%CHECKQUEUE_FREQUENCY == 0) {
				checkqueue(mqd_p38top39, p38top39, &noti_tran);//check if the queue is congested and process needs to be splited.
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
	

	printf("%s has transfered %lld packets. \n", proname, i);
	checkcpu();

	//p37top39
	mq_return = mq_close(mqd_p37top39);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p37top39);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	//p38top39
	mq_return = mq_close(mqd_p38top39);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p38top39);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");


	//ctrltop39
	mq_return = mq_close(mqd_ctrltop39);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctrltop39);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");
	//p39toctrl
	mq_return = mq_close(mqd_p39toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(p39toctrl);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");




	exit(0);

}

