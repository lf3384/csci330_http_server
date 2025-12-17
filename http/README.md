# csci330_http_server
Fully Functional HTTP Server

## PHASE 1: Socket Server Foundation

### Objective
Create the basic socket server to accept connections, read data, and respond.

### Initial Implementation - Socket Creation

First compilation errors due to missing headers and syntax issues. Resolved by correcting `socketaddr_in` to `sockaddr_in` and `s_in_*` to `sin_*`.

### Socket Establishment
```cpp
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int main () {
    int server = socket(AF_INET, SOCK_STREAM, 0);
}
```

### Server Address Configuration
```cpp
struct sockaddr_in address;
address.sin_family  = AF_INET;
address.sin_addr.s_addr  = INADDR_ANY;
address.sin_port = htons(8080);
```

### Socket Binding and Listening
```cpp
bind(server, (struct sockaddr*)&address, sizeof(address));
listen(server, 2);

printf("Server is listening on http://localhost:8080\n");
```

### Connection Acceptance and Request Buffer
```cpp
while (true) {
    int client_accept = accept(server, NULL, NULL);

    char buffer[1024];
    read(client_accept, buffer, 1024);

    printf("Received request:\n%s\n", buffer);
    close(client_accept);
}
```

---

## PHASE 2: HTTP Response Implementation

### HTTP Response Structure
```cpp
if (bytes_read > 0) {
    const char* response_body = "<!DOCTYPE html>...";
    size_t content_length = strlen(response_body);

    char response[8192];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n%s",
        content_length, response_body
    );

    write(client_accept, response, strlen(response));
}
```

### Error Handling Implementation
Added error checking for socket operations using `perror`.

### MIME Type Detection
```cpp
const char* get_mime_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    // ... more types
}
```

### HTTP Error Response Functions
Implemented functions for 404, 403, 405, and 400 error responses with Bootstrap-styled HTML pages.

---

## PHASE 3: Frontend Enhancement

Implemented Bootstrap 5 for responsive UI design. Updated `index.html` and `test_page.html` with modern styling, navigation bars, card layouts, and Bootstrap icons. Fixed HTML structure issues and JavaScript errors.

---

## PHASE 4: Object-Oriented Refactor

Complete transformation from procedural C to object-oriented C++.

### Architecture

**5 Core Classes:**

1. **ConfigManager** (Singleton)
   - Reads `config.ini` file
   - Provides type-safe getters
   - Default configuration values

2. **HTTPRequest**
   - Parses HTTP request line, headers, and body
   - Query string parsing (`?name=value&page=2`)
   - Path security validation (prevents `..` traversal)
   - Case-insensitive header lookup

3. **HTTPResponse**
   - Builder pattern for responses
   - Automatic status text lookup
   - Factory methods: `createError()`, `createHTML()`, `createJSON()`, `createRedirect()`

4. **FileHandler**
   - MIME type detection for 13+ file types
   - File size limits
   - Automatic `index.html` serving for directories
   - Binary file support

5. **HTTPServer**
   - Socket lifecycle management
   - Request/response orchestration
   - Graceful shutdown (Ctrl+C handler)

### C++ Features Demonstrated

**OOP:**
- Encapsulation, abstraction, single responsibility
- Design patterns: Singleton (ConfigManager), Factory (HTTPResponse)

**STL:**
- `std::string` for all string operations
- `std::map` for headers, config, query params
- `std::ifstream` for file I/O
- `std::ostringstream` for string building

**Memory Management:**
- RAII throughout
- Automatic cleanup with destructors
- No manual malloc/free

**Modern C++:**
- Range-based for loops
- Const correctness
- Initializer lists
- Auto type deduction

### File Structure
```
http/
├── ConfigManager.h/.cpp
├── HTTPRequest.h/.cpp
├── HTTPResponse.h/.cpp
├── FileHandler.h/.cpp
├── HTTPServer.h/.cpp
├── main.cpp
├── config.ini
├── compile.sh / Makefile
└── index.html
```

### Compilation

```bash
chmod +x compile.sh
./compile.sh
./server
```

Or:
```bash
make
./server
```

Visit: `http://localhost:8080`

### Configuration (config.ini)

```ini
port = 8080
server_name = C++ HTTP Server
document_root = .
max_file_size = 10485760
max_connections = 10
```

### Request Flow

```
Client → HTTPServer::handleClient()
      → HTTPRequest::parse()
      → HTTPServer::processRequest()
      → FileHandler::serveFile()
      → HTTPResponse::send()
      → Client
```

---

## Lines of Code

| Component      | Lines |
|----------------|-------|
| ConfigManager  | ~150  |
| HTTPRequest    | ~250  |
| HTTPResponse   | ~200  |
| FileHandler    | ~200  |
| HTTPServer     | ~220  |
| Main           | ~40   |
| **Total**      | **~1060** |
