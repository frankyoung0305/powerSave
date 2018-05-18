#!/bin/sh

#sh /home/fnl/changecstate.sh
if [ ! -e /home/fnl/cpuonline ]; then
	mkdir /home/fnl/cpuonline
	echo mkdir /home/fnl/cpuonline
fi

if [ ! -e /home/fnl/cpuonline/part1 ]; then
	mkdir /home/fnl/cpuonline/part1
	echo mkdir /home/fnl/cpuonline/part1
fi
num=0
tempnum=0
half=0
#9 19 29 39 49 59 69 79
for num in 79 69 59 49 39 29 19 9
do
	echo start test $num cpus
	if [ ! -e /home/fnl/cpuonline/part1/"$num"cpus ]; then
		mkdir /home/fnl/cpuonline/part1/"$num"cpus
		echo mkdir /home/fnl/cpuonline/part1/"$num"cpus
	fi
	tempnum=1
	echo tempnum = $tempnum
	while [ $tempnum -le $num ]
	do
		echo 1 > /sys/devices/system/cpu/cpu"$tempnum"/online
		tempnum=`expr $tempnum + 1`
	done
	
	tempnum=`expr $num + 1`
	echo tempnum = $tempnum
	while [ $tempnum -le 79 ]
	do
		echo 0 > /sys/devices/system/cpu/cpu"$tempnum"/online
		tempnum=`expr $tempnum + 1`
	done
	cat /sys/devices/system/cpu/online
	
	sh ./stop_loop.sh
	echo start test run0CPUs.txt
	powerstat -RDH > /home/fnl/cpuonline/part1/"$num"cpus/run0CPUs.txt
	
	echo start test runhalfCPUs.txt
	tempnum=0
	half=`expr $num + 1`
	half=`expr $half / 2`
	echo half = $half, num = $num
	while [ $tempnum -le $half ]
	do
		./loop.o &
		tempnum=`expr $tempnum + 1`
	done
	powerstat -RDH > /home/fnl/cpuonline/part1/"$num"cpus/run40CPUs.txt
	sh /home/fnl/stop_loop.sh
	
	echo start test runallCPUs.txt
	echo num = $num
	tempnum=0
	while [ $tempnum -le $num ]
	do
		./loop.o &
		tempnum=`expr $tempnum + 1`
	done
	
	powerstat -RDH > /home/fnl/cpuonline/part1/"$num"cpus/run80CPUs.txt
	sh /home/fnl/stop_loop.sh

	echo finish test $num cpus
	sleep 1s
done
tempnum=1
while [ $tempnum -le 79 ]
do
	echo 1 > /sys/devices/system/cpu/cpu"$tempnum"/online
	tempnum=`expr $tempnum + 1`
done

cat /sys/devices/system/cpu/online
