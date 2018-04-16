#define _GNU_SOURCE
#include <fcntl.h>
#include <sched.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>

static sem_t * semaphore_1 = NULL;
static sem_t * semaphore_2 = NULL;
static struct timeval * avant = NULL;

void producteur (void)
{
	sleep(1);
	while (1) {
		gettimeofday(avant, NULL);
		sem_post(semaphore_1);
		sem_wait(semaphore_2);
	}
}

#define NB_MESURES 1000

void consommateur(void)
{
	struct timeval apres;

	long int * durees = NULL;
	durees = calloc(NB_MESURES, sizeof(long int));
	if (durees == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	int n = 0;
	while (n < NB_MESURES) {
		sem_wait(semaphore_1);
		gettimeofday(& apres, NULL);
		durees[n]  = apres.tv_sec - avant->tv_sec;
		durees[n] *= 1000000;
		durees[n] += apres.tv_usec - avant->tv_usec;
		n++;
		sem_post(semaphore_2);
	}
	for (n=0; n < NB_MESURES; n ++)
		printf("%ld\n", durees[n]);
}


int main(void)
{	
	int md;
	cpu_set_t ensemble;

	semaphore_1 = sem_open("/semaphore-1", O_CREAT | O_RDWR, 0600, 0);
	if (semaphore_1 == SEM_FAILED) {
		perror("/semaphore-1");
		exit(EXIT_FAILURE);
	}
	sem_unlink("/semaphore-1");

	semaphore_2 = sem_open("/semaphore-2", O_CREAT | O_RDWR, 0600, 0);
	if (semaphore_2 == SEM_FAILED) {
		perror("/semaphore-2");
		exit(EXIT_FAILURE);
	}
	sem_unlink("/semaphore-2");

	md = shm_open("/memoire", O_CREAT | O_RDWR, 0600);
	if (md < 0) {
		perror("/memoire");
		exit(EXIT_FAILURE);
	}
	ftruncate(md, sizeof(struct timeval));
	avant = mmap(NULL, sizeof(struct timeval), PROT_READ | PROT_WRITE, MAP_SHARED, md, 0);
	if (avant == MAP_FAILED) {
		perror("/memoire");
		exit(EXIT_FAILURE);
	}
	shm_unlink("/memoire");

	CPU_ZERO(& ensemble);

	if (fork() == 0) {
		/* Premier fils */
		CPU_SET(1, & ensemble);
		sched_setaffinity(0, sizeof(cpu_set_t), & ensemble);
		producteur();
		exit(EXIT_SUCCESS);
	}

	if (fork() == 0) {
		/* Second fils */
		CPU_SET(0, & ensemble);
		sched_setaffinity(0, sizeof(cpu_set_t), & ensemble);
		consommateur();
		exit(EXIT_SUCCESS);
	}
	
	/* pere */
	waitpid(-1 , NULL, 0);
	waitpid(-1 , NULL, 0);

	return EXIT_SUCCESS;
}

