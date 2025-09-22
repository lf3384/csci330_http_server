# csci330_http_server
Fully Functional HTTP server

PHASE 1:

Part 1.
I have decided to first code the socket server which will accept connections, read data and respond. Not much structure to it, it is just going to make a connection to a host, read any data that comes in and send back a response.

It might not be the most technically difficult part of the whole program, but it will be a great backbone for the entire project.

```cpp
main.cpp:9:22: error: variable has incomplete type 'struct socketaddr_in'
    9 | struct socketaddr_in address;
      |                      ^
main.cpp:9:8: note: forward declaration of 'socketaddr_in'
    9 | struct socketaddr_in address;
      |        ^
main.cpp:10:22: error: expected unqualified-id
   10 | address.s_in_family. = AF_INET;
      |                      ^
main.cpp:11:21: error: use of undeclared identifier 'INADDR_ANY'
   11 | address.s_in_addr = INADDR_ANY;
      |                     ^
main.cpp:14:14: error: expected expression
   14 | bind(server (struct sockaddr*)&address, sizeof(address));
      |              ^
main.cpp:15:8: error: use of undeclared identifier 'server_fd'; did you mean 'server'?
   15 | listen(server 3);
      |        ^~~~~~~~~
      |        server
main.cpp:7:5: note: 'server' declared here
    7 | int server = socket(AF_INET, SOCK_STREAM, 0);
      |     ^
```
Some header files missing, syntax errors on my part, down to these errors.
```cpp
main.cpp:11:22: error: variable has incomplete type 'struct socketaddr_in'
   11 | struct socketaddr_in address;
      |                      ^
main.cpp:11:8: note: forward declaration of 'socketaddr_in'
   11 | struct socketaddr_in address;
      |        ^
main.cpp:12:22: error: expected unqualified-id
   12 | address.s_in_family. = AF_INET;
      |                      ^
main.cpp:16:14: error: expected expression
   16 | bind(server (struct sockaddr*)&address, sizeof(address));
      |
```
1st successful compilation.
Note: Naming does matter, half the issues was this block:
```cpp
struct socketaddr_in address;
address.s_in_family  = AF_INET;
address.s_in_addr.s_addr  = INADDR_ANY;
address.s_in_port = htons(8080);
```
Not supposed to s_in, supposed to be sin, and 'socketaddr' had to be 'sockaddr'

Current working code:
This block will establish a socket connection
AF_INET states IPv4, and SOCK_STREAM states TCP protocol
```cpp
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int main () {


int server = socket(AF_INET, SOCK_STREAM, 0);
```

This block configures address of server.
AF_INET again states IPv4 server address.
INADDR_ANY is just stating IN ADDRESSES? ANY. Accepts connections on any interface, which is low security, will be fixed.
```cpp
struct sockaddr_in address;
address.sin_family  = AF_INET;
address.sin_addr.s_addr  = INADDR_ANY;
address.sin_port = htons(8080);
```

This block binds the socket to the IP that is accepted. It also puts the socket in listening mode, to listen for connections, allowing only 2 in the queue at a time.
```cpp

bind(server, (struct sockaddr*)&address, sizeof(address));
listen(server, 2);

printf("Server is listening on http://localhost:8080\n");
}
```
Output:
```output
Server is listening on http://localhost:8080
```

Part 2.
Need socket to accept thesse connections. Seems to be compiling and running, but would like to test to make sure it is accepting rather than just listening.

Creating a way to accept:
```cpp
main.cpp:22:21: error: no matching function for call to 'accept'
   22 | int client_accept = accept(server);
      |                     ^~~~~~
/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/socket.h:708:9: note: candidate function not viable: requires 3 arguments, but 1 was provided
  708 | int     accept(int, struct sockaddr * __restrict, socklen_t * __restrict)
      |         ^      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1 error generated.
```
Going to try to use NULL or NaN, not sure how else to pass those other two arguments.
Null did compile.

Huge change in output.
Update code:
```cpp
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

}
```
Added in a buffer to read the request.
```cpp
char buffer[1024];
read(client_accept, buffer, 1024);
```
Then we print the buffer to screen and close connection.
```cpp
printf("Received request:\n%s\n", buffer);
close(client_accept);
```



Updated output:
```output
lucasfield@Lucass-MacBook-Air-3 csci330_http_server % ./main
Server is listening on http://localhost:8080

Received request:
GET / HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:143.0) Gecko/20100101 Firefox/143.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br, zstd
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Priority: u=0, i
```
This is great. As you can see in the output, we are getting tons of information. Will have to close connection after it accepts, while in the testing stages at least, until we have parameters that decide when the connection closes.

I think I should integrate a while loop, that will run infinitely, that way the socket keeps accepting connections
Updated code with while loop:
```cpp
  1 #include <cstdio>
  2 #include <sys/socket.h>
  3 #include <unistd.h>
  4 #include <netinet/in.h>
  5
  6 int main () {
  7
  8 //This will establish a socket connection
  9
 10 int server = socket(AF_INET, SOCK_STREAM, 0);
 11
 12 struct sockaddr_in address;
 13 address.sin_family  = AF_INET;
 14 address.sin_addr.s_addr  = INADDR_ANY;
 15 address.sin_port = htons(8080);
 16
 17 bind(server, (struct sockaddr*)&address, sizeof(address));
 18 listen(server, 3);
 19
 20 while (true) {
 21 printf("Server is listening on http://localhost:8080\n");
 22
 23 int client_accept = accept(server, NULL, NULL);
 24
 25 char buffer[1024];
 26 read(client_accept, buffer, 1024);
 27
 28 printf("Received request:\n%s\n", buffer);
 29
 30 close(client_accept);
 31 }
 32
 33 }
 ```
 Updated output.
 Tried multiple refreshes on web browser to see if while loop keeps request going.
 ```output
 lucasfield@Lucass-MacBook-Air-3 csci330_http_server % ./main
Server is listening on http://localhost:8080
Received request:
GET / HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:143.0) Gecko/20100101 Firefox/143.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br, zstd
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Priority: u=0, i


Server is listening on http://localhost:8080
Received request:
GET / HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:143.0) Gecko/20100101 Firefox/143.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br, zstd
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Priority: u=0, i
```
Several more requests, which is good that means it is working as it is supposed to.