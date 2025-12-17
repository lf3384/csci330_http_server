#include <iostream>
#include <csignal>
#include "ConfigManager.h"
#include "HTTPServer.h"

// Global server pointer for signal handler
HTTPServer* globalServer = nullptr;

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    if (globalServer) {
        globalServer->stop();
    }
    exit(signum);
}

int main(int argc, char* argv[]) {
    // Register signal handler for Ctrl+C
    signal(SIGINT, signalHandler);

    // Get configuration manager instance
    ConfigManager* config = ConfigManager::getInstance();

    // Load configuration from file
    std::string configFile = "config.ini";
    if (argc > 1) {
        configFile = argv[1];
    }

    config->loadConfig(configFile);
    config->printConfig();

    // Create and start server
    HTTPServer server;
    server.setConfig(config);

    globalServer = &server;

    server.start();

    return 0;
}
