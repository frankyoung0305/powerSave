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
#######################################################################
gcc -o send1.o send1.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p8_l3.o p8_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p9_IDS.o p9_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p10_IDS.o p10_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p11_l3.o p11_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p12_nDPI.o p12_nDPI.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p13_FW.o p13_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p14_FW.o p14_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p15_counter.o p15_counter.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function
#######################################################################
gcc -o send2.o send2.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p16_l3.o p16_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p17_IDS.o p17_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p18_IDS.o p18_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p19_l3.o p19_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p20_nDPI.o p20_nDPI.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p21_FW.o p21_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p22_FW.o p22_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p23_counter.o p23_counter.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function
#######################################################################
gcc -o send3.o send3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p24_l3.o p24_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p25_IDS.o p25_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p26_IDS.o p26_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p27_l3.o p27_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p28_nDPI.o p28_nDPI.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p29_FW.o p29_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p30_FW.o p30_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p31_counter.o p31_counter.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function
#######################################################################
gcc -o send4.o send4.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p32_l3.o p32_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p33_IDS.o p33_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p34_IDS.o p34_IDS.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p35_l3.o p35_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p36_nDPI.o p36_nDPI.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p37_FW.o p37_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p38_FW.o p38_FW.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function

gcc -o p39_counter.o p39_counter.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /usr/local/lib/libndpi.a -Wall -Wno-unused-function



#gcc -o ndpi.o main.c /usr/local/lib/libndpi.a -lm -Wall
