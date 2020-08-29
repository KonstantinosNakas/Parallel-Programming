//Nakas Konstantinos-Panagiotis	2501

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
	int i, N, myid, nproc, taskid, source;
	double W, result = 0.0, temp;
	MPI_Status status;
	int lower_bound, upper_bound;
	double t1, t2, t3, t4, total_comm_per_proc = 0, total;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	if (myid == 0){
		printf("Enter number of divisions: \n");
		scanf("%d", &N);
	}
	
	if (myid == 0) t1 = MPI_Wtime();
	
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	W = 1.0/N;
	if (myid == 0){
		for (i = 1; i < nproc; i++){
			taskid = i;
			MPI_Send(&taskid, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		while ((taskid*10000) < N){
			MPI_Recv(&temp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			source = status.MPI_SOURCE;
			taskid = taskid + 1;
			result += temp;
			MPI_Send(&taskid, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
		}
		taskid = taskid + 1;
		for (i = 1; i < nproc; i++){
			MPI_Send(&taskid, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		printf("To teliko apotelesma einai: %lf\n",result);	
	}else{
		while(1){
			t3 = MPI_Wtime();
			MPI_Recv(&taskid, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			t4 = MPI_Wtime();
			total_comm_per_proc += t4 - t3;
			if (taskid > N/10000){
				break;
			}
			result = 0.0;
			lower_bound = (taskid - 1) * 10000;
			upper_bound = taskid * 10000;
			if (upper_bound <= N-10000){
				for (i = lower_bound; i <= upper_bound; i++){
					result += 4*W / (1 + (i+0.5)*(i+0.5)*W*W);
				}
			}else{
				for (i = lower_bound; i <= N; i++){
					result += 4*W / (1 + (i+0.5)*(i+0.5)*W*W);
				}
			}	
			MPI_Send(&result, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);			
		}
	}
	
	if (myid != 0)
		printf("O sunolikos xronos gia thn epikoinwnia sthn diergasia %d einai: %f\n",myid, total_comm_per_proc);


	if (myid == 0) t2 = MPI_Wtime();
	if (myid == 0){
		total = t2 - t1;
		printf("O xronos upologismou einai: %f\n", total);
	}

	
	MPI_Finalize();

	return (0);
}	
