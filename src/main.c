#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "s7.h"

#define PORT 8080
#define BUFFER_SIZE 4096

/* Forward declare Scheme handler */
static s7_pointer handle_request(s7_scheme *s7, s7_pointer args);

// Add a global or static variable to hold the Scheme file path
static char scheme_file_path[1024] = "src/main.sch"; // default

/* Thread function to handle each client */
void* client_handler(void *arg) {
    int client_socket = *(int*)arg;
    free(arg);

    // Read HTTP request
    char buffer[BUFFER_SIZE];
    read(client_socket, buffer, BUFFER_SIZE);

    // Inside client_handler:
    char *saveptr;  // For strtok_r's context pointer

    // Parse method
    char *method = strtok_r(buffer, " ", &saveptr);
    if (!method) {
        fprintf(stderr, "Invalid request (no method)\n");
        close(client_socket);
        return NULL;
    }

    // Parse path (trimming trailing HTTP/version)
    char *path = strtok_r(NULL, " ", &saveptr);
    if (!path) {
        fprintf(stderr, "Invalid request (no path)\n");
        close(client_socket);
        return NULL;
    }

    // Remove query params (optional, e.g., "/page?foo=bar" → "/page")
    char *query_start = strchr(path, '?');
    if (query_start) *query_start = '\0';

    // Initialize s7 for this thread
    s7_scheme *s7 = s7_init();
    s7_pointer load_result = s7_load(s7, scheme_file_path);

    if (s7_is_boolean(load_result) && !s7_boolean(s7, load_result)) {
        fprintf(stderr, "Error: Failed to load Scheme file '%s'.\n", scheme_file_path);
        char err_response[] = "HTTP/1.1 500 Internal Server Error\r\n\r\nScheme file load failed.";
        write(client_socket, err_response, strlen(err_response));
        close(client_socket);
        s7_quit(s7);
        return NULL;
    }

    // Call Scheme handler
    s7_pointer response = handle_request(s7, 
        s7_list(s7, 2, s7_make_string(s7, method), s7_make_string(s7, path)));

    // Send HTTP response
    const char *res_str = s7_string(response);
    write(client_socket, res_str, strlen(res_str));
    close(client_socket);

    s7_quit(s7);
    return NULL;
}

int main(int argc, char *argv[]) {
    // Parse command-line arguments for -i
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            strncpy(scheme_file_path, argv[i + 1], sizeof(scheme_file_path) - 1);
            scheme_file_path[sizeof(scheme_file_path) - 1] = '\0';
            i++; // skip next arg
        }
    }

    // Create TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 128);  // Allow 128 pending connections

    printf("Server running on http://localhost:%d\n", PORT);

    // Accept connections indefinitely
    while (1) {
        int *client_socket = malloc(sizeof(int));
        *client_socket = accept(server_fd, NULL, NULL);
        
        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, client_socket);
        pthread_detach(thread);
    }

    return 0;
}

/* C-to-Scheme request handler */
static s7_pointer handle_request(s7_scheme *s7, s7_pointer args) {
    return s7_call(s7, s7_name_to_value(s7, "handle-request"), args);
}