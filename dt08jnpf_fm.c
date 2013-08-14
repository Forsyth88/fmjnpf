#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "rational.h"

static unsigned long long	fm_count;
static volatile bool		proceed = false;

static void done(int unused)
{
	proceed = false;
	unused = unused;
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

   rational_t ZERO, ONE;
   ZERO.num = 0;
   ZERO.den = 1;
   ONE.num = 1;
   ONE.den = 1;

   size_t r = cols;
   size_t s = rows;
   size_t n1;
   size_t n2;
   while(1){

       // Determine n1 and n2.
       for(i = 0; i < r; i += 1){
           if(rat_cmp(&t[i][r-1], &ZERO) > 0)
               n1 += 1;
           else if(rat_cmp(&t[i][r-1], &ZERO) < 0)
               n2 += 1;
       }
       n2 += n1;

       // Sort positive first, then negative, last 0.
       // - t_temp can be reused?
       rational_t **t_sort = malloc(r*sizeof(rational_t*));
       size_t n1_i = 0, n2_i = n1, n3_i = n2;
       size_t cur = 0;
       while(n1_i != n1 || n2_i != n2 || n3_i != r){
           if(rat_cmp(&t[cur][r-1], &ZERO) > 0)
               t_sort[n1_i++] = t[cur];
           else if(rat_cmp(&t[cur][r-1], &ZERO) < 0)
               t_sort[n2_i++] = t[cur];
           else
               t_sort[n3_i++] = t[cur];
           cur++;
       }
       free(t);
       t = t_sort;
       
       // - Can be done with one for-loop?
       for(i = 0; i < r; i += 1)
           for(j = 0; j < n2; j += 1)
               rat_div(&t[j][i], &t[j][r]);
       for(j = 0; j < n2; j += 1)
           rat_div(&q[j], &t[j][r]);
       if(r == 0){
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
        for(; n2 < s; n2 += 1)
            if(rat_cmp(&q[n2], &ZERO) < 0)
                return false;
        return true;
	}
	size_t s_pr = s - n2 + n1 * (n2 - n1);
	if (s_pr == 0)
		return true;
	
	rational_t **t2 = malloc((r-1)*sizeof(rational_t*));
    rational_t *q2 = malloc(s_pr*sizeof(rational_t));
	for (i = 0; i < r-1; i += 1)
        t2[i] = malloc(s_pr*sizeof(rational_t));
    int k;
    for(i = 0; i < n1; i += 1)
        for(j = n1; j < n2; j += 1){
			rat_sub(&q[i],&q[j]);
            q2[i*(n2-n1)+j-n1] = q[i];
    		for(k = 0; k < r-1; k += 1) {
				rat_sub(&t[i][k],&t[j][k]);
                t2[i*(n2-n1)+j-n1][k] = t[i][k];  
			}     
		}
	for(i = n2; i < s; i += 1){
   		q2[s_pr-s+i] = q[i];
    	for(k = 0; k < r-1; k += 1)
        	t2[s_pr-s+i][k] = t[i][k];       
		}
	free(t);
	free(q);
	q = q2;
	t = t2;
   }    
}

void print_all(rational_t** A, rational_t* cc, size_t r, size_t c) {
	int i;	
	int j;	
	for (i = 0; i < r; i++) {
		rational_t* row = A[i];
		for (j = 0; j < c; j++) {
			printf("%d ", row[j].num);
		}
		printf(" = %d\n", cc[i].num);
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
	fscanf(afile, "%zu %zu\n", &rows, &cols);
	rational_t **a = malloc(rows*sizeof(rational_t*));
   	rational_t *c = malloc(rows*sizeof(rational_t));
	fscanf(cfile, "%zu\n", &rows);
	size_t i, j;
	for(i = 0; i < rows; i += 1){
		a[i] = malloc(cols*sizeof(rational_t));
		for(j = 0; j < cols; j += 1) {
			fscanf(afile, "%d\t", &a[i][j].num);
			a[i][j].den = 1;
		}
		fscanf(afile, "\n");
		fscanf(cfile, "%d\n", &c[i].num);
		c[i].den = 1;
	}
	
	print_all(a,c,rows,cols);

	if (seconds == 0) {
		/* Just run once for validation. */
			
		// Uncomment when your function and variables exist...
		 return fm_elim(rows, cols, a, c);
		//return 0; // return one, i.e. has a solution for now...
	}

	/* Tell operating system to call function DONE when an ALARM comes. */
	signal(SIGALRM, done);
	alarm(seconds);

	/* Now loop until the alarm comes... */
	proceed = true;
	while (proceed) {
		// Uncomment when your function and variables exist...
		// fm_elim(rows, cols, a, c);

		fm_count++;
	}

	return fm_count;
}
