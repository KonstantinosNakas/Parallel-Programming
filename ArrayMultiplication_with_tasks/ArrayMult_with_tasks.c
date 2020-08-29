//Nakas Konstantinos-Panagiotis	2501


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define _mat(i,j) (mat[(i)*n + (j)])
#define N 1024

int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);
void block_mult(int t,int S);


int main(int argc, char **argv){
	double start, end, time;
	int task, i, j, k, sum, S, M;

	if (argc > 1){
		S = atoi(argv[1]);
	}
	
	if (readmat("Amat1024.txt", (int *) A, N) < 0) 
		exit( 1 + printf("file problem\n") );
	if (readmat("Bmat1024.txt", (int *) B, N) < 0) 
		exit( 1 + printf("file problem\n") );
	
	M = N/S;
	int total_blocks = M*M;
	
	start = omp_get_wtime();

	#pragma omp parallel for firstprivate(S,total_blocks) num_threads(4) schedule(static)
		for (task = 0; task < total_blocks; task++){
			#pragma omp task firstprivate(task,S) 
				block_mult(task,S);
		}
	
	end = omp_get_wtime();
	time = end - start;
	printf("O xronos ekteleshs einai: %lf sec\n",time);

	writemat("Results.txt",(int *) C, N);
	
	return 0;
}


void block_mult(int t, int S){
	int i, j, k, x, y, me = t;
	double sum;

	x = me/(N/S);
	y = me%(N/S);
	for (i = x*S; i < (x+1)*S; i++){
		for (j = y*S; j < (y+1)*S; j++) {
			for (k = 0, sum = 0.0; k < N; k++){
				sum += A[i][k]*B[k][j];	
			}
			C[i][j] = sum;
		}
	}
}


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
