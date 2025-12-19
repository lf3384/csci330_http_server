# C++ HTTP Server - Complete Implementation

A fully functional HTTP server built from scratch in C++, demonstrating object-oriented design principles, modern C++ features, and professional software engineering practices.

## 🚀 Quick Start

### Build and Run
```bash
cd http/
./compile.sh
./server
```

Then visit: **http://localhost:8080**

### Alternative Build (Makefile)
```bash
make
./server
```

---

## 📸 Features Demo

**Successfully serves static files:**
- HTML pages with Bootstrap styling
- CSS, JavaScript, images
- Automatic MIME type detection
- Directory index serving (`index.html`)

**Error handling:**
- 400 Bad Request
- 403 Forbidden (path traversal protection)
- 404 Not Found
- 405 Method Not Allowed

**Request logging:**
- All requests logged with timestamps
- Dual output: console + `server.log` file
- Client IP tracking
- HTTP status codes

**Example log output:**
```
[2024-12-18 23:45:12] ========================================
[2024-12-18 23:45:12] Server started
[2024-12-18 23:45:12] ========================================
[2024-12-18 23:45:15] 127.0.0.1 GET /index.html 200 OK
[2024-12-18 23:45:17] 127.0.0.1 GET /test.html 200 OK
[2024-12-18 23:45:20] 127.0.0.1 GET /fake.html 404 Not Found
```

---

## 🏗️ Architecture

### Core Classes (6 Total, ~1200 lines)

#### 1. **ConfigManager** (Singleton)
- Reads and parses `config.ini`
- Type-safe configuration getters
- Default values for missing config

#### 2. **HTTPRequest**
- Parses HTTP request line, headers, body
- Query string parsing (`?name=value&page=2`)
- Path security validation (prevents `../` attacks)
- Case-insensitive header lookup

#### 3. **HTTPResponse**
- Builder pattern for response construction
- Factory methods: `createError()`, `createHTML()`, `createJSON()`, `createRedirect()`
- Automatic status text lookup
- Header management

#### 4. **FileHandler**
- MIME type detection (13+ file types)
- File size limits (configurable)
- Automatic `index.html` serving for directories
- Binary file support

#### 5. **HTTPServer**
- Socket lifecycle management (create, bind, listen, accept)
- Request/response orchestration
- Graceful shutdown (Ctrl+C handler)
- Client connection handling

#### 6. **Logger** (Singleton) ⭐ *New in Phase 5*
- Thread-safe logging with `std::mutex`
- Timestamp formatting with `std::chrono`
- Dual output: console and file
- Structured log format
- Request tracking with IP, method, path, status

---

## 📁 Project Structure

```
csci330_http_server/
├── http/
│   ├── ConfigManager.h/.cpp      # Configuration management
│   ├── HTTPRequest.h/.cpp         # Request parsing
│   ├── HTTPResponse.h/.cpp        # Response building
│   ├── FileHandler.h/.cpp         # File operations & MIME types
│   ├── HTTPServer.h/.cpp          # Main server logic
│   ├── Logger.h/.cpp              # Logging system ⭐ NEW
│   ├── main.cpp                   # Entry point
│   ├── config.ini                 # Server configuration
│   ├── compile.sh                 # Build script
│   ├── cleanup.sh                 # Directory cleanup script
│   ├── Makefile                   # Alternative build system
│   ├── .gitignore                 # Git ignore rules
│   ├── index.html                 # Default landing page
│   ├── test_page.html             # Additional test page
│   └── README.md                  # This file
└── .gitignore                     # Root git ignore
```

---

## ⚙️ Configuration

Edit `config.ini` to customize server behavior:

```ini
port = 8080                # Server port (default: 8080)
server_name = My Server    # Server name in responses
document_root = .          # Root directory for serving files
max_file_size = 10485760   # Max file size in bytes (10MB)
max_connections = 10       # Max queued connections
```

---

## 🔧 C++ Features Demonstrated

### Object-Oriented Design
- **Encapsulation**: Private members, public interfaces
- **Abstraction**: Clear class responsibilities
- **Single Responsibility Principle**: Each class has one job
- **Design Patterns**:
  - Singleton (ConfigManager, Logger)
  - Factory (HTTPResponse)
  - RAII (Resource Acquisition Is Initialization)

