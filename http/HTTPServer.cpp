#include "HTTPServer.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

HTTPServer::HTTPServer() 
    : server_socket(-1), config(nullptr), fileHandler(nullptr), running(false) {
}

HTTPServer::~HTTPServer() {
    stop();
    if (fileHandler) {
        delete fileHandler;
    }
}

void HTTPServer::setConfig(ConfigManager* cfg) {
    config = cfg;
}

bool HTTPServer::createSocket() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "ERROR: Failed to create socket" << std::endl;
        return false;
    }
    
    // Set socket options - allow reuse
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "WARNING: Failed to set SO_REUSEADDR" << std::endl;
    }
    
    return true;
}

bool HTTPServer::bindSocket() {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config->getPort());
    
    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "ERROR: Failed to bind socket to port " << config->getPort() << std::endl;
        close(server_socket);
        return false;
    }
    
    return true;
}

bool HTTPServer::startListening() {
    if (listen(server_socket, config->getMaxConnections()) < 0) {
        std::cerr << "ERROR: Failed to listen on socket" << std::endl;
        close(server_socket);
        return false;
    }
    
    return true;
}

bool HTTPServer::initialize() {
    // Get config instance
    if (!config) {
        config = ConfigManager::getInstance();
    }
    
    // Create file handler
    fileHandler = new FileHandler(config->getDocumentRoot(), config->getMaxFileSize());
    
    // Create and configure socket
    if (!createSocket()) return false;
    if (!bindSocket()) return false;
    if (!startListening()) return false;
    
    return true;
}

HTTPResponse HTTPServer::processRequest(const HTTPRequest& request) {
    // Log request
    std::cout << "  Method: " << request.getMethod() << std::endl;
    std::cout << "  Path: " << request.getPath() << std::endl;
    
    // Validate request
    if (!request.isValid()) {
        return HTTPResponse::createError(400, "Invalid HTTP request");
    }
    
    // Only support GET for now
    if (request.getMethod() != "GET") {
        return HTTPResponse::createError(405, "Only GET method is supported");
    }
    
    // Check path security
    if (!request.isSecurePath()) {
        return HTTPResponse::createError(403, "Access to '" + request.getPath() + "' is forbidden");
    }
    
    // Serve file
    return fileHandler->serveFile(request.getPath());
}

void HTTPServer::handleClient(int client_socket) {
    char buffer[8192] = {0};
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }
    
    // Parse request
    std::string rawRequest(buffer, bytes_read);
    HTTPRequest request(rawRequest);
    
    // Process request and get response
    HTTPResponse response = processRequest(request);
    
    // Send response
    response.send(client_socket);
    
    // Log response
    std::cout << "  Status: " << response.getStatusCode() << std::endl;
    std::cout << std::endl;
    
    close(client_socket);
}

void HTTPServer::start() {
    if (!initialize()) {
        std::cerr << "ERROR: Failed to initialize server" << std::endl;
        return;
    }
    
    running = true;
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << config->getServerName() << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Server running on http://localhost:" << config->getPort() << std::endl;
    std::cout << "Document root: " << config->getDocumentRoot() << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << std::string(50, '=') << "\n" << std::endl;
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (running) {
                std::cerr << "ERROR: Failed to accept connection" << std::endl;
            }
            continue;
        }
        
        // Get client IP
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        
        std::cout << "Connection from " << client_ip << std::endl;
        
        // Handle request
        handleClient(client_socket);
    }
}

void HTTPServer::stop() {
    running = false;
    if (server_socket >= 0) {
        close(server_socket);
        server_socket = -1;
    }
    std::cout << "\nServer stopped" << std::endl;
}
