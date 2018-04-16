/**
* Programme: LecteurEcrivain
* Auteur: Sami Metoui
* Description: Démarre des threads lecteurs
* et des threads ecrivains. L'écriture est exclusive.
* Plusieurs lectures simultanées sont permises.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "LecteurEcrivain.h"

#define NB_LECTEUR 5
#define NB_ECRIVAIN 2

int dataLue;

/**
 * structure paramètre du lecteur
 */

struct lparam {
  int id;
  s_le* l;
};

/**
 * structure paramètre de l'ecrivain
 */

struct eparam {
  int id;
  int data;
  s_le* l;
};

/**
 * Fonction: t_lire()
 * Descritption: thread lire
 * Entrée : pointeur sur structure lParam
 */

void* t_lire(void* pl) {
  int i;
  struct lparam* ple=(struct lparam*)pl;
  for (i = 0; i < 5; i++) {
    int tempo = (float)rand()/RAND_MAX * ple->id * 100 + 100;
    usleep(10000*tempo);
    dataLue=lire(ple->l,ple->id*10+i+1);
  }
  return NULL;
}

/**
 * Fonction: t_ecrire()
 * Descritption: thread ecrire
 * Entrée : pointeur sur structure eParam
 */

void* t_ecrire(void* pe) {
  int i;
  struct eparam* ple=(struct eparam*)pe;
  for (i = 0; i < 5; i++) {
    int tempo = (float)rand()/RAND_MAX * ple->id * 100 + 100;
    usleep(10000*tempo);
    ple->data=tempo;
    ecrire(ple->l,ple->data,ple->id*10+i+1);

  }
  return NULL;
}

/**
 * Fonction: init_le()
 * Descritption: initialise un objet LecteurEcrivain
 * Entrée : pointeur sur structure LecteurEcrivain
 */

void init_le(s_le* le) {
  le->data=0;
  le->nb_ecriv_atten=0;
  le->nb_lecteur=0;
  pthread_mutex_init(&le->lock, NULL);
  pthread_cond_init(&(le->lecteur), NULL);
  pthread_cond_init(&(le->ecrivain), NULL);
}

/**
 * MAIN
 */

int main(int argc, char *argv[])
{
  int i;

  struct lparam* pLecteurs[NB_LECTEUR];
  struct eparam* pEcrivains[NB_ECRIVAIN];

  pthread_t tLecteurs[NB_LECTEUR];
  pthread_t tEcrivains[NB_ECRIVAIN];

  s_le* lect_ecriv=malloc(sizeof(s_le));

  init_le(lect_ecriv);

  for (i=0; i<NB_LECTEUR; i++) {
    pLecteurs[i]=malloc(sizeof(struct lparam));
    pLecteurs[i]->id=i+1;
    pLecteurs[i]->l=lect_ecriv;
  }

  for (i=0; i<NB_ECRIVAIN; i++) {
    pEcrivains[i]=malloc(sizeof(struct eparam));
    pEcrivains[i]->id=i+1;
    pEcrivains[i]->data=0;
    pEcrivains[i]->l=lect_ecriv;
  }

  for (i=0; i<NB_LECTEUR; i++) {
    pthread_create(&tLecteurs[i], NULL,t_lire,(void*)(pLecteurs[i]));
  }

  for (i=0; i<NB_ECRIVAIN; i++) {
    pthread_create(&tEcrivains[i], NULL,t_ecrire,(void*)(pEcrivains[i]));
  }

  for (i=0; i<NB_LECTEUR; i++) {
    pthread_join(tLecteurs[i], NULL);
    if (i<NB_ECRIVAIN) pthread_join(tEcrivains[i], NULL);
  }

  return 0;
}
