/**
 * Programme philosophes
 * Auteur: Sami Metoui
 * Description: programme qui met en évidence le problème du partage
 * des ressources utilisées par des threads. problème des philosophes.
 * Sans famine.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define NB_PHIL 4

void mange(int*);
void pense(int*);
void* philosophe(void*);

pthread_mutex_t baguette[NB_PHIL];

/**
 * MAIN
 */

int main()
{
  int philId[NB_PHIL];
  int i,rc;

  pthread_t threads[NB_PHIL];

  for(i=0; i<NB_PHIL; i++) pthread_mutex_init (&baguette[i], NULL);

  for(i=0; i<NB_PHIL; i++) philId[i]=i;

  for(i=0; i<NB_PHIL; i++)
  {

    rc = pthread_create(&threads[i], NULL,philosophe,(void*)(&philId[i]));
    if (rc)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  for(i=0; i<NB_PHIL; i++) pthread_join(threads[i], NULL);

  for(i=0; i<NB_PHIL; i++) pthread_mutex_destroy(&baguette[i]);

  return 0;
}

/**
 * Fonction pense()
 * interromp le programme pendant une durée aléatoire.
 * Entrée :  id du philosophe.
 */

void pense(int* Id)
{
  int tempo = (float)rand()/RAND_MAX * (*Id) * 100 + 100;
  printf("Le philosophe %i pense\n",(int)(*Id));
  usleep(1000*tempo);

  return;
}

/**
 * Fonction mange()
 * interromp le programme pendant une durée aléatoire.
 * Entrée :  id du philosophe.
 */

void mange(int* Id)
{
  int tempo = (float)rand()/RAND_MAX * (*Id) * 100 + 100;
  printf("Le philosophe %i mange\n",(int)(*Id));
  usleep(1000*tempo);

  return;
}

/**
 * Fonction philosophe()
 * Fait manger un philosophe lorsqu'il dispose de sa
 * baguette et celle à sa droite. Fait penser le philosophe
 * lorsqu'il relache une baguette.
 * Note: afin d'éviter la famine, les philosophes d'id pair
 * commencent par prendre leur baguette, les autres commencent
 * par prendre la baguette qui est à leur droite.
 * Entrée : entier, Id du philosophe.
 */

void* philosophe(void* Id)
{

 if ((*(int*)Id)%2==0)
  {
    pthread_mutex_lock(&baguette[*(int*)Id]);
    usleep(100);
    pthread_mutex_lock(&baguette[(*(int*)Id+1)%NB_PHIL]);
  }
  else
  {
    pthread_mutex_lock(&baguette[(*(int*)Id+1)%NB_PHIL]);
    pthread_mutex_lock(&baguette[*(int*)Id]);
  }

  mange((int*) Id);

  if ((*(int*)Id)%2==0)
  {
    pthread_mutex_unlock(&baguette[*(int*)Id]);
    pense((int*) Id);
    pthread_mutex_unlock(&baguette[(*(int*)Id+1)%NB_PHIL]);
  }
  else
  {
    pthread_mutex_unlock(&baguette[(*(int*)Id+1)%NB_PHIL]);
    pthread_mutex_unlock(&baguette[*(int*)Id]);
  }
  pense((int*) Id);

  pthread_exit(NULL);

  return NULL;
}
