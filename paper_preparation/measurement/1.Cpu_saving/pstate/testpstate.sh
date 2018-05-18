#!/bin/sh

#sh /home/fnl/changecstate.sh
if [ ! -e /home/fnl/pstate ]; then
	mkdir /home/fnl/pstate
	echo mkdir /home/fnl/pstate
fi
num=0
#performance
for governor in powersave ondemand conservative
do
	echo start test $governor governor
	if [ ! -e /home/fnl/pstate/"$governor" ]; then
		mkdir /home/fnl/pstate/"$governor"
		echo mkdir /home/fnl/pstate/"$governor"
	fi
	num=0
	while [ $num -le 79 ]
	do
		sudo cpufreq-set -c "$num" -g $governor
		num=`expr $num + 1`
	done
	
	echo start test run0CPUs.txt
	powerstat -RDH > /home/fnl/pstate/"$governor"/run0CPUs.txt
	
	echo start test run40CPUs.txt
	sh /home/fnl/use_half_cpu.sh
	powerstat -RDH > /home/fnl/pstate/"$governor"/run40CPUs.txt
	sh /home/fnl/stop_loop.sh
	
	echo start test run80CPUs.txt
	sh /home/fnl/use_all_cpu.sh
	powerstat -RDH > /home/fnl/pstate/"$governor"/run80CPUs.txt
	sh /home/fnl/stop_loop.sh

	echo finish test $governor governor
	sleep 1s
done
