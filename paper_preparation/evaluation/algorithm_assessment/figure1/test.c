#include "find_the_best.h"

#define EDGES_START 2
#define EDGES_END 15 //15
#define PER_CASE_TIMES 10000 //5000
#define ADJ_SIZE 5

int main(void) {
	int set1[ADJ_SIZE];
	int set2[ADJ_SIZE];
	int i = 0;
	for(i = 0;i < ADJ_SIZE;i++) {
		set1[i] = i;
		set2[i] = i + ADJ_SIZE;
	}
	do {
		printf("set1:");
		for(i = 0;i < ADJ_SIZE;i++) {
			printf("%d ", set1[i]);
		}
		printf("\n");
		printf("set2:");
		for(i = 0;i < ADJ_SIZE;i++) {
			printf("%d ", set2[i]);
		}
		printf("\n\n");
	} while(next_set(ADJ_SIZE, set1, set2, ADJ_SIZE * 2));

}
