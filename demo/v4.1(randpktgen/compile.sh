#!/bin/bash

gcc -o clear.o clear.c -lrt -lpthread -Wall -lm

gcc -o control.o control.c -lrt -lpthread -Wall -lm

gcc -o packet_send.o packet_sending.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p1_l3.o p1_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p2_IDS.o p2_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p3_IDS.o p3_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p4_l3.o p4_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p5_nDPI.o p5_nDPI.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p6_FW.o p6_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p7_FW.o p7_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm

gcc -o p8_counter.o p8_counter.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function -lm



#gcc -o ndpi.o main.c /usr/local/lib/libndpi.a -lm -Wall
