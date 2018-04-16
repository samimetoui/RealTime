/**
 * Programme : tfile.c
 * Auteur : Sami Metoui
 * Description : programme qui implémente une file safe-tread
 * d'entiers dans un tableau
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "tfile.h"

/**
 * Fonction qui initialise la file
 * Entrée: pointeur sur file
 */

void initQueue(p_file t)
{
  sem_init(&t->sem_f,0,MAX);            //initialise le sémaphore Full de la taille de la FIFO
  sem_init(&t->sem_e,0,0);              //initialise le sémaphore Empty à zéro
  pthread_mutex_init (&t->mut, NULL);   //initizalize le mutex

  t->iFirst=0;
  t->iLast=0;
}

/**
 * Fonction qui défile un entier
 * Entrée : pointeur sur caractère défilé, pointeur de file
 * Sortie : 0 lorsque la file est vide sinon 1
 */

int pull(char* retv, p_file t)
{
  int e;

  sem_wait(&t->sem_e);          //décrémente le sémaphore Empty
  pthread_mutex_lock(&t->mut);  //début de la section critique

  if(t->iLast == t->iFirst) {   //Lorsque iFirst et iLast sont égaux la file est vide
    return 0;                  //Queue is empty
  }
  e=t->tab[(t->iLast%MAX)];
  t->iLast++;
  t->iLast=t->iLast%(2*MAX);    //lors que iLast dépasse la valeur 2MAX il est mis à 0
  *retv = e;                    //afin que les indexes n'augmentent pas indéfiniment

  sem_post(&t->sem_f);          //incrémente le sémaphore Full
  pthread_mutex_unlock(&t->mut);//fin de la section critique
  return 1;
}

/**
 * Fonction qui enfile un caractère
 * Entrée : pointeur de file, caractère à enfiler
 */

int push(p_file t, char e)
{
  sem_wait(&t->sem_f);          //décrémente le sémaphore Full
  pthread_mutex_lock(&t->mut);  //début de la section critique

  if(t->iLast == t->iFirst || t->iLast%MAX != t->iFirst%MAX)
    //lorsque iFirst et iLast sont égaux la file est vide
    //losque iFirst%MAX et iLast%MAX sont <> la file
    //n'est pas encore pleine
  {
    t->tab[(t->iFirst % MAX)]=e;
    t->iFirst++;
    t->iFirst=t->iFirst%(2*MAX);    //lors que iFirst dépasse la valeur 2MAX il est mis à 0
  }                                 //afin que les indexes n'augmentent pas indéfiniment
  else return 0;

  sem_post(&t->sem_e);            //incrémente le sémaphore Empty
  pthread_mutex_unlock(&t->mut);  //fin de la section critique

  return 1;
}

/**
 * Fonction qui affiche le contenu de la file
 * Entrée : pointeur de file
 */

void displayQueue(p_file t)
{
  int i;
  for (i=0; i<MAX; i++) printf("%i-(%i); ",i,t->tab[i]);
  printf("\n");
  printf("iFirst=%i\n",t->iFirst);
  printf("iLast=%i\n",t->iLast);
}

