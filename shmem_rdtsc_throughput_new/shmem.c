#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "../hwtimer.h"
#include <pthread.h>
#define ITER 100

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

int   shared_mem_id;
int   *shared_mem_ptr;
int   shared_flag_id;
int   *shared_flag_ptr;
int   shared_buffer_id;
char  *shared_buffer_ptr;
int   shmemSize=0;
int   sharedCounter;
int*  sharedCounterPtr;

pthread_mutexattr_t mutex_shared_attr;
pthread_mutex_t	*mptr;


int main(int argc, char* argv[])
{
  uint64_t min = 0;
  uint64_t max = 0;
  pin_on_cpu (0);
  hwtimer_t rdtimer;
  initTimer(&rdtimer);

  pid_t  child_pid;
  int  status, rtn;

  /* initialize shared memory segment */
  if (argc < 2)
  {
    printf("<usage> %s <sharedLocationSize>\n",argv[0]);
    exit(1);
  }
  shmemSize = atoi(argv[1]); 
  printf("Buffer Size requested is %d\n",shmemSize);

  //Shared memory location for mutex
  if ((shared_mem_id = shmget(IPC_PRIVATE, 1*sizeof(pthread_mutex_t), 0660)) < 0)
    perror("shmget"), exit(1) ;

  if ((shared_mem_ptr = (int *)shmat(shared_mem_id, (void *)0, 0)) == NULL)
    perror("shmat"), exit(1);

  //Shared memory location for flag
  if ((shared_flag_id = shmget(IPC_PRIVATE, 1*sizeof(int), 0660)) < 0) //This IPC_PRIVATE with 0660 means that the shared variable is only accessible to child processes of the current process
    perror("shmget"), exit(1) ;

  if ((shared_flag_ptr = (int *)shmat(shared_flag_id, (void *)0, 0)) == NULL)
    perror("shmat"), exit(1);

  int bufferSize = 5120000;
  //Shared memory location for Communication buffer between the two variables
  if ((shared_buffer_id = shmget(IPC_PRIVATE, bufferSize*sizeof(char), 0660)) < 0) //This IPC_PRIVATE with 0660 means that the shared variable is only accessible to child processes of the current process
    perror("shmget"), exit(1) ;

  if ((shared_buffer_ptr = (char *)shmat(shared_buffer_id, (void *)0, 0)) == NULL)
    perror("shmat"), exit(1);

  //Shared memory location for Counter between the two variables
  if ((sharedCounter = shmget(IPC_PRIVATE, sizeof(int), 0660)) < 0) //This IPC_PRIVATE with 0660 means that the shared variable is only accessible to child processes of the current process
    perror("shmget"), exit(1) ;

  if ((sharedCounterPtr = (int *)shmat(sharedCounter, (void *)0, 0)) == NULL)
    perror("shmat"), exit(1);


  mptr = (pthread_mutex_t *)shared_mem_ptr; //setting mutex location to shared memory location

  //Initializing mutex attributes
  if (( rtn = pthread_mutexattr_init(&mutex_shared_attr)) != 0) 
    fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);

  if (( rtn = pthread_mutexattr_setpshared(&mutex_shared_attr,PTHREAD_PROCESS_SHARED)) != 0)
    fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);

  //Initializing mutex
  if (( rtn = pthread_mutex_init(mptr, &mutex_shared_attr)) != 0)
    fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);

  //Fun Begins
  

  uint64_t time_elapsed = 0;
  uint64_t time_accum = 0;

  *sharedCounterPtr = 0;
  if ((child_pid = fork()) == 0) {
    /* first child */
    pin_on_cpu (1);
    char * tempBuffer = (char *) malloc(sizeof(char)*shmemSize);
    
    for (int k = 0;k<ITER;k++)
    {
      // printf("[CHILD] k - %d\n",k);
      // fflush(stdout);
      for (int j =0;j<bufferSize/shmemSize;j++)
      {
	while  (*sharedCounterPtr == 0);
        memcpy(shared_buffer_ptr+shmemSize*j,tempBuffer,sizeof(char)*shmemSize);
	if ((rtn = pthread_mutex_lock(mptr)) != 0)
	  fprintf(stderr,"child:pthread_mutex_lock %s",strerror(rtn)),exit(1);

        // printf("[CHILD]Child has lock!\n");
        // fflush(stdout);

	(*sharedCounterPtr)--;

	// printf("[CHILD]Child will release lock!\n");
	// fflush(stdout);

	if ((rtn = pthread_mutex_unlock(mptr)) != 0)
	  fprintf(stderr,"child:pthread_unmutex_lock %s",strerror(rtn)),exit(1);
      }
      // printf ("[CHILD]Writing Shared Flag Value\n");
      // fflush(stdout);

      *shared_flag_ptr = 1;
      
      // printf ("[CHILD]Shared Counter Value %d\n",*sharedCounterPtr);
      // fflush(stdout);
    }
  } else {
    char * tempBuffer = (char *) malloc(sizeof(char)*shmemSize);
    memset(tempBuffer,'p',sizeof(char)*shmemSize);
    /* parent */ 
    for(int k =0;k<ITER;k++)
    {
      *shared_flag_ptr=0;
      //printf("[PARENT] k - %d\n",k);
      //fflush(stdout);
      resetTimer(&rdtimer);
      startTimer (&rdtimer);
      for (int j =0;j<bufferSize/shmemSize;j++)
      {
	memcpy(tempBuffer,shared_buffer_ptr+shmemSize*j,sizeof(char)*shmemSize);
	//printf("\n");
      	//fflush(stdout);
	if ((rtn = pthread_mutex_lock(mptr)) != 0)
	  fprintf(stderr,"parent:pthread_mutex_lock %d",strerror(rtn)),exit(1); //Parent will acquire the lock to the file here!
	//printf("[PARENT]Parent has lock!\n");
	//fflush(stdout);
	(*sharedCounterPtr)++;
      	// printf("[PARENT]Parent will release lock!\n");
      	// fflush(stdout);
	if ((rtn = pthread_mutex_unlock(mptr)) != 0)
	  fprintf(stderr,"child:pthread_unmutex_lock %d",strerror(rtn)),exit(1);
      }

      // printf("[PARENT]Shared Flag Value - %d\n",*shared_flag_ptr);
      // fflush(stdout);
      while  (*shared_flag_ptr%2 == 0);
      // printf("[PARENT]Shared Flag Value - %d\n",*shared_flag_ptr);
      // fflush(stdout);
      stopTimer (&rdtimer);
      time_elapsed = getTimerNs(&rdtimer);
      if (k > 0) { 
	  time_accum += time_elapsed; 
      } 
      if (k == 1) { min = time_elapsed; max = time_elapsed;}
      if (time_elapsed > max) { max = time_elapsed; }
      if (time_elapsed < min) { min = time_elapsed; }
      printf("time is %lld\n", (long long)time_elapsed);

      printf("\n");
      fflush(stdout);
    }
    printf ("==========================\n");
    printf("Throughput with avg is: %f\nThroughput with min is: %f\nThroughput with max is: %f\n", (float)(bufferSize)/(time_accum / (ITER - 1)), (float)(bufferSize)/min, (float)(bufferSize)/max);
  }


  return 0;
}
