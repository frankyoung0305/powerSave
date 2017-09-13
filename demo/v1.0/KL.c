#include<stdio.h>//printf()
#include<unistd.h>//fork() sleep()
#include<sys/types.h>//fork()
#include<stdlib.h>//malloc()
#include<string.h>//perror()
#include<errno.h>//errno

struct KL_return {
	int length;
	int set1[50];
	int set2[50];
};//maybe we could use a two-dimensional array to replace the struct.


struct KL_return g_max(int edges, int adj_array[edges][edges], int set_edges, int set1[set_edges], int set2[set_edges], int set1_Dvalue[set_edges], int set2_Dvalue[set_edges]);



struct KL_return KL_step(int edges, int adj_array[edges][edges], int set_edges, int set1[set_edges], int set2[set_edges]);


int * KL_partition(int old_edges, int adj_array[old_edges][old_edges]);//return a one dimensional array(with [(edges+edges%2)/2] members)
			//WARNING:remember to free the returned series!!!



void show_series(int length, int showed_series[length], char * series_name);
void show_array(int edges, int showed_array[edges][edges], char * array_name);

int clear_series(int edges, int cleared_series[edges], char * series_name);
int clear_array(int edges, int cleared_array[edges][edges], char * array_name);

void copy_series(int edges, int src_series[edges], int dst_series[edges]);
void copy_array(int edges, int src_array[edges][edges], int dst_array[edges][edges]);

//int external(int row_number, int edges, int set_array[edges][edges]);
//int internal(int row_number, int edges, int set_array[edges][edges]);

int Diff_value(int row_number, int edges, int set_array[edges][edges], int set_edges, int set_in[set_edges], int set_other[set_edges]);

int * cut_series(int length, int old_series[length], int cut_number);


//int get_maxgain(int set_edges, int gain_array[set_edges][set_edges]);


int main() {
	//raw data.
	int easy_array_1[6][6] = {{0, 1, 0, 0, 6, 0}, //1
				{1, 0, 2, 0, 3, 0}, //2
				{0, 2, 0, 3, 5, 0}, //3
				{0, 0, 3, 0, 7, 4}, //4
				{6, 3, 5, 7, 0, 0}, //5
				{0, 0, 0, 4, 0, 0}}; //6
	int edges_1 = 6;
	int set_edges_1 = 0;//the rows of a set.
	set_edges_1 = (edges_1 + (edges_1 % 2)) / 2;

	int easy_array_2[8][8] = {{0, 5, 6, 1, 0, 3, 0, 0}, 
				{5, 0, 2, 2, 0, 1, 0, 2}, 
				{6, 2, 0, 0, 1, 3, 5, 4}, 
				{1, 2, 0, 0, 3, 0, 0, 1},  
				{0, 0, 1, 3, 0, 9, 10, 2},  
				{3, 1, 3, 0, 9, 0, 1, 0},  
				{0, 0, 5, 0, 10, 1, 0, 0},  
				{0, 2, 4, 1, 2, 0, 0, 0}};
	int edges_2 = 8;
	int set_edges_2 = 0;//the rows of a set.
	set_edges_2 = (edges_2 + (edges_2 % 2)) / 2;

	int easy_array_3[7][7] = {{0, 1, 2, 5, 6, 0, 0}, 
				{1, 0, 3, 4, 2, 0, 1},  
				{2, 3, 0, 0, 1, 4, 4},  
				{5, 4, 0, 0, 1, 2, 5},  
				{6, 2, 1, 1, 0, 0, 1},  
				{0, 0, 4, 2, 0, 0, 0},  
				{0, 1, 4, 5, 1, 0, 0}};
	int edges_3 = 7;
	int set_edges_3 = 0;
	set_edges_3 = (edges_3 + (edges_3 % 2)) / 2;


	int * cut_order;


/*	cut_order = KL_partition(edges_1, easy_array_1);
	printf("we have passed \'KL_partition\'!\n");
	show_series(set_edges_1, cut_order, "cut_order");
//	printf("*************************************\n");
*/
/*	cut_order = KL_partition(edges_2, easy_array_2);
	printf("we have passed \'KL_partition\'!\n");
	show_series(set_edges_2, cut_order, "cut_order");
*/

	cut_order = KL_partition(edges_3, easy_array_3);
	printf("we have passed \'KL_partition\'!\n");
	show_series(set_edges_3, cut_order, "cut_order");


	free(cut_order);//!!!IMPORTANT to free it!!!
}


