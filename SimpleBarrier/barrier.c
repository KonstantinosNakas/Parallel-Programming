//Nakas Konstantinos-Panagiotis		2501

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>


pthread_barrier_t bar;		


typedef struct barrier{
	pthread_mutex_t lock;
	pthread_cond_t condvar;
	int count;
	int N;
	bool flag;
}barrier_t;


void barrier_init(barrier_t *b, int n){
	pthread_mutex_init(&b->lock, NULL);
	pthread_cond_init(&b->condvar,NULL);
	b->count = 0;
	b->N = n;
	b->flag = true;
}


void barrier_wait(barrier_t *b){
	bool flag;
		
	pthread_mutex_lock(&b->lock);
	b->count++;
	flag = b->flag;
	if (b->count == b->N){
		b->count = 0;
		pthread_cond_broadcast(&b->condvar);
		b->flag = !b->flag;
	}else{
		while (b->flag == flag){
			pthread_cond_wait(&b->condvar,&b->lock);
		}
	}
	pthread_mutex_unlock(&b->lock);
}


void barrier_destroy(barrier_t *b){
	pthread_mutex_destroy(&b->lock);
	pthread_cond_destroy(&b->condvar);
	b->count = 0;
	b->N = 0;
	b->flag = true;
}


void *threadfunc1(void *arg){
	barrier_t  *b = arg;
	int i;
	
	for (i = 0; i < 100; i++){
		usleep(1000);
		barrier_wait(b);
		//printf("thread calls my barrier_wait %d time\n",i);
	}
}


void *threadfunc2(void *arg){
	int i;
	
	for (i = 0; i < 100; i++){
		usleep(1000);
		pthread_barrier_wait(&bar);
		//printf("thread calls  pthread_barrier_wait %d time\n",i);
	}
}


int main(){
	barrier_t b;
	int i,n;
	struct timeval tv1, tv2,tv3,tv4;
	double time1,time2;

	printf("Dwse to plhthos twn nhmatwn pou summetexoun: \n");
	scanf("%d", &n);

	pthread_t thrid1[n];
	pthread_t thrid2[n];

	barrier_init(&b,n);
	pthread_barrier_init(&bar, NULL, n);

	gettimeofday(&tv1, NULL);
	for (i = 0; i < n; i++){
		pthread_create(&thrid1[i], NULL,threadfunc1, (void *) &b);
	}
	for (i = 0; i < n; i++){
		pthread_join(thrid1[i], NULL);
	}
	gettimeofday(&tv2, NULL);
	time1 = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec - 100000)*1.0E-6;

	gettimeofday(&tv3, NULL);
	for (i = 0; i < n; i++){
		pthread_create(&thrid2[i], NULL,threadfunc2,NULL);
	}
	for (i = 0; i < n; i++){
		pthread_join(thrid2[i], NULL);
	}
	gettimeofday(&tv4, NULL);
	time2 = (tv4.tv_sec - tv3.tv_sec) + (tv4.tv_usec - tv3.tv_usec - 100000)*1.0E-6;

	printf("The average time of my barrier wait is: %lfsec and the average time of pthread_barrier_wait is : %lfsec.\n",time1/100,time2/100);
	
	barrier_destroy(&b);
	pthread_barrier_destroy(&bar);

	return 0;
}
