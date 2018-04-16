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

static sem_t * semaphore = NULL;
static struct timeval * avant = NULL;

void producteur (void)
{
	sleep(1);
	while (1) {
		gettimeofday(avant, NULL);
		sem_post(semaphore);
		usleep(5000000);
	}
}


void consommateur(void)
{
	struct timeval apres;
	long int duree;
	while (1) {
		sem_wait(semaphore);
		gettimeofday(& apres, NULL);
		duree  = apres.tv_sec - avant->tv_sec;
		duree *= 1000000;
		duree += apres.tv_usec - avant->tv_usec;
		printf("%ld\n", duree);
	}
}


int main(void)
{	
	int md;
	cpu_set_t ensemble;

	semaphore = sem_open("/semaphore", O_CREAT | O_RDWR, 0600, 0);
	if (semaphore == SEM_FAILED) {
		perror("/semaphore");
		exit(EXIT_FAILURE);
	}
	sem_unlink("/semaphore");

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

