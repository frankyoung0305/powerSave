#include"../nfv.h"

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


void ctrl_notifysetup(struct ctrltrans * trans);//notifysetup for controller.
void controller_control(union sigval sv);//control function for controller.


int main() {
	/*initialization about mqueue*/
	char proname[] = "controller";

/*
//this is used to set POSIX message queues (checked by `ulimit -q`)
	struct rlimit rlim;
	memset(&rlim, 0, sizeof(rlim));
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;

	setrlimit(RLIMIT_MSGQUEUE, &rlim);
*/

	struct mq_attr attr_ct;
	attr_ct.mq_maxmsg = MSGCTOP;//maximum is 382.
	attr_ct.mq_msgsize = 2048;
	attr_ct.mq_flags = 0;


	int flags = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_ctosd, mqd_sdtoc;

	int mq_return = 0;
	char ctosd[] = "/controllertosend";
	char sdtoc[] = "/sendtocontroller";

	mqd_ctosd = mq_open(ctosd, flags, PERM, &attr_ct);
	check_return(mqd_ctosd, proname, "mq_open");
	mqd_sdtoc = mq_open(sdtoc, flags, PERM, &attr_ct);
	check_return(mqd_sdtoc, proname, "mq_open");

	int i = 0;
	struct record pstats;
	pstats.queues = 0;
	for(i = 0;i < MAXQUEUES;i++); {
		pstats.queuelength[i] = -1;
		pstats.qmax[i] = -1;
	}

	/*pthread*/
	struct ctrltrans noti_tran;
	noti_tran.mqd_ctop = mqd_ctosd;
	noti_tran.mqd_ptoc = mqd_sdtoc;
	noti_tran.statistics = &pstats;
	ctrl_notifysetup(&noti_tran);


	char buffer[2048];
	struct ctrlmsg ctrlbuffer;
	ctrlbuffer.cpu = 5;


	for(i = 0;i<5;i++) {
		ctrlbuffer.cpu = i + 3;
		ctrlbuffer.service_number = 3;
		mq_return = mq_send(mqd_ctosd, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, proname, "mq_send in controller");
		printf("controller has sent the ctrlmsg(sevice_number = 3) that i = %d \n", i);
		usleep(500000);

		ctrlbuffer.service_number = 1;
		mq_return = mq_send(mqd_ctosd, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, proname, "mq_send in controller");
		printf("controller has sent the ctrlmsg(sevice_number = 1) that i = %d \n", i);
		usleep(100000);

	}

	

	printstar();
	printf("controller has sent all kinds of ctrlmsg. \n");
	printstar();

	mq_return = mq_close(mqd_ctosd);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctosd);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	
/*	mq_return = mq_close(mqd_ctorc);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(ctorc);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	for(process_i = 0;i<PROCESSES;i++) {
		mq_return = mq_close(mqd_ctop[process_i]);//returns 0 on success, or -1 on error.
		check_return(mq_return, proname, "mq_close");
		mq_return = mq_unlink(ctop[i]);//returns 0 on success, or -1 on error.
		check_return(mq_return, proname, "mq_unlink");
	}
*/

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
