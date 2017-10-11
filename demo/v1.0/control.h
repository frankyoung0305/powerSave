#ifndef CONTROL_H
#define CONTROL_H

#include"nfv.h"
#include"extended_KL.h"
#define PROS_NUMBER 3
#define ADJ_ARRAY_EDGES 3//same as PROS_NUMBER
#define IDLE_THRESHOLD 0.2//point_weight less than IDLE_THRESHOLD is seen as idle.
#define QUEUE_WEIGHT 0.1//every queue will add QUEUE_WEIGHT to point_weight.


struct record {//we wiil change and add more to this part!
	int number;//distinguish the progress.
	mqd_t mqd_ctop;//queue controller to progress.
	mqd_t mqd_ptoc;//queue progress to controller.
	pid_t pid_in_record;//progress's pid.

	int cpu;//the cpu where the process runs on.
	int queues;//the number of queues which the process receive.
	long queuelength[MAXQUEUES];
	long qmax[MAXQUEUES];//max packets in a queue.
	double cpu_usage;

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
	struct record * statistics[PROS_NUMBER];
	double (*adj_array) [ADJ_ARRAY_EDGES];
	double * point_weight;
};

//we will write notifysetup function for every processes;
void ctrl_notifysetup(struct ctrltrans * trans);//notifysetup for controller.
void controller_control(union sigval sv);//control function for controller.

void update_adj_array(struct record * statistics[PROS_NUMBER], double adj_array[ADJ_ARRAY_EDGES][ADJ_ARRAY_EDGES]);
void update_point_weight(struct record * statistics[PROS_NUMBER], double * point_weight);

void changefrequency(long frequency, int CPU);
struct check_report check_status(struct record * statistics[PROS_NUMBER], double * point_weight);
void send_ctrl_down(int idle_cpu, struct record * p_statistics);
int find_idle_cpu(struct record * statistics[PROS_NUMBER]);

