#ifndef	NFV_H
#define	NFV_H

#define _GNU_SOURCE

#include<stdio.h>//printf()
#include<unistd.h>//fork() sleep()
#include<sys/types.h>//fork()
#include<stdlib.h>//malloc(), calloc()
#include<mqueue.h>
#include<sys/stat.h>//for mode constants---mq_open()
#include<fcntl.h>//for O_* constants---mq_open()
#include<string.h>//perror()
#include<errno.h>//errno
#include<signal.h>//union signal, struct sigevent, sigset_t
#include<pthread.h>//pthread
#include<sys/resource.h>//rlimit
#include<sched.h>//sched_setaffinity


#define PERM 0766//the authority of m_queue.

#define PACKETS 5000//the number of packets that we will send.

#define MAXMSG 300//the stable number of mq_attr.mq_maxmsg.

#define MAXMSGCTOP 10//the queue from controller to process .

#define MAXQUEUES 3//the maximum queues we will use.

#define QUEUERATIO 0.8//the ratio of queue needed to report.

#define CPUS 8

struct transfer {//this needs to be global struct.
	mqd_t mqd_ctop;//queue from controlloer to process.
	mqd_t mqd_ptoc;//queue from process to controller.
	mqd_t mqd_p[5];//queue with other processes.
	int qds;//the number of queues that process receives from other processes.
};

struct datamsg {
	int a;
	int b;
	int c;
	int d;
	char char_data[100];
	float f1;
	float f2;
	float f3;
	float f4;
};

struct ctrlmsg {
	long service_number;/*	1:the controller asks processes to report message about themselves.
				2:the answer message about vertex sent by processes.
				3:the control message that controller sends.
				4:the urgent message that process sends to controller that the process needs a new cpu immediately.
				5:the processes send to controller to tell that the process has finished all work and controller could free the queues between them. */
	pid_t pid_in_ctrlmsg;//the pid of process
	int cpu;//the cpu where the process runs on or the controller wants it to run on
	int edges;//the amount of edges who go into the vertex
	long qsize[MAXQUEUES];//the current packets in every queue received by process.
	long qmaxsize[MAXQUEUES];//the max packets in every queu received by process.
};

struct cpu_sta {
	int cpu[7];
};




void func_quit(char * proname);

void check_return(int return_value, char * proname, char * function_name);//check the return value of mq_* functions

void notifysetup(struct transfer * trans);//set the mq_notify up which is used for first setup or in process_control function.

void process_control(union sigval sv);//processes use notifysetup to set this function to process the control message from controller.

int setcpu(int cpu_id);//set the process to cpu 'cpu_id'.

int getcpu();//get the cpu where the process is working on.

struct cpu_sta checkcpu();//check which CPUs the process can run on.

void printstar();//print a line of stars.'************'.

void printerror();//print "you should not see this line. Please debug your code!!!"

int checkqueue(mqd_t mqd);//return 1 when queue is busy or return 0.

void process_report(struct transfer * parameter);//process use the function to report the status to controller.

void func_quit(char * proname) {
	printf("*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*\n");
	printf("*!*!*!*!*!*!*!* %s quits! *!*!*!*!*!*!*!*\n", proname);
	printf("*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*\n");
}


void check_return(int return_value, char * proname, char * function_name) {
	if(return_value == -1) {
		printf("%s: %s fails: %s, errno = %d. \n", proname, function_name, strerror(errno), errno);
		if(strcmp(function_name, "mq_open") == 0) {
			printf("******************************************\n");
			printf("***mq_oepn failed in %s !***\n", proname);
			printf("******************************************\n");
			exit(0);
		}
	}
	else if((return_value > 0) && (strcmp(function_name, "mq_receive") == 0)) {
		printf("%s: %s receives %d bytes succesfully. \n", proname, function_name, return_value);
	}
	else if((return_value > 0) && (strcmp(function_name, "mq_open") == 0)) {
		printf("%s: %s succeeds, mqd_t = %d. \n", proname, function_name, return_value);
	}
	else if(return_value == 0) {
		printf("%s: %s succeeds. \n", proname, function_name);
	}
	else {
		printf("WARNING!!!WARNING!!!WARNING!!!\nThere is a abnormal condition in check_return needed to be set!!!Please debug!!!\n");
		printf("return_value = %d, fucntion_name = %s, happened in pro %s \n",return_value, function_name, proname);
		printf("WARNING!!!WARNING!!!WARNING!!!\n");
	}
}


