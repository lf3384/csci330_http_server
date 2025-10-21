#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>

struct HTTPRequest {
    char method[16];
    char path[256];
    char version[16];
};

bool parse_http_request(const char* buffer, HTTPRequest* request) {
    int parsed = sscanf(buffer, "%15s %255s %15s",
                        request->method,
                        request->path,
                        request->version);
    return parsed == 3;
}

// Function to validate
bool is_safe_path(const char* path) {
    if (strstr(path, "..") != NULL) {
        return false;
    }


    if (path[0] != '/') {
        return false;
    }

    // Check for null
    size_t path_len = strlen(path);
    for (size_t i = 0; i < path_len; i++) {
        if (path[i] == '\0' && i < path_len - 1) {
            return false;
        }
    }

    return true;
}

const char* get_mime_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".txt") == 0) return "text/plain";

    return "application/octet-stream";
}

char* read_file(const char* filepath, size_t* file_size) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) return NULL;

    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0) {
        close(fd);
        return NULL;
    }

    if (S_ISDIR(file_stat.st_mode)) {
        close(fd);
        return NULL;
    }

    *file_size = file_stat.st_size;


    if (*file_size > 10 * 1024 * 1024) {
        close(fd);
        return NULL;
    }

    char* content = (char*)malloc(*file_size + 1);
    if (!content) {
        close(fd);
        return NULL;
    }

    ssize_t bytes_read = read(fd, content, *file_size);
    close(fd);

    if (bytes_read != (ssize_t)*file_size) {
        free(content);
        return NULL;
    }

    content[*file_size] = '\0';
    return content;
}

void send_response(int client_socket, int status_code, const char* status_text,
                   const char* content_type, const char* body, size_t body_length) {
    char header[2048];
    snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_text, content_type, body_length
    );

    write(client_socket, header, strlen(header));
    if (body && body_length > 0) {
        write(client_socket, body, body_length);
    }
}

void send_404(int client_socket, const char* path) {
    char body[1024];
    snprintf(body, sizeof(body),
        "<!DOCTYPE html><html><head><title>404 Not Found</title></head>"
        "<body><h1>404 - Not Found</h1>"
        "<p>The requested file <code>%s</code> was not found.</p></body></html>",
        path
    );
    send_response(client_socket, 404, "Not Found", "text/html", body, strlen(body));
}

void send_403(int client_socket, const char* path) {
    char body[1024];
    snprintf(body, sizeof(body),
        "<!DOCTYPE html><html><head><title>403 Forbidden</title></head>"
        "<body><h1>403 - Forbidden</h1>"
        "<p>Access to <code>%s</code> is forbidden.</p></body></html>",
        path
    );
    send_response(client_socket, 403, "Forbidden", "text/html", body, strlen(body));
}

int main () {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("Socket creation failed");
        return 1;
    }

    //socket reuse
    int opt = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server);
        return 1;
    }

    if (listen(server, 3) < 0) {
        perror("Listen failed");
        close(server);
        return 1;
    }

    printf("Server is listening on http://localhost:8080\n");
    printf("Serving files from current directory\n");
    printf("Security: Path traversal protection enabled\n\n");

    while (true) {
        int client_accept = accept(server, NULL, NULL);
        if (client_accept < 0) {
            perror("Accept failed");
            continue;
        }

        char buffer[4096] = {0};
        ssize_t bytes_read = read(client_accept, buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            HTTPRequest request;
            if (parse_http_request(buffer, &request)) {
                printf("Request: %s %s\n", request.method, request.path);

                //handle GET requests
                if (strcmp(request.method, "GET") != 0) {
                    const char* error_body = "<!DOCTYPE html><html><body><h1>405 Method Not Allowed</h1></body></html>";
                    send_response(client_accept, 405, "Method Not Allowed", "text/html",
                                error_body, strlen(error_body));
                    printf("  -> 405 Method Not Allowed\n\n");
                    close(client_accept);
                    continue;
                }

                // Validate path for security
                if (!is_safe_path(request.path)) {
                    send_403(client_accept, request.path);
                    printf("  -> 403 Forbidden (invalid path)\n\n");
                    close(client_accept);
                    continue;
                }

                char filepath[512];
                if (strcmp(request.path, "/") == 0) {
                    snprintf(filepath, sizeof(filepath), "./index.html");
                } else {

                    snprintf(filepath, sizeof(filepath), ".%s", request.path);
                }

                size_t file_size;
                char* file_content = read_file(filepath, &file_size);

                if (file_content) {
                    const char* mime_type = get_mime_type(filepath);
                    send_response(client_accept, 200, "OK", mime_type,
                                file_content, file_size);
                    printf("  -> 200 OK (%s, %zu bytes)\n\n", mime_type, file_size);
                    free(file_content);
                } else {
                    send_404(client_accept, request.path);
                    printf("  -> 404 Not Found\n\n");
                }

            } else {
                const char* error_body = "<!DOCTYPE html><html><body><h1>400 Bad Request</h1></body></html>";
                send_response(client_accept, 400, "Bad Request", "text/html",
                            error_body, strlen(error_body));
                printf("ERROR: Failed to parse request\n\n");
            }
        }

        close(client_accept);
    }

    close(server);
    return 0;
}
