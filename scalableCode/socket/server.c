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
#define ITER 100

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    char Buff[1024*1024];
    time_t ticks; 
    int size_of_packet;

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
        int n = read(connfd, &size_of_packet, sizeof(int));
	if (n > 0)
	{
	  printf("Size of message is %d\n",size_of_packet);
	}
	int writeSize = size_of_packet;

	for (int k=0;k<ITER;k++)
	{
	    printf("Loop %d\n",k);
	    n = writeSize;
	    int acc=0;
	    while (n > 0)
	    {
	      int j = read(connfd, Buff, sizeof(char)*1024*1024);
	      if (j<0)
	      {
	        printf ("error sending packet\n");
	      }
	      else
	      {
	        printf("Recv %d bytes\n",j);
	        n = n-j;
	      }
	      acc= acc+j;
	    }
	    printf("Total bytes Recv %d\n",acc);
            write(connfd, Buff, sizeof(char)*writeSize); 
	}
        close(connfd);
     }
}
