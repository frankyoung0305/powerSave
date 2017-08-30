#include"../nfv.h"

int main() {
	/*initialization about mqueue*/
	char proname[] = "receive_process";

	struct mq_attr attr, *attrp;
	attrp = NULL;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;
	attrp = &attr;

	int flags = O_CREAT | O_RDWR;;
	mqd_t mqd_p3torc;
	int mq_return = 0;
	char p3torc[]="/sendtoreceive";

	mqd_p3torc = mq_open(p3torc, flags, PERM, attrp);
	check_return(mqd_p3torc, proname, "mq_open");

	char buffer[2048];
	struct datamsg * databufferp;
	struct datamsg databuffer;
	long long i;


	for(i = 0;i < PACKETS;i++) {
		mq_return = mq_receive(mqd_p3torc, buffer, 2048, 0);
		if(mq_return == -1) {
			printf("%s:receive %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
		}

		databufferp = (struct datamsg *) buffer;
		databuffer = *databufferp;

		if((i%100000 == 0) || (i < 400)) {
			printf("%s has received %lld packets \n", proname, i);
			printf("a:%d, b:%d, c:%d, d:%d, char_data:%s\nf1:%f, f2:%f, f3:%f, f4:%f\n", databuffer.a, databuffer.b, databuffer.c, databuffer.d, databuffer.char_data, databuffer.f1, databuffer.f2, databuffer.f3, databuffer.f4);
		}

	}
	mq_return = mq_close(mqd_p3torc);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");

	exit(0);

}
