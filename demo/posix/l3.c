#include <stdlib.h> 
#include <stdio.h>
#include "ndpi_api.h" //iphdr
#include <string.h> 

#define BUFSZ 2048
#define MAXLEN 1500  //最大caplen
#define REPS 10000000  //发包数

#define GTP_U_V1_PORT        2152

#define LEN 16 

#include <pcap.h> //pcapreader

static u_int16_t decode_tunnels = 0;

///////////////////////////////////////router


#define BUFFER_SIZE 9000
#define DBG_PRINT if(1 == g_ulDbgPrint) printf
int g_ulDbgPrint = 0;
//路由表元素节点
typedef struct node {
    struct node *pLeftChild;
    struct node *pRightChild;
    int iPort;
}NODE;

//路由表root
NODE *g_pRouteTree = NULL;

//'创建节点'函数
NODE *createNode() {
    NODE *pNode = malloc(sizeof(NODE));
    pNode->pLeftChild = NULL;
    pNode->pRightChild = NULL;
    pNode->iPort = -1;
    return pNode;
}

//'创建路由表'函数
void createRouteTree(int iRoute, int iMask, int iPort) {
    int i = 0;
    // 0 -- left, 1 -- right
    int iLeftOrRight = 0;
    static int iInitFlag = 0;
    
    if (0 == iInitFlag) {//why not just check if g_pRouteTree is NULL?
        g_pRouteTree = createNode();
        iInitFlag = 1;
    }
    
    DBG_PRINT("input rounte: %8x, mask: %d, port: %d\n", iRoute, iMask, iPort);

    NODE *pCurrNode = g_pRouteTree;
    for (i = 0; i < iMask; i++) {
        iLeftOrRight = (iRoute >> (31 - i)) & 0x1;
        
        if(0 == iLeftOrRight) {
            if (NULL == pCurrNode->pLeftChild) {
                pCurrNode->pLeftChild = createNode();
            }
            pCurrNode = pCurrNode->pLeftChild;
            DBG_PRINT("0 left\n");
        }
        else {
            if (NULL == pCurrNode->pRightChild) {
                pCurrNode->pRightChild = createNode();
            }
            pCurrNode = pCurrNode->pRightChild;
            DBG_PRINT("1 right\n");
        }
        
    }

    pCurrNode->iPort = iPort;
    DBG_PRINT("%d port\n", iPort);
    return;
}

//ip转发查找路由表函数
int getIpFwdPort(int iIp) {
    int i = 0, iLeftOrRight = 0, iPort = -1;
    NODE *pCurrNode = g_pRouteTree;
    iPort = (-1 == pCurrNode->iPort)?iPort:pCurrNode->iPort;
    
    DBG_PRINT("input ip: %8x\n", iIp);

    for (i = 0; i < 32; i++) {
        iLeftOrRight = (iIp >> (31-i)) & 0x1;
        
        if (0 == iLeftOrRight) {
            if (NULL != pCurrNode->pLeftChild) {
                pCurrNode = pCurrNode->pLeftChild;
                iPort = (-1 == pCurrNode->iPort)?iPort:pCurrNode->iPort;
                DBG_PRINT("0 go left, %d port\n", iPort);
            }
            else {
                break;
            }
        }
        else {// (1 == iLeftOrRight)
            if (NULL != pCurrNode->pRightChild) {
                pCurrNode = pCurrNode->pRightChild;
                iPort = (-1 == pCurrNode->iPort)?iPort:pCurrNode->iPort;
                DBG_PRINT("1 go right, %d port\n", iPort);
            }
            else {
                break;
            }
        }
    }
    return iPort;
}
//////////////////////////////////////////////////////
////////////////////////////////////////////////////pcap////////////
int getPkt(pcap_t* p,struct pcap_pkthdr** pkt_header,const u_char** pkt_data){
	if(pcap_next_ex(p, pkt_header, pkt_data) != 1){
		pcap_close(p);
		char errbuf[100];  //error buf for pcapReader
		p = pcap_open_offline("/tmp/traffic_sample.pcap", errbuf);  //head
		pcap_next_ex(p, pkt_header, pkt_data);

		printf("get all packets!\n");
	}
	return 0;
}  //获取下一个数据包



/////////////////parser////////////////
struct ndpi_iphdr * packet_preprocess(const u_int16_t pktlen, const u_char * packet)
{
  const struct ndpi_ethhdr *ethernet = (struct ndpi_ethhdr *) packet;
  struct ndpi_iphdr *iph = (struct ndpi_iphdr *) &packet[sizeof(struct ndpi_ethhdr)];
  u_int64_t time;
//static u_int64_t lasttime = 0;
  u_int16_t ip_offset;
  u_int16_t frag_off = 0;




  // just work on Ethernet packets that contain IP
//  if (_pcap_datalink_type == DLT_EN10MB && type == htons(ETH_P_IP)
//      && header->caplen >= sizeof(struct ndpi_ethhdr)) {
//    u_int16_t frag_off = ntohs(iph->frag_off);
//
//    if(header->caplen < header->len) {
//      static u_int8_t cap_warning_used = 0;
//    if (cap_warning_used == 0) {
//	printf("\n\nWARNING: packet capture size is smaller than packet //size, DETECTION MIGHT NOT WORK CORRECTLY\n\n");
//	cap_warning_used = 1;
//      }
//    }
//
//    if (iph->version != 4) {
//      static u_int8_t ipv4_warning_used = 0;
//
//    v4_warning:
//    if (ipv4_warning_used == 0) {
//	printf("\n\nWARNING: only IPv4 packets are supported in this demo (nDPI supports both IPv4 and IPv6), all other packets will be discarded\n\n");
//	ipv4_warning_used = 1;
//      }
//      return;
//    }

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

int main(){
	system("cp traffic_sample.pcap /tmp/");   //转移到tmpfs
	char errbuf[100];  //error buf for pcapReader

	pcap_t *pfile = pcap_open_offline("/tmp/traffic_sample.pcap", errbuf);  //head
	if (NULL == pfile) {
		printf("%s\n", errbuf);
		return -1;
	} 
	//printf("file opened\n");

	struct pcap_pkthdr *pkthdr = 0;
	const u_char *pktdata = 0;


	createRouteTree(0, 0, -1);
	int i = 0;
	for(i = 0; i < 5; ){  //写5条路由表项
		getPkt(pfile, &pkthdr, &pktdata);
		//for (bpf_u_int32 i = 0; i < pkthdr->caplen; ++i) {
		//	if (0 < i && 0 == i % 16) printf("\n");
		//	printf("%2x ", pktdata[i]);
		//	} //print pkt
		struct ndpi_iphdr* iph = packet_preprocess(pkthdr->caplen, pktdata);
		int port = getIpFwdPort(iph->daddr);//先查询
		if (port == -1){
			createRouteTree(iph->daddr, 32, i);  //前5个包写入路由，端口为i
		printf("router->daddr:%X , port: %d\n",iph->daddr, i);//process here
		i++;
		}
		else {
			//printf("dupicate: %d\n", port);
				
		}	
		
	}
	createRouteTree(0, 0, 999);//加入默认路由（验证lpm

	pfile = pcap_open_offline("/tmp/traffic_sample.pcap", errbuf);  //head
	for(i = 0; i < 1000; i++){//9715
		getPkt(pfile, &pkthdr, &pktdata);
		struct ndpi_iphdr* iph = packet_preprocess(pkthdr->caplen, pktdata);
		printf("i = %d, ", i);
		printf("daddr:%X, ",iph->daddr);//process here
		printf("eth%d\n", getIpFwdPort(iph->daddr));	//查询路由
	}

}
