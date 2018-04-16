/**
 * Fichier header : tfile.h
 * Auteur : Sami Metoui
 */

#ifndef TFILE_INCLUDED
#define TFILE_INCLUDED
#define MAX 10

/**
 * structure s_file
 */

struct s_file {
  sem_t sem_e;          //semaphore d'entrée
  sem_t sem_f;          //semaphore de sortie
  pthread_mutex_t mut;
  char tab[MAX];
  int iFirst;
  int iLast;
};

typedef struct s_file* p_file;

/**
 * Prototype des fonctions de la FIFO
 */

int pull(char*, p_file);
int push(p_file, char);
void initQueue(p_file);
void displayQueue(p_file);

#endif // TFILE_INCLUDED
