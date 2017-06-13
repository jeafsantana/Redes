/* Exemplo usando threads.
 * A funcao thread_exemplo() sera chamada duas vezes e irá executar em 
 * paralelo. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS	2

void * thread_exemplo(void *args)
{
	int i;
	long tid;
	tid = (long) args;
	
	for (i = 0; i < 10; i++) {
		printf("Ola mundo! Eu sou a thread %ld.\n", tid);
		sleep(1);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	
	
	
	
	
	pthread_t threads[NUM_THREADS];
	long tid;

	for (tid = 0; tid < NUM_THREADS; tid++) {
		printf("Criando thread %ld.\n", tid);
		if (pthread_create(&threads[tid], NULL, thread_exemplo, (void *) tid) != 0) {
			printf("Erro ao criar a thread.\n");
			exit(-1);
		}
	}

	/* Ultima coisa que a main() deve fazer. */
	pthread_exit(NULL);
}
