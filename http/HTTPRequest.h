#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>

class HTTPRequest {
private:
    std::string method;
    std::string path;
    std::string version;
    std::string query_string;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> query_params;
    std::string body;
    
    void parseRequestLine(const std::string& line);
    void parseHeaders(const std::string& headerSection);
    void parseQueryString(const std::string& query);
    std::string trim(const std::string& str);
    
public:
    HTTPRequest();
    HTTPRequest(const std::string& rawRequest);
    
    bool parse(const std::string& rawRequest);
    
    // Getters
    std::string getMethod() const { return method; }
    std::string getPath() const { return path; }
    std::string getVersion() const { return version; }
    std::string getQueryString() const { return query_string; }
    std::string getBody() const { return body; }
    
    // Header methods
    std::string getHeader(const std::string& name) const;
    bool hasHeader(const std::string& name) const;
    const std::map<std::string, std::string>& getHeaders() const { return headers; }
    
    // Query parameter methods
    std::string getQueryParam(const std::string& name) const;
    bool hasQueryParam(const std::string& name) const;
    const std::map<std::string, std::string>& getQueryParams() const { return query_params; }
    
    // Validation
    bool isValid() const;
    bool isSecurePath() const;
    
    void print() const;
};

#endif
