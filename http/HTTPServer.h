#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include "ConfigManager.h"
#include "FileHandler.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HTTPServer {
private:
    int server_socket;
    ConfigManager* config;
    FileHandler* fileHandler;
    bool running;
    
    // Socket operations
    bool createSocket();
    bool bindSocket();
    bool startListening();
    
    // Request handling
    void handleClient(int client_socket);
    HTTPResponse processRequest(const HTTPRequest& request);
    
public:
    HTTPServer();
    ~HTTPServer();
    
    // Server lifecycle
    bool initialize();
    void start();
    void stop();
    
    // Configuration
    void setConfig(ConfigManager* cfg);
};

#endif
