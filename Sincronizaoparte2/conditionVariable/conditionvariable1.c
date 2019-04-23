//SOURCE:
//https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS  3
#define TCOUNT 5
#define COUNT_LIMIT 6

int     count = 0;
int     thread_ids[3] = {0,1,2};

pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void *inc_count(void *t) 
{
  int i;
  long my_id = (long)t;
  for (i=0; i<TCOUNT; i++) {
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
	/*Espera 1 segundo de tal forma que as threads possam
	acessar a região crítica.*/
    sleep(1);
  }
  pthread_exit(NULL);
}

void *watch_count(void *t) 
{
  long my_id = (long)t;
  pthread_mutex_lock(&count_mutex);
  /*Se o contador for maior do que o limite, não faz
	sentido aguardar pelo sinal pois, da forma como o
	programa foi implementado, o contador nunca atingirá
	o valor desejado.*/
  if (count<COUNT_LIMIT) {
	 printf("(%ld) aguardando sinal.\n", my_id);
	  /*Note que a rotina pthread_cond_wait libera o mutex 
		enquanto espera pelo sinal.
	 Observe que a espera ocorre por meio do "count_threshold".*/
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
    printf("(%ld) sinal recebido.\n", my_id);
    count += 100;
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
  pthread_create(&threads[1], &attr, inc_count, (void *)t2);
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
