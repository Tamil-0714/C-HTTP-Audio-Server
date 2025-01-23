/*
TODO: fix buffer overflow
//*completed

TODO: free all memory after complted it usage
^not completed
---------------------------------------------
TODO: remove unwanter printf statemnts
^not completed
*/

// #include "bstfnc.h"
#include "clrfnc.h"
#include "dbfnc.h"
#include "fileio.h"
#include "globals.h"
#include "stdfnc.h"
#include <asm-generic/socket.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#define PORT 8020
#define BUFFER_SIZE 1024
void send_headers(int client_socket, int status_code,
                  const char *status_message, const char *content_type,
                  size_t content_length);
const char *get_status_message(int status_code);
void send_response(int client_socket, const char *content, int status_code,
                   const char *content_type);

struct http_request {
  char method[16];
  char route[512];
  char version[16];
};

void handle_client(int client_socket) {
  char buffer[BUFFER_SIZE] = {0};
  struct http_request *req = malloc(sizeof(struct http_request));

  if (req == NULL) {
    perror("Memory allocation failed");
    close(client_socket);
    return;
  }

  const char *NOT_FOUND_RESPONSE =
      "<html><body><h1>404 Not Found</h1></body></html>";
  const char *METHOD_NOT_ALLOWED =
      "<html><body><h1>405 Method Not Allowed</h1></body></html>";
  const char *svg_favicon =
      "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 16 16\">"
      "<rect width=\"16\" height=\"16\" fill=\"teal\"/>"
      "<text x=\"8\" y=\"12\" font-size=\"10\" text-anchor=\"middle\" "
      "fill=\"white\">T</text>"
      "</svg>";

  // Set socket to non-blocking mode
  int flags = fcntl(client_socket, F_GETFL, 0);
  fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

  // Timeout for reading the request

  struct timeval tv;
  tv.tv_sec = 1; //  old is 1 second timeout
  tv.tv_usec = 0;

  fd_set readfds;
  FD_ZERO(&readfds);

  FD_SET(client_socket, &readfds);

  int retires = 3;
  int ready = select(client_socket + 1, &readfds, NULL, NULL, &tv);

  if (ready > 0) {

    ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    printf("\033[0;31m");
    printf("Bytes read : %zu\n", bytes_read);
    printf("\033[0m");
    green();
    printf("Actial bytes : %s\n", buffer);
    if (bytes_read >= 1023) {
      perror("Illeagal input violated.");
      send_response(client_socket, "input violation", 404, "text/plain");
      close(client_socket);
      return;
    }
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0'; // Null-terminate the buffer

      const char *newline_pos = strchr(buffer, '\n');
      size_t line_length = newline_pos - buffer;
      // char *first_line_str = extract_first_line(buffer, line_length);

      if (line_length > 512) {
        perror("Too long header input");
        // struct timespec dleay_time = {2, 0}; // 10ms
        // nanosleep(&dleay_time, NULL);
        send_response(client_socket, "Too Many head input", 404, "text/plain");
        close(client_socket);
        return;
      }
      sscanf(buffer, "%s %s %s", req->method, req->route, req->version);

      // Handle GET request
      if (strcmp(req->method, "GET") == 0) {
        if (strcmp(req->route, "/files") == 0) {

          // char *json_str = generate_file_id_json();
          char *json_str = display_all_files();
          if (NULL == json_str) {
            perror("error in converting to json");
            send_response(client_socket, "internal server error", 500,
                          "text/plain");
            close(client_socket);
            return;
          }
          send_response(client_socket, json_str, 200, "application/json");
          free(json_str);

        } else if (verify_audio_route(req->route)) {
          char *file_name = extract_file_id(req->route);
          if (file_name == NULL) {
            perror("File name extraction failed");
            send_response(client_socket, "Invalid file name", 404,
                          "text/plain");
            free(req);
            close(client_socket);
            return;
          }

          printf("actual file name : %s\n\n", file_name);
          char *file_search_result = get_from_db(file_name);
          if (strlen(file_name) == 0 || strlen(file_name) > 16) {

            perror("File Name is not valid");
            send_response(client_socket, "Invalid file name", 404,
                          "text/plain");
            free(req);
            free(file_name);
            free(file_search_result);
            close(client_socket);

            return;
          }
          // printf("This is file name : %s\n", file_name);

          size_t audio_size;
          unsigned char *audio_data =
              read_audio(file_search_result, &audio_size);
          if (NULL == audio_data) {
            perror("erro in reading audio");
            send_response(client_socket,
                          "Error reading audio file or file not found", 500,
                          "text/plain");
            free(req);
            free(file_name);
            free(file_search_result);
            close(client_socket);
            return;
          }
          size_t encoded_audio_size;
          char *base64_string =
              base64_encode(audio_data, audio_size, &encoded_audio_size);
          free(audio_data);
          if (base64_string == NULL) {
            send_response(client_socket, "Error encoding audio", 500,
                          "text/plain");
            free(req);
            free(file_name);
            free(file_search_result);
            close(client_socket);
            return;
          }
          // printf("Size is : %ld\n", strlen(base64_string)); //LOG_OLD
          send_response(client_socket, base64_string, 200, "text/plain");
          free(base64_string);
          free(file_name);
          free(file_search_result);
          printf("Base64 audio sent\n");
        } else if (strcmp(req->route, "/favicon.ico") == 0) {
          send_response(client_socket, svg_favicon, 200, "image/svg+xml");
        } else {
          // printf("404 Not Found route triggered\n"); //LOG_OLD
          send_response(client_socket, NOT_FOUND_RESPONSE, 404, "text/html");
        }
      } else {
        // Handle unsupported methods
        // printf("405 Method Not Allowed triggered\n");//LOG_OLD
        send_response(client_socket, METHOD_NOT_ALLOWED, 405, "text/html");
      }
    } else if (bytes_read == 0) {
      // printf("Client closed the connection\n");//LOG_OLD
    } else {
      perror("recv() failed");
      send_response(client_socket, "Illegal input.", 500, "text/plain");
      close(client_socket);
      return;
    }
  } else {
    // printf("Timeout occurred or no data available\n");
    // printf("retring\n");//LOG_OLD
  }

  // Ensure all data is sent before closing
  shutdown(client_socket, SHUT_WR);

  // Clear any remaining incoming data
  int total_bytes_received = 0;
  int bytes_received;
  while ((bytes_received = recv(client_socket, buffer + total_bytes_received,
                                BUFFER_SIZE - total_bytes_received, 0)) > 0) {
    // Discard remaining data
    total_bytes_received += bytes_received;

    // Check if you've filled the buffer
    if (total_bytes_received >= sizeof(buffer)) {
      // Handle the case where the buffer is full
      break;
    }
  }

  close(client_socket);
  free(req); // Free allocated memory
}