void notifysetup(struct transfer * trans) {
	int mq_return = 0;
	struct sigevent mq_notification;

	char funcname[] = "notifysetup";

	printf("now in %s, trans->mqd_ctop = %d \n", funcname, trans->mqd_ctop);
	mq_notification.sigev_notify = SIGEV_THREAD;
	mq_notification.sigev_notify_function = process_control;
	mq_notification.sigev_notify_attributes = NULL;
	mq_notification.sigev_value.sival_ptr = trans;


	mq_return = mq_notify(trans->mqd_ctop, &mq_notification);
	check_return(mq_return, funcname, "mq_notify in notifysetup");

}

void process_control(union sigval sv) {
	struct transfer * parameter;
	parameter = (struct transfer *) sv.sival_ptr;
	int func_re;
	char ptname[] = "process_control";

	printstar();
	printf("Now in %s \n", ptname);

	struct mq_attr rc_mq_attr, q_mq_attr;
	mq_getattr(parameter->mqd_ctop, &rc_mq_attr);

	struct ctrlmsg ctrlbuffer;/*, up_ctrlmsg*/
	struct ctrlmsg * ctrlbufferp;
	char msg_buffer[rc_mq_attr.mq_msgsize];
	printf("msg_buffer size is %lu \n", sizeof(msg_buffer));
	printf("parameter->mqd_ctop = %d \n", parameter->mqd_ctop);

/*	up_ctrlmsg.service_number = 2;
	up_ctrlmsg.pid_in_ctrlmsg = getpid();
	up_ctrlmsg.cpu = getcpu();*/
	printf("%s is working in pid %d, cpu %d\n", ptname, getpid(), getcpu());

	notifysetup(parameter);

	while ((func_re = mq_receive(parameter->mqd_ctop, msg_buffer, rc_mq_attr.mq_msgsize, 0)) >= 0) {
		check_return(func_re, ptname, "mq_receive");
		ctrlbufferp = (struct ctrlmsg *) msg_buffer;
		ctrlbuffer = * ctrlbufferp;
		printf("%s received ctrlbuffer.service_number is %ld \n", ptname, ctrlbuffer.service_number);
	}

//	check_return(func_re, ptname, "mq_receive enough");
	if (errno != EAGAIN) {//in nonblock mode, "errno = EAGAIN" means that there is no message in queue.	EAGAIN = 11
		check_return(func_re, ptname, "mq_receive");
		printf("mq_receive exits unnormally");
		exit(0);/* Unexpected error */
	}

	printstar();

	switch(ctrlbuffer.service_number)
	{
		case 1:
/*			printf("process received ctrlmsg.service_number = 1 \n");
			up_ctrlmsg.cpu = getcpu();
			up_ctrlmsg.edges = parameter->qds;
			int i_edges = 0;
			for(i_edges = 0;i_edges < up_ctrlmsg.edges;i_edges++) {
				mq_getattr(parameter->mqd_p[i_edges], &q_mq_attr);
				up_ctrlmsg.qsize[i_edges] = q_mq_attr.mq_curmsgs;
				up_ctrlmsg.qmaxsize[i_edges] = q_mq_attr.mq_maxmsg;
			}
			func_re = mq_send(parameter->mqd_ptoc, (char *) &up_ctrlmsg, sizeof(struct ctrlmsg), 0);
			check_return(func_re, ptname, "mq_send");*/
			process_report(parameter);

			break;//we will process this later
		case 3:
			printf("send is working on CPU %d \n",getcpu());
			checkcpu();
			setcpu(ctrlbuffer.cpu);
			checkcpu();
			printf("pid %d has been set to core %d \n", getpid(), ctrlbuffer.cpu);
			break;
	}

	printf("finish a process control \n");
	printstar();
	pthread_exit(NULL);
}



