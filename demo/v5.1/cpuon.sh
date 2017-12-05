mode=1

number=1
cpunum=80
while [ $number -le $cpunum ]
do
	echo $mode > /sys/devices/system/cpu/cpu"$number"/online
	number=`expr $number + 1`
done

