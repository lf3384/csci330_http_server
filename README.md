# csci330_http_server
Fully Functional HTTP server

PHASE 1:
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

