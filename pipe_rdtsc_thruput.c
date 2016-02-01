#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<string.h>
#include <sys/types.h>

#include "hwtimer.h"

#define ITER 50
 
int main(int argc, char**argv)
{
        int     fd1[2], fd2[2], nbytes;
        pid_t   childpid;
        // char    string1[] = "Sava\n";
        // char    string2[] = "YoYo\n";
        // char    readbuffer[80];
        uint64_t time_elapsed = 0; 
        uint64_t time_accum = 0;
        double av_time = 0;
        int mindex = -1;
        int i = 0;

        char dict[] = "abcdefghijklmnopqrstuvwxyz";
        long int message_sizes[] = {4, 16, 64, 256, 1000, 4000, 16000, 64000, 256000, 512000, 1024000};

        if (argc != 2)
        { printf ("ERROR! Expect the message size index\n"); exit(1);}
        else
        { mindex = atoi(argv[1]); printf ("mindex is %d and message size is %ld\n", mindex, message_sizes[mindex]);}


        char * string1 = malloc(message_sizes[mindex] * sizeof(char));
        char * string2 = malloc(message_sizes[mindex] * sizeof(char));
        char * readbuffer = malloc(message_sizes[mindex] * sizeof(char));

        // initialize the two strings to be passed
        for (i = 0; i < message_sizes[mindex] - 1; i++)
        {
          string1[i] = dict[i%26];
          string2[i] = dict[i%26];
        }
          string1[i] = '\0';
          string2[i] = '\0';
  
        printf ("Done init\n");

        hwtimer_t rdtimer;
        initTimer(&rdtimer);
        
        pipe(fd1);
        pipe(fd2);
       
        // put parent on core 0
        pin_on_cpu (0);
 
        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if(childpid == 0)
        {
                pin_on_cpu(1);
                initTimer(&rdtimer);
                /* Child process closes up input side of pipe */
                close(fd1[0]);
                close(fd2[1]);
                for (i = 0; i < ITER ; i++)
                {
                  resetTimer(&rdtimer);
                  startTimer (&rdtimer);
                  /* Send "string" through the output side of pipe */
                  write(fd1[1], string1, message_sizes[mindex]);
                  
                  nbytes = read(fd2[0], readbuffer, message_sizes[mindex]);
                  
                  stopTimer (&rdtimer);
                  time_elapsed = getTimerNs(&rdtimer);
                  if (i > 0) { time_accum += time_elapsed; }
 
                  printf("time is %lld\n", (long long)time_elapsed);
                  // printf("Received string: %s", readbuffer);
                }
                av_time = time_accum / ((double)(ITER - 1));
                printf("accum_time is %lld and average time is %f and thruput is %f\n", time_accum, av_time, message_sizes[mindex]/av_time);
                exit(0);
        }
        else
        {
                /* Parent process closes up output side of pipe */
                close(fd1[1]);
                close(fd2[0]);

                
                for (i = 0; i < ITER ; i++)
                {
                  /* Read in a string from the pipe */
                  nbytes = read(fd1[0], readbuffer, message_sizes[mindex]);
                  
                  write(fd2[1], "DONE", 4);
                
                // printf("Received string: %s", readbuffer);
                }
        }
        
        return(0);
}