struct KL_return g_max(int edges, int adj_array[edges][edges], int set_edges, int set1[set_edges], int set2[set_edges], int set1_Dvalue[set_edges], int set2_Dvalue[set_edges]) {
	printf("now is in g_max, set_edges = %d\n", set_edges);
	struct KL_return KL_re_old;
	struct KL_return KL_re_new;
	KL_re_new.length = set_edges;
	int i = 0;
	int j = 0;
	int gain[set_edges][set_edges];
	for(i = 0;i < set_edges;i++) {
		for(j = 0;j < set_edges;j++) {
			gain[i][j] = set1_Dvalue[i] + set2_Dvalue[j] - 2 * adj_array[set1[i]][set2[j]];
		}
	}
	printf("got the gain array\n");
	show_array(set_edges, gain, "gain");
	
	int max_i = 0, max_j = 0, temp = gain[0][0];
	for(i = 0;i < set_edges;i++) {
		for(j = 0;j < set_edges;j++) {
			if(gain[i][j] > temp) {
				temp = gain[i][j];
				max_i = i;
				max_j = j;
			}
		}
	}
	printf("gain_max is gain[%d][%d] = %d\n", max_i, max_j, gain[max_i][max_j]);


	if(gain[max_i][max_j] > 0) {//we need to go on for more exchanges.
		if(set_edges > 1) {
			//create 'new_set1' and 'new_set2'.
			int new_set_edges = set_edges - 1;
			int * free_p;
			int new_set1[new_set_edges];
			int new_set2[new_set_edges];
			free_p = cut_series(set_edges, set1, max_i);
			copy_series(new_set_edges, free_p, new_set1);
			free(free_p);
			free_p = cut_series(set_edges, set2, max_j);
			copy_series(new_set_edges, free_p, new_set2);
			free(free_p);
			show_series(new_set_edges, new_set1, "new_set1");
			show_series(new_set_edges, new_set2, "new_set2");

			printf("old Dvalue!!!!!!!!!!!!!!!!!!!\n");
			show_series(set_edges, set1_Dvalue, "set1_Dvalue");
			show_series(set_edges, set2_Dvalue, "set2_Dvalue");
			//modify the Dvalue array
			for(i = 0;i < set_edges;i++) {
				if(i != max_i) {
					set1_Dvalue[i] = set1_Dvalue[i] + (2 * adj_array[set1[i]][set1[max_i]]) - (2 * adj_array[set1[i]][set2[max_j]]);
					//printf("set1_Dvalue[%d] = %d, adj_array[set1[i]][set1[max_i]] = %d, adj_array[set1[i]][set2[max_j]] = %d\n", i, set1_Dvalue[i], adj_array[set1[i]][set1[max_i]], adj_array[set1[i]][set2[max_j]]);
					//printf("set1[i] = %d, set1[max_i] = %d, set2[max_j] = %d\n", set1[i], set1[max_i], set2[max_j]);
				}
				if(i != max_j) {
					set2_Dvalue[i] = set2_Dvalue[i] + (2 * adj_array[set2[i]][set2[max_j]]) - (2 * adj_array[set2[i]][set1[max_i]]);
				//printf("set2_Dvalue[%d] = %d, adj_array[set2[i]][set2[max_i]] = %d, adj_array[set2[i]][set2[max_j]] = %d\n", i, set2_Dvalue[i], adj_array[set2[i]][set1[max_i]], adj_array[set2[i]][set2[max_j]]);

				}
			}

			printf("Dvalue update!!!!!!!!!!!!!!!!!\n");

			show_series(set_edges, set1_Dvalue, "set1_Dvalue");
			show_series(set_edges, set2_Dvalue, "set2_Dvalue");

			//create 'new_set1_Dvalue' and 'new_set2_Dvalue'.
			int new_set1_Dvalue[new_set_edges];
			int new_set2_Dvalue[new_set_edges];
			free_p = cut_series(set_edges, set1_Dvalue, max_i);
			copy_series(new_set_edges, free_p, new_set1_Dvalue);
			free(free_p);
			free_p = cut_series(set_edges, set2_Dvalue, max_j);
			copy_series(new_set_edges, free_p, new_set2_Dvalue);
			free(free_p);
			show_series(new_set_edges, new_set1_Dvalue, "new_set1_Dvalue");
			show_series(new_set_edges, new_set2_Dvalue, "new_set2_Dvalue");
			free_p = NULL;


			KL_re_old = g_max(edges, adj_array, new_set_edges, new_set1, new_set2, new_set1_Dvalue, new_set2_Dvalue);
			copy_series(new_set_edges, KL_re_old.set1, KL_re_new.set1);
			copy_series(new_set_edges, KL_re_old.set2, KL_re_new.set2);
			KL_re_new.set1[new_set_edges] = set2[max_j];
			KL_re_new.set2[new_set_edges] = set1[max_i];
			return KL_re_new;

		}
		else {//set_edges = 1, just exchange two nodes is alright.
			copy_series(set_edges, set2, KL_re_new.set1);
			copy_series(set_edges, set1, KL_re_new.set2);
		}
		return KL_re_new;
	}
	else {//the maximum in gain_array is less than 0, it's enough we don't have to exchange anymore.
		copy_series(set_edges, set1, KL_re_new.set1);
		copy_series(set_edges, set2, KL_re_new.set2);
		return KL_re_new;

	}

