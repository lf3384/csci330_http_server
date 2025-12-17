# PHASE 4 - Object-Oriented HTTP Server

## Quick Start

### Compile and Run
```bash
chmod +x compile.sh
./compile.sh
./server
```

Or using Make:
```bash
make
./server
```

Then visit: `http://localhost:8080`

## What's New in Phase 4

Complete refactor from procedural C to object-oriented C++:

### 5 Core Classes (~1060 lines)
1. **ConfigManager** - Singleton for config.ini management
2. **HTTPRequest** - Parses requests, validates paths, extracts headers & query params
3. **HTTPResponse** - Builds responses with factory methods
4. **FileHandler** - File operations, MIME types, directory serving
5. **HTTPServer** - Main server orchestration

### Key Features
✅ Configuration file support (config.ini)
✅ Query string parsing (`?name=value&page=2`)
✅ Header parsing (case-insensitive)
✅ Path traversal protection
✅ MIME type detection (13+ types)
✅ Error pages (400, 403, 404, 405)
✅ Graceful shutdown (Ctrl+C)

### C++ Concepts Demonstrated
- Object-Oriented Design (encapsulation, abstraction, SRP)
- Design Patterns (Singleton, Factory)
- STL (std::string, std::map, std::ifstream, std::ostringstream)
- RAII & Memory Management
- Modern C++ (range-based loops, const correctness, auto)

## File Structure
```
http/
├── ConfigManager.h/.cpp    - Configuration management
├── HTTPRequest.h/.cpp       - Request parsing
├── HTTPResponse.h/.cpp      - Response building
├── FileHandler.h/.cpp       - File operations
├── HTTPServer.h/.cpp        - Main server
├── main.cpp                 - Entry point
├── config.ini               - Configuration file
├── compile.sh               - Build script
├── Makefile                 - Alternative build system
└── index.html               - Test page
```

## Configuration (config.ini)

```ini
port = 8080                # Server port
server_name = My Server    # Server name in responses
document_root = .          # Root directory for files
max_file_size = 10485760   # Max file size (10MB)
max_connections = 10       # Max queued connections
```

## Testing

1. **Basic Test:**
   - Visit `http://localhost:8080`
   - Should see Phase 4 landing page

2. **Error Tests:**
   - `http://localhost:8080/fake.html` → 404
   - `http://localhost:8080/../etc/passwd` → 403

3. **Query String Test:**
   - `http://localhost:8080/?name=test&page=1`
   - Check terminal for parsed parameters

## Architecture Highlights

### Request Flow
```
Client → HTTPServer::handleClient()
      → HTTPRequest::parse()
      → HTTPServer::processRequest()
      → FileHandler::serveFile()
      → HTTPResponse::send()
      → Client
```

### Memory Management
- RAII throughout (no manual malloc/free)
- Automatic cleanup with destructors
- STL containers handle memory

### Error Handling
- Structured error responses
- HTTP-compliant status codes
- Detailed logging

## Advantages Over Phase 3

1. **Maintainability** - Clear class responsibilities
2. **Extensibility** - Easy to add features
3. **Testability** - Each class can be tested independently
4. **Type Safety** - C++ types vs C primitives
5. **Configurability** - External config file
6. **Readability** - Clean separation of concerns

## Next Steps (Phase 5 Preview)

This architecture enables:
- Multithreading (std::thread)
- POST request handling
- Logging system
- Request/response caching
- Session management
- Rate limiting

## Compilation Options

**Using compile.sh:**
```bash
./compile.sh
```

**Using Makefile:**
```bash
make          # Build
make clean    # Remove artifacts
make rebuild  # Clean + build
make run      # Build + run
```

**Manual:**
```bash
g++ -std=c++11 -Wall -Wextra -o server \
    main.cpp ConfigManager.cpp HTTPRequest.cpp \
    HTTPResponse.cpp FileHandler.cpp HTTPServer.cpp
```

---

**Total Lines:** ~1060 lines of clean, professional C++
**Classes:** 5 interconnected classes with clear responsibilities
**Patterns:** Singleton, Factory, RAII
