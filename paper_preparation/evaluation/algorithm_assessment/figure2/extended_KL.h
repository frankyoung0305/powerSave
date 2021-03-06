#ifndef EXTENDED_KL_H
#define EXTENDED_KL_H

#include<stdio.h>//printf()
#include<unistd.h>//fork() sleep()
#include<sys/types.h>//fork()
#include<stdlib.h>//malloc()
#include<string.h>//perror()
#include<errno.h>//errno
#include<math.h>//double fabs(double x)

#include"series_array.h"//show/clear/copy series/array.

#define EPSILON 1e-7

//#define MAX_LOOP_TIMES 10

#define KL_ALPHA 0.3

//#define PRINT_MODE





struct KL_return {
	//int length;
	int set1[200];
	int set2[200];
	int loop_times;
};//maybe we could use a two-dimensional array to replace the struct.



struct KL_return KL_step(int MAX_LOOP_TIMES, int edges, double adj_array[edges][edges], double point_weight[edges], int set_edges, int set1[set_edges], int set2[set_edges]);


//int * KL_partition(int old_edges, double adj_array[old_edges][old_edges], double raw_point_weight[old_edges]);//return a one dimensional array(with [(edges+edges%2)/2] members)
			//WARNING:remember to free the returned series!!!


double Diff_value(int row_number, int edges, double adj_array[edges][edges], int set_edges, int set_in[set_edges], int set_other[set_edges]);



struct KL_return KL_step(int MAX_LOOP_TIMES, int edges, double adj_array[edges][edges], double point_weight[edges],int set_edges, int set1[set_edges], int set2[set_edges]) {
	#ifdef PRINT_MODE
	printf("now is in KL_step, set_edges = %d\n", set_edges);
	#endif
	struct KL_return KL_re;
	copy_int_series(set_edges, set1, KL_re.set1);
	copy_int_series(set_edges, set2, KL_re.set2);
	#ifdef PRINT_MODE
	show_int_series(set_edges, KL_re.set1, "KL_step's return value: KL_re.set1");
	show_int_series(set_edges, KL_re.set2, "KL_step's return value: KL_re.set2");
	show_double_series(edges, point_weight, "point_weight");
	#endif

	int i = 0;
	int j = 0;
	double sum_weight = 0;
	for(i = 0;i < set_edges;i++) {
		sum_weight = sum_weight + point_weight[i];
	}

	double gain_edgecut[set_edges][set_edges];
	double gain_balance[set_edges][set_edges];
	double gain[set_edges][set_edges];
	double edgecut_old = 0;
	double max_gain = 0;
	int loop_times = 0;

