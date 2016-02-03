#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <string.h>

#include <stdio.h>

// for setting cpu affinity
#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE
#include<sched.h>

typedef uint64_t hrtime_t;

__inline__ hrtime_t _rdtsc() {
    unsigned long int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi));
    return (hrtime_t)hi << 32 | lo;
}

typedef struct {
	hrtime_t start;
	hrtime_t end;
	double cpuMHz;
} hwtimer_t;

inline void resetTimer(hwtimer_t* timer)
{
	timer->start = 0;
	timer->end = 0;
}

inline void initTimer(hwtimer_t* timer) 
{
#if defined(__linux) || defined(__linux__) || defined(linux)
    FILE* cpuinfo;
    char str[100];
    cpuinfo = fopen("/proc/cpuinfo","r");
    while(fgets(str,100,cpuinfo) != NULL){
        char cmp_str[8];
        strncpy(cmp_str, str, 7);
        cmp_str[7] = '\0';
        if (strcmp(cmp_str, "cpu MHz") == 0) {
			double cpu_mhz;
			sscanf(str, "cpu MHz : %lf", &cpu_mhz);
			timer->cpuMHz = cpu_mhz;
			break;
        }
    }
    fclose( cpuinfo );
#else
    timer->cpuMHz = 0;
#endif

	resetTimer(timer);
}

inline void startTimer(hwtimer_t* timer)
{
	timer->start = _rdtsc();
}

inline void stopTimer(hwtimer_t* timer)
{
	timer->end = _rdtsc();
}

inline uint64_t getTimerTicks(hwtimer_t* timer)
{
	return timer->end - timer->start;
}

inline uint64_t getTimerNs(hwtimer_t* timer)
{
	if (timer->cpuMHz == 0) {
		/* Cannot use a timer without first initializing it 
		   or if not on linux 
		*/
		return 0;
	}
	return (uint64_t)(((double)getTimerTicks(timer))/timer->cpuMHz*1000);
}

void pin_on_cpu (int cpu)
{
  // unsigned long cpuMask;
  // cpuMask = 3; // bind to cpu 1
  // sched_setaffinity(0, sizeof(cpuMask), &cpuMask);
  
  cpu_set_t  mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  sched_setaffinity(0, sizeof(mask), &mask);
}

#endif