int main() {
  // initialize_files_hash(); uncomment this when needed
  int server_fd;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  // Create socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Set socket options, // !"for overcome the address already in use error"
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Setsockopt failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind socket to port // !PORT
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_fd, 10) < 0) { // Increased backlog to 10
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", PORT);

  while (1) {
    int new_socket;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
      perror("Accept failed");
      continue; // Continue instead of exit to keep server running
    }

    handle_client(new_socket);
    // free_files_hash();
    // initialize_files_hash();
    // free(files_hash);
    // files_hash = NULL;
  }

  return 0;
}

// Helper function to send HTTP headers
void send_headers(int client_socket, int status_code,
                  const char *status_message, const char *content_type,
                  size_t content_length) {
  char headers[512];
  snprintf(headers, sizeof(headers),
           "HTTP/1.1 %d %s\r\n"
           "Content-Type: %s\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "Content-Length: %zu\r\n"
           "Connection: Keep-Alive\r\n"
           "\r\n",
           status_code, status_message, content_type, content_length);

  send(client_socket, headers, strlen(headers), 0);
}

// Function to get status message
const char *get_status_message(int status_code) {
  switch (status_code) {
  case 200:
    return "OK";
  case 404:
    return "Not Found";
  case 405:
    return "Not Allowed";
  default:
    return "Unknown Status";
  }
}

// Improved send_response function
void send_response(int client_socket, const char *content, int status_code,
                   const char *content_type) {
  // Set socket to non-blocking mode
  int flags = fcntl(client_socket, F_GETFL, 0);
  if (flags < 0) {
    perror("Failed to get socket flags");
    close(client_socket);
    return;
  }
  if (fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
    perror("Failed to set socket non-blocking");
    close(client_socket);
    return;
  }

  // First, send the headers with the total content length
  const char *status_message = get_status_message(status_code);
  size_t content_length = strlen(content);
  send_headers(client_socket, status_code, status_message, content_type,
               content_length);

  // Send the content in chunks
  const size_t CHUNK_SIZE = 4096; // Reduced to 4KB chunks for better handling
  size_t remaining = content_length;
  const char *current_pos = content;

  while (remaining > 0) {
    size_t chunk = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;
    ssize_t sent = 0;
    int retry_count = 0;
    const int MAX_RETRIES = 5;

    // Keep trying to send until all data is sent or max retries reached
    while (chunk > 0 && retry_count < MAX_RETRIES) {
      sent = send(client_socket, current_pos, chunk, MSG_NOSIGNAL);

      if (sent > 0) {
        current_pos += sent;
        remaining -= sent;
        chunk -= sent;
        retry_count = 0; // Reset retry count on successful send
      } else if (sent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          // Socket buffer is full, wait a bit
          struct timespec sleep_time = {0, 10000000}; // 10ms
          nanosleep(&sleep_time, NULL);
          retry_count++;
          continue;
        } else if (errno == EINTR) {
          // Interrupted by signal, try again
          continue;
        } else {
          // Unrecoverable error
          perror("Failed to send data");
          close(client_socket);
          return;
        }
      }
    }

    if (retry_count >= MAX_RETRIES) {
      fprintf(stderr, "Maximum retries reached while sending data\n");
      close(client_socket);
      return;
    }
  }

  // Restore original socket flags
  if (fcntl(client_socket, F_SETFL, flags) < 0) {
    perror("Failed to restore socket flags");
  }
}

/*
 if (!(strlen(file_name) > 0 && file_name != NULL) &&
                strlen(file_name) > 16) {


*/