### Modern C++ (C++11)
- **STL Containers**:
  - `std::string` for all string operations
  - `std::map` for headers, config, query params
  - `std::vector` for collections
- **File I/O**:
  - `std::ifstream` for reading files
  - `std::ofstream` for writing logs
  - `std::ostringstream` for string building
- **Time Handling**:
  - `std::chrono` for timestamps
- **Thread Safety**:
  - `std::mutex` for concurrent access protection
  - `std::lock_guard` for automatic lock management
- **Modern Features**:
  - Range-based for loops
  - Auto type deduction
  - Initializer lists
  - Const correctness
  - Default/delete for constructors

### Memory Management
- **RAII**: Automatic resource cleanup via destructors
- **No manual memory management**: STL containers handle allocations
- **Smart pointers ready**: Architecture supports `std::unique_ptr`/`std::shared_ptr`

---

## 🧪 Testing

### Basic Functionality
```bash
# Start server
./server

# In browser, test:
http://localhost:8080              # Should show index.html
http://localhost:8080/test_page.html  # Should show test page
```

### Error Testing
```bash
http://localhost:8080/fake.html    # Should return 404
http://localhost:8080/../etc/passwd # Should return 403 (path traversal blocked)
```

### Query String Testing
```bash
http://localhost:8080/?name=Lucas&page=1
# Check console/log for parsed parameters
```

### Log File Verification
```bash
# After making requests, check:
cat server.log
```

---

## 📊 Request Flow Diagram

```
Client Browser
     ↓
HTTPServer::handleClient()
     ↓
HTTPRequest::parse()
     ↓ (validation)
HTTPServer::processRequest()
     ↓
FileHandler::serveFile()
     ↓
HTTPResponse::send()
     ↓
Logger::logRequest() ⭐
     ↓
Client Browser
```

---

## 📈 Project Evolution

### Phase 1: Socket Foundation
- Basic TCP socket creation
- Accept connections and read data
- Raw C implementation

### Phase 2: HTTP Protocol
- HTTP response formatting
- MIME type detection
- Error responses (404, 403, etc.)
- Static file serving

### Phase 3: Frontend Polish
- Bootstrap 5 integration
- Responsive HTML pages
- Modern UI design

### Phase 4: Object-Oriented Refactor
- Complete C++ transformation
- 5 core classes with clear responsibilities
- Design patterns implementation
- STL integration
- Configuration file support

### Phase 5: Professional Polish ⭐
- **Logging system** with timestamps and file output
- Thread-safe implementation
- Code cleanup and organization
- Professional documentation
- `.gitignore` for clean repository
- Build automation improvements


## 🛠️ Compilation Details

### Using compile.sh (Recommended)
```bash
chmod +x compile.sh
./compile.sh
```

### Using Makefile
```bash
make          # Build
make clean    # Remove artifacts
make rebuild  # Clean + build
make run      # Build + run
```

### Manual Compilation
```bash
g++ -std=c++11 -Wall -Wextra -o server \
    main.cpp \
    ConfigManager.cpp \
    HTTPRequest.cpp \
    HTTPResponse.cpp \
    FileHandler.cpp \
    HTTPServer.cpp \
    Logger.cpp
```

---

## 🧹 Project Cleanup

To remove compiled binaries and temporary files:

```bash
chmod +x cleanup.sh
./cleanup.sh
```

This removes:
- Compiled binaries (`server`, `main`, `*.o`)
- Log files (`server.log`)
- Old documentation files

---

## 📝 Code Statistics

| Component      | Lines | Purpose                          |
|----------------|-------|----------------------------------|
| ConfigManager  | ~150  | Configuration management         |
| HTTPRequest    | ~250  | Request parsing & validation     |
| HTTPResponse   | ~200  | Response building & formatting   |
| FileHandler    | ~200  | File I/O & MIME detection        |
| HTTPServer     | ~220  | Main server orchestration        |
| Logger         | ~150  | Logging system ⭐ NEW            |
| Main           | ~40   | Entry point                      |
| **Total**      | **~1210** | **Professional C++ HTTP Server** |

