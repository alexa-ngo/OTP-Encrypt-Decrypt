#include "dec_server.h"

/*
 *  Usage: ./dec_server data.txt port_num
 *          valgrind ./dec_server --leak-check=full message_hello key-5-hello 1234
 */

// Listener_d is a global variable declared in main
int listener_d;

// Handles the shutdown process
void handle_shutdown(int sig) {
    if (listener_d) {
        close(listener_d);
    }
    printf("Bye!\n");
    exit(0);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {       // argv[0]      argv[1]
        printf("Usage: <program_name> <port>\n");
        exit(1);
    }

    const char* port = argv[1];
    int port_num = atoi(port);

    // This will call the handle_shutdown() if CTRL-C is hit
    if (catch_signal(SIGINT, handle_shutdown) == DOES_NOT_EXIST) {
        fprintf(stderr, "Can't set the interrupt handler");
        exit(1);
    }

    // Listen and bind to a port
    int listener_d = open_listener_socket();
    bind_to_port(listener_d, port_num);
    if (listen(listener_d, MAX_5_NUM_OF_RUNNING_PROCESSES) == DOES_NOT_EXIST) {
        fprintf(stderr, "Can't listen");
        exit(1);
    }

    printf(">> Listening to port %i\n\nWaiting for a connection... \n", port_num);

    struct sockaddr_storage client_addr;
    unsigned int address_size = sizeof(client_addr);
    char buf[BYTES_OF_DATA_100000];

    while (1) {
        int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
        if (connect_d == DOES_NOT_EXIST) {
            fprintf(stderr, "Can't open secondary socket using the accept method");
            continue;
        }

        int child_pid = fork();
        if (child_pid == DOES_NOT_EXIST) {
            fprintf(stderr, "Could not fork the child\n");
            exit(1);
        }
        if (child_pid > 0) {
            fprintf(stderr, "Parent is closing child socket. Child PID: %d\n\n", child_pid);
            // Parent is closing the CLIENT SOCKET
            close(connect_d);
        } else {
            // The child process will handle the request, so close the listener
            close(listener_d);

            // Clear the buffer before data can be sent in
            memset(buf, '\0', BYTES_OF_DATA_100000);

            // Read the client's message from the socket
            int chars_read = recv(connect_d, buf, BYTES_OF_DATA_100000, 0);
            if (chars_read == DOES_NOT_EXIST) {
                fprintf(stderr, "Error in receiving\n");
                close(connect_d);
                exit(1);
            }
            if (chars_read == 0) {
                fprintf(stderr, "Error. Received 0 bytes.");
                close(connect_d);
                exit(1);
            }

            char* save_ptr;
            // Get the role of the sender
            char* token = strtok_r(buf, "#", &save_ptr);
            if (token == NULL) {
                fprintf(stderr, "Error. No role.");
                close(connect_d);
                exit(1);
            }

            // Get the key length
            token = strtok_r(NULL, "#", &save_ptr);
            if (token == NULL) {
                fprintf(stderr, "Error. No key length.");
                close(connect_d);
                exit(1);
            }
            char* key_str_size = strdup(token);
            int key_str_size_int = atoi(key_str_size);

            // Get the key string
            token = strtok_r(NULL, "#", &save_ptr);
            if (token == NULL) {
                fprintf(stderr, "Error. No key string.");
                close(connect_d);
                exit(1);
            }
            char* key_file_str = strdup(token);

            // Get the plain text
            token = strtok_r(NULL, "#", &save_ptr);
            if (token == NULL) {
                fprintf(stderr, "Error. No plain text length.");
                close(connect_d);
                exit(1);
            }
            char* encrypted_data_str_size = strdup(token);
            int encrypted_data_str_size_int = atoi(encrypted_data_str_size);

            // Get the plain text string
            token = strtok_r(NULL, "#", &save_ptr);
            if (token == NULL) {
                fprintf(stderr, "Error. No plain text.");
                close(connect_d);
                exit(1);
            }
            char* encrypted_data_str = strdup(token);

            // Decode the buf string and send it back to the client
            char* decrypted_result = decrypt_data(key_str_size_int, key_file_str, encrypted_data_str_size_int, encrypted_data_str);

            // Sends the data to the client
            int send_result = send(connect_d, decrypted_result, strlen(decrypted_result), 0);
            if (send_result == DOES_NOT_EXIST) {
                fprintf(stderr, "Error in sending\n");
                exit(1);
            }

            printf(">> Decrypted result: %s\n", decrypted_result);

            // Close the connection
            free(decrypted_result);
            free(encrypted_data_str_size);
            free(encrypted_data_str);
            free(key_file_str);
            free(key_str_size);
            close(connect_d);
            exit(0);
        }
    }
}

