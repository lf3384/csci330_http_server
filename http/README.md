# csci330_http_server
Fully Functional HTTP Server

## PHASE 1: Socket Server Foundation

### Objective
Create the basic socket server to accept connections, read data, and respond. This forms the backbone of the entire HTTP server.

### Initial Implementation - Socket Creation

First compilation errors due to missing headers and syntax issues:
```cpp
main.cpp:9:22: error: variable has incomplete type 'struct socketaddr_in'
    9 | struct socketaddr_in address;
      |                      ^
main.cpp:11:21: error: use of undeclared identifier 'INADDR_ANY'
   11 | address.s_in_addr = INADDR_ANY;
      |                     ^
```

**Resolution**: Naming matters - corrected `socketaddr_in` to `sockaddr_in` and `s_in_*` to `sin_*`.

### Socket Establishment
Establishes a socket connection using IPv4 and TCP protocol:
```cpp
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int main () {
    int server = socket(AF_INET, SOCK_STREAM, 0);
}
```

- `AF_INET`: IPv4 protocol
- `SOCK_STREAM`: TCP protocol

### Server Address Configuration
```cpp
struct sockaddr_in address;
address.sin_family  = AF_INET;
address.sin_addr.s_addr  = INADDR_ANY;
address.sin_port = htons(8080);
```

- `AF_INET`: IPv4 server address
- `INADDR_ANY`: Accepts connections on any interface (low security, to be improved)
- `htons(8080)`: Port 8080 in network byte order

### Socket Binding and Listening
```cpp
bind(server, (struct sockaddr*)&address, sizeof(address));
listen(server, 2);

printf("Server is listening on http://localhost:8080\n");
```

Binds the socket to the IP address and puts it in listening mode, allowing 2 connections in the queue.

**Output:**
```
Server is listening on http://localhost:8080
```

### Connection Acceptance

Initial attempt encountered function signature error:
```cpp
main.cpp:22:21: error: no matching function for call to 'accept'
   22 | int client_accept = accept(server);
      |                     ^~~~~~
/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/socket.h:708:9: note: candidate function not viable: requires 3 arguments, but 1 was provided
```

**Resolution**: Used NULL for unused parameters.

### Request Buffer Implementation
```cpp
int client_accept = accept(server, NULL, NULL);

char buffer[1024];
read(client_accept, buffer, 1024);

printf("Received request:\n%s\n", buffer);
close(client_accept);
```

**Output:**
```
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

### Infinite Loop Implementation
Added while loop to continuously accept connections:
```cpp
while (true) {
    printf("Server is listening on http://localhost:8080\n");
    
    int client_accept = accept(server, NULL, NULL);
    
    char buffer[1024];
    read(client_accept, buffer, 1024);
    
    printf("Received request:\n%s\n", buffer);
    
    close(client_accept);
}
```

Successfully handles multiple consecutive requests.

---

## PHASE 2: HTTP Response Implementation

### Objective
Implement proper HTTP responses with headers and HTML content.

### HTTP Response Structure
```cpp
if (bytes_read > 0) {
    printf("Received request:\n%s\n", buffer);

    // 1. Define HTML content to send back
    const char* response_body =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>C++ HTTP Server</title></head>\n"
        "<body>\n"
        "  <h1>Hello from C++ HTTP Server!</h1>\n"
        "  <p>Your server is working correctly.</p>\n"
        "  <p>Request received at port 8080</p>\n"
        "</body>\n"
        "</html>\n";

    // 2. Calculate content length
    size_t content_length = strlen(response_body);

    // 3. Build complete HTTP response (headers + body)
    char response[8192];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"              // Status line
        "Content-Type: text/html\r\n"      // Tell browser it's HTML
        "Content-Length: %zu\r\n"          // Tell browser how long
        "Connection: close\r\n"            // Close after sending
        "\r\n"                             // Blank line separates headers from body
        "%s",                              // The actual HTML content
        content_length,
        response_body
    );

    // 4. Send the response to the browser
    write(client_accept, response, strlen(response));

    printf("Response sent successfully\n\n");
}
```

**Browser output at `http://localhost:8080`:**
```
Hello from C++ HTTP Server!

Your server is working correctly.

Request received at port 8080
```

