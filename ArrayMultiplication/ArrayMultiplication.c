//Nakas Konstantinos-Panagiotis		2501

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>


#define N 1024
#define _mat(i,j) (mat[(i)*n + (j)])
#define NTHR 16
#define M 32
#define S N/M
int A[N][N], B[N][N], C[N][N];
int taskid = 0;
pthread_mutex_t tlock = PTHREAD_MUTEX_INITIALIZER;


int readmat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "r")) == NULL)
		return (-1);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (fscanf(fp, "%d", &_mat(i,j)) == EOF)
			{
				fclose(fp);
				return (-1); 
			};
	fclose(fp);
	return (0);
}

int writemat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "w")) == NULL)
		return (-1);
	for (i = 0; i < n; i++, fprintf(fp, "\n"))
		for (j = 0; j < n; j++)
			fprintf(fp, " %d", _mat(i, j));
	fclose(fp);
	return (0);
}


void block_mult(int t){
	int i, j, k, x, y, me = t;
	double sum;

	x = me/M;
	y = me%M;
	for (i = x*S; i < (x+1)*S; i++){
		for (j = y*S; j < (y+1)*S; j++) {
			for (k = 0, sum = 0.0; k < N; k++){
				sum += A[i][k]*B[k][j];	
			}
		C[i][j] = sum;
		}
	}
}


void *thrfunc(void *arg){
	int t;

	while (1){
		pthread_mutex_lock(&tlock);
		t = taskid++;
		pthread_mutex_unlock(&tlock);
		if (t >= M*M){
			break;
		}
		block_mult(t);
	}
}



int main(){
	int i;	
	pthread_t thr[NTHR];
	struct timeval tv1, tv2;
	double time;

	if (readmat("Amat1024.txt", (int *) A, N) < 0) 
		exit( 1 + printf("file problem\n") );
	if (readmat("Bmat1024.txt", (int *) B, N) < 0) 
		exit( 1 + printf("file problem\n") );

	gettimeofday(&tv1, NULL);
	
	for (i = 0; i < NTHR; i++){
		pthread_create(&thr[i], NULL, thrfunc, (void *) i);
	}
	for (i = 0; i < NTHR; i++){
		pthread_join(thr[i], NULL);
	}

	gettimeofday(&tv2, NULL);
	time = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)*1.0E-6;
	printf("O pragmatikos xronos ekteleshs einai: %lf sec.\n", time);

	writemat("Results.txt", (int *) C, N);

	return 0;
}

