#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger {
private:
    static Logger* instance;
    std::ofstream logFile;
    std::mutex logMutex;
    
    // Private constructor for Singleton
    Logger();
    
    // Get current timestamp as string
    std::string getCurrentTimestamp();
    
public:
    // Singleton access
    static Logger* getInstance();
    
    // Destructor
    ~Logger();
    
    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logging methods
    void log(const std::string& message);
    void logRequest(const std::string& clientIP, 
                   const std::string& method, 
                   const std::string& path, 
                   int statusCode, 
                   const std::string& statusText);
    
    // Utility
    void flush();
};

#endif
