#include<stdio.h>
#include <stdint.h> /* for uint64 definition */
#include <stdlib.h> /* for exit() definition */
#include <time.h> /* for clock_gettime */
#include <sys/time.h> // for gettimeofday
#include "hwtimer.h" // for RDTSC

#define BILLION 1000000000L
#define MILLION 1000000L
#define ITER 10
int main (void)
{
  uint64_t diff=0, base=0, base_accum = 0, dummy_a = 0, dummy_b = 0;

#ifdef CLOCK_GET_TIME
 struct timespec start, end;
#endif

#ifdef GET_TIME_OF_DAY
 struct timeval start, end;
#endif
#ifdef RDTSC
  hwtimer_t rdtimer;
  initTimer(&rdtimer);
#endif
  int i = 0;
  
#if(1)
  for (i =0; i < ITER; i++)
  {
  #ifdef CLOCK_GET_TIME
    clock_gettime(CLOCK_MONOTONIC, &start); /* mark start time */
    ;
    clock_gettime(CLOCK_MONOTONIC, &end); /* mark the end time */
  #endif

  #ifdef GET_TIME_OF_DAY
    gettimeofday(&start, NULL); /* mark start time */
    gettimeofday(&end, NULL); /* mark start time */
  #endif
  #ifdef RDTSC
    startTimer(&rdtimer);
    stopTimer(&rdtimer);
  #endif

  #ifdef CLOCK_GET_TIME
    if ( i > 0 ) {  base = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec; }// this is the overhead of calling clock_gettime
    printf("base is = %llu nanoseconds\n", (long long unsigned int) base);
  #endif
  #ifdef GET_TIME_OF_DAY
    if ( i > 0 ) {  base = MILLION * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec; }// this is the overhead of calling clock_gettime
    printf("base is = %llu microseconds\n", (long long unsigned int) base);
  #endif
  #ifdef RDTSC
    if ( i > 0 ) {base = getTimerNs(&rdtimer);   }// this is the overhead of calling clock_gettime
    printf("base is = %llu nanoseconds\n", (long long unsigned int) base);
  #endif
    base_accum += base;
    
  }
  base = base_accum / (ITER - 1);
  
  #ifdef CLOCK_GET_TIME
    printf("CLOCK_GET_TIME Final base is = %llu nanoseconds\n", (long long unsigned int) base);
  #endif
  #ifdef GET_TIME_OF_DAY
    printf("GET_TIME_OF_DAY Final base is = %llu microseconds\n", (long long unsigned int) base);
  #endif
  #ifdef RDTSC
    printf("RDTSC Final base is = %llu nanoseconds\n", (long long unsigned int) base);
  #endif
  
#endif


#if(1)
 for (i =0; i < 10; i++)
 {
   /* measure monotonic time */

  #ifdef CLOCK_GET_TIME
   clock_gettime(CLOCK_MONOTONIC, &start); /* mark start time */
  #endif
  #ifdef GET_TIME_OF_DAY
    gettimeofday(&start, NULL); /* mark start time */
  #endif
  #ifdef RDTSC
    resetTimer(&rdtimer);
    startTimer(&rdtimer);
  #endif
   // sleep(1); /* do stuff */
   //; /* do stuff */  


  #ifdef CLOCK_GET_TIME
 asm(
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
   );
  #endif

  #ifdef GET_TIME_OF_DAY
   for (dummy_a = 0; dummy_a < 1000; dummy_a++); 
  #endif
  
  #ifdef RDTSC
  asm(
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
     "movl $56, %eax\n"
   ); 
  #endif



  #ifdef CLOCK_GET_TIME
  clock_gettime(CLOCK_MONOTONIC, &end); /* mark the end time */
   diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec ;
  #endif
  #ifdef GET_TIME_OF_DAY
    gettimeofday(&end, NULL); /* mark start time */
   diff = MILLION * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec ;
  #endif
  #ifdef RDTSC
    stopTimer(&rdtimer);
    diff = getTimerNs(&rdtimer);
  #endif

   diff = diff- base; // removing the overhead
  #ifdef CLOCK_GET_TIME
    printf("clock_gettime elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
  #endif
  #ifdef GET_TIME_OF_DAY
    printf("gettime_ofday elapsed time = %llu microseconds\n", (long long unsigned int) diff);
  #endif
  #ifdef RDTSC
    printf("RDTSC elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
  #endif
 }
#endif
  return 0;
}
