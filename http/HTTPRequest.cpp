#include "HTTPRequest.h"
#include <sstream>
#include <iostream>
#include <algorithm>

HTTPRequest::HTTPRequest() : method(""), path(""), version(""), query_string(""), body("") {}

HTTPRequest::HTTPRequest(const std::string& rawRequest) {
    parse(rawRequest);
}

std::string HTTPRequest::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void HTTPRequest::parseRequestLine(const std::string& line) {
    std::istringstream iss(line);
    iss >> method >> path >> version;
    
    // Parse query string from path
    size_t queryPos = path.find('?');
    if (queryPos != std::string::npos) {
        query_string = path.substr(queryPos + 1);
        path = path.substr(0, queryPos);
        parseQueryString(query_string);
    }
}

void HTTPRequest::parseHeaders(const std::string& headerSection) {
    std::istringstream stream(headerSection);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));
            
            // Convert header name to lowercase for case-insensitive lookup
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            headers[name] = value;
        }
    }
}

void HTTPRequest::parseQueryString(const std::string& query) {
    std::istringstream stream(query);
    std::string pair;
    
    while (std::getline(stream, pair, '&')) {
        size_t equals = pair.find('=');
        if (equals != std::string::npos) {
            std::string key = pair.substr(0, equals);
            std::string value = pair.substr(equals + 1);
            query_params[key] = value;
        }
    }
}

bool HTTPRequest::parse(const std::string& rawRequest) {
    if (rawRequest.empty()) {
        return false;
    }
    
    // Split request into lines
    size_t firstNewline = rawRequest.find("\r\n");
    if (firstNewline == std::string::npos) {
        firstNewline = rawRequest.find("\n");
    }
    
    if (firstNewline == std::string::npos) {
        return false;
    }
    
    // Parse request line
    std::string requestLine = rawRequest.substr(0, firstNewline);
    parseRequestLine(requestLine);
    
    // Find headers section
    size_t headerStart = firstNewline;
    if (rawRequest[firstNewline] == '\r') headerStart += 2;
    else headerStart += 1;
    
    size_t headerEnd = rawRequest.find("\r\n\r\n", headerStart);
    if (headerEnd == std::string::npos) {
        headerEnd = rawRequest.find("\n\n", headerStart);
        if (headerEnd != std::string::npos) {
            std::string headerSection = rawRequest.substr(headerStart, headerEnd - headerStart);
            parseHeaders(headerSection);
            
            // Body starts after headers
            size_t bodyStart = headerEnd + 2;
            if (bodyStart < rawRequest.length()) {
                body = rawRequest.substr(bodyStart);
            }
        }
    } else {
        std::string headerSection = rawRequest.substr(headerStart, headerEnd - headerStart);
        parseHeaders(headerSection);
        
        // Body starts after headers
        size_t bodyStart = headerEnd + 4;
        if (bodyStart < rawRequest.length()) {
            body = rawRequest.substr(bodyStart);
        }
    }
    
    return isValid();
}

std::string HTTPRequest::getHeader(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    auto it = headers.find(lowerName);
    if (it != headers.end()) {
        return it->second;
    }
    return "";
}

bool HTTPRequest::hasHeader(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    return headers.find(lowerName) != headers.end();
}

std::string HTTPRequest::getQueryParam(const std::string& name) const {
    auto it = query_params.find(name);
    if (it != query_params.end()) {
        return it->second;
    }
    return "";
}

bool HTTPRequest::hasQueryParam(const std::string& name) const {
    return query_params.find(name) != query_params.end();
}

bool HTTPRequest::isValid() const {
    return !method.empty() && !path.empty() && !version.empty();
}

bool HTTPRequest::isSecurePath() const {
    // Check for path traversal
    if (path.find("..") != std::string::npos) {
        return false;
    }
    
    // Must start with /
    if (path.empty() || path[0] != '/') {
        return false;
    }
    
    // Check for null bytes
    if (path.find('\0') != std::string::npos) {
        return false;
    }
    
    return true;
}

void HTTPRequest::print() const {
    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;
    std::cout << "Version: " << version << std::endl;
    
    if (!query_string.empty()) {
        std::cout << "Query String: " << query_string << std::endl;
    }
    
    if (!headers.empty()) {
        std::cout << "Headers:" << std::endl;
        for (const auto& header : headers) {
            std::cout << "  " << header.first << ": " << header.second << std::endl;
        }
    }
    
    if (!query_params.empty()) {
        std::cout << "Query Parameters:" << std::endl;
        for (const auto& param : query_params) {
            std::cout << "  " << param.first << " = " << param.second << std::endl;
        }
    }
    
    if (!body.empty()) {
        std::cout << "Body Length: " << body.length() << " bytes" << std::endl;
    }
}
