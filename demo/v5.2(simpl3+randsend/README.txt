Please run control.o in root terminal!!!
cpuon.sh will turn every CPU from number to cpunum on.Please remember to change cpunum!

v5.2
change p0_l3.c p8_l3.c p16_l3.c p24_l3.c p32_l3.c to forward packets simplely
send0.c send1.c send2.c send3.c send4.c send packets between usleep(100) or usleep(10).

v5.1
make topo to 40 processes and ready to run on server
fix some bugs in extended_KL.h


v5.0
do some prepare for 40 CPUs

v4.0
now we only use ondemand governor but change /sys/devices/system/cpu/cpuX/online in root.(X from 1 to CPUS - 1)
we turn on/off a physical cpu i.e. two logical CPUs now.
add an macro called 'RUNMODE' to control whether setcpu will work, which is used in different measurement experiment.
we do not use macro 'CPUS' now but 'PHYSICAL_CPUS', so many things are changed.

v3.1
Frank Young debuged the fan.h and fan.h hac no warning now.
change l3 from creating route table to hashtable.

v3.0
ready to make topo bigger with 8 processes.

v2.0 
O	throughput represents edge_weight.
O	process cpu usage represents point_weight.
O	queuelength / max_queuelength to notify that CPU is congested.
O	packet_sending sends packets more slowly than that in v1.0 who sends using up all CPU0.

get new fan.h with DPI
we may add new nDPI to the topo and make topo larger.

decide whether we should edit the extended_KL.h for double's accuracy with EPSILON(maybe useless)

edit the nfv process in fan.h with the loop of while and idle_i to promote the nfv's cpu usage
