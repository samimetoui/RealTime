/**
 * Fichier barriere.c
 * Auteur: Sami Metoui
 * Description : fonction barrière réutilisable
 * Utilise un second sémaphore comme deuxième barrière (tourniquet).
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "barriere.h"

static int count = 0;

/**
 * Fonction reusableBarrier()
 * Description: Le dernier coureur ferme la seconde barrière "derrière lui".
 * Les coureurs en attente sont libérés un à un. Au tour suivant, le premièr
 * arrivé ouvre la seconde barrière et ferme la première barrière "devant lui"
 * Entrée: pointeur sur entier, coureur
 *         entier, numéro du tour
 */

void reusableBarrier(int* coureur,int tour) {

  sem_wait(&mutex);
  count++;
  if (count == NB_COUREUR) {
    sem_wait(&barriere2);
    sem_post(&barriere);
  }
  printf("Le coureur %i arrive.\n",*(int*) coureur);
  sem_post(&mutex);
  sem_wait(&barriere);
  sem_post(&barriere);

  //point critique
  sem_wait(&mutex);
  count--;

  if (count == 0) {
    sem_wait(&barriere);
    sem_post(&barriere2);
    printf("Les coureurs arrivent. %i (èr)me tour.\n",tour+1);
  }
  sem_post(&mutex);
  sem_wait(&barriere2);
  sem_post(&barriere2);
}
