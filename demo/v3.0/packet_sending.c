#include "nfv.h"
#include "fan.h"

#define GTP_U_V1_PORT        2152



struct ndpi_iphdr * packet_preprocess(const u_int16_t pktlen, const u_char * packet);



int main() {
	/*initialization about mqueue*/
	char proname[] = "packet_sending";
	setcpu(0);

	printf("Now is in packet_sending!\n");

	struct mq_attr attr;
	attr.mq_maxmsg = MAXMSG;//maximum is 382.
	attr.mq_msgsize = 2048;
	attr.mq_flags = 0;
	int flags = O_CREAT | O_RDWR;
	//int flags_ctrl = O_CREAT | O_RDWR | O_NONBLOCK;

	mqd_t mqd_sdtop1;
	int mq_return = 0;
	char sdtop1[]="/sendtop1";

	mqd_sdtop1 = mq_open(sdtop1, flags, PERM, &attr);
	check_return(mqd_sdtop1, proname, "mq_open");



	system("cp ../posix/traffic_sample.pcap /tmp/");   //转移到tmpfs
	char errbuf[100];  //error buf for pcapReader

	pcap_t *pfile = pcap_open_offline("/tmp/traffic_sample.pcap", errbuf);  //head
	if (NULL == pfile) {
		printf("%s\n", errbuf);
		return -1;
	} 
	//printf("file opened\n");

	struct pcap_pkthdr *pkthdr = 0;
	const u_char *pktdata = 0;

	pfile = pcap_open_offline("/tmp/traffic_sample.pcap", errbuf);  //head

	long long int i = 0;
	struct ndpi_iphdr * iph;

	for(i = 0; i < PACKETS; i++){//9715
		getPkt(pfile, &pkthdr, &pktdata);
		iph = packet_preprocess(pkthdr->caplen, pktdata);
		mq_return = mq_send(mqd_sdtop1, (char *) iph, pkthdr->caplen, 0);
		if(mq_return == -1) {
			printf("%s:send %lld times fails:%s, errno = %d \n", proname, i, strerror(errno), errno);
		}
		if(i < 200) {
			printf("i = %lld \n", i);
		}
		if(i % 100000 == 0) {
			printf("packet_sending has sent %lld packets \n", i);
		}
		//let the packet_sending.o works more slowly.
		else if(i%50 == 0) {
			//printf("packet_sending has sent %lld packets \n", i);
			usleep(10);
		}
/*		int j = 0;
		for(j = 0;j < 1000;j++) {
			while(0);
		}*/
	}

	printstar();
	printf("packet_sending has sent %lld packets and is closing.\n", i);
	mq_return = mq_close(mqd_sdtop1);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_close");
	mq_return = mq_unlink(sdtop1);//returns 0 on success, or -1 on error.
	check_return(mq_return, proname, "mq_unlink");

	func_quit(proname);
	exit(0);

}




/////////////////parser////////////////
struct ndpi_iphdr * packet_preprocess(const u_int16_t pktlen, const u_char * packet)
{
	const struct ndpi_ethhdr *ethernet = (struct ndpi_ethhdr *) packet;
	struct ndpi_iphdr *iph = (struct ndpi_iphdr *) &packet[sizeof(struct ndpi_ethhdr)];
	u_int64_t time;
	//static u_int64_t lasttime = 0;
	u_int16_t ip_offset;
	u_int16_t frag_off = 0;


	ip_offset = sizeof(struct ndpi_ethhdr);
	if(decode_tunnels && (iph->protocol == IPPROTO_UDP) && ((frag_off & 0x3FFF) == 0)) {
		u_short ip_len = ((u_short)iph->ihl * 4);
		struct ndpi_udphdr *udp = (struct ndpi_udphdr *)&packet[sizeof(struct ndpi_ethhdr)+ip_len];
		u_int16_t sport = ntohs(udp->source), dport = ntohs(udp->dest);

		if((sport == GTP_U_V1_PORT) || (dport == GTP_U_V1_PORT)) {
		/* Check if it's GTPv1 */
		u_int offset = sizeof(struct ndpi_ethhdr)+ip_len+sizeof(struct ndpi_udphdr);
		u_int8_t flags = packet[offset];
		u_int8_t message_type = packet[offset+1];

			if((((flags & 0xE0) >> 5) == 1 /* GTPv1 */) && (message_type == 0xFF /* T-PDU */)) {
				ip_offset = sizeof(struct ndpi_ethhdr)+ip_len+sizeof(struct ndpi_udphdr)+8 /* GTPv1 header len */;

				if(flags & 0x04) ip_offset += 1; /* next_ext_header is present */
				if(flags & 0x02) ip_offset += 4; /* sequence_number is present (it also includes next_ext_header and pdu_number) */
				if(flags & 0x01) ip_offset += 1; /* pdu_number is present */

				iph = (struct ndpi_iphdr *) &packet[ip_offset];

				if (iph->version != 4) {
					 printf("WARNING: not good !\n");
					//goto v4_warning;
				}
			}
		}

	}
	/////////////////////process
	return iph;
}




