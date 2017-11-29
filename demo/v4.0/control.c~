#include"nfv.h"
#include"extended_KL.h"
#include"control.h"




int main() {
	/*initialization about mqueue*/
	char proname[] = "controller";
	
	#ifdef RUNMODE
	printf("Please not define RUNMODE!!! \n");
	return 1;
	#endif
	
	
	int i = 0, j = 0;
	int re_turn_cpu = 0;
	//turn on CPUs
	for(i = 1;i <= LAST_WORKING_CPU;i++) {
		re_turn_cpu = turn_cpu(1, i);
		if(re_turn_cpu) {
			printf("something wrong happened in turn_cpu \n");
		}
	}
	//turn down CPUs
	for(i = LAST_WORKING_CPU + 1;i < PHYSICAL_CPUS;i++) {
		turn_cpu(0, i);
		if(re_turn_cpu) {
			printf("something wrong happened in turn_cpu \n");
		}
	}

	setcpu(CONTROLLER_CPU);
	
	
	struct mq_attr attr_ct;
	attr_ct.mq_maxmsg = MAXMSGCTOP;//maximum is 382.
	attr_ct.mq_msgsize = 2048;
	attr_ct.mq_flags = 0;


	int flags = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_ctop1, mqd_p1toc;
	mqd_t mqd_ctop2, mqd_p2toc;
	mqd_t mqd_ctop3, mqd_p3toc;
	mqd_t mqd_ctop4, mqd_p4toc;
	mqd_t mqd_ctop5, mqd_p5toc;
	mqd_t mqd_ctop6, mqd_p6toc;
	mqd_t mqd_ctop7, mqd_p7toc;
	mqd_t mqd_ctop8, mqd_p8toc;



	int mq_return = 0;
	char ctop1[] = "/ctrltop1";
	char p1toc[] = "/p1toctrl";

	char ctop2[] = "/ctrltop2";
	char p2toc[] = "/p2toctrl";

	char ctop3[] = "/ctrltop3";
	char p3toc[] = "/p3toctrl";
	
	char ctop4[] = "/ctrltop4";
	char p4toc[] = "/p4toctrl";
	
	char ctop5[] = "/ctrltop5";
	char p5toc[] = "/p5toctrl";
	
	char ctop6[] = "/ctrltop6";
	char p6toc[] = "/p6toctrl";
	
	char ctop7[] = "/ctrltop7";
	char p7toc[] = "/p7toctrl";
	
	char ctop8[] = "/ctrltop8";
	char p8toc[] = "/p8toctrl";


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
	
	mqd_ctop4 = mq_open(ctop4, flags, PERM, &attr_ct);
	check_return(mqd_ctop4, ctop4, "mq_open");
	mqd_p4toc = mq_open(p4toc, flags, PERM, &attr_ct);
	check_return(mqd_p4toc, p3toc, "mq_open");

	mqd_ctop5 = mq_open(ctop5, flags, PERM, &attr_ct);
	check_return(mqd_ctop5, ctop5, "mq_open");
	mqd_p5toc = mq_open(p5toc, flags, PERM, &attr_ct);
	check_return(mqd_p5toc, p5toc, "mq_open");
	
	mqd_ctop6 = mq_open(ctop6, flags, PERM, &attr_ct);
	check_return(mqd_ctop6, ctop6, "mq_open");
	mqd_p6toc = mq_open(p6toc, flags, PERM, &attr_ct);
	check_return(mqd_p6toc, p6toc, "mq_open");

	mqd_ctop7 = mq_open(ctop7, flags, PERM, &attr_ct);
	check_return(mqd_ctop7, ctop7, "mq_open");
	mqd_p7toc = mq_open(p7toc, flags, PERM, &attr_ct);
	check_return(mqd_p7toc, p7toc, "mq_open");

	mqd_ctop8 = mq_open(ctop8, flags, PERM, &attr_ct);
	check_return(mqd_ctop8, ctop8, "mq_open");
	mqd_p8toc = mq_open(p8toc, flags, PERM, &attr_ct);
	check_return(mqd_p8toc, p8toc, "mq_open");

	
	

	
	struct record pstats[PROC_NUMBER];//PROC_NUMBER = 3 defined in control.h.
	printstar();
	for(i = 0;i < PROC_NUMBER;i++) {
		pstats[i].number = i;
		pstats[i].queues = 0;
		pstats[i].cpu = 1;
		for(j = 0;j < MAXQUEUES;j++) {
			//pstats[i].queuelength[j] = 0;
			//pstats[i].qmax[j] = MAXMSG;
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

	pstats[3].mqd_ctop = mqd_ctop4;
	pstats[3].mqd_ptoc = mqd_p4toc;

	pstats[4].mqd_ctop = mqd_ctop5;
	pstats[4].mqd_ptoc = mqd_p5toc;

	pstats[5].mqd_ctop = mqd_ctop6;
	pstats[5].mqd_ptoc = mqd_p6toc;

	pstats[6].mqd_ctop = mqd_ctop7;
	pstats[6].mqd_ptoc = mqd_p7toc;

	pstats[7].mqd_ctop = mqd_ctop8;
	pstats[7].mqd_ptoc = mqd_p8toc;


	double adj_array[ADJ_ARRAY_EDGES][ADJ_ARRAY_EDGES];
	clear_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array in control.c");
	show_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array in control.c");

	double point_weight[PROC_NUMBER];
	clear_double_series(PROC_NUMBER, point_weight, "point_weight in control.c");
	struct ctrltrans noti_p[PROC_NUMBER];
	for(i = 0;i < PROC_NUMBER;i++) {
		for(j = 0;j < PROC_NUMBER;j++) {
			noti_p[i].statistics[j] = &pstats[j];
		}
	}


	for(i = 0;i < PROC_NUMBER;i++) {
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


	for(i = 0;i < 180;i++) {
		ctrlbuffer.service_number = 1;
		mq_return = mq_send(mqd_ctop1, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop1, "mq_send in controller");

		mq_return = mq_send(mqd_ctop2, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop2, "mq_send in controller");

		mq_return = mq_send(mqd_ctop3, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop3, "mq_send in controller");
		
		mq_return = mq_send(mqd_ctop4, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop4, "mq_send in controller");
		
		mq_return = mq_send(mqd_ctop5, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop5, "mq_send in controller");
		
		mq_return = mq_send(mqd_ctop6, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop6, "mq_send in controller");
		
		mq_return = mq_send(mqd_ctop7, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop7, "mq_send in controller");
		
		mq_return = mq_send(mqd_ctop8, (char *) &ctrlbuffer, sizeof(struct ctrlmsg), 0);
		check_return(mq_return, ctop8, "mq_send in controller");

		printf("\nLOOP i = %d \n\n", i);
		sleep(UPDATE_PERIOD);

	}



	printstar();
	printf("controller has sent all ctrlmsg. \n");
	for(i = 0;i < PROC_NUMBER;i++) {
		printf("i = %d, pstats[%d].queues = %d \n", i, i, pstats[i].queues);
		for(j = 0;j < pstats[i].queues;j++) {
			printf("pstats[%d].i[%d] = %lld \n", i, j, pstats[i].i[j]);
		}
	}
	printf("Now show CPUs that processes work on. \n");
	int cpu_status[PHYSICAL_CPUS];
	clear_int_series(PHYSICAL_CPUS, cpu_status, "cpu_status");
	for(i = 0;i < PROC_NUMBER;i++) {
		printf("p_number = %d, works on CPU %d \n", i, pstats[i].cpu);
		cpu_status[pstats[i].cpu] += 1;
	}
	show_int_series(PHYSICAL_CPUS, cpu_status, "cpu_status");
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
	
	//ctop4
	mq_return = mq_close(mqd_ctop4);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop4, "mq_close");
	mq_return = mq_unlink(ctop4);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop4, "mq_unlink");
	//p4toc
	mq_return = mq_close(mqd_p4toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p4toc, "mq_close");
	mq_return = mq_unlink(p4toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p4toc, "mq_unlink");

	//ctop5
	mq_return = mq_close(mqd_ctop5);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop5, "mq_close");
	mq_return = mq_unlink(ctop5);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop5, "mq_unlink");
	//p5toc
	mq_return = mq_close(mqd_p5toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p5toc, "mq_close");
	mq_return = mq_unlink(p5toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p5toc, "mq_unlink");

	//ctop6
	mq_return = mq_close(mqd_ctop6);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop6, "mq_close");
	mq_return = mq_unlink(ctop6);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop6, "mq_unlink");
	//p6toc
	mq_return = mq_close(mqd_p6toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p6toc, "mq_close");
	mq_return = mq_unlink(p6toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p6toc, "mq_unlink");

	//ctop7
	mq_return = mq_close(mqd_ctop7);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop7, "mq_close");
	mq_return = mq_unlink(ctop7);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop7, "mq_unlink");
	//p7toc
	mq_return = mq_close(mqd_p7toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p7toc, "mq_close");
	mq_return = mq_unlink(p7toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p7toc, "mq_unlink");

	//ctop8
	mq_return = mq_close(mqd_ctop8);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop8, "mq_close");
	mq_return = mq_unlink(ctop8);//returns 0 on success, or -1 on error.
	check_return(mq_return, ctop8, "mq_unlink");
	//p8toc
	mq_return = mq_close(mqd_p8toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p8toc, "mq_close");
	mq_return = mq_unlink(p8toc);//returns 0 on success, or -1 on error.
	check_return(mq_return, p8toc, "mq_unlink");

	for(i = 1;i < PHYSICAL_CPUS;i++) {
		re_turn_cpu = turn_cpu(1, i);
		if(re_turn_cpu) {
			printf("something wrong happened in turn_cpu \n");
		}
	}


	func_quit(proname);
	exit(0);

}


