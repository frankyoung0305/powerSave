#include"nfv.h"
#include"control.h"

#define PRO_NUMBER 6
#define Q_LOW_THRESHOLD 30
#define Q_HIGH_THRESHOLD 50

struct check_report{
	int type;//	3:a cpu need split;
		//	2:nothing needs to  be done
		//	1:some cpus need to combine;
	int count;
	int cpus[20];
	int cpu_status[CPUS];
};

void show_status(int pro_number, struct record pros[pro_number]);

struct check_report check_status(int pro_number, struct record pros[pro_number]);

int main() {
	printf("now is in combine.c!\n");
	printstar();
	struct record processes[PRO_NUMBER];
	int i = 0;
	int j = 0;
	for(i = 0;i < PRO_NUMBER;i++) {
		processes[i].number = i;
		processes[i].queues = 3;
		processes[i].pid_in_record = i + 10000 + 100*i;
		processes[i].cpu = 0;
		for(j = 0;j < MAXQUEUES;j++) {
			processes[i].queuelength[j] = 0;
			processes[i].qmax[j] = 100;
		}
	}
	processes[0].cpu = 0;
	processes[0].queuelength[0] = 50;
	processes[1].cpu = 0;
	processes[1].queuelength[0] = 0;
	processes[2].cpu = 1;
	processes[2].queuelength[0] = 90;
	processes[3].cpu = 1;
	processes[3].queuelength[0] = 10;
	processes[4].cpu = 3;
	processes[4].queuelength[0] = 20;
	processes[5].cpu = 4;
	processes[5].queuelength[0] = 20;





	show_status(PRO_NUMBER, processes);

	struct check_report report;
	report = check_status(PRO_NUMBER, processes);
	printf("from check_status report.type = %d, .count = %d\n", report.type, report.count);
	for(i = 0;i < report.count;i++) {
		printf("report.cpus[%d] = %d\n", i, report.cpus[i]);
	}


	switch(report.type)
	{
		case 3:
			printf("report.type = %d, some cpus need to be splited\n", report.type);
			printf("split to cpu %d\n", report.cpus[report.count]);

			//transfer data from the biggest data array into a smaller one.
			//KL_partition.
			break;
		case 1:
			printf("report.type = %d, some cpus need to be merged.\n", report.type);
			//send messages to processes who are free to work on another cpu.
			break;
		case 2:
			printf("report.type = %d, nothing needs to be done.\n", report.type);
			break;
		printerror();
	}


}

void show_status(int pro_number, struct record pros[pro_number]) {
	int i = 0;
	int j = 0;
	for(i = 0;i < pro_number;i++) {
		printf("processes[%d]: .number = %d, .queues = %d, .pid_in_record = %d, .cpu = %d\n", i, pros[i].number, pros[i].queues, pros[i].pid_in_record, pros[i].cpu);
		for(j = 0;j < pros[i].queues;j++) {
			printf(".queuelength[%d] = %ld .qmax[%d] = %ld \n", j, pros[i].queuelength[j], j, pros[i].qmax[j]);
		}
	}
}




struct check_report check_status(int pro_number, struct record pros[pro_number]) {
	int i = 0;
	int j = 0;
	int cpu_count[4] = {0, 0, 0, 0};//this 4 means 0~1 status.
	int cpu_status[CPUS];//	0:cpu is not working
			//	1:cpu is cool and can be merged.
			//	2:cpu is warm and can not be merged but still not need to be splited.
			//	3:cpu is hot and need to be splited.
	for(i = 0;i < CPUS;i++) {
		cpu_status[i] = 0;
	}
	for(i = 0;i < pro_number;i++) {
		for(j = 0;j < pros[i].queues;j++) {
			if(pros[i].queuelength[j] > Q_HIGH_THRESHOLD) {
				cpu_status[pros[i].cpu] = 3;
			}
			else if((pros[i].queuelength[j] > Q_LOW_THRESHOLD) && (cpu_status[pros[i].cpu] != 3)) {//-0,1,2 can be changed.
				cpu_status[pros[i].cpu] = 2;
			}
			else if((pros[i].queuelength[j] < Q_LOW_THRESHOLD) && (cpu_status[pros[i].cpu] == 0)) {//0,1 is OK.
				cpu_status[pros[i].cpu] = 1;
			}
		}
	}
	for(i = 0;i < CPUS;i++) {
		switch(cpu_status[i])
		{
			case 0:
				cpu_count[0]++;
				break;
			case 1:
				cpu_count[1]++;
				break;
			case 2:
				cpu_count[2]++;
				break;
			case 3:
				cpu_count[3]++;
				break;
		}
		printf("cpu_status[%d] = %d\n", i, cpu_status[i]);
	}
	for(i = 0;i < pro_number;i++) {
		printf("pros[%d].cpu = %d\n", i, pros[i].cpu);
	}
	for(i = 0;i < 4;i++) {
		printf("cpu_count[%d] = %d\n", i, cpu_count[i]);

	}


	struct check_report report;
	report.type = 2;
	report.count = 0;

	for(i = 0;i < CPUS;i++) {
		report.cpu_status[i] = cpu_status[i];
	}

	if(cpu_count[3] > 0) {
		int hot_cpus[cpu_count[3]];//store the serial number of hot cpus.
		for(i = 0, j = 0;i < CPUS;i++) {
			if(cpu_status[i] == 3) {
				hot_cpus[j] = i;
				j++;
			}
		}
		int on_cpu[cpu_count[3]];//store the number of processes on the hot cpus.
		for(i = 0;i < cpu_count[3];i++) {
			on_cpu[i] = 0;
		}
		for(i = 0;i < pro_number;i++) {
			for(j = 0;j < cpu_count[3];j++) {
				if(pros[i].cpu == hot_cpus[j]) {
					on_cpu[j]++;
				}
			}
		}//check how many processes are on the hot cpu.

		for(i = 0, j = 0;i < cpu_count[3];i++) {
			if(on_cpu[i] >= 2) {
				report.count++;
				report.cpus[j] = hot_cpus[i];
				j++;
			}
		}

		for(i = 0;i < CPUS;i++) {
			if(cpu_status[i] == 0) {
				report.cpus[j] = i;
				break;
			}
		}//find an idle cpu.
		if(report.count > 0) {
			report.type = 3;
		}

	}
	else if(cpu_count[1] >= 2) {
		report.type = 1;
		report.count = cpu_count[1];
		for(i = 0, j = 0;i < CPUS;i++) {
			if(cpu_status[i] == 1) {
				report.cpus[j] = i;
				j++;
			}
		}

	}


	return report;
}
