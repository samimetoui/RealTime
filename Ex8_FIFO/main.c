/**
 * Programme : FIFO
 * Auteur : Sami Metoui
 * Description : Implémente une FIFO thread-safe.
 * Une première thread lit le contenu d'un fichier et transmet
 * les caractères lus à un seconde thread un utilisant une FIFO.
 * La seconde thread convertit les caractères reçus en majuscule
 * et transmet les caractères converits à un 3ième thread à travers une FIFO.
 * Ce dernièr thread afficher les caractères reçus à l'écran.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include "tfile.h"
#include <unistd.h>

#define MAX_S 10

/**
 * Fonction lireFichier()
 * Description : Lit le contenu du fichier et enfile les caractères
 * dans la file d'entrée
 * Entrée : pointeur sur file
 */

void* lireFichier(void* t_in) {

  char c_in;
  int i=0;

  FILE* fp=fopen("file.txt","rb");

  while (!feof(fp)) {

    fread(&c_in,sizeof(char),1,fp);
    usleep(1000);
    if (!push(t_in,c_in)) {
      fseek(fp,-1,SEEK_CUR);
      break;
    }
    i++;
  }
  push(t_in,0);

  return NULL;
}

/**
 * Fonction claculer()
 * Description : Extrait les caractères de la file d'entrée,
 * les convertit en majuscule et les enfile dans la file de sortie.
 * Entrée : tableau de pointeur sur files
 */

void* calculer(void* tf) {


  char c_tmp;
  p_file* t = (p_file*) tf;

  int i=0;
  usleep(1000);
  while (pull(&c_tmp,t[0])) {
    c_tmp=toupper(c_tmp);

    push(t[1],c_tmp);
    if(c_tmp==0) break;
    i++;
  }

  return NULL;
}

/**
 * Fonction afficher()
 * Description : Extrait les caractères de la file de sortie et les affichent
 * Entrée : pointeur sur file de sortie
 */

void* afficher(void* t_out) {

  char c_out;
  int i=0;

  while (pull(&c_out,t_out)) {
    if(c_out==0) break;
    printf("%c",c_out);
    i++;
  }
  return NULL;
}

/**
 * MAIN
 */

int main()
{

  p_file t_e;
  p_file t_s;

  t_e=malloc(sizeof(struct s_file));
  t_s=malloc(sizeof(struct s_file));

  p_file tf[2];

  tf[0]= t_e;
  tf[1]= t_s;

  pthread_t threads[3];

  initQueue(t_e);
  initQueue(t_s);

  pthread_create(&threads[0], NULL,lireFichier,(void*)(t_e));
  pthread_create(&threads[1], NULL,calculer,(void*)(tf));
  pthread_create(&threads[2], NULL,afficher,(void*)(t_s));

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);

  return 0;
}

