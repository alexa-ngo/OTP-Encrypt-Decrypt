#include "enc_client.h"
#include "enc_server.h"

int main(int argc, char* argv[]){

    if (argc < 4) {          // argv[0]    argv[1]       arg[2] arg[3]
        printf("Usage: <program_name> <myplaintext> <key> <port> \n");
        exit(1);
    }

    char* my_plain_text = argv[1];
    char* key_file = argv[2];
    const char* port = argv[3];
    int port_num = atoi(port);

    // Role of the current server where EC stands for encryption server (EC)
    char* role = "EC";

    // Make the key string
    int num_of_key_file_content_char = count_num_of_chars_in_key_file(key_file);
    char* key_int_as_str = num_2_key_str(num_of_key_file_content_char);
    char* key_str = make_key_str(key_file, num_of_key_file_content_char);

    // Make the data string
    int num_of_plain_text_file_content_char = count_num_of_chars_in_plain_text_file(my_plain_text);
    char* plain_text_int_as_str = num_2_plain_text_str(num_of_plain_text_file_content_char);
    char* plain_text_str = make_plain_file_str(my_plain_text, num_of_plain_text_file_content_char);

    // Build a large string to send to the server:
    // <role>#<key_int_as_str>#<key_str_ptr>#<text_file_int_as_str>#<text_file_str_ptr>
    char large_buf[BYTES_OF_DATA_100000] = "";
    strcat(large_buf, role);
    strcat(large_buf, "#");
    strcat(large_buf, key_int_as_str);
    strcat(large_buf, "#");
    strcat(large_buf, key_str);
    strcat(large_buf, "#");
    strcat(large_buf, plain_text_int_as_str);
    strcat(large_buf, "#");
    strcat(large_buf, plain_text_str);

    // Free the pointers after filling the large buffer
    free(plain_text_int_as_str);
    free(plain_text_str);
    free(key_int_as_str);
    free(key_str);

    struct sockaddr_in serverAddress;
    char buffer[BYTES_OF_DATA_100000];

    // Create a socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        fprintf(stderr, "CLIENT: ERROR opening socket");
        exit(1);
    }

    // Set up the server address struct and connect to the server
    setup_address_struct(&serverAddress, port_num, HOSTNAME);
    if (connect(socket_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "Error: could not contact enc_server on port %i\n", port_num);
        exit(1);
    }

    // Clear the buffer array and strcspn calculates the length of the initial segment
    memset(buffer, '\0', sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = '\0';

    // Send the string data to the server
    // << placeholder for the processed string >>char* large_data_result = "data from the client";
    int chars_written = send(socket_fd, large_buf, strlen(large_buf), 0);
    if (chars_written < 0) {
        fprintf(stderr, "CLIENT: ERROR writing to socket");
        exit(1);
    }
    if (chars_written < strlen(buffer)) {
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
        exit(1);
    }

    // Get the return message from the server
    // Clear and reuse the buffer
    memset(buffer, '\0', sizeof(buffer));

    // Read data from the socket, leaving the \0 at the end
    int charsRead = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
    if (charsRead < 0) {
        fprintf(stderr, "CLIENT: ERROR reading for the socket\n");
        close(socket_fd);
        exit(1);
    }

    // Save the encrypted string (buffer) and output that into a file in a directory
    char* the_filename = save_encrypted_str_into_dir(buffer);

    // Malloc a new string to store the data for our server
    char* new_str = malloc(sizeof(char) * strlen(buffer) + 2);
    strcpy(new_str, buffer);
    strcat(new_str, "\n");

    // Open the filename and write the encrypted data into the file
    FILE *fptr = fopen(the_filename, "w");
    if (fptr == NULL) {
        printf("Error opening file\n");
        exit(1);
    } else {
        fputs(new_str, fptr);
    }

    // Sending data to the server
    ssize_t bytes_sent = send(socket_fd, new_str, strlen(buffer), 0);
    if (bytes_sent == IS_NONE) {
        fprintf(stderr, "No bytes were sent\n");
        close(socket_fd);
        exit(1);
    }

    // Free the pointers and close the socket file descriptor
    free(the_filename);
    free(new_str);
    close(socket_fd);

    return 0;
}
