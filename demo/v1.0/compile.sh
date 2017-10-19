#!/bin/bash

gcc -o clear.o clear.c -lrt -lpthread -Wall

gcc -o packet_sending.o packet_sending.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /home/hunter/nDPI/src/lib/.libs/libndpi.so.1.0.0 

gcc -o p1_l3.o p1_l3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /home/hunter/nDPI/src/lib/.libs/libndpi.so.1.0.0 -Wall

gcc -o p2.o p2.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /home/hunter/nDPI/src/lib/.libs/libndpi.so.1.0.0 -Wall

gcc -o p3.o p3.c -lrt -lpthread /usr/lib/x86_64-linux-gnu/libpcap.a /home/hunter/nDPI/src/lib/.libs/libndpi.so.1.0.0 -Wall

gcc -o control.o control.c -lrt -lpthread -Wall
