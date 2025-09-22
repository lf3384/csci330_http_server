#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int main () {

//This will establish a socket connection

int server = socket(AF_INET, SOCK_STREAM, 0);

struct sockaddr_in address;
address.sin_family  = AF_INET;
address.sin_addr.s_addr  = INADDR_ANY;
address.sin_port = htons(8080);

bind(server, (struct sockaddr*)&address, sizeof(address));
listen(server, 3);

printf("Server is listening on http://localhost:8080\n");

int client_accept = accept(server, NULL, NULL);

char buffer[1024];
read(client_accept, buffer, 1024);
        
printf("Received request:\n%s\n", buffer);

close(client_accept);
}
