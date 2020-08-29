//Nakas Konstantinos-Panagiotis	2501

#include <stdio.h>
#include <omp.h>

#define UPTO 10000000

long int count,      /* number of primes */
         lastprime;  /* the last prime found */


void serial_primes(long int n) {
	long int i, num, divisor, quotient, remainder;

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	for (i = 0; i < (n-1)/2; ++i) {    /* For every odd number */
		num = 2*i + 3;

		divisor = 1;
		do 
		{
			divisor += 2;                  /* Divide by the next odd */
			quotient  = num / divisor;  
			remainder = num % divisor;  
		} while (remainder && divisor <= quotient);  /* Don't go past sqrt */

		if (remainder || divisor == num) /* num is prime */
		{
			count++;
			lastprime = num;
		}
	}
}


void openmp_primes(long int n) {
	long int i, num, divisor, quotient, remainder;

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	/* 
	 * Parallelize the serial algorithm but you are NOT allowed to change it!
	 * Don't add/remove/change global variables
	 */

	#pragma omp parallel for shared(lastprime) private(num,divisor,quotient,remainder) reduction(+:count) schedule(guided)
		for (i = 0; i < (n-1)/2; ++i) {    /* For every odd number */
			num = 2*i + 3;
	
			divisor = 1;
			do 
			{
				divisor += 2;                  /* Divide by the next odd */
				quotient  = num / divisor;  
				remainder = num % divisor;  
			} while (remainder && divisor <= quotient);  /* Don't go past sqrt */

			if (remainder || divisor == num) /* num is prime */
			{
				count++;
				lastprime = num;
			}
		}
}


int main()
{
	double t1, t2, t3, t4, serial_time, parallel_time;
	
	printf("Serial and parallel prime number calculations:\n\n");
	
	/* Time the following to compare performance 
	 */
	t1 = omp_get_wtime();
	serial_primes(UPTO);        /* time it */
	t2 = omp_get_wtime();
	serial_time = t2 - t1;
	printf("[serial] count = %ld, last = %ld (time = %lf)\n", count, lastprime,serial_time);

	t3 = omp_get_wtime();
	openmp_primes(UPTO);        /* time it */
	t4 = omp_get_wtime();
	parallel_time = t4 - t3;
	printf("[openmp] count = %ld, last = %ld (time = %lf)\n", count, lastprime,parallel_time);
	
	return 0;
}

