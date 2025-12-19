#include "ConfigManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

ConfigManager* ConfigManager::instance = nullptr;

ConfigManager::ConfigManager() {
    // Set default values
    config["port"] = "8080";
    config["document_root"] = ".";
    config["max_file_size"] = "10485760"; // 10MB
    config["max_connections"] = "10";
    config["server_name"] = "C++ HTTP Server";
}

ConfigManager* ConfigManager::getInstance() {
    if (instance == nullptr) {
        instance = new ConfigManager();
    }
    return instance;
}

std::string ConfigManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void ConfigManager::parseConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file '" << filename 
                  << "'. Using defaults." << std::endl;
        return;
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Parse key=value
        size_t equals = line.find('=');
        if (equals != std::string::npos) {
            std::string key = trim(line.substr(0, equals));
            std::string value = trim(line.substr(equals + 1));
            
            if (!key.empty()) {
                config[key] = value;
            }
        }
    }
    
    file.close();
}

void ConfigManager::loadConfig(const std::string& filename) {
    parseConfigFile(filename);
}

int ConfigManager::getPort() const {
    auto it = config.find("port");
    if (it != config.end()) {
        return std::stoi(it->second);
    }
    return 8080;
}

std::string ConfigManager::getDocumentRoot() const {
    return get("document_root", ".");
}

size_t ConfigManager::getMaxFileSize() const {
    auto it = config.find("max_file_size");
    if (it != config.end()) {
        return std::stoull(it->second);
    }
    return 10 * 1024 * 1024; // 10MB default
}

int ConfigManager::getMaxConnections() const {
    auto it = config.find("max_connections");
    if (it != config.end()) {
        return std::stoi(it->second);
    }
    return 10;
}

std::string ConfigManager::getServerName() const {
    return get("server_name", "C++ HTTP Server");
}

std::string ConfigManager::get(const std::string& key, const std::string& defaultValue) const {
    auto it = config.find(key);
    if (it != config.end()) {
        return it->second;
    }
    return defaultValue;
}

void ConfigManager::printConfig() const {
    std::cout << "\n=== Server Configuration ===" << std::endl;
    std::cout << "Port: " << getPort() << std::endl;
    std::cout << "Document Root: " << getDocumentRoot() << std::endl;
    std::cout << "Max File Size: " << getMaxFileSize() << " bytes" << std::endl;
    std::cout << "Max Connections: " << getMaxConnections() << std::endl;
    std::cout << "Server Name: " << getServerName() << std::endl;
    std::cout << "============================\n" << std::endl;
}
