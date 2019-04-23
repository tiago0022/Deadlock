#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "check.h"

#define NUM_THREADS 10

/* The mutex lock */
pthread_mutex_t mutex;

unsigned long long x = 0 ;

void *threadBody (void *id)
{
   long tid = (long) id ;
	int i;
	for (i=0;i<1000;i++){
      pthread_mutex_lock(&mutex);
		x++ ;
      pthread_mutex_unlock(&mutex);
	}
   pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
   pthread_t thread [NUM_THREADS] ;
   long i, status ;

   /* Create the mutex lock */
   pthread_mutex_init(&mutex, NULL);
   
   for (i=0; i<NUM_THREADS; i++) {
      status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ;
		CHECK(status==0);
   }
	for (i=0; i<NUM_THREADS; i++){
		   status = pthread_join (thread[i], NULL) ;
			CHECK(status==0);
	}
	printf ("Valor final de x: %lld\n\n", x) ;	
}