int setcpu(int cpu_id) {
	printf("setcpu's pid is %d\n", getpid());
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpu_id, &mask);
	int check;
	check = sched_setaffinity(getpid(), sizeof(mask), &mask);
	check_return(check, "setcpu","sched_setaffinity");
	return 0;
}//  set cpu affinity

int getcpu() {
	printf("getcpu's pid is %d\n", getpid());
	char func_name[] = "getcpu";
	int func_return = 0;
	cpu_set_t set;
	CPU_ZERO(&set);
	func_return = sched_getaffinity(getpid(), sizeof(cpu_set_t), &set);
	check_return(func_return, func_name, "sched_getaffinity");
	int i = 0;
	int cpunow = -1;
	int test_cpu = 0;
	int count = 0;
	for(i = 0;i < 8;i++) {
		test_cpu = CPU_ISSET(i, &set);
		//printf("i = %d, test_cpu = %d \n", i, test_cpu);
		if(test_cpu != 0) {
			cpunow = i;
			count++;
		}
	}
	if(count == 1) {
		return cpunow;
	}
	else if(count > 1) {
		return 8;//there is more than one cpu being used.
	}
	else return -1;//something wrong.
}

struct cpu_sta checkcpu() {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	int func_re = 0;
	func_re = sched_getaffinity(getpid(), sizeof(cpu_set_t), &cpuset);
	check_return(func_re, "checkcpu", "sched_getaffinity");
	int i = 0;
	int test_cpu = 0;
	struct cpu_sta statistics;
	for(i = 0;i <= 7;i++) {
		test_cpu = CPU_ISSET(i, &cpuset);
		if(test_cpu != 0) {
			printf("test_cpu = %d, process can work on CPU %d\n", test_cpu, i);
			statistics.cpu[i] = 1;
		}
		else {
			printf("test_cpu = %d, process can't work on CPU %d\n", test_cpu, i);
			statistics.cpu[i] = 0;
		}
	}
	return statistics;
}

void printstar() {
	printf("******************************************\n");
}

void printerror() {
	printf("you should not see this line. Please debug your code!!!\n");
}

int checkqueue(mqd_t mqd) {
	char proname[] = "checkqueue";
	struct mq_attr q_attr;
	int func_re;
	func_re = mq_getattr(mqd, &q_attr);
	check_return(func_re, proname, "mq_getattr");

	float ratio = 0;
	ratio = q_attr.mq_curmsgs / q_attr.mq_maxmsg;
	if(ratio > QUEUERATIO) {
		return 1;
	}
	return 0;
}

void process_report(struct transfer * parameter) {
	char funcname[] = "process_report";
	struct ctrlmsg up_ctrlmsg;
	struct mq_attr q_mq_attr;
	int func_re = 0;
	up_ctrlmsg.service_number = 2;
	up_ctrlmsg.cpu = getcpu();
	up_ctrlmsg.edges = parameter->qds;
	up_ctrlmsg.pid_in_ctrlmsg = getpid();
	int i_edges = 0;
	for(i_edges = 0;i_edges < up_ctrlmsg.edges;i_edges++) {
		mq_getattr(parameter->mqd_p[i_edges], &q_mq_attr);
		up_ctrlmsg.qsize[i_edges] = q_mq_attr.mq_curmsgs;
		up_ctrlmsg.qmaxsize[i_edges] = q_mq_attr.mq_maxmsg;
	}
	func_re = mq_send(parameter->mqd_ptoc, (char *) &up_ctrlmsg, sizeof(struct ctrlmsg), 0);
	check_return(func_re, funcname, "mq_send");
}

#endif
