#include <stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
int createTCPIpv4Socket() { return socket(AF_INET, SOCK_STREAM, 0); }
struct sockaddr_in* createIPv4Address(char *ip, int port) {

    struct sockaddr_in  *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if(strlen(ip) ==0)
        address->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET,ip,&address->sin_addr.s_addr);

    return address;
}
int main()
{
    int socketFD = createTCPIpv4Socket(); // SocketStream means we are passing TCP socket
    // Here 0 means we want Ip layer as the layer to be used underneath our transport layer
    // socket(domain: AF_INET, type: SOCK_STREAM,protocol:0);
    // char *ip = "127.0.0.1";
    // char *ip="142.251.167.26";
    // struct  sockaddr_in address;
    // address.sin_port = htons(80);//Big Endian presentation
    // inet_pton(AF_INET, ip, &address.sin_addr.s_addr);
    // address.sin_addr.s_addr;
    // int result=connect(socketFD,&address,sizeof address);
    struct sockaddr_in *address=createIPv4Address("142.251.167.26",80);
    int result = connect(socketFD,address, sizeof(*address));

    if(result==0)
    {
        printf("Connection Successful\n");
    }
    char *message;
    message="GET \\ HTTP/1.0\r\nHost:google.com\r\n";
    send(socketFD,message,strlen(message),0);
    char buffer[1024];
    recv(socketFD,buffer,1024,0 );
    printf("Response was %s\n",buffer);
    return 0;
}
