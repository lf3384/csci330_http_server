#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <map>

class ConfigManager {
private:
    std::map<std::string, std::string> config;
    static ConfigManager* instance;
    
    ConfigManager(); // Private constructor for Singleton
    void parseConfigFile(const std::string& filename);
    std::string trim(const std::string& str);
    
public:
    // Singleton pattern
    static ConfigManager* getInstance();
    
    // Get configuration values
    int getPort() const;
    std::string getDocumentRoot() const;
    size_t getMaxFileSize() const;
    int getMaxConnections() const;
    std::string getServerName() const;
    
    // Generic getter with default value
    std::string get(const std::string& key, const std::string& defaultValue = "") const;
    
    void loadConfig(const std::string& filename);
    void printConfig() const;
};

#endif
