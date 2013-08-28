#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include "rational.h"

static unsigned long long	fm_count;
static volatile bool		proceed = false;

static void done(int unused)
{
	proceed = false;
	unused = unused;
}


void print_all(rational_t** A, rational_t* cc, size_t r, size_t c) {
	size_t i;	
	size_t j;
	printf("Rows: %zu, Cols: %zu\n", r, c);
	for (i = 0; i < r; i++) {
		rational_t* row = A[i];
		for (j = 0; j < c; j++) {
			printf("%d ", row[j].num);
		}
		printf(" = %d\n", cc[i].num);
	}
}

void print_all_rat(rational_t** A, rational_t* cc, size_t r, size_t c) {
	size_t i;	
	size_t j;
	printf("Rows: %zu, Cols: %zu\n", r, c);
	for (i = 0; i < r; i++) {
		rational_t* row = A[i];
		for (j = 0; j < c; j++) {
			rat_print(&row[j]);
			printf(" ");
		}
		printf(" = ");
		rat_print(&cc[i]);
		printf("\n");
	}
}

static void free_all(size_t rows, rational_t** a, rational_t* c) {
	size_t i;
	for(i = 0; i < rows ; i++)
		free(a[i]);
	free(a);
	free(c);
}

static int fm_elim(size_t rows, size_t cols, rational_t** a, rational_t* c)
{
	size_t i, j;    
	rational_t **t = malloc(rows*sizeof(rational_t*));
	rational_t *q = malloc(rows*sizeof(rational_t));
	for (i = 0; i < rows; i += 1){
		t[i] = malloc(cols*sizeof(rational_t));
		q[i] = c[i];
		for(j = 0; j < cols; j += 1)
		   t[i][j] = a[i][j];
	}
	rational_t ZERO;
	ZERO.num = 0;
	ZERO.den = 1;
	size_t n1 = 0;
	size_t n2 = 0;
	
	while(1){
		n1 = 0;
		n2 = 0;
		
		// Determine n1 and n2.
		for(i = 0; i < rows; i += 1){
			int cmp = rat_cmp(&t[i][cols-1], &ZERO);
			if(cmp > 0)
				n1 += 1;
			else if(cmp < 0)
				n2 += 1;
		}
		n2 += n1;
		
		rational_t **t_sort = malloc(rows*sizeof(rational_t*));
		rational_t *q_sort = malloc(rows*sizeof(rational_t));
		size_t n1_i = 0, n2_i = n1, n3_i = n2;
		size_t cur = 0;
		while(cur != rows){
			int cmp = rat_cmp(&t[cur][cols-1], &ZERO);
			if(cmp > 0){
				q_sort[n1_i] = q[cur];               
				t_sort[n1_i++] = t[cur];
			}else if(cmp < 0){
				q_sort[n2_i] = q[cur];
				t_sort[n2_i++] = t[cur];
			}else{
				q_sort[n3_i] = q[cur];
				t_sort[n3_i++] = t[cur];
			}
			cur++;
		}		
		
		free(t);
		free(q);
		q = q_sort;
		t = t_sort;
		for(i = 0; i < n2; i += 1) {
			rational_t* div = &t[i][cols-1];
			rat_div(&q[i], div);
			for(j = 0; j < cols; j += 1) {
				rat_div(&t[i][j], div);
			}
		}
				
		if(cols == 1){
			rational_t tmp;
			tmp.num = 100000000;
			tmp.den = 1;
			int min = 0;

			int max = 0;
				for(i = 0; i < n1; i +=1)
					if(rat_cmp(&q[i], &tmp) < 0){
						tmp.num = q[i].num;
						tmp.den = q[i].den;
						min = i;
					}

			tmp.num = 0;
			tmp.den = 1;

			for(i = n1; i < n2; i +=1)
				if(rat_cmp(&q[i], &tmp) > 0 ){
					tmp.num = q[i].num;
					tmp.den = q[i].den;
					max = i;
				}

			if (rat_cmp(&q[max], &q[min]) > 0) {
				free_all(rows, t, q);
				return false;
			}
			
			for(; n2 < rows; n2 += 1)
				if(rat_cmp(&q[n2], &ZERO) < 0) {
					free_all(rows, t, q);
					return false;
				}
			free_all(rows, t, q);
			return true;
		}
		size_t rows_pr = rows - n2 + n1 * (n2 - n1);
		if (rows_pr == 0) {
			free_all(rows, t, q);
			return true;
		}

		
		rational_t **t2 = malloc(rows_pr*sizeof(rational_t*));
		rational_t *q2 = malloc(rows_pr*sizeof(rational_t));
		for (i = 0; i < rows_pr; i += 1)
			 t2[i] = malloc((cols-1)*sizeof(rational_t));
		size_t k;
		size_t q_index = 0;
		for(i = 0; i < n1; i += 1)
			 for(j = n1; j < n2; j += 1){
				q2[q_index] = q[i];
				rat_sub(&q2[q_index],&q[j]);
				
				for(k = 0; k < cols-1; k += 1) {
					t2[q_index][k] = t[i][k]; 
					rat_sub(&t2[q_index][k],&t[j][k]);
				}
				q_index++;
			}
		for(i = n2; i < rows; i += 1){
				q2[q_index] = q[i];
			for(k = 0; k < cols-1; k += 1)
				t2[q_index][k] = t[i][k];
			q_index++;
		}
		free_all(rows, t, q);
		q = q2;
		t = t2;
		cols--;
		rows = rows_pr;
	}
}

	
unsigned long long dt08jnpf_fm(char* aname, char* cname, int seconds)
{
	FILE*		afile = fopen(aname, "r");
	FILE*		cfile = fopen(cname, "r");

	if (afile == NULL) {
		fprintf(stderr, "could not open file A\n");
		exit(1);
	}

	if (cfile == NULL) {
		fprintf(stderr, "could not open file c\n");
		exit(1);
	}
	
	
	size_t rows, cols;
	assert(fscanf(afile, "%zu %zu\n", &rows, &cols)==2);
	rational_t **a = malloc(rows*sizeof(rational_t*));
	rational_t *c = malloc(rows*sizeof(rational_t));
	assert(fscanf(cfile, "%zu\n", &rows)==1);
	size_t i, j;
	for(i = 0; i < rows; i += 1){
		a[i] = malloc(cols*sizeof(rational_t));
		for(j = 0; j < cols; j += 1) {
			assert(fscanf(afile, "%d\t", &a[i][j].num)==1);
			a[i][j].den = 1;
		}
		assert(fscanf(afile, "\n")==0);
		assert(fscanf(cfile, "%d\n", &c[i].num)==1);
		c[i].den = 1;
	}
	fclose(afile);
    fclose(cfile);
	//print_all(a,c,rows,cols);
	if (seconds == 0) {
		/* Just run once for validation. */
		// Uncomment when your function and variables exist...
		bool result = fm_elim(rows, cols, a, c);
		free_all(rows, a ,c);
		return result;
		//return 0; // return one, i.e. has a solution for now...
	}

	/* Tell operating system to call function DONE when an ALARM comes. */
	signal(SIGALRM, done);
	alarm(seconds);
	/* Now loop until the alarm comes... */
	proceed = true;
	while (proceed) {
		// Uncomment when your function and variables exist...
		fm_elim(rows, cols, a, c);
		fm_count++;
	}
	free_all(rows,a,c);
	return fm_count;
}
