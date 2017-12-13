#define _GNU_SOURCE
#define _USE_GNU

#include<sched.h>//sched_setaffinity
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

//#define CPUS 8
#define PHYSICAL_CPUS 40

#define SLEEP_FREQUENCY 1000

#define LOOP_CPU 5

#define LOOP0_CPU 3

#define LOOP1_CPU 4

#define LOOP2_CPU 5

#define LOOP3_CPU 6

#define LOOP4_CPU 7

#define LOOP5_CPU 8

int setcpu(int cpu_id) {//set the process to work on cpu 'cpu_id'.
	printf("setcpu's pid is %d, set to CPU %d \n", getpid(), cpu_id);
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpu_id, &mask);
	CPU_SET(cpu_id + PHYSICAL_CPUS, &mask);
	sched_setaffinity(getpid(), sizeof(mask), &mask);
	return 0;
}//  set cpu affinity

