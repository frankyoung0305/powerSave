#include"queue.h"

void clearqueue(char * queuename);//give it a name of a queue where packets left will be cleared.


int main() {
	char proname[] = "clear_queues";


	clearqueue("/sendtoreceive");

	printf("%s finished \n", proname);
	return 0;

}



void clearqueue(char * queuename) {
	int flags = O_CREAT | O_RDWR | O_NONBLOCK;
	mqd_t mqd_clear;

	struct mq_attr attr, *attrp;
	attrp = NULL;
	attr.mq_maxmsg = 2000;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;
	attrp = &attr;

	long long int i = 0;
	int func_re = 0;
	char msg_buffer[2048];



	mqd_clear = mq_open(queuename, flags, PERM, attrp);
	if(mqd_clear == -1) {
		printf("%s mq_open failed, error explanation is:%s, errno = %d \n", queuename, strerror(errno), errno);
		return ;
	}

	i = 0;
	while((func_re = mq_receive(mqd_clear, msg_buffer, 2048, 0)) >= 0) {
		i++;
		//if(i < 500) printf("%s:receive %lld times \n", queuename, i);

	}
	if(i > 0) {
		printf("ATTENTION PLEASE!!!ATTENTION PLEASE!!!ATTENTION PLEASE!!!\n\n");
		printf("In queue %s, there are %lld packets left. \n", queuename, i);
		printf("\nATTENTION PLEASE!!!ATTENTION PLEASE!!!ATTENTION PLEASE!!!\n");
	}
	else {
		printf("There is nothing left in queue %s. \n", queuename);
	}
	func_re = mq_close(mqd_clear);//returns 0 on success, or -1 on error.
	check_return(func_re, queuename, "mq_close");
	func_re = mq_unlink(queuename);//returns 0 on success, or -1 on error.
	check_return(func_re, queuename, "mq_unlink");
	printnewline();
}

