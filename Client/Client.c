// #include <stdio.h>
// #include<string.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h> 
// /*
//     struct sockaddr_in
//   {
//     __SOCKADDR_COMMON (sin_);
//     in_port_t sin_port;			
//     struct in_addr sin_addr;
//     unsigned char sin_zero[sizeof (struct sockaddr)
// 			   - __SOCKADDR_COMMON_SIZE
// 			   - sizeof (in_port_t)
// 			   - sizeof (struct in_addr)];
//   };

// */
// int main()
// {

//    int socketFD=socket(AF_INET,SOCK_STREAM,0);
//    char *ip="142.250.188.46";
//    struct sockaddr_in address;
//    address.sin_family=AF_INET;
//    address.sin_port=htons(80);//port 1-1000 is mostly reserved
//    inet_pton(AF_INET,ip,&address.sin_addr.s_addr);
//    //address.sin_addr.s_addr;
//    int result=connect(socketFD,&address,sizeof(address));
//    if(result==0)
//    {
//     printf("Connection Successful..");
//     printf("\n");
//    }
//    char *message="GET\\HTTP/1.1\r\nHost:google.com\r\n\r\n";
//    send(socketFD,message,strlen(message),0);
//    char buffer[1024];
//    //recv()
//    return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFD == -1) {
        perror("socket");
        exit(1);
    }

    char *ip = "142.251.167.26";
    int port = 80;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
        perror("inet_pton");
        exit(1);
    }

    if (connect(socketFD, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connection Successful\n");

    close(socketFD); // Close the socket when done

    return 0;
}