	printf("something wrong happened in KL_step,set_edges = %d, you shouldn't see this line, but if you have seen it, please debug!!!\n", set_edges);
	return KL_re_new;

}



struct KL_return KL_step(int edges, int adj_array[edges][edges], int set_edges, int set1[set_edges], int set2[set_edges]) {
	printf("now is in KL_step, set_edges = %d\n", set_edges);
	struct KL_return KL_re;
	KL_re.length = set_edges;
	copy_series(set_edges, set1, KL_re.set1);
	copy_series(set_edges, set2, KL_re.set2);
	show_series(set_edges, KL_re.set1, "KL_re.set1");
	show_series(set_edges, KL_re.set2, "KL_re.set2");


	int i = 0;
	int j = 0;
	int set1_Dvalue[set_edges];
	int set2_Dvalue[set_edges];

	for(i = 0;i < set_edges;i++) {
		set1_Dvalue[i] = Diff_value(set1[i], edges, adj_array, set_edges, set1, set2);
		set2_Dvalue[i] = Diff_value(set2[i], edges, adj_array, set_edges, set2, set1);
	}

	show_series(set_edges, set1_Dvalue, "set1_Dvalue");
	show_series(set_edges, set2_Dvalue, "set2_Dvalue");

	KL_re = g_max(edges, adj_array, set_edges, set1, set2, set1_Dvalue, set2_Dvalue);//we may modify this later!!!!!!

	return KL_re;
}




int * KL_partition(int old_edges, int raw_adj_array[old_edges][old_edges]/*, int vertex_weight[edges]*/) {
	show_array(old_edges, raw_adj_array, "raw_adj_array");
	int set_edges = (old_edges + (old_edges % 2)) / 2;
	int * cut_order = (int *) malloc(set_edges);//WARNING!!!the pointer needs to be freed!
	clear_series(set_edges, cut_order, "cut_order");

	int new_edges = old_edges;
	if((old_edges % 2) != 0) {
		new_edges = old_edges + 1;
	}

	int adj_array[new_edges][new_edges];
	clear_array(new_edges, adj_array, "adj_array");
	if((old_edges % 2) == 0) {
		copy_array(new_edges, raw_adj_array, adj_array);
	}
	if((old_edges % 2) != 0) {
		int i = 0;
		int j = 0;
		for(i = 0;i < old_edges;i++) {
			for(j = 0;j < old_edges;j++) {
				adj_array[i][j] = raw_adj_array[i][j];
			}
		}
		for(i = 0;i < new_edges;i++) {//let the last column of array equal 0.
			adj_array[i][old_edges] = 0;
		}
		for(j = 0;j < new_edges;j++) {//let the last row of array equal 0.
			adj_array[old_edges][j] = 0;
		}

	}
	show_array(new_edges, adj_array, "adj_array");

	int i = 0;
	struct KL_return KL_re;
	for(i = 0;i < set_edges;i++) {	//initialize the orders.
		KL_re.set1[i] = i;
		KL_re.set2[i] = i + set_edges;
	}


/*	for(i = 0;i < set_edges;i++) {
		KL_re.set1[i] = 2*i;
		KL_re.set2[i] = 2*i + 1;
	}
*/
	printf("set1 and set2 have been initialized!\n");
	show_series(set_edges, KL_re.set1, "KL_re.set1");
	show_series(set_edges, KL_re.set2, "KL_re.set2");

	for(i = 0;i < 2;i++) {
		KL_re = KL_step(new_edges, adj_array, set_edges, KL_re.set1, KL_re.set2);//the 'return' can tell us to split which processes to another CPU.
	}
	copy_series(set_edges, KL_re.set1, cut_order);
	return cut_order;


}




