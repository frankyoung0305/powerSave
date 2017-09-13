#include"nfv.h"

struct record {//we wiil change and add more to this part!
	int number;//distinguish the progress.
	mqd_t mqd_ctop;//queue controller to progress.
	mqd_t mqd_ptoc;//queue progress to controller.
	pid_t pid_in_record;//progress's pid.

	int cpu;//the cpu where the process runs on.
	int queues;//the number of queues which the process receive.
	long queuelength[MAXQUEUES];
	long qmax[MAXQUEUES];//max packets in a queue.

};

struct ctrltrans {
	mqd_t mqd_ctop;//queue from controlloer to process.
	mqd_t mqd_ptoc;//queue from process to controller.
	struct record * statistics;
};

//we will write notifysetup function for every processes;
void ctrl_notifysetup(struct ctrltrans * trans);//notifysetup for controller.
void controller_control(union sigval sv);//control function for controller.


int main() {
	/*initialization about mqueue*/
	char proname[] = "controller";


	struct mq_attr attr_ct;
	attr_ct.mq_maxmsg = MSGCTOP;//maximum is 382.
	attr_ct.mq_msgsize = 2048;
	attr_ct.mq_flags = 0;


	int flags = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_ctop1, mqd_p1toc;
	mqd_t mqd_ctop2, mqd_p2toc;
	mqd_t mqd_ctop3, mqd_p3toc;

	int mq_return = 0;
	char ctop1[] = "/ctrltop1";
	char p1toc[] = "/p1toctrl";

	char ctop2[] = "/ctrltop2";
	char p2toc[] = "/p2toctrl";

	char ctop3[] = "/ctrltop3";
	char p3toc[] = "/p3toctrl";


	mqd_ctop1 = mq_open(ctop1, flags, PERM, &attr_ct);
	check_return(mqd_ctop1, proname, "mq_open");
	mqd_p1toc = mq_open(p1toc, flags, PERM, &attr_ct);
	check_return(mqd_p1toc, proname, "mq_open");

	mqd_ctop2 = mq_open(ctop2, flags, PERM, &attr_ct);
	check_return(mqd_ctop2, proname, "mq_open");
	mqd_p2toc = mq_open(p2toc, flags, PERM, &attr_ct);
	check_return(mqd_p2toc, proname, "mq_open");

	mqd_ctop3 = mq_open(ctop3, flags, PERM, &attr_ct);
	check_return(mqd_ctop3, proname, "mq_open");
	mqd_p3toc = mq_open(p3toc, flags, PERM, &attr_ct);
	check_return(mqd_p3toc, proname, "mq_open");

	int i = 0, j = 0;
	struct record pstats[3];
	for(i = 0;i < 3;i++) {
		pstats[i].number = i + 1;
		pstats[i].queues = 0;
		for(j = 0;j < MAXQUEUES;j ++); {
			pstats[i].queuelength[j] = -1;
			pstats[i].qmax[j] = -1;
		}
	}
	pstats[0].mqd_ctop = mqd_ctop1;
	pstats[0].mqd_ptoc = mqd_p1toc;

	pstats[1].mqd_ctop = mqd_ctop2;
	pstats[1].mqd_ptoc = mqd_p2toc;

	pstats[2].mqd_ctop = mqd_ctop2;
	pstats[2].mqd_ptoc = mqd_p2toc;

	/*pthread*/
	struct ctrltrans noti_p1;
	noti_p1.mqd_ctop = mqd_ctop1;
	noti_p1.mqd_ptoc = mqd_p1toc;
	noti_p1.statistics = &pstats[0];
	ctrl_notifysetup(&noti_p1);

	struct ctrltrans noti_p2;
	noti_p2.mqd_ctop = mqd_ctop2;
	noti_p2.mqd_ptoc = mqd_p2toc;
	noti_p2.statistics = &pstats[1];
	ctrl_notifysetup(&noti_p2);

	struct ctrltrans noti_p3;
	noti_p3.mqd_ctop = mqd_ctop3;
	noti_p3.mqd_ptoc = mqd_p3toc;
	noti_p3.statistics = &pstats[2];
	ctrl_notifysetup(&noti_p3);


	char buffer[2048];
	struct ctrlmsg ctrlbuffer;
	ctrlbuffer.cpu = 5;


	for(i = 0;i < 5;i++) {
		ctrlbuffer.cpu = i + 3;
		ctrlbuffer.service_number = 3;
		mq_return = mq_send(mqd_ctop1, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop1, "mq_send in controller");

		mq_return = mq_send(mqd_ctop2, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop2, "mq_send in controller");

		mq_return = mq_send(mqd_ctop3, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop3, "mq_send in controller");

		printf("controller has sent the ctrlmsg(sevice_number = 3) that i = %d \n", i);
		usleep(500000);

		ctrlbuffer.service_number = 1;
		mq_return = mq_send(mqd_ctop1, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop1, "mq_send in controller");

		mq_return = mq_send(mqd_ctop2, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop2, "mq_send in controller");

		mq_return = mq_send(mqd_ctop3, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop3, "mq_send in controller");

		printf("controller has sent the ctrlmsg(sevice_number = 1) that i = %d \n", i);
		usleep(500000);

	}

	

	printstar();
	printf("controller has sent all kinds of ctrlmsg. \n");
	printstar();

	//ctop1
	mq_return = mq_close(mqd_ctop1);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop1, "mq_close");
	mq_return = mq_unlink(ctop1);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop1, "mq_unlink");
	//p1toc
	mq_return = mq_close(mqd_p1toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p1toc, "mq_close");
	mq_return = mq_unlink(p1toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p1toc, "mq_unlink");

	//ctop2
	mq_return = mq_close(mqd_ctop2);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop2, "mq_close");
	mq_return = mq_unlink(ctop2);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop2, "mq_unlink");
	//p2toc
	mq_return = mq_close(mqd_p2toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p2toc, "mq_close");
	mq_return = mq_unlink(p2toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p2toc, "mq_unlink");

	//ctop3
	mq_return = mq_close(mqd_ctop3);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop3, "mq_close");
	mq_return = mq_unlink(ctop3);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop3, "mq_unlink");
	//p3toc
	mq_return = mq_close(mqd_p3toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p3toc, "mq_close");
	mq_return = mq_unlink(p3toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p3toc, "mq_unlink");
	


	func_quit(proname);
	exit(0);

}