### Error Handling Implementation
Added error checking for socket operations using `perror`:
```cpp
if (bind(server, (struct sockaddr*)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    return 1;
}

if (listen(server, 3) < 0) {
    perror("Listen failed");
    return 1;
}

if (client_accept < 0) {
    perror("Accept failed");
    continue;
}
```

### MIME Type Detection
Implemented file extension detection for proper content type headers:
```cpp
const char* get_mime_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    
    return "application/octet-stream";
}
```

### HTTP Error Response Functions

#### 404 Not Found
```cpp
void send_404(int client_socket, const char* path) {
    char body[2048];
    snprintf(body, sizeof(body),
        "<!DOCTYPE html>"
        "<html><head><title>404 Not Found</title></head>"
        "<body>"
        "<h1>404 Not Found</h1>"
        "<p>The requested path '%s' was not found on this server.</p>"
        "</body></html>",
        path
    );
    
    char response[4096];
    snprintf(response, sizeof(response),
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n%s",
        strlen(body), body
    );
    
    write(client_socket, response, strlen(response));
}
```

#### 403 Forbidden
```cpp
void send_403(int client_socket, const char* path) {
    char body[2048];
    snprintf(body, sizeof(body),
        "<!DOCTYPE html>"
        "<html><head><title>403 Forbidden</title></head>"
        "<body>"
        "<h1>403 Forbidden</h1>"
        "<p>Access to '%s' is forbidden.</p>"
        "</body></html>",
        path
    );
    
    char response[4096];
    snprintf(response, sizeof(response),
        "HTTP/1.1 403 Forbidden\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n%s",
        strlen(body), body
    );
    
    write(client_socket, response, strlen(response));
}
```

#### 405 Method Not Allowed
```cpp
void send_405(int client_socket) {
    const char* body = 
        "<!DOCTYPE html>"
        "<html><head><title>405 Method Not Allowed</title></head>"
        "<body>"
        "<h1>405 Method Not Allowed</h1>"
        "<p>This server only supports GET requests.</p>"
        "</body></html>";
    
    char response[4096];
    snprintf(response, sizeof(response),
        "HTTP/1.1 405 Method Not Allowed\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Allow: GET\r\n"
        "Connection: close\r\n\r\n%s",
        strlen(body), body
    );
    
    write(client_socket, response, strlen(response));
}
```

#### 400 Bad Request
```cpp
void send_400(int client_socket) {
    const char* body = 
        "<!DOCTYPE html>"
        "<html><head><title>400 Bad Request</title></head>"
        "<body>"
        "<h1>400 Bad Request</h1>"
        "<p>The server could not understand your request.</p>"
        "</body></html>";
    
    char response[4096];
    snprintf(response, sizeof(response),
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n%s",
        strlen(body), body
    );
    
    write(client_socket, response, strlen(response));
}
```

---

## PHASE 3: Frontend Enhancement

### Overview
Implemented Bootstrap 5 for responsive UI design across all pages. Updated `index.html` and `test_page.html` with modern styling, navigation bars, card layouts, and Bootstrap icons.

### Key Features
- Responsive navigation with collapsible menu
- Card-based layouts with shadow effects
- Bootstrap Icons integration
- Professional error pages with consistent styling
- Mobile-first responsive design
- Fixed HTML structure issues (corrected nested body tags, fixed `onlick` → `onclick` typo)

### Testing
To test the server:
```bash
g++ main.cpp -o main
./main
```

Visit:
- `http://localhost:8080` - Home page
- `http://localhost:8080/test_page.html` - Test page
- Invalid URLs - Styled 404 error page
- Non-GET requests - 405 error page
