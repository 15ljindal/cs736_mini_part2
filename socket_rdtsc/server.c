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
#include "../hwtimer.h"
#define ITER 100

int main(int argc, char *argv[])
{
    pin_on_cpu (0);
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
	  int n = read(connfd, Buff, sizeof(char)*1024*1024);
	
	  if (n > 0)
	  {
	    Buff[n] = 0;
	  }

	  write(connfd, Buff, sizeof(char)*n); 
	}

        close(connfd);
        sleep(1);
     }
}
