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
#define ITER 100
#include "../hwtimer.h"

int main(int argc, char *argv[])
{
    uint64_t min = 0;
    uint64_t max = 0;
    pin_on_cpu (1);
    hwtimer_t rdtimer;
    initTimer(&rdtimer);
    uint64_t time_elapsed = 0;
    uint64_t time_accum = 0;
    int sockfd = 0, n = 0;
    char* sendBuff;
    char* recvBuff;
    struct sockaddr_in serv_addr; 

    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server>  <size of message>\n",argv[0]);
        return 1;
    } 
    sendBuff = (char *)malloc(sizeof(char)*atoi(argv[2]));
    recvBuff = (char *)malloc(sizeof(char)*(atoi(argv[2])));
    memset(sendBuff, '0',sizeof(char)*atoi(argv[2]));
    memset(recvBuff, '0',sizeof(char)*(atoi(argv[2])+1));
    int messageSize = atoi(argv[2]);
    int transferSize = 5120000;
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
    //Sent a Packet to determine size of communication
    int sentBytes = 0;
    sentBytes = send(sockfd,&transferSize,sizeof(int),0);
    if (sentBytes < 0 )
        printf ("error sending packet\n");
    else
        printf ("packet send done %d\n",sentBytes);
    
    for (int k=0;k<ITER;k++)
    {	
	//printf("Loop %d\n",k);
	resetTimer(&rdtimer);
	startTimer (&rdtimer);
	for (int i=0;i<(transferSize/messageSize);i++)
	{
	  sentBytes = send(sockfd,sendBuff,sizeof(char)*messageSize,0);
    	  if (sentBytes < 0 )
    	      printf ("error sending packet\n");
    	  //else
    	  //    printf ("packet send done  %d\n",sentBytes);
    	    
    	}
    	n = 4;
    	int acc=0;
    	while (n > 0)
    	{
    	  int j= read(sockfd, recvBuff, sizeof(char)*messageSize);	
    	  if (j<0)
    	  {
    	    printf ("error sending packet\n");
    	  }
    	  else
    	  {
    	    //printf("Recv %d bytes\n",j);
    	    n = n-j;
    	  }
    	  acc= acc+j;
    	}
	stopTimer (&rdtimer);
	time_elapsed = getTimerNs(&rdtimer); 
        if (k > 0) { 
	    time_accum += time_elapsed; 
	} 

        if (k == 1) { min = time_elapsed; max = time_elapsed;}
        if (time_elapsed > max) { max = time_elapsed; }
        if (time_elapsed < min) { min = time_elapsed; }
        printf("time is %lld\n", (long long)time_elapsed);

    	//printf ("Total bytes received is %d\n",acc);
    }
    printf ("==========================\n");
    printf("Throughput with avg is: %f\nThroughput with min is: %f\nThroughput with max is: %f\n", (float)(messageSize*10^9)/(time_accum / (ITER - 1)), (float)(messageSize*10^9)/min, (float)(messageSize*10^9)/max);
    close(sockfd);
    return 0;
}
