#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#define MAX_TRANSFER_SIZE 4097
#include "../hwtimer.h"
#define ITER 100

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char* sendBuff;
    char recvBuff[1024*1024];
    struct sockaddr_in serv_addr; 
    uint64_t time_elapsed = 0; 
    uint64_t time_accum = 0;
    hwtimer_t rdtimer;
    initTimer(&rdtimer);
    pin_on_cpu (1);
    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server>  <size of message>\n",argv[0]);
        return 1;
    } 
    int sendBytes =  atoi(argv[2]);
    sendBuff = (char *)malloc(sizeof(char)*1024*1024);
    memset(sendBuff, '1',sizeof(char)*1024*1024);
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
    for (int k=0;k<ITER;k++)
    {
      resetTimer(&rdtimer);
      startTimer (&rdtimer);
      //printf("-----------------\n");
      //printf("Loop Count %d\n",k);
      int l = MAX_TRANSFER_SIZE;
      int acc = 0;
      n = sendBytes;
      //printf("Start sending\n");
      while (n>0)
      {
        if (n < l)
        {
          l = n;
        }
	if (send(sockfd,sendBuff,sizeof(char)*l,0) <0)
	    printf ("error sending packet\n");
        n = n-l; 
        acc = acc+l;
	//printf("Sent %d bytes\n",l);
      }
      //printf("Total Sent %d bytes\n",acc);

      n=0;
      int j = MAX_TRANSFER_SIZE;
      //printf("Start receiving\n");
      while (j == MAX_TRANSFER_SIZE)
      {
	j = read(sockfd, recvBuff, sizeof(char)*MAX_TRANSFER_SIZE);
	n=n+j;  
	//printf("Recv %d bytes\n",j);
      }
      //printf("Total Received bytes equal to %d\n",n);
      stopTimer (&rdtimer);
      time_elapsed = getTimerNs(&rdtimer);
      if (k > 0) { 
	  time_accum += time_elapsed; 
	  printf("Accumulated Time is - %d\n",time_accum);
      }
      //printf("-----------------\n");
    }
    close(sockfd);
    printf("accum_time is %lld and average time is %lld\n", time_accum, (float)time_accum / (float)(ITER - 1));
    return 0;
}
