//Nakas Konstantinos-Panagiotis	2501

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1024
int A[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);   
void array_mult(int myid, int nproc);    
#define _mat(i,j) (mat[(i)*n + (j)])


int main(int argc, char *argv[]){
	int myid, nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	array_mult(myid, nproc);
	
	MPI_Finalize();

	return (0);
}



void array_mult(int myid, int nproc){
	int i, j, k, sum; 
	int WORK = N/nproc;		//plhthos grammwn gia kathe diergasia
	int *mypart, *Apart;
	double t1, t2, t3, t4, total_comm, total;
	int B[N][N];


	if (myid == 0){
		if (readmat("Amat1024.txt", (int *) A, N) < 0) 
			exit( 1 + printf("file problem\n") );
		if (readmat("Bmat1024.txt", (int *) B, N) < 0) 
			exit( 1 + printf("file problem\n") );
	}
	
	if ((mypart = (int *) malloc(WORK * N * sizeof(int ))) == NULL){
		printf("Error in malloc");
		exit(1);
	}
	if ((Apart = (int *) malloc(WORK * N  * sizeof(int ))) == NULL){
		printf("Error in malloc");
		exit(1);
	}

	if (myid == 0) t1 = MPI_Wtime();

	MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, WORK*N, MPI_INT, Apart, WORK*N, MPI_INT, 0, MPI_COMM_WORLD);	
	
	if (myid == 0) t2 = MPI_Wtime();

	for (i = 0; i < WORK; i++){
		for (j = 0; j < N; j++)
		{
			sum = 0;
			for (k = 0; k < N; k++){
				sum += Apart[(i*N)+k]*B[k][j];
			}
			mypart[(i*N)+j] = sum;
		}
	}
	
	if (myid == 0) t3 = MPI_Wtime();
	
	MPI_Gather(mypart, WORK*N, MPI_INT, C, WORK*N, MPI_INT, 0, MPI_COMM_WORLD);

	if (myid == 0){
		t4 = MPI_Wtime();
		total_comm = (t2 - t1) + (t4 - t3);
		total = t4 - t1;
		printf("O sunolikos xronos gia thn epikoinwnia einai: %f\n", total_comm);
		printf("Genika o sunolikos xronos einai: %f\n", total);
	}
		
	if (myid == 0){
		writemat("Results.txt", (int *) C, N);
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