int * cut_series(int length, int old_series[length], int cut_number) {
	int i = 0;
	int j = 0;
	int new_length = length - 1;
	int * new_series = (int *) malloc(new_length);
	for(i = 0, j = 0;i < length;i++) {
		if(i != cut_number) {
			new_series[j] = old_series[i];
			j++;
		}
	}
	return new_series;
}


int Diff_value(int row_number, int edges, int set_array[edges][edges], int set_edges, int set_in[set_edges], int set_other[set_edges]) {
	int d = 0;
	int i = 0;
	for(i = 0;i < set_edges;i++) {//calculate the externals
		d = d + set_array[row_number][set_other[i]];
	}

	for(i = 0;i < set_edges;i++) {//calculate the internals
		d = d - set_array[row_number][set_in[i]];
	}
	return d;
}


void show_series(int length, int showed_series[length], char * series_name) {
	printf("Series \'%s\':", series_name);
	int i = 0;
	printf("%d", showed_series[0]);
	for(i = 1;i < length;i++) {
		printf(", %d", showed_series[i]);
	}
	printf("\n");
}

void show_array(int edges, int showed_array[edges][edges], char * array_name) {
	printf("Array \'%s\':\n", array_name);
	int i = 0;
	int j = 0;
	for(i = 0;i < edges;i++) {
		for(j = 0;j < edges;j++) {
			printf("%d\t", showed_array[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int clear_series(int edges, int cleared_series[edges], char * series_name) {
	int i = 0;
	for(i = 0;i < edges;i++) {
		cleared_series[i] = 0;
	}
	printf("cleared the series \'%s\'\n", series_name);
}


int clear_array(int edges, int cleared_array[edges][edges], char * array_name) {
	int i = 0;
	int j = 0;
	for(i = 0;i < edges;i++) {
		for(j = 0;j < edges;j++) {
			cleared_array[i][j] = 0;
		}
	}
	printf("cleared the array \'%s\'\n", array_name);
}

void copy_series(int edges, int src_series[edges], int dst_series[edges]) {
	int i = 0;
	for(i = 0;i < edges;i++) {
		dst_series[i] = src_series[i];
	}
}

void copy_array(int edges, int src_array[edges][edges], int dst_array[edges][edges]) {
	int i = 0;
	int j = 0;
	for(i = 0;i < edges;i++) {
		for(j = 0;j < edges;j++) {
			dst_array[i][j] = src_array[i][j];
		}
	}
}


/*useless now
int external(int row_number, int edges, int set_array[edges][edges]) {
	int externals = 0;
	int start_row = 0;

	if(edges % 2 != 0) return -10000;

	if(row_number < (edges / 2)) {
		for(start_row = edges / 2;start_row < edges;start_row++) {
			externals = externals + set_array[row_number][start_row];
		}
	}
	else {
		for(start_row = 0;start_row < edges / 2;start_row++) {
			externals = externals + set_array[row_number][start_row];
		}
	}

	return externals;
}

int internal(int row_number, int edges, int set_array[edges][edges]) {
	int internals = 0;
	int start_row = 0;

	if(edges % 2 != 0) return -10000;

	if(row_number < (edges / 2)) {
		for(start_row = 0;start_row < edges / 2;start_row++) {
			internals = internals + set_array[row_number][start_row];
		}

	}
	else {
		for(start_row = edges / 2;start_row < edges;start_row++) {
			internals = internals + set_array[row_number][start_row];
		}
	}

	return internals;
}
*/