void ctrl_notifysetup(struct ctrltrans * trans) {
	int mq_return = 0;
	struct sigevent mq_notification;

	char funcname[] = "ctrl_notifysetup";

	printstar();
	printf("now in %s, trans->mqd_ctop = %d \n", funcname, trans->mqd_ptoc);
	mq_notification.sigev_notify = SIGEV_THREAD;
	mq_notification.sigev_notify_function = controller_control;
	mq_notification.sigev_notify_attributes = NULL;
	mq_notification.sigev_value.sival_ptr = trans;


	mq_return = mq_notify(trans->mqd_ptoc, &mq_notification);
	check_return(mq_return, funcname, "mq_notify in ctrl_notifysetup");
	printstar();

}

void controller_control(union sigval sv) {
	printstar();
	struct ctrltrans * parameter;
	parameter = (struct ctrltrans *) sv.sival_ptr;
	int func_re;
	char ptname[] = "controller_control";

	printstar();
	printf("Now in %s \n", ptname);

	struct mq_attr rc_mq_attr, q_mq_attr;
	mq_getattr(parameter->mqd_ptoc, &rc_mq_attr);

	struct ctrlmsg ctrlbuffer, down_ctrlmsg;
	struct ctrlmsg * ctrlbufferp;
	char msg_buffer[rc_mq_attr.mq_msgsize];
	printf("msg_buffer size is %lu \n", sizeof(msg_buffer));
	printf("parameter->mqd_ptoc = %d \n", parameter->mqd_ptoc);

	ctrl_notifysetup(parameter);

	while ((func_re = mq_receive(parameter->mqd_ptoc, msg_buffer, rc_mq_attr.mq_msgsize, 0)) >= 0) {
		check_return(func_re, ptname, "mq_receive");
		ctrlbufferp = (struct ctrlmsg *) msg_buffer;
		ctrlbuffer = * ctrlbufferp;
		printf("controlller received ctrlbuffer.service_number is %ld \n", ctrlbuffer.service_number);
	}


	if (errno != EAGAIN) {//in nonblock mode, "errno = EAGAIN" means that there is no message in queue.	EAGAIN = 11
		check_return(func_re, ptname, "mq_receive");
		printf("mq_receive exits unnormally");
		exit(0);/* Unexpected error */
	}



	switch(ctrlbuffer.service_number)
	{
		case 2:
			parameter->statistics->cpu = ctrlbuffer.cpu;
			parameter->statistics->queues = ctrlbuffer.edges;
			parameter->statistics->pid_in_record = ctrlbuffer.pid_in_ctrlmsg;
			int i_edges = 0;
			for(i_edges = 0;i_edges < ctrlbuffer.edges;i_edges++) {
				parameter->statistics->queuelength[i_edges] = ctrlbuffer.qsize[i_edges];
				parameter->statistics->qmax[i_edges] = ctrlbuffer.qmaxsize[i_edges];
			}
			printf("now show the result of ctrlbuffer.\nthe process %d runs on cpu %d, it receives %d queues \n", parameter->statistics->pid_in_record, parameter->statistics->cpu, parameter->statistics->queues);
			for(i_edges = 0;i_edges < parameter->statistics->queues;i_edges++) {
				printf("the queue %d has %ld packets of most %ld packets \n", i_edges, parameter->statistics->queuelength[i_edges], parameter->statistics->qmax[i_edges]);
			}
			break;
	}

	printf("finish a controller control \n");
	printstar();
	pthread_exit(NULL);
}
