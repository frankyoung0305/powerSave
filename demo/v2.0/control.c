#include"nfv.h"
#include"extended_KL.h"
#include"control.h"




int main() {
	/*initialization about mqueue*/
	char proname[] = "controller";
	setcpu(7);
	changefrequency(3401000, 0);
	changefrequency(3401000, 1);
	changefrequency(3401000, 7);

	changefrequency(800000, 2);
	changefrequency(800000, 3);
	changefrequency(800000, 4);
	changefrequency(800000, 5);
	changefrequency(800000, 6);


	struct mq_attr attr_ct;
	attr_ct.mq_maxmsg = MAXMSGCTOP;//maximum is 382.
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
	check_return(mqd_ctop1, ctop1, "mq_open");
	mqd_p1toc = mq_open(p1toc, flags, PERM, &attr_ct);
	check_return(mqd_p1toc, p1toc, "mq_open");

	mqd_ctop2 = mq_open(ctop2, flags, PERM, &attr_ct);
	check_return(mqd_ctop2, ctop2, "mq_open");
	mqd_p2toc = mq_open(p2toc, flags, PERM, &attr_ct);
	check_return(mqd_p2toc, p2toc, "mq_open");

	mqd_ctop3 = mq_open(ctop3, flags, PERM, &attr_ct);
	check_return(mqd_ctop3, ctop3, "mq_open");
	mqd_p3toc = mq_open(p3toc, flags, PERM, &attr_ct);
	check_return(mqd_p3toc, p3toc, "mq_open");

	int i = 0, j = 0;
	struct record pstats[PROS_NUMBER];//PROS_NUMBER = 3 defined in control.h.
	printstar();
	for(i = 0;i < PROS_NUMBER;i++) {
		pstats[i].number = i;
		pstats[i].queues = 0;
		pstats[i].cpu = 1;
		for(j = 0;j < MAXQUEUES;j++) {
			pstats[i].queuelength[j] = 0;
			pstats[i].qmax[j] = MAXMSG;
			pstats[i].i[j] = 0;
			pstats[i].throughput[j] = 0;
		}
	}
	pstats[0].mqd_ctop = mqd_ctop1;
	pstats[0].mqd_ptoc = mqd_p1toc;

	pstats[1].mqd_ctop = mqd_ctop2;
	pstats[1].mqd_ptoc = mqd_p2toc;

	pstats[2].mqd_ctop = mqd_ctop3;
	pstats[2].mqd_ptoc = mqd_p3toc;

	double adj_array[ADJ_ARRAY_EDGES][ADJ_ARRAY_EDGES];
	clear_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array in control.c");
	show_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array in control.c");

	double point_weight[ADJ_ARRAY_EDGES];
	clear_double_series(ADJ_ARRAY_EDGES, point_weight, "point_weight in control.c");
	struct ctrltrans noti_p[PROS_NUMBER];
	for(i = 0;i < PROS_NUMBER;i++) {
		for(j = 0;j < PROS_NUMBER;j++) {
			noti_p[i].statistics[j] = &pstats[j];
		}
	}


	for(i = 0;i < PROS_NUMBER;i++) {
		noti_p[i].p_number = i;/*	i = 0 p1_l3
						i = 1 p2.o
						i = 2 p3.o	*/
		noti_p[i].adj_array = adj_array;
		noti_p[i].point_weight = point_weight;
		ctrl_notifysetup(&noti_p[i]);//pthread
		usleep(50000);
	}


	//char buffer[2048];
	struct ctrlmsg ctrlbuffer;
	ctrlbuffer.cpu = 5;


	for(i = 0;i < 60;i++) {
		ctrlbuffer.service_number = 1;
		mq_return = mq_send(mqd_ctop1, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop1, "mq_send in controller");

		mq_return = mq_send(mqd_ctop2, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop2, "mq_send in controller");

		mq_return = mq_send(mqd_ctop3, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop3, "mq_send in controller");

		printf("\nLOOP i = %d \n\n", i);
		sleep(UPDATE_PERIOD);

	}

	

	printstar();
	printf("controller has sent all ctrlmsg. \n");
	printf("Now show CPUs that processes work on. \n");
	for(i = 0;i < PROS_NUMBER;i++) {
		printf("p_number = %d, works on CPU %d \n", i, pstats[i].cpu);
	}
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


