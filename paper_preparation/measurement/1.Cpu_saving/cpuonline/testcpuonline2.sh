#!/bin/sh

#sh /home/fnl/changecstate.sh
if [ ! -e /home/fnl/cpuonline ]; then
	mkdir /home/fnl/cpuonline
	echo mkdir /home/fnl/cpuonline
fi

if [ ! -e /home/fnl/cpuonline/part2 ]; then
	mkdir /home/fnl/cpuonline/part2
	echo mkdir /home/fnl/cpuonline/part2
fi
num=0
tempnum=0
half=0
all=0
goal2=0
#39 29 19 9
for num in 39 29 19 9
do
	echo start test $num cpus
	if [ ! -e /home/fnl/cpuonline/part2/"$num"cpus ]; then
		mkdir /home/fnl/cpuonline/part2/"$num"cpus
		echo mkdir /home/fnl/cpuonline/part2/"$num"cpus
	fi
	tempnum=1
	tempnum2=40
	goal2=`expr $num + 40`
	echo tempnum = $tempnum
	while [ $tempnum -le $num ]
	do
		echo 1 > /sys/devices/system/cpu/cpu"$tempnum"/online
		tempnum=`expr $tempnum + 1`
	done
		
	tempnum=`expr $num + 1`
	echo tempnum = $tempnum
	while [ $tempnum -le 39 ]
	do
		echo 0 > /sys/devices/system/cpu/cpu"$tempnum"/online
		tempnum=`expr $tempnum + 1`
	done
	
	echo tempnum2 = $tempnum2
	while [ $tempnum2 -le $goal2 ]
	do
		echo 1 > /sys/devices/system/cpu/cpu"$tempnum2"/online
		tempnum2=`expr $tempnum2 + 1`
	done
	tempnum2=`expr $goal2 + 1`
	while [ $tempnum2 -le 79 ]
	do
		echo 0 > /sys/devices/system/cpu/cpu"$tempnum2"/online
		tempnum2=`expr $tempnum2 + 1`
	done
	
	
	
	cat /sys/devices/system/cpu/online
	
	sh ./stop_loop.sh
	echo start test run0CPUs.txt
	powerstat -RDH > /home/fnl/cpuonline/part2/"$num"cpus/run0CPUs.txt
	
	echo start test runhalfCPUs.txt
	tempnum=0
	half=`expr $num + 1`
	echo half = $half, num = $num
	while [ $tempnum -le $half ]
	do
		./loop.o &
		tempnum=`expr $tempnum + 1`
	done
	powerstat -RDH > /home/fnl/cpuonline/part2/"$num"cpus/run40CPUs.txt
	sh /home/fnl/stop_loop.sh
	
	echo start test runallCPUs.txt
	all=`expr $half \* 2`
	echo all = $all
	tempnum=0
	while [ $tempnum -le $all ]
	do
		./loop.o &
		tempnum=`expr $tempnum + 1`
	done
	
	powerstat -RDH > /home/fnl/cpuonline/part2/"$num"cpus/run80CPUs.txt
	sh /home/fnl/stop_loop.sh

	echo finish test $num cpus
	sleep 1s
done
echo finish all test
tempnum=1
while [ $tempnum -le 79 ]
do
	echo 1 > /sys/devices/system/cpu/cpu"$tempnum"/online
	tempnum=`expr $tempnum + 1`
done

cat /sys/devices/system/cpu/online
