mode=1

number=1
cpunum=79
while [ $number -le $cpunum ]
do
	echo $mode > /sys/devices/system/cpu/cpu"$number"/online
	number=`expr $number + 1`
done
sh /home/fnl/changecstate.sh
cat /sys/devices/system/cpu/online
