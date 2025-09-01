#include "dec_client.h"

/*
 *  Decryption client reads the data string from a file and sends that data to the decryption server.
 *  The decryption server decrypts the data string and sends the result back to the decryption client.
 *      Usage: ./dec_client encrypted_word.txt key 1234
*/

int main(int argc, char* argv[]) {

    // Factor in leak-check=full
    if (argc < 4) {   //    argv[0]        argv[1]          argv[2]    argv[3]
        printf("Usage: <program_name> <encrypted_data> <key_file> <port>\n");
        exit(1);
    }

    char* encrypted_data_filename = argv[1];
    char* key_filename = argv[2];
    int port_num = atoi(argv[3]);

    // If the key filename or the encrypted data filename does not exist
    if (key_filename == NULL) {
        fprintf(stderr, "The key.txt file does not exist\n");
        exit(1);
    }
    if (encrypted_data_filename == NULL) {
        fprintf(stderr, "The encrypted.txt file does not exist\n");
        exit(1);
    }

    // Role of the current server where DC stands for decryption server (DS)
    char* role = "DS";

    // Make the key string
    int num_of_key_file_content_char = count_num_of_chars_in_key_file(key_filename);
    char* key_int_as_str = num_2_key_str(num_of_key_file_content_char);
    char* read_key_file_data = get_key_file_data(key_filename);
	
    // Make the encrypted data string
    int num_of_encrypted_file_content_char = count_num_of_chars_in_plain_text_file(encrypted_data_filename);
    char* encrypted_data_int_as_str = num_2_plain_text_str(num_of_encrypted_file_content_char);
    char* read_encrypted_data = get_encrypted_data(encrypted_data_filename);

    // Build a large string to send to the server:
    // <role>#<key_int_as_str>#<key_str_ptr>#<text_file_int_as_str>#<text_file_str_ptr>
    char large_buf[BYTES_OF_DATA_100000] = "";
    strcat(large_buf, role);
    strcat(large_buf, "#");
    strcat(large_buf, key_int_as_str);
    strcat(large_buf, "#");
    strcat(large_buf, read_key_file_data);
    strcat(large_buf, "#");
    strcat(large_buf, encrypted_data_int_as_str);
    strcat(large_buf, "#");
    strcat(large_buf, read_encrypted_data);

    // Free the pointers after filling the large buffer
    free(encrypted_data_int_as_str);
    free(read_encrypted_data);
    free(key_int_as_str);
    free(read_key_file_data);

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
        fprintf(stderr, "Error: could not contact enc_server on port_num %i\n", port_num);
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

    // Data received from the server
    printf(">> %s\n", buffer);

    close(socket_fd);

    return 0;
}
