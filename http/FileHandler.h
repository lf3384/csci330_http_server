#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include "HTTPResponse.h"

class FileHandler {
private:
    std::string document_root;
    size_t max_file_size;
    
    std::string getMimeType(const std::string& path) const;
    std::string getExtension(const std::string& path) const;
    
public:
    FileHandler(const std::string& root, size_t maxSize);
    
    // File operations
    bool fileExists(const std::string& path) const;
    bool isDirectory(const std::string& path) const;
    size_t getFileSize(const std::string& path) const;
    
    // Read file content
    std::string readFile(const std::string& path, bool& success) const;
    
    // Build file path
    std::string buildFilePath(const std::string& requestPath) const;
    
    // Create HTTP response for file
    HTTPResponse serveFile(const std::string& requestPath) const;
    
    // Directory listing (optional feature)
    HTTPResponse serveDirectoryListing(const std::string& dirPath) const;
};

#endif
