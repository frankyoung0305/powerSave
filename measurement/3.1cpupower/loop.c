#include "loop.h"
int main(void) {
	//setcpu(LOOP_CPU);
	int sum = 0;
	int i = 0;
	while(1) {
		sum = sum + i;
		sum = sum - i;
		sum++;
		/*if(i%SLEEP_FREQUENCY == 0) {
			usleep(1);
		}*/
		i++;
	}
	return 0;
}
