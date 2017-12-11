#!/bin/bash

pkill -e -x send0.o
pkill -e -x p0_l3.o
pkill -e -x p1_IDS.o
pkill -e -x p2_IDS.o
pkill -e -x p3_l3.o
pkill -e -x p4_nDPI.o
pkill -e -x p5_FW.o
pkill -e -x p6_FW.o
pkill -e -x p7_counter.o

pkill -e -x send1.o
pkill -e -x p8_l3.o
pkill -e -x p9_IDS.o
pkill -e -x p10_IDS.o
pkill -e -x p11_l3.o
pkill -e -x p12_nDPI.o
pkill -e -x p13_FW.o
pkill -e -x p14_FW.o
pkill -e -x p15_counter.o

pkill -e -x send2.o
pkill -e -x p16_l3.o
pkill -e -x p17_IDS.o
pkill -e -x p18_IDS.o
pkill -e -x p19_l3.o
pkill -e -x p20_nDPI.o
pkill -e -x p21_FW.o
pkill -e -x p22_FW.o
pkill -e -x p23_counter.o

pkill -e -x send3.o
pkill -e -x p24_l3.o
pkill -e -x p25_IDS.o
pkill -e -x p26_IDS.o
pkill -e -x p27_l3.o
pkill -e -x p28_nDPI.o
pkill -e -x p29_FW.o
pkill -e -x p30_FW.o
pkill -e -x p31_counter.o

pkill -e -x send4.o
pkill -e -x p32_l3.o
pkill -e -x p33_IDS.o
pkill -e -x p34_IDS.o
pkill -e -x p35_l3.o
pkill -e -x p36_nDPI.o
pkill -e -x p37_FW.o
pkill -e -x p38_FW.o
pkill -e -x p39_counter.o
pkill -e -x control.o

sleep 0.01
./clear.o
