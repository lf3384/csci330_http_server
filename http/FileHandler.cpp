#include "FileHandler.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>

FileHandler::FileHandler(const std::string& root, size_t maxSize) 
    : document_root(root), max_file_size(maxSize) {
}

std::string FileHandler::getExtension(const std::string& path) const {
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        return path.substr(dotPos);
    }
    return "";
}

std::string FileHandler::getMimeType(const std::string& path) const {
    std::string ext = getExtension(path);
    
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif") return "image/gif";
    if (ext == ".svg") return "image/svg+xml";
    if (ext == ".ico") return "image/x-icon";
    if (ext == ".txt") return "text/plain";
    if (ext == ".pdf") return "application/pdf";
    if (ext == ".xml") return "application/xml";
    
    return "application/octet-stream";
}

bool FileHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool FileHandler::isDirectory(const std::string& path) const {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

size_t FileHandler::getFileSize(const std::string& path) const {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return 0;
    }
    return buffer.st_size;
}

std::string FileHandler::buildFilePath(const std::string& requestPath) const {
    std::string path = requestPath;
    
    // Handle root path
    if (path == "/" || path.empty()) {
        path = "/index.html";
    }
    
    // Remove leading slash for filesystem
    if (path[0] == '/') {
        path = path.substr(1);
    }
    
    // Combine with document root
    std::string fullPath = document_root;
    if (fullPath.back() != '/') {
        fullPath += "/";
    }
    fullPath += path;
    
    return fullPath;
}

std::string FileHandler::readFile(const std::string& path, bool& success) const {
    success = false;
    
    // Check if file exists
    if (!fileExists(path)) {
        return "";
    }
    
    // Check if it's a directory
    if (isDirectory(path)) {
        return "";
    }
    
    // Check file size
    size_t fileSize = getFileSize(path);
    if (fileSize > max_file_size) {
        std::cerr << "File too large: " << path << " (" << fileSize << " bytes)" << std::endl;
        return "";
    }
    
    // Read file
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    success = true;
    return buffer.str();
}

HTTPResponse FileHandler::serveFile(const std::string& requestPath) const {
    std::string filePath = buildFilePath(requestPath);
    
    bool success = false;
    std::string content = readFile(filePath, success);
    
    if (!success) {
        // Check if directory
        if (isDirectory(filePath)) {
            // Try index.html in directory
            std::string indexPath = filePath;
            if (indexPath.back() != '/') indexPath += "/";
            indexPath += "index.html";
            
            content = readFile(indexPath, success);
            if (success) {
                filePath = indexPath;
            } else {
                return HTTPResponse::createError(403, "Directory listing not available");
            }
        } else {
            return HTTPResponse::createError(404, "The requested file '" + requestPath + "' was not found on this server.");
        }
    }
    
    // Create successful response
    HTTPResponse response(200);
    response.setContentType(getMimeType(filePath));
    response.setBody(content);
    
    return response;
}

HTTPResponse FileHandler::serveDirectoryListing(const std::string& dirPath) const {
    std::string fullPath = buildFilePath(dirPath);
    
    if (!isDirectory(fullPath)) {
        return HTTPResponse::createError(404, "Not a directory");
    }
    
    DIR* dir = opendir(fullPath.c_str());
    if (!dir) {
        return HTTPResponse::createError(403, "Cannot read directory");
    }
    
    std::ostringstream html;
    html << "<!DOCTYPE html><html><head><title>Directory: " << dirPath << "</title>"
         << "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css\" rel=\"stylesheet\">"
         << "</head><body class=\"bg-light\">"
         << "<div class=\"container mt-5\">"
         << "<div class=\"card shadow\">"
         << "<div class=\"card-header bg-primary text-white\">"
         << "<h2>Directory Listing: " << dirPath << "</h2></div>"
         << "<div class=\"card-body\">"
         << "<ul class=\"list-group\">";
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        
        // Skip hidden files
        if (name[0] == '.') continue;
        
        std::string itemPath = dirPath;
        if (itemPath.back() != '/') itemPath += "/";
        itemPath += name;
        
        if (entry->d_type == DT_DIR) {
            html << "<li class=\"list-group-item\">📁 <a href=\"" << itemPath << "/\">" 
                 << name << "/</a></li>";
        } else {
            html << "<li class=\"list-group-item\">📄 <a href=\"" << itemPath << "\">" 
                 << name << "</a></li>";
        }
    }
    
    html << "</ul></div></div></div></body></html>";
    
    closedir(dir);
    
    return HTTPResponse::createHTML(html.str());
}
