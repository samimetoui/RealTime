/**
* Fichier: LecteurEcrivain.c
* Auteur: Sami Metoui
* description: fonctions utilisées par Lecteurecrivain
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "LecteurEcrivain.h"

/**
 * Fonction : lire()
 * Description : Démarre un lecteur lorsqu'il n'y pas d'écrivain
 * en attente.
 * Entrée : pointeur sur structure LecteurEcrivain
 *          entier, id de l'objet
 * Sortie : entier, data lue
 */

int lire(s_le* this,int t_id) {
  int readdat;
  pthread_mutex_lock(&this->lock);
  while (this->nb_ecriv_atten > 0) {
    pthread_cond_wait(&this->lecture_ecriture,&this->lock);
    this->nb_lecteur++;
  }
  pthread_mutex_unlock(&this->lock);
  readdat=lire_data(this,t_id);
  pthread_mutex_lock(&this->lock);
  this->nb_lecteur--;
  pthread_cond_broadcast(&this->lecture_ecriture);
  pthread_mutex_unlock(&this->lock);
  return readdat;
}

/**
 * Fonction : ecrire()
 * Description : Démarre une ecriture lorsqu'il n'y pas de lecteurs
 * et libère tout les LecteursEcrivains en en attente.
 * Entrée : pointeur sur structure LecteurEcrivain
 *          entier, data à écrire
 *          entier, id de l'objet
 */

void ecrire(s_le* this, int data,int t_id) {
  pthread_mutex_lock(&this->lock);
  this->nb_ecriv_atten++;
  while(this->nb_lecteur>0) {
    pthread_cond_wait(&this->lecture_ecriture,&this->lock);
  }
  this->nb_ecriv_atten--;
  ecrire_data(this,data,t_id);
  pthread_cond_broadcast(&this->lecture_ecriture);
  pthread_mutex_unlock(&this->lock);
}

/**
 * Fonction ecrire_data()
 * Entrée : pointeur sur structure LecteurEcrivain
 *          entier, data à écrire
 *          entier, id de l'objet
 */

void ecrire_data(s_le* this,int dat,int t_id) {
  printf("Ecrivain %i - data %i",t_id,dat);printf("\n");
  this->data=dat;
}

/**
 * Fonction lire_data()
 * Entrée : pointeur sur structure LecteurEcrivain
 *          entier, id de l'objet
 * Sortie : entier, data lue
 */

int lire_data(s_le* this,int t_id) {
  printf("Lecteur %i - data %i",t_id,this->data);printf("\n");
  return this->data;
}
