#include "HTTPResponse.h"
#include <sstream>
#include <unistd.h>
#include <cstring>

HTTPResponse::HTTPResponse() : status_code(200), status_text("OK") {
    setHeader("Connection", "close");
    setHeader("Server", "C++ HTTP Server");
}

HTTPResponse::HTTPResponse(int code) : status_code(code) {
    status_text = getStatusText(code);
    setHeader("Connection", "close");
    setHeader("Server", "C++ HTTP Server");
}

std::string HTTPResponse::getStatusText(int code) const {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

void HTTPResponse::setStatus(int code) {
    status_code = code;
    status_text = getStatusText(code);
}

void HTTPResponse::setStatus(int code, const std::string& text) {
    status_code = code;
    status_text = text;
}

void HTTPResponse::setHeader(const std::string& name, const std::string& value) {
    headers[name] = value;
}

void HTTPResponse::setContentType(const std::string& type) {
    setHeader("Content-Type", type);
}

void HTTPResponse::setContentLength(size_t length) {
    setHeader("Content-Length", std::to_string(length));
}

void HTTPResponse::setBody(const std::string& content) {
    body = content;
    setContentLength(body.length());
}

void HTTPResponse::setBody(const char* content, size_t length) {
    body.assign(content, length);
    setContentLength(length);
}

std::string HTTPResponse::build() const {
    std::ostringstream response;
    
    // Status line
    response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
    
    // Headers
    for (const auto& header : headers) {
        response << header.first << ": " << header.second << "\r\n";
    }
    
    // Blank line between headers and body
    response << "\r\n";
    
    // Body
    response << body;
    
    return response.str();
}

void HTTPResponse::send(int socket) const {
    std::string response = build();
    write(socket, response.c_str(), response.length());
}

HTTPResponse HTTPResponse::createError(int code, const std::string& message) {
    HTTPResponse response(code);
    
    std::string errorMessage = message.empty() ? response.getStatusText(code) : message;
    
    std::ostringstream html;
    html << "<!DOCTYPE html>"
         << "<html lang=\"en\">"
         << "<head>"
         << "<meta charset=\"UTF-8\">"
         << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
         << "<title>" << code << " " << response.getStatusText(code) << "</title>"
         << "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css\" rel=\"stylesheet\">"
         << "<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.1/font/bootstrap-icons.css\">"
         << "</head>"
         << "<body class=\"bg-light\">"
         << "<div class=\"container\">"
         << "<div class=\"row justify-content-center align-items-center\" style=\"min-height: 100vh;\">"
         << "<div class=\"col-md-6\">"
         << "<div class=\"card shadow text-center\">"
         << "<div class=\"card-body p-5\">";
    
    // Icon based on error code
    if (code == 404) {
        html << "<i class=\"bi bi-exclamation-triangle text-warning\" style=\"font-size: 4rem;\"></i>";
    } else if (code == 403) {
        html << "<i class=\"bi bi-shield-lock text-danger\" style=\"font-size: 4rem;\"></i>";
    } else if (code == 405) {
        html << "<i class=\"bi bi-x-circle text-danger\" style=\"font-size: 4rem;\"></i>";
    } else {
        html << "<i class=\"bi bi-bug text-warning\" style=\"font-size: 4rem;\"></i>";
    }
    
    html << "<h1 class=\"display-4 mt-3\">" << code << "</h1>"
         << "<h2 class=\"h4 text-muted\">" << response.getStatusText(code) << "</h2>"
         << "<p class=\"mt-3\">" << errorMessage << "</p>"
         << "<a href=\"/\" class=\"btn btn-primary mt-3\"><i class=\"bi bi-house-door\"></i> Go Home</a>"
         << "</div></div></div></div></div>"
         << "</body></html>";
    
    response.setContentType("text/html");
    response.setBody(html.str());
    
    return response;
}

HTTPResponse HTTPResponse::createHTML(const std::string& html, int code) {
    HTTPResponse response(code);
    response.setContentType("text/html");
    response.setBody(html);
    return response;
}

HTTPResponse HTTPResponse::createJSON(const std::string& json, int code) {
    HTTPResponse response(code);
    response.setContentType("application/json");
    response.setBody(json);
    return response;
}

HTTPResponse HTTPResponse::createRedirect(const std::string& location, int code) {
    HTTPResponse response(code);
    response.setHeader("Location", location);
    response.setBody("");
    return response;
}
