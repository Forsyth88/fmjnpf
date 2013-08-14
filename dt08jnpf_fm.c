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

// - Multiply every t_rj to same determinator instead?
static int fm_elim(size_t rows, size_t cols, rational_t** a, rational_t* c)
{
    size_t i, j;    
    // Copying a and c
    // - Use reallocate with static memory better?
    // - Fastest with rows or column first?
    // - Use memcpy instead of copying one by one?
    rational_t **t = malloc(rows*sizeof(rational_t*));
    rational_t *q = malloc(rows*sizeof(rational_t));
    for (i = 0; i < rows; i += 1){
       t[i] = malloc(cols*sizeof(rational_t));
       q[i] = c[i];
       for(j = 0; j < cols; j += 1)
           t[i][j] = a[i][j];
   }
   printf("Heh2\n");
   rational_t ZERO;
   ZERO.num = 0;
   ZERO.den = 1;
	printf("Heh3\n");
   //size_t r = cols;
   //size_t s = rows;
   size_t n1 = 0;
   size_t n2 = 0;
   while(1){
	printf("Heh4\n");
       // Determine n1 and n2.
       for(i = 0; i < rows; i += 1){
           if(rat_cmp(&t[i][cols-1], &ZERO) > 0)
               n1 += 1;
           else if(rat_cmp(&t[i][cols-1], &ZERO) < 0)
               n2 += 1;
       }
       n2 += n1;
		printf("Heh5\n");
       // Sort positive first, then negative, last 0.
       // - t_temp can be reused?
       rational_t **t_sort = malloc(rows*sizeof(rational_t*));
		rational_t *c_sort = malloc(rows*sizeof(rational_t));
       size_t n1_i = 0, n2_i = n1, n3_i = n2;
       size_t cur = 0;
		printf("n1 = %zu, n2 = %zu, n3 = %zu\n", n1, n2, rows);   
	   while(n1_i != n1 || n2_i != n2 || n3_i != rows){
			
           if(rat_cmp(&t[cur][cols-1], &ZERO) > 0){
				printf("n1\n");
				c_sort[n1_i] = c[cur];               
				t_sort[n1_i++] = t[cur];
           }else if(rat_cmp(&t[cur][cols-1], &ZERO) < 0){
				printf("n2\n");
				c_sort[n2_i] = c[cur];
               t_sort[n2_i++] = t[cur];
           }else{
				printf("n3\n");
				c_sort[n3_i] = c[cur];
               t_sort[n3_i++] = t[cur];
			}           
			cur++;
       }
		printf("Heh7\n");
       free(t);
		free(c);
		c = c_sort;
       t = t_sort;
       print_all(t_sort,c,rows,cols);
       // - Can be done with one for-loop?
       	for(i = 0; i < cols; i += 1)
           	for(j = 0; j < n2; j += 1)
               rat_div(&t[j][i], &t[j][cols-1]);
       	for(j = 0; j < n2; j += 1)
           rat_div(&q[j], &t[j][cols-1]);
		print_all_rat(t,q,rows,cols);
       	if(cols == 0){
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

	        if (rat_cmp(&q[max], &q[min]) > 0) //b1 > B1
	            return false;
	        for(; n2 < rows; n2 += 1)
	            if(rat_cmp(&q[n2], &ZERO) < 0)
	                return false;
	        return true;
		}
		size_t rows_pr = rows - n2 + n1 * (n2 - n1);
		if (rows_pr == 0)
			return true;
		
		 rational_t **t2 = malloc((cols-1)*sizeof(rational_t*));
	     rational_t *q2 = malloc(rows_pr*sizeof(rational_t));
		 for (i = 0; i < cols-1; i += 1)
	         t2[i] = malloc(rows_pr*sizeof(rational_t));
	     size_t k;
	    for(i = 0; i < n1; i += 1)
	         for(j = n1; j < n2; j += 1){
		 		rat_sub(&q[i],&q[j]);
	             q2[i*(n2-n1)+j-n1] = q[i];
	     		for(k = 0; k < cols-1; k += 1) {
		 			rat_sub(&t[i][k],&t[j][k]);
	                 t2[i*(n2-n1)+j-n1][k] = t[i][k];  
		 		}     
		 	}
		 for(i = n2; i < rows; i += 1){
	    		q2[rows_pr-rows+i] = q[i];
	     	for(k = 0; k < cols-1; k += 1)
	         	t2[rows_pr-rows+i][k] = t[i][k];       
		 	}
		 free(t);
		 free(q);
		 q = q2;
		 t = t2;

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
	print_all(a,c,rows,cols);
	printf("HEHEHEHEHE%d\n",seconds);
	if (seconds == 0) {
		/* Just run once for validation. */
		printf("Heh\n");
		// Uncomment when your function and variables exist...
		 return fm_elim(rows, cols, a, c);
		//return 0; // return one, i.e. has a solution for now...
	}

	/* Tell operating system to call function DONE when an ALARM comes. */
	printf("MONGO1\n");	
	signal(SIGALRM, done);
	alarm(seconds);
	/* Now loop until the alarm comes... */
	proceed = true;
	printf("MONGO2\n");
	while (proceed) {
		// Uncomment when your function and variables exist...
<<<<<<< HEAD
		// fm_elim(rows, cols, a, c);
		printf("HAHAHA\n");
		fm_count++;
	}
	printf("HEJDA!\n");
=======
		fm_elim(rows, cols, a, c);

		fm_count++;
	}
>>>>>>> 299d2ca68fa087bd8444b0a3062b7f665040cba6
	return fm_count;
}
