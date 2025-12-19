#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>
#include <map>

class HTTPResponse {
private:
    int status_code;
    std::string status_text;
    std::map<std::string, std::string> headers;
    std::string body;
    
    std::string getStatusText(int code) const;
    
public:
    HTTPResponse();
    HTTPResponse(int code);
    
    // Status
    void setStatus(int code);
    void setStatus(int code, const std::string& text);
    int getStatusCode() const { return status_code; }
    std::string getStatusText() const { return status_text; }
    
    // Headers
    void setHeader(const std::string& name, const std::string& value);
    void setContentType(const std::string& type);
    void setContentLength(size_t length);
    
    // Body
    void setBody(const std::string& content);
    void setBody(const char* content, size_t length);
    std::string getBody() const { return body; }
    
    // Build response
    std::string build() const;
    void send(int socket) const;
    
    // Convenience methods for common responses
    static HTTPResponse createError(int code, const std::string& message = "");
    static HTTPResponse createHTML(const std::string& html, int code = 200);
    static HTTPResponse createJSON(const std::string& json, int code = 200);
    static HTTPResponse createRedirect(const std::string& location, int code = 302);
};

#endif
