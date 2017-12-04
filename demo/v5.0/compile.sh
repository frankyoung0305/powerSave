#!/bin/bash

gcc -o clear.o clear.c -lrt -lpthread -Wall

gcc -o control.o control.c -lrt -lpthread -Wall

gcc -o send0.o send0.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p0_l3.o p0_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p1_IDS.o p1_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p2_IDS.o p2_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p3_l3.o p3_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p4_nDPI.o p4_nDPI.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p5_FW.o p5_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p6_FW.o p6_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p7_counter.o p7_counter.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function



#gcc -o ndpi.o main.c /usr/local/lib/libndpi.a -lm -Wall
