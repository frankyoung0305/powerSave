#!/bin/bash

pkill -x send0.o
pkill -x p0_l3.o
pkill -x p1_IDS.o
pkill -x p2_IDS.o
pkill -x p3_l3.o
pkill -x p4_nDPI.o
pkill -x p5_FW.o
pkill -x p6_FW.o
pkill -x p7_counter.o

pkill -x send1.o
pkill -x p8_l3.o
pkill -x p9_IDS.o
pkill -x p10_IDS.o
pkill -x p11_l3.o
pkill -x p12_nDPI.o
pkill -x p13_FW.o
pkill -x p14_FW.o
pkill -x p15_counter.o

pkill -x send2.o
pkill -x p16_l3.o
pkill -x p17_IDS.o
pkill -x p18_IDS.o
pkill -x p19_l3.o
pkill -x p20_nDPI.o
pkill -x p21_FW.o
pkill -x p22_FW.o
pkill -x p23_counter.o

pkill -x send3.o
pkill -x p24_l3.o
pkill -x p25_IDS.o
pkill -x p26_IDS.o
pkill -x p27_l3.o
pkill -x p28_nDPI.o
pkill -x p29_FW.o
pkill -x p30_FW.o
pkill -x p31_counter.o

pkill -x send4.o
pkill -x p32_l3.o
pkill -x p33_IDS.o
pkill -x p34_IDS.o
pkill -x p35_l3.o
pkill -x p36_nDPI.o
pkill -x p37_FW.o
pkill -x p38_FW.o
pkill -x p39_counter.o
pkill -x control.o
./clear.o