void ctrl_notifysetup(struct ctrltrans * trans) {
	int mq_return = 0;
	struct sigevent mq_notification;

	char funcname[] = "ctrl_notifysetup";

	printstar();
	printf("now in %s, trans->p_number = %d \n", funcname, trans->p_number);
	mq_notification.sigev_notify = SIGEV_THREAD;
	mq_notification.sigev_notify_function = controller_control;
	mq_notification.sigev_notify_attributes = NULL;
	mq_notification.sigev_value.sival_ptr = trans;
	printstar();



	mq_return = mq_notify(trans->statistics[trans->p_number]->mqd_ptoc, &mq_notification);
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
	mq_getattr(parameter->statistics[parameter->p_number]->mqd_ptoc, &rc_mq_attr);

	struct ctrlmsg ctrlbuffer, down_ctrlmsg;
	struct ctrlmsg * ctrlbufferp;
	char msg_buffer[rc_mq_attr.mq_msgsize];
	printf("msg_buffer size is %lu \n", sizeof(msg_buffer));
	printf("parameter->statistics[%d]->mqd_ptoc = %d \n", parameter->p_number, parameter->statistics[parameter->p_number]->mqd_ptoc);

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
			
			printstar();
			for(i_edges = 0;i_edges < ctrlbuffer.edges;i_edges++) {
				parameter->statistics[parameter->p_number]->queuelength[i_edges] = ctrlbuffer.qsize[i_edges];
				parameter->statistics[parameter->p_number]->qmax[i_edges] = ctrlbuffer.qmaxsize[i_edges];
			}
			printf("now show the result of ctrlbuffer.\n");
			printf("the process %d pid = %d, runs on cpu %d, it receives %d queues \n", parameter->p_number, parameter->statistics[parameter->p_number]->pid_in_record, parameter->statistics[parameter->p_number]->cpu, parameter->statistics[parameter->p_number]->queues);
			

			for(i_edges = 0;i_edges < parameter->statistics[parameter->p_number]->queues;i_edges++) {
				printf("the queue %d has %ld packets of most %ld packets \n", i_edges, parameter->statistics[parameter->p_number]->queuelength[i_edges], parameter->statistics[parameter->p_number]->qmax[i_edges]);
			}
			//then update the adj_array/point_weight and do the combine check.
			update_adj_array(parameter->statistics, parameter->adj_array);
			update_point_weight(parameter->statistics, parameter->point_weight);
			//check status of all cpus.
			//check if we could combine some CPUs.
			struct check_report report;
			report = check_status(parameter->statistics, parameter->point_weight);
			if(report.type == 1) {
				int i = 0;
			//send ctrlmsg to combine processes.
				for(i = 0;i < PROS_NUMBER;i++) {
					if(parameter->statistics[i]->cpu == report.cpus[1]) {
						send_ctrl_down(report.cpus[0], parameter->statistics[i]);
					}
				}
				//changefrequency
				changefrequency(800000, report.cpus[1]);

			}

			break;

		case 4://the urgent message that process sends to controller that the process needs a new cpu immediately.
//*************************update record************************************
			parameter->statistics[parameter->p_number]->cpu = ctrlbuffer.cpu;
			parameter->statistics[parameter->p_number]->queues = ctrlbuffer.edges;
			parameter->statistics[parameter->p_number]->pid_in_record = ctrlbuffer.pid_in_ctrlmsg;

			for(i_edges = 0;i_edges < ctrlbuffer.edges;i_edges++) {
				parameter->statistics[parameter->p_number]->queuelength[i_edges] = ctrlbuffer.qsize[i_edges];
				parameter->statistics[parameter->p_number]->qmax[i_edges] = ctrlbuffer.qmaxsize[i_edges];
			}
			printf("now show the result of ctrlbuffer.\n");
			printf("the process %d runs on cpu %d, it receives %d queues \n", parameter->statistics[parameter->p_number]->pid_in_record, parameter->statistics[parameter->p_number]->cpu, parameter->statistics[parameter->p_number]->queues);

			for(i_edges = 0;i_edges < parameter->statistics[parameter->p_number]->queues;i_edges++) {
				printf("the queue %d has %ld packets of most %ld packets \n", i_edges, parameter->statistics[parameter->p_number]->queuelength[i_edges], parameter->statistics[parameter->p_number]->qmax[i_edges]);
			}
			//update the adj_array/point_weight and do the extended_KL algotithm.
			update_adj_array(parameter->statistics, parameter->adj_array);
			update_point_weight(parameter->statistics, parameter->point_weight);
			//get all processes on the urgent cpu
			int p_on_hot_cpu[10];
			int count_p_on_hot_cpu = 0;
			int i = 0;
			int j = 0;
			for(i = 0;i < PROS_NUMBER;i++) {
				if(parameter->statistics[i]->cpu == ctrlbuffer.cpu) {
					p_on_hot_cpu[count_p_on_hot_cpu] = i;//include the process who sent the ctrlmsg.
					count_p_on_hot_cpu++;
				}
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
			int * cut_order = KL_partition(count_p_on_hot_cpu, parameter->adj_array, parameter->point_weight);
			//get the return cut_order and send ctrlmsg to set cpu.
			int set_edges =(count_p_on_hot_cpu + count_p_on_hot_cpu%2) / 2;
			int idle_cpu = find_idle_cpu(parameter->statistics);
			if(idle_cpu != -1) {
				for(i = 0;i < set_edges;i++) {
					send_ctrl_down(idle_cpu, parameter->statistics[p_on_hot_cpu[i]]);
				}
				//changefrequency
				changefrequency(3400000, idle_cpu);
			}
			free(cut_order);

			break;
	}

	printf("finish a controller control \n");
	printstar();
	pthread_exit(NULL);
}


