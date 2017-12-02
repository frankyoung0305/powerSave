#!/bin/bash

pkill -x packet_send.o
pkill -x p1_l3.o
pkill -x p2_IDS.o
pkill -x p3_IDS.o
pkill -x p4_l3.o
pkill -x p5_nDPI.o
pkill -x p6_FW.o
pkill -x p7_FW.o
pkill -x p8_counter.o
./clear.o
