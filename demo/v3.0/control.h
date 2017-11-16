#ifndef CONTROL_H
#define CONTROL_H

#include"nfv.h"
#include"extended_KL.h"
#include"series_array.h"

#define PROC_NUMBER 8

#define ADJ_ARRAY_EDGES 8//same as PROC_NUMBER

#define IDLE_THRESHOLD 0.25//point_weight less than IDLE_THRESHOLD is seen as idle.

#define QUEUE_WEIGHT 0.1//every queue will add QUEUE_WEIGHT to point_weight.

#define QUEUE_DEFAULT_WEIGHT 0.2//every queue has an queue_default_weight in adj_array.

#define POINT_WEIGHT_ALPHA 0.5//point_weight = alpha * point_weight_old + (1 - alpha) * p_cpu_usage used in update_point_weight

#define CPU_IDLE_TIMES 2//cpu who is idle for 'CPU_IDLE_TIMES' times can be combined.

#define UPDATE_PERIOD 1//controller update all statistics in every UPDATE_PERIOD second.


struct record {//we wiil change and add more to this part!
	int number;//distinguish the progress.
	mqd_t mqd_ctop;//queue controller to progress.
	mqd_t mqd_ptoc;//queue progress to controller.
	pid_t pid_in_record;//progress's pid.

	int cpu;//the cpu where the process runs on.
	int queues;//the number of queues which the process receive.
	//long queuelength[MAXQUEUES];
	//long qmax[MAXQUEUES];//max packets in a queue.
	
	double cpu_usage;//the process cpu usage.
	long long int i[MAXQUEUES];
	long long int throughput[MAXQUEUES];
};


struct check_report{
	int type;//	3:a cpu need to be splited;
		//	2:nothing needs to  be done
		//	1:some cpus need to combine;
	//int count;//number of CPUs can be combined.
	//int cpus[CPUS];
	int cpus[2];
	//int cpu_status[CPUS];
};


struct ctrltrans {
/*	mqd_t mqd_ctop;//queue from controlloer to process.
	mqd_t mqd_ptoc;//queue from process to controller.
*/
	int p_number;
	struct record * statistics[PROC_NUMBER];
	double (*adj_array) [ADJ_ARRAY_EDGES];
	double * point_weight;
};

//we will write notifysetup function for every processes;
void ctrl_notifysetup(struct ctrltrans * trans);//notifysetup for controller.
void controller_control(union sigval sv);//control function for controller.

void update_adj_array(struct record * statistics[PROC_NUMBER], double adj_array[ADJ_ARRAY_EDGES][ADJ_ARRAY_EDGES]);
void update_point_weight(struct record * statistics[PROC_NUMBER], double * point_weight);

struct check_report check_status(struct record * statistics[PROC_NUMBER], double * point_weight);
void send_ctrl_down(int msg_type, int work_cpu, struct record * p_statistics);
int find_idle_cpu(struct record * statistics[PROC_NUMBER]);
void show_working_cpu(struct record * statistics[PROC_NUMBER]);//show which CPU processes are working on
void changefrequency(long frequency, int CPU);//change CPU's frequency


double get_cpu_usage(int pid);

void ctrl_notifysetup(struct ctrltrans * trans) {
	int mq_return = 0;
	struct sigevent mq_notification;

	char funcname[] = "ctrl_notifysetup";


	printf("now in %s, trans->p_number = %d \n", funcname, trans->p_number);
	mq_notification.sigev_notify = SIGEV_THREAD;
	mq_notification.sigev_notify_function = controller_control;
	mq_notification.sigev_notify_attributes = NULL;
	mq_notification.sigev_value.sival_ptr = trans;




	mq_return = mq_notify(trans->statistics[trans->p_number]->mqd_ptoc, &mq_notification);
	check_return(mq_return, funcname, "mq_notify in ctrl_notifysetup");
	printf("now get out of ctrl_notifysetup\n");

}