void changefrequency(long frequency, int cpu) {//frequency's unit is kHz
	char cmdhead[] = "echo 1 | sudo -S ";
	char cmdgovernor[200];
	sprintf(cmdgovernor, "cpufreq-set -c %d -g userspace", cpu);
	char cmdfrequency[200];
	sprintf(cmdfrequency, "cpufreq-set -f %ld -c %d", frequency, cpu);

	char * cmdbash1 = (char *) calloc(1, strlen(cmdhead) + strlen(cmdgovernor) + 1);//1 = sizeof(char), + 1 for '\0'
	strcat(cmdbash1, cmdhead);
	strcat(cmdbash1, cmdfrequency);
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





struct check_report check_status(struct record * statistics[PROS_NUMBER], double * point_weight) {
	int i = 0;
	int j = 0;
	struct check_report report;//return value
	//int cpu_count[4] = {0, 0, 0, 0};//this 4 means 0~1 status.
	int count_idle_cpu = 0;
	int p_on_cpu[CPUS];
	clear_int_series(CPUS, p_on_cpu, "p_on_cpu in check_status");
	double cpu_status[CPUS];//the number is bigger, cpu is hotter.
	clear_double_series(CPUS, cpu_status, "cpu_status in check_status");
	for(i = 0;i < PROS_NUMBER;i++) {
		//printf("cpu_status[statistics[%d]->cpu] = %f, point_weight[%d] = %f\n", i, cpu_status[statistics[i]->cpu], i, point_weight[i]);
		cpu_status[statistics[i]->cpu] += point_weight[i];//we may set edge_cut into this.
		p_on_cpu[statistics[i]->cpu] += 1;
	}
	show_double_series(CPUS, cpu_status, "cpu_status in check_status");
	show_int_series(CPUS, p_on_cpu, "p_on_cpu in check_status");
	for(i = 1;i < CPUS;i++) {//CPU0 is only for PACKET_SENDING!!!
		if((cpu_status[i] < IDLE_THRESHOLD)&&(p_on_cpu[i] > 0)) {
			count_idle_cpu++;
		}
	}
	if(count_idle_cpu <= 1) {
		report.type = 2;
		return report;
	}
	else {
		report.type = 1;
		for(i = 1, j = 0;i < CPUS;i++) {
			if(cpu_status[i] < IDLE_THRESHOLD) {
				report.cpus[j] = i;
				j++;
				if(j > 1) {
					break;
				} 
			}
		}
		printf("CPU%d and CPU%d are idle and can be combined.\n", report.cpus[0], report.cpus[1]);
		return report;
	}

	printf("something wrong happened in check_status, please debug!!!\n");
	return report;
}








int find_idle_cpu(struct record * statistics[PROS_NUMBER]) {
	int idle_cpu = -1;
	int cpu_status[CPUS];//	0:cpu is not working
			//	1:cpu is cool and can be merged.
			//	2:cpu is warm and can not be merged but still not need to be splited.
			//	3:cpu is hot and need to be splited.
	clear_int_series(CPUS, cpu_status, "cpu_status in find_idle_cpu");
	int i = 0;
	for(i = 0;i < PROS_NUMBER;i++) {
		cpu_status[statistics[i]->cpu] = 2;
	}
	for(i = 1;i < CPUS;i++) {//!!!!we will leave CPU0 for packet_sending.o!!!!
		if(cpu_status[i] == 0) {
			idle_cpu = i;
			break;
		}
	}

	return idle_cpu;
}



void send_ctrl_down(int work_cpu, struct record * p_statistics) {
	char funcname[] = "ctrl_down";
	struct ctrlmsg down_ctrlmsg;
	struct mq_attr q_mq_attr;
	int func_re = 0;
	down_ctrlmsg.service_number = 3;
	down_ctrlmsg.cpu = work_cpu;
	func_re = mq_send(p_statistics->mqd_ctop, (char *) &down_ctrlmsg, sizeof(struct ctrlmsg), 0);
	check_return(func_re, funcname, "mq_send");
}



void update_adj_array(struct record * statistics[PROS_NUMBER], double adj_array[ADJ_ARRAY_EDGES][ADJ_ARRAY_EDGES]) {
	show_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array in update_adj_array");
	printstar();
	int i = 0;
	for(i = 0;i < PROS_NUMBER;i++) {
		printf("statistics[%d]->queuelength[0] = %ld\n", i, statistics[i]->queuelength[0]);
	}
	printstar();
	//we will change this to another way to replace the adj_array.we need to use throughput to replace the queuelength!!!!!!!!!!!!!!
	adj_array[0][1] = adj_array[1][0] = ((double) statistics[1]->queuelength[0]) / ((double) statistics[0]->qmax[0]);//p1top2
	adj_array[0][2] = adj_array[2][0] = ((double) statistics[2]->queuelength[0]) / ((double) statistics[0]->qmax[0]);//p1top3
	show_double_array(ADJ_ARRAY_EDGES, adj_array, "adj_array in update_adj_array");
}



void update_point_weight(struct record * statistics[PROS_NUMBER], double * point_weight) {
	int i = 0;
	int j = 0;
	clear_double_series(PROS_NUMBER, point_weight, "point_weight in update_point_weight");
	for(i = 0;i < PROS_NUMBER;i++) {
		for(j = 0;j < statistics[i]->queues;j++) {
			point_weight[i] =point_weight[i] + ((double) statistics[i]->queuelength[j]) / ((double) statistics[i]->qmax[j]) + QUEUE_WEIGHT;
		}
	}
	show_double_series(ADJ_ARRAY_EDGES, point_weight, "point_weight in update_point_weight");
}

#endif
