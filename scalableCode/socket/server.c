#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#define MAX_TRANSFER_SIZE 4097

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    char Buff[1024*1024];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(Buff, '0', sizeof(Buff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
	for (int k=0;k<100;k++)
	{
	  printf("-----------------\n");
	  printf("Loop Count %d\n",k);
	  int n = 0;
	  int j = MAX_TRANSFER_SIZE;
	  printf("Start receiving\n");
	  while (j == MAX_TRANSFER_SIZE)
	  {
	    j = recv(connfd, Buff+n, sizeof(char)*MAX_TRANSFER_SIZE,0);
	    n=n+j;  
	    printf("Recv %d bytes\n",j);
	  }
	
	  printf("Total Received bytes equal to %d\n",n);
	  if (n > 0)
	  {
	    Buff[n] = 0;
	  }
	  int l = MAX_TRANSFER_SIZE;
	  int acc = 0;
	  printf("Start sending\n");
	  while (n>0)
	  {
	    if (n < l)
	    {
	      l = n;
	    }
	    if(send(connfd, Buff+acc, sizeof(char)*l,0) <0) 
		printf ("error sending packet\n");
	    printf("Sent %d bytes\n",l);
	    n = n-l; 
	    acc = acc+l;
	  }
	  printf("Total Sent bytes equal to %d\n",acc);
	  printf("-----------------\n");
	}
	printf("Move to Next Connection\n");

        close(connfd);
     }
}
