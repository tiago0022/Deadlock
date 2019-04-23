/*Semelhante ao anterior, porém existem 2 threads aguardando
pela condição e apenas 1 aumentando o valor do contador.*/

//Quando um sinal é enviado, qual thread o receberá ? 

//SOURCE:
//https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS  3
#define TCOUNT 15
#define COUNT_LIMIT 4

int     count = 0;
int     thread_ids[3] = {0,1,2};

pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void *inc_count(void *t) 
{
  long my_id = (long)t;
  while(1){
    pthread_mutex_lock(&count_mutex);
    count++;
    /*Se a condição foi atendida, então sinaliza
	a thread que está esperando. Note que o mutex
	está bloqueado. */
    printf("(%ld) count = %d\n", my_id, count);
    if (count == COUNT_LIMIT) {
      pthread_cond_signal(&count_threshold_cv);
      printf("(%ld) condição atendida (count= %d).\n", my_id, count);
    }
    pthread_mutex_unlock(&count_mutex);
	/*Espera 1 segundo de tal forma que as outras threads possam
	acessar a região crítica.*/
    sleep(1);//O QUE PODE ACONTECER SE ESTE SLEEP FOR RETIRADO? 
	 //Entenda a saída do programa.
  }
  pthread_exit(NULL);
}

void *watch_count(void *t) 
{
  long my_id = (long)t;
  pthread_mutex_lock(&count_mutex);
  while (1) {
	 printf("(%ld) aguardando sinal.\n", my_id);
	 /*Observe que a espera ocorre por meio do "count_threshold".
	 Enquanto espera, libera o "count_mutex".
	 Após receber o sinal, o "count_mutex" deve ser bloqueado novamente.*/
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
	 /*ANALISE A AFIRMATIVA: 
	 Mesmo após receber um sinal, a thread pode continuar bloqueada
	 caso o "count_mutex" esteja bloqueado.*/
    printf("(%ld) sinal recebido.\n", my_id);
    count = 0;
    printf("(%ld) contador alterado para %d.\n", my_id, count);
  }
  pthread_mutex_unlock(&count_mutex);
  pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
  int i, rc;
  long t1=1, t2=2, t3=3;
  pthread_t threads[3];
  pthread_attr_t attr;

  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init (&count_threshold_cv, NULL);

  pthread_attr_init(&attr);
  pthread_create(&threads[0], &attr, watch_count, (void *)t1);
  pthread_create(&threads[1], &attr, watch_count, (void *)t2);
  pthread_create(&threads[2], &attr, inc_count, (void *)t3);

  /* Wait for all threads to complete */
  for (i=0; i<NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  printf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

  /* Clean up and exit */
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit(NULL);

}