void controller_control(union sigval sv) {
	printstar();
	struct ctrltrans * parameter;
	parameter = (struct ctrltrans *) sv.sival_ptr;
	int func_re;
	char ptname[] = "controller_control";

	static int p_report_status[PROC_NUMBER] = {0, 0, 0};

	printf("********controller_control begins!!!!********\n");
	//printf("Now in %s \n", ptname);


	struct mq_attr rc_mq_attr;
	mq_getattr(parameter->statistics[parameter->p_number]->mqd_ptoc, &rc_mq_attr);

	struct ctrlmsg ctrlbuffer;
	struct ctrlmsg * ctrlbufferp;
	char msg_buffer[rc_mq_attr.mq_msgsize];
	//printf("msg_buffer size is %lu \n", sizeof(msg_buffer));
	//printf("parameter->statistics[%d]->mqd_ptoc = %d \n", parameter->p_number, parameter->statistics[parameter->p_number]->mqd_ptoc);

	ctrl_notifysetup(parameter);

	while ((func_re = mq_receive(parameter->statistics[parameter->p_number]->mqd_ptoc, msg_buffer, rc_mq_attr.mq_msgsize, 0)) >= 0) {//get the newest ctrlmsg.
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


	int i_edges = 0;
	switch(ctrlbuffer.service_number)
	{
		case 2://the answer message about process sent by processes.
			//*************************update record************************************
			parameter->statistics[parameter->p_number]->cpu = ctrlbuffer.cpu;
			parameter->statistics[parameter->p_number]->queues = ctrlbuffer.edges;
			parameter->statistics[parameter->p_number]->pid_in_record = ctrlbuffer.pid_in_ctrlmsg;
			

			for(i_edges = 0;i_edges < ctrlbuffer.edges;i_edges++) {
				//parameter->statistics[parameter->p_number]->queuelength[i_edges] = ctrlbuffer.qsize[i_edges];
				//parameter->statistics[parameter->p_number]->qmax[i_edges] = ctrlbuffer.qmaxsize[i_edges];
				parameter->statistics[parameter->p_number]->throughput[i_edges] = ctrlbuffer.i[i_edges] - parameter->statistics[parameter->p_number]->i[i_edges];
				parameter->statistics[parameter->p_number]->i[i_edges] = ctrlbuffer.i[i_edges];
			}
			printf("Show the ctrlmsg sent up.\n");
			if(parameter->statistics[parameter->p_number]->queues <= 1) {
				printf("the process %d pid = %d, runs on cpu %d, it receives %d queue \n", parameter->p_number, parameter->statistics[parameter->p_number]->pid_in_record, parameter->statistics[parameter->p_number]->cpu, parameter->statistics[parameter->p_number]->queues);
			}
			else {
				printf("the process %d pid = %d, runs on cpu %d, it receives %d queues \n", parameter->p_number, parameter->statistics[parameter->p_number]->pid_in_record, parameter->statistics[parameter->p_number]->cpu, parameter->statistics[parameter->p_number]->queues);
			}

			for(i_edges = 0;i_edges < parameter->statistics[parameter->p_number]->queues;i_edges++) {
				//printf("the queue %d has %ld packets of most %ld packets \n", i_edges, parameter->statistics[parameter->p_number]->queuelength[i_edges], parameter->statistics[parameter->p_number]->qmax[i_edges]);
				printf("the queue %d has passed %lld packets in the past %ds, i = %lld \n", i_edges, parameter->statistics[parameter->p_number]->throughput[i_edges], UPDATE_PERIOD, parameter->statistics[parameter->p_number]->i[i_edges]);
			}
			p_report_status[parameter->p_number]++;
			int i = 0;
			int update_status = 1;//0 can not update now, 1 can update now.
			for(i = 0;i < PROC_NUMBER;i++) {
				if(p_report_status[i] == 0) {
					update_status = 0;
					break;
				}
			}
			if(update_status == 1) {
				printf("\nAll update messages have been sent to controller, begin update and manage\n\n");
				clear_int_series(PROC_NUMBER, p_report_status, "p_report_status");
				//then update the adj_array/point_weight and do the combine check.
				update_adj_array(parameter->statistics, parameter->adj_array);
				update_point_weight(parameter->statistics, parameter->point_weight);
				//check status of all cpus.
				//check if we could combine some CPUs.
				struct check_report report;
				report = check_status(parameter->statistics, parameter->point_weight);
				if(report.type == 1) {
				//send ctrlmsg to combine processes.
					for(i = 0;i < PROC_NUMBER;i++) {
						if(parameter->statistics[i]->cpu == report.cpus[1]) {
							printf("set process %d to work on CPU %d\n", report.cpus[0], report.cpus[0]);
							send_ctrl_down(3, report.cpus[0], parameter->statistics[i]);
							parameter->statistics[i]->cpu = report.cpus[0];
						}
					}
					//changefrequency
					changefrequency(800000, report.cpus[1]);//let the cpu be cool
				}
				show_working_cpu(parameter->statistics);
			}
			break;

		case 4://the urgent message that process sends to controller that the process needs a new cpu immediately.
//*************************update record************************************
			/*parameter->statistics[parameter->p_number]->cpu = ctrlbuffer.cpu;
			parameter->statistics[parameter->p_number]->queues = ctrlbuffer.edges;
			parameter->statistics[parameter->p_number]->pid_in_record = ctrlbuffer.pid_in_ctrlmsg;*/

			for(i_edges = 0;i_edges < ctrlbuffer.edges;i_edges++) {
				//parameter->statistics[parameter->p_number]->queuelength[i_edges] = ctrlbuffer.qsize[i_edges];
				//parameter->statistics[parameter->p_number]->qmax[i_edges] = ctrlbuffer.qmaxsize[i_edges];
			}
			//printf("now show the result of ctrlbuffer.\n");
			//printf("the process %d runs on cpu %d, it receives %d queues \n", parameter->statistics[parameter->p_number]->pid_in_record, parameter->statistics[parameter->p_number]->cpu, parameter->statistics[parameter->p_number]->queues);
			/*for(i_edges = 0;i_edges < parameter->statistics[parameter->p_number]->queues;i_edges++) {
				printf("the queue %d has %ld packets of most %ld packets \n", i_edges, parameter->statistics[parameter->p_number]->queuelength[i_edges], parameter->statistics[parameter->p_number]->qmax[i_edges]);
			}*/
			static int partitioning_cpu = -1;
			if(partitioning_cpu != -1) {
				printf("CPU %d is being splited, please p_number = %d on CPU %d(ctrlbuffer)/%d(record.cpu) wait for a moment.\n", partitioning_cpu, parameter->p_number, ctrlbuffer.cpu, parameter->statistics[parameter->p_number]->cpu);
				break;
			}
			partitioning_cpu = parameter->statistics[parameter->p_number]->cpu;
			//update the adj_array/point_weight and do the extended_KL algotithm.
			update_adj_array(parameter->statistics, parameter->adj_array);
			update_point_weight(parameter->statistics, parameter->point_weight);
			printf("p_number = %d is working hard, CPU%d needs to be splited!\n", parameter->p_number, parameter->statistics[parameter->p_number]->cpu);
			show_working_cpu(parameter->statistics);


			int idle_cpu = find_idle_cpu(parameter->statistics);
			if(idle_cpu != -1) {
				//get all processes on the urgent cpu
				int p_on_hot_cpu[PROC_NUMBER];//store processes who work on the hot CPU.
				int count_p_on_hot_cpu = 0;
				int i = 0;
				int j = 0;
				for(i = 0;i < PROC_NUMBER;i++) {
					if(parameter->statistics[i]->cpu == parameter->statistics[parameter->p_number]->cpu) {
						p_on_hot_cpu[count_p_on_hot_cpu] = i;//find processes who work on the hot CPU and write their p_number in the p_on_hot_cpu series.include the process who sent the ctrlmsg.
						count_p_on_hot_cpu++;
					}
				}
				if(count_p_on_hot_cpu == 1) {
					//You are using a CPU alone, nothing could be done, sorry :(.
					printf("p_number = %d is using CPU%d alone but still working hard.\n", parameter->p_number, parameter->statistics[parameter->p_number]->cpu);//maybe we could tell him to be quiet!!!!!!!!!!!!!!!
					//send_ctrl_down(5, 0, parameter->statistics[parameter->p_number]);
					partitioning_cpu = -1;
					break;
				}
				//generate a new adj_array
				double new_adj_array[count_p_on_hot_cpu][count_p_on_hot_cpu];
				clear_double_array(count_p_on_hot_cpu, new_adj_array, "new_adj_array in control.h");
				for(i = 0;i < count_p_on_hot_cpu;i++) {
					for(j = 0;j < count_p_on_hot_cpu;j++)
						new_adj_array[i][j] = parameter->adj_array[p_on_hot_cpu[i]][p_on_hot_cpu[j]];
				}
				//generate a new point_weight
				double new_point_weight[count_p_on_hot_cpu];
				for(i = 0;i < count_p_on_hot_cpu;i++) {
					new_point_weight[i] = parameter->point_weight[p_on_hot_cpu[i]];
				}
				//set the new adj_array to KL_partition function
				int * cut_order = KL_partition(count_p_on_hot_cpu, new_adj_array, new_point_weight);
				//get the return cut_order and send ctrlmsg to set cpu.
				int set_edges =(count_p_on_hot_cpu + count_p_on_hot_cpu%2) / 2;

				for(i = 0;i < set_edges;i++) {
					printf("set process %d to work on CPU %d\n", p_on_hot_cpu[cut_order[i]], idle_cpu);
					send_ctrl_down(3, idle_cpu, parameter->statistics[p_on_hot_cpu[cut_order[i]]]);
					parameter->statistics[p_on_hot_cpu[cut_order[i]]]->cpu = idle_cpu;
				}
				//changefrequency
				changefrequency(3401000, idle_cpu);
				free(cut_order);
				show_working_cpu(parameter->statistics);
				usleep(1000);//wait for ctrlmsg 
			}
			else {
				printf("There is no more idle CPUs, please wait for a moment.\n");
			}
			partitioning_cpu = -1;
			break;
	}

	printf("\n********finish a controller control!!!!******** \n\n");
	pthread_exit(NULL);
}

void show_working_cpu(struct record * statistics[PROC_NUMBER]) {
	int i = 0;
	for(i = 0; i < PROC_NUMBER; i++) {
		printf("p_number = %d is working on CPU %d \n", i, statistics[i]->cpu);
	}

}


void changefrequency(long frequency, int cpu) {//frequency's unit is kHz
	char cmdhead[] = "echo 1 | sudo -S ";
	char cmdgovernor[200];
	sprintf(cmdgovernor, "cpufreq-set -c %d -g userspace", cpu);
	char cmdfrequency[200];
	sprintf(cmdfrequency, "cpufreq-set -f %ld -c %d", frequency, cpu);

	char * cmdbash1 = (char *) calloc(1, strlen(cmdhead) + strlen(cmdgovernor) + 1);//1 = sizeof(char), + 1 for '\0'
	strcat(cmdbash1, cmdhead);
	strcat(cmdbash1, cmdgovernor);
	printf("cmdbash1 = %s\n", cmdbash1);

	char * cmdbash2 = (char *) calloc(1, strlen(cmdhead) + strlen(cmdfrequency) + 1);//1 = sizeof(char), + 1 for '\0'
	strcat(cmdbash2, cmdhead);
	strcat(cmdbash2, cmdfrequency);
	printf("cmdbash2 = %s\n", cmdbash2);

	FILE * bashPipe1 = popen(cmdbash1, "r");
	FILE * bashPipe2 = popen(cmdbash2, "r");
	pclose(bashPipe1);
	pclose(bashPipe2);
	free(cmdbash1);
	free(cmdbash2);
}




//we need a better algorithm here.
struct check_report check_status(struct record * statistics[PROC_NUMBER], double * point_weight) {
	static int old_cpu_status[CPUS] = {-100, 0, 0, 0, 0, 0, 0, -100};
	int i = 0;
	int j = 0;
	struct check_report report;//return value
	//int cpu_count[4] = {0, 0, 0, 0};//this 4 means 0~1 status.
	int count_idle_cpu = 0;
	int p_on_cpu[CPUS];
	clear_int_series(CPUS, p_on_cpu, "p_on_cpu in check_status");
	double cpu_status[CPUS];//the number is bigger, cpu is hotter.
	clear_double_series(CPUS, cpu_status, "cpu_status in check_status");
	for(i = 0;i < PROC_NUMBER;i++) {
		//printf("cpu_status[statistics[%d]->cpu] = %f, point_weight[%d] = %f\n", i, cpu_status[statistics[i]->cpu], i, point_weight[i]);
		cpu_status[statistics[i]->cpu] += point_weight[i];//we may set edge_cut into this.
		p_on_cpu[statistics[i]->cpu] += 1;
	}
	show_double_series(CPUS, cpu_status, "cpu_status in check_status");
	show_int_series(CPUS, p_on_cpu, "p_on_cpu in check_status");


	for(i = 1;i < CPUS - 1;i++) {////i from 1 to 6.CPU0 is only for PACKET_SENDING!!!CPU7 for controller.
		if(p_on_cpu[i] > 0) {
			if(cpu_status[i] < IDLE_THRESHOLD) {
					old_cpu_status[i]++;
					if(old_cpu_status[i] >= CPU_IDLE_TIMES) {
						count_idle_cpu++;
					}
			}
			else {
				old_cpu_status[i] = 0;
			}
		}
		else {
			old_cpu_status[i] = 0;
		}
	}
	show_int_series(CPUS, old_cpu_status, "old_cpu_status");
	if(count_idle_cpu <= 1) {
		report.type = 2;
		return report;
	}
	else {
		report.type = 1;
		for(i = 1, j = 0;i < CPUS - 1;i++) {//i from 1 to 6
			if(old_cpu_status[i] >= CPU_IDLE_TIMES) {
				report.cpus[j] = i;
				j++;
				old_cpu_status[i] = 0;
				if(j > 1) {
					break;
				}
			}
		}
		printf("CPU%d and CPU%d are idle and can be combined.\n", report.cpus[0], report.cpus[1]);
		return report;
	}

	printstar();
	printstar();
	printstar();
	printf("something wrong happened in check_status, please debug!!!\n");
	printstar();
	printstar();
	printstar();
	return report;
}







int find_idle_cpu(struct record * statistics[PROC_NUMBER]) {
	int idle_cpu = -1;
	int cpu_status[CPUS];//	0:cpu is not working
			//	1:cpu is cool and can be merged.
			//	2:cpu is warm and can not be merged but still not need to be splited.
			//	3:cpu is hot and need to be splited.
	clear_int_series(CPUS, cpu_status, "cpu_status in find_idle_cpu");
	int i = 0;
	for(i = 0;i < PROC_NUMBER;i++) {
		cpu_status[statistics[i]->cpu] = 2;
	}
	for(i = 1;i < 7;i++) {//!!!!we will leave CPU0 for packet_sending.o!!!!Maybe 7 for control.o.
		if(cpu_status[i] == 0) {
			idle_cpu = i;
			break;
		}
	}

	return idle_cpu;
}



void send_ctrl_down(int msg_type, int work_cpu, struct record * p_statistics) {
	char funcname[] = "ctrl_down";
	struct ctrlmsg down_ctrlmsg;
	int func_re = 0;
	down_ctrlmsg.service_number = msg_type;
	down_ctrlmsg.cpu = work_cpu;
	func_re = mq_send(p_statistics->mqd_ctop, (char *) &down_ctrlmsg, sizeof(struct ctrlmsg), 0);
	check_return(func_re, funcname, "mq_send");
}


void update_adj_array(struct record * statistics[PROC_NUMBER], double adj_array[ADJ_ARRAY_EDGES][ADJ_ARRAY_EDGES]) {
	printf("now start to update the adj_array\n");
	int i = 0;
	int j = 0;
	long long int Tput = 1;
	for(i = 0;i < PROC_NUMBER;i++) {
		for(j = 0;j < statistics[i]->queues;j++) {
			Tput += statistics[i]->throughput[j];
		}
	}
	adj_array[0][1] = adj_array[1][0] = ((double) statistics[1]->throughput[0]) / ((double) Tput);//p1top2
	adj_array[0][2] = adj_array[2][0] = ((double) statistics[2]->throughput[0]) / ((double) Tput);//p1top3
	adj_array[1][4] = adj_array[4][1] = ((double) statistics[4]->throughput[0]) / ((double) Tput);//p2top5
	adj_array[1][3] = adj_array[3][1] = ((double) statistics[3]->throughput[0]) / ((double) Tput);//p2top4
	adj_array[2][3] = adj_array[3][2] = ((double) statistics[3]->throughput[1]) / ((double) Tput);//p3top4
	adj_array[2][6] = adj_array[6][2] = ((double) statistics[6]->throughput[1]) / ((double) Tput);//p3top7
	adj_array[4][5] = adj_array[5][4] = ((double) statistics[5]->throughput[0]) / ((double) Tput);//p5top6
	adj_array[3][5] = adj_array[5][3] = ((double) statistics[5]->throughput[1]) / ((double) Tput);//p4top6
	adj_array[3][6] = adj_array[6][3] = ((double) statistics[6]->throughput[0]) / ((double) Tput);//p4top7
	adj_array[5][7] = adj_array[7][5] = ((double) statistics[7]->throughput[0]) / ((double) Tput);//p6top8
	adj_array[6][7] = adj_array[7][6] = ((double) statistics[7]->throughput[1]) / ((double) Tput);//p7top8
	show_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array after update");
}




/*
void update_adj_array(struct record * statistics[PROC_NUMBER], double adj_array[ADJ_ARRAY_EDGES][ADJ_ARRAY_EDGES]) {
	//show_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array before update in update_adj_array");
	printstar();
	int i = 0;
	for(i = 0;i < PROC_NUMBER;i++) {
		printf("statistics[%d]->queuelength[0] = %ld\n", i, statistics[i]->queuelength[0]);
	}
	printstar();
	//we will change this to another way to replace the adj_array.we need to use throughput to replace the queuelength!!!!!!!!!!!!!!
	adj_array[0][1] = adj_array[1][0] = ((double) statistics[1]->queuelength[0]) / ((double) statistics[0]->qmax[0]) + QUEUE_DEFAULT_WEIGHT;//p1top2
	adj_array[0][2] = adj_array[2][0] = ((double) statistics[2]->queuelength[0]) / ((double) statistics[0]->qmax[0]) + QUEUE_DEFAULT_WEIGHT;//p1top3
	show_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array after update");
}*/



void update_point_weight(struct record * statistics[PROC_NUMBER], double * point_weight) {
	int i = 0;
	for(i = 0;i < PROC_NUMBER;i++) {
		point_weight[i] = statistics[i]->cpu_usage = POINT_WEIGHT_ALPHA * statistics[i]->cpu_usage + (1 - POINT_WEIGHT_ALPHA) * get_cpu_usage(statistics[i]->pid_in_record);
	}
	show_double_series(ADJ_ARRAY_EDGES, point_weight, "point_weight after update in update_point_weight");
}


double get_cpu_usage(int pid) {
	char cmdtop[100];
	sprintf(cmdtop, "top -b -n 1 -d 0.000001 -p %d | awk 'NR==8{print $9}'", pid);
	char usage[10];
	double cpuusage = 0;

	FILE * bashPipe = popen(cmdtop, "r");
	if(bashPipe != NULL) {
		fgets(usage, 10, bashPipe);
		cpuusage = atof(usage);
		printf("pid = %d, double cpu usage = %f\n", pid, cpuusage);
	}
	else {
		printf("there is nothing to show in get_cpu_usage for pid = %d\n", pid);
	}
	pclose(bashPipe);
	return cpuusage / 100;
}





#endif