	do {
		loop_times++;
		#ifdef PRINT_MODE
		printf("******************************************\n");
		printf("LOOP %d!!!\n", loop_times);
		show_int_series(set_edges, KL_re.set1, "KL_re.set1");
		show_int_series(set_edges, KL_re.set2, "KL_re.set2");
		#endif
		
		edgecut_old = 0;
		for(i = 0;i < set_edges;i++) {
			for(j = 0;j < set_edges;j++) {
				edgecut_old = edgecut_old + adj_array[KL_re.set1[i]][KL_re.set2[j]];
			}
		}
		#ifdef PRINT_MODE
		printf("edgecut_old = %f\n", edgecut_old);
		#endif
		
		if(fabs(edgecut_old) <= EPSILON) {
			#ifdef PRINT_MODE
			printf("Surprise!edgecut_old = 0!!!!\n");
			#endif
			edgecut_old = 1;
		}
		for(i = 0;i < set_edges;i++) {
			for(j = 0;j < set_edges;j++) {
				gain_edgecut[i][j] = Diff_value(KL_re.set1[i], edges, adj_array, set_edges, KL_re.set1, KL_re.set2);
				//printf("gain_edgecut[%d][%d] = %f Diff_value(KL_re.set1[%d]) = %f \n", i, j, gain_edgecut[i][j], i, Diff_value(KL_re.set1[i], edges, adj_array, set_edges, set1, set2));
				gain_edgecut[i][j] = gain_edgecut[i][j] + Diff_value(KL_re.set2[j], edges, adj_array, set_edges, KL_re.set2, KL_re.set1);
				//printf("gain_edgecut[%d][%d] = %f Diff_value(KL_re.set2[%d]) = %f \n", i, j, gain_edgecut[i][j], j, Diff_value(KL_re.set1[i], edges, adj_array, set_edges, set2, set1));
				gain_edgecut[i][j] = gain_edgecut[i][j] - 2 * adj_array[KL_re.set1[i]][KL_re.set2[j]];
				//printf("gain_edgecut[%d][%d] = %f \t adj_array[KL_re.set1[%d]][KL_re.set2[%d]] = %f\n", i, j, gain_edgecut[i][j], i, j, adj_array[KL_re.set1[i]][KL_re.set2[j]]);
				gain_edgecut[i][j] = gain_edgecut[i][j] / edgecut_old;
				//printf("gain_edgecut[%d][%d] = %f \n", i, j, gain_edgecut[i][j]);
			}
		}
/*		show_double_array(set_edges, gain_edgecut, "gain_edgecut");

		for(i = 0;i < set_edges;i++) {
			for(j = 0;j < set_edges;j++) {
				gain_edgecut[i][j] = gain_edgecut[i][j] / edgecut_old;

			}
		}*/
		#ifdef PRINT_MODE
		printf("got the gain_edgecut array\n");
		show_double_array(set_edges, gain_edgecut, "gain_edgecut");
		#endif

		double D_workloaddiff = 0;
		double workloaddiff_old = 0;
		double workloaddiff_new = 0;
		#ifdef PRINT_MODE
		printf("******************************************\n");
		#endif
		for(i = 0;i < set_edges;i++) {
			workloaddiff_old = workloaddiff_old + point_weight[KL_re.set1[i]] - point_weight[KL_re.set2[i]];
			//printf("point_weight[KL_re.set1[%d]] = %f, point_weight[KL_re.set2[%d]] = %f\n", i, point_weight[KL_re.set1[i]], i, point_weight[KL_re.set2[i]]);
		}
		#ifdef PRINT_MODE
		printf("workloaddiff_old = %f \n", workloaddiff_old);
		#endif

		if(fabs(workloaddiff_old) <= EPSILON) {
			for(i = 0;i < set_edges;i++) {
				for(j = 0;j < set_edges;j++) {
					workloaddiff_new = workloaddiff_old - 2 * point_weight[KL_re.set1[i]] + 2 * point_weight[KL_re.set2[j]];
					//printf("i = %d, j = %d, point_weight[KL_re.set1[%d]] = %f, point_weight[KL_re.set2[%d]] = %f\n", i, j, i, point_weight[KL_re.set1[i]], j, point_weight[KL_re.set2[j]]);
					//printf("workloaddiff_old = %f, workloaddiff_new = %f \n", workloaddiff_old, workloaddiff_new);
					D_workloaddiff = fabs(workloaddiff_old) - fabs(workloaddiff_new);
					if(fabs(D_workloaddiff) <= EPSILON) {
						gain_balance[i][j] = 0;
					}
					else {
						gain_balance[i][j] = -1;
					}
				}
			}

		}
		else {
			for(i = 0;i < set_edges;i++) {
				for(j = 0;j < set_edges;j++) {
					workloaddiff_new = workloaddiff_old - 2 * point_weight[KL_re.set1[i]] + 2 * point_weight[KL_re.set2[j]];
					//printf("i = %d, j = %d, point_weight[KL_re.set1[%d]] = %f, point_weight[KL_re.set2[%d]] = %f\n", i, j, i, point_weight[KL_re.set1[i]], j, point_weight[KL_re.set2[j]]);
					//printf("workloaddiff_old = %f, workloaddiff_new = %f \n", workloaddiff_old, workloaddiff_new);
					D_workloaddiff = fabs(workloaddiff_old) - fabs(workloaddiff_new);
					gain_balance[i][j] = D_workloaddiff / fabs(workloaddiff_old);
				}
			}
		}

		
		#ifdef PRINT_MODE
		printf("got the gain_balance array\n");
		show_double_array(set_edges, gain_balance, "gain_balance");
		#endif

		double alpha = KL_ALPHA;
		for(i = 0;i < set_edges;i++) {
			for(j = 0;j < set_edges;j++) {
				gain[i][j] = alpha * gain_edgecut[i][j] + (1 - alpha) * gain_balance[i][j];
			}
		}
		#ifdef PRINT_MODE
		printf("got the gain array\n");
		show_double_array(set_edges, gain, "gain");
		#endif

		int max_i = 0, max_j = 0;
		max_gain = gain[0][0];
		for(i = 0;i < set_edges;i++) {
			for(j = 0;j < set_edges;j++) {
				if(gain[i][j] > max_gain) {
					max_gain = gain[i][j];
					max_i = i;
					max_j = j;
				}
			}
		}
		#ifdef PRINT_MODE
		printf("max_gain is gain[%d][%d] = %f\n", max_i, max_j, gain[max_i][max_j]);
		#endif
		if(max_gain > 0) {
			int temp = 0;//exchange two nodes.
			temp = KL_re.set1[max_i];
			KL_re.set1[max_i] = KL_re.set2[max_j];
			KL_re.set2[max_j] = temp;
		}
	}while(max_gain > EPSILON && loop_times < MAX_LOOP_TIMES);

	KL_re.loop_times = loop_times;
	return KL_re;
}

double Diff_value(int row_number, int edges, double adj_array[edges][edges], int set_edges, int set_in[set_edges], int set_other[set_edges]) {
	double d = 0;
	int i = 0;
	for(i = 0;i < set_edges;i++) {//calculate the externals
		d = d + adj_array[row_number][set_other[i]];
	}

	for(i = 0;i < set_edges;i++) {//calculate the internals
		d = d - adj_array[row_number][set_in[i]];
	}
	return d;
}




#endif
