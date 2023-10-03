#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
/*
    struct sockaddr_in
  {
    __SOCKADDR_COMMON (sin_);
    in_port_t sin_port;			
    struct in_addr sin_addr;
    unsigned char sin_zero[sizeof (struct sockaddr)
			   - __SOCKADDR_COMMON_SIZE
			   - sizeof (in_port_t)
			   - sizeof (struct in_addr)];
  };

*/
int main()
{

   int socketFD=socket(AF_INET,SOCK_STREAM,0);
   char *ip="142.250.188.46";
   struct sockaddr_in address;
   address.sin_family=AF_INET;
   address.sin_port=htons(80);//port 1-1000 is mostly reserved
   inet_pton(AF_INET,ip,&address.sin_addr.s_addr);
   //address.sin_addr.s_addr;
   int result=connect(socketFD,&address,sizeof(address));
   if(result==0)
   {
    printf("Connection Successful..");
    printf("\n");
   }
    return 0;
}
