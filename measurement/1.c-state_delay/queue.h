#define _GNU_SOURCE

#include<stdio.h>//printf()
#include<unistd.h>//fork() sleep()
#include<sys/types.h>//fork()
#include<stdlib.h>//malloc()
#include<mqueue.h>
#include<sys/stat.h>//for mode constants---mq_open()
#include<fcntl.h>//for O_* constants---mq_open()
#include<string.h>//perror()
#include<errno.h>//errno
#include<signal.h>//union signal, struct sigevent, sigset_t
#include<pthread.h>//pthread
#include<sys/resource.h>//rlimit
#include<sched.h>//sched_setaffinity
#include<sys/time.h>//for timeval, gettimeofday()

#define PHYSICAL_CPUS 40

#define CORES_PER_CPU 10

#define SEND_CPU 4
#define FWD_CPU 5
#define RECEIVE_CPU 6

#define USLEEP_FREQUENCY 1000

#define USLEEP_TIME 10


#define SHOW_FREQUENCY 1000

#define PACKETS 200

#define PERM 0666

struct datamsg{
	long long int i;
	struct timeval time;
};

int setcpu(int cpu_id);//set the process to work on cpu 'cpu_id'.
void check_return(int return_value, char * proname, char * function_name);//check the return value of mq_* functions
void printnewline(void);


void check_return(int return_value, char * proname, char * function_name) {
	if(return_value == -1) {
		printf("%s: %s fails: %s, errno = %d. \n", proname, function_name, strerror(errno), errno);
		if(strncmp(function_name, "mq_open", 7) == 0) {
			printf("******************************************\n");
			printf("***mq_oepn failed in %s !***\n", proname);
			printf("******************************************\n");
			exit(0);
		}
	}
	else if((return_value > 0) && (strncmp(function_name, "mq_receive", 10) == 0)) {//so we can input "mq_receive blabla"
		printf("%s: %s receives %d bytes succesfully. \n", proname, function_name, return_value);
	}
	else if((return_value > 0) && (strncmp(function_name, "mq_open", 7) == 0)) {//so we can input "mq_open blabla"
		printf("%s: %s succeeds, mqd_t = %d. \n", proname, function_name, return_value);
	}
	else if(return_value == 0) {
		/*if(strlen(function_name)) {
			printf("%s: %s succeeds. \n", proname, function_name);
		}*/
	}
	else {
		printf("WARNING!!!WARNING!!!WARNING!!!\nThere is an abnormal condition in check_return needed to be set!!!Please debug!!!\n");
		printf("return_value = %d, fucntion_name = %s, happened in pro %s \n",return_value, function_name, proname);
		printf("WARNING!!!WARNING!!!WARNING!!!\n");
	}
}

int setcpu(int cpu_id) {//set the process to work on cpu 'cpu_id'.
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpu_id, &mask);
	CPU_SET(cpu_id + PHYSICAL_CPUS, &mask);
	int check;
	check = sched_setaffinity(getpid(), sizeof(mask), &mask);
	check_return(check, "setcpu", "sched_setaffinity");

	return 0;
}//  set cpu affinity

void printnewline(void) {
	printf("\n");
}
