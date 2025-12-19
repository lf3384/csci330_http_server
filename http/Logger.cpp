#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>

// Initialize static instance
Logger* Logger::instance = nullptr;

Logger::Logger() {
    // Open log file in append mode
    logFile.open("server.log", std::ios::app);
    
    if (!logFile.is_open()) {
        std::cerr << "WARNING: Failed to open server.log" << std::endl;
    } else {
        // Log server startup
        log("========================================");
        log("Server started");
        log("========================================");
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        log("Server shutdown");
        log("========================================\n");
        logFile.close();
    }
}

Logger* Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

std::string Logger::getCurrentTimestamp() {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    // Format: [YYYY-MM-DD HH:MM:SS]
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "[%Y-%m-%d %H:%M:%S]");
    
    return ss.str();
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (logFile.is_open()) {
        logFile << getCurrentTimestamp() << " " << message << std::endl;
        logFile.flush(); // Ensure immediate write
    }
    
    // Also print to console
    std::cout << getCurrentTimestamp() << " " << message << std::endl;
}

void Logger::logRequest(const std::string& clientIP, 
                       const std::string& method, 
                       const std::string& path, 
                       int statusCode, 
                       const std::string& statusText) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::stringstream ss;
    ss << getCurrentTimestamp() << " " 
       << clientIP << " "
       << method << " "
       << path << " "
       << statusCode << " "
       << statusText;
    
    std::string logLine = ss.str();
    
    if (logFile.is_open()) {
        logFile << logLine << std::endl;
        logFile.flush();
    }
    
    // Also print to console
    std::cout << logLine << std::endl;
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.flush();
    }
}
