#include "dec_server.h"

/*
*      This is the encryption server that encrypts a string from the client
*
*/

int find_key_size(char* key_str) {

    int result = strlen(key_str) - 1;
    if (result == -1) {
        printf("There isn't data sent from the client\n");
        exit(1);
    }
    return result;
}

char* decrypt_data(int key_size, char* key_file_str, int data_size, char* encrypted_data_str) {

    int idx_of_data = 0;
    int idx_of_key = 0;

    // Stores the decrypted string
    char* output_malloced_decrypted_string = malloc(sizeof(char) * data_size + 1);

    for (int i = 0; i < data_size - 1; i++) {
        int data_value = encrypted_data_str[i] - LETTER_OF_A;
        idx_of_key = idx_of_data % key_size;

        int key_value = key_file_str[idx_of_key] - LETTER_OF_A;
        //printf("Here is the key_value: %i\n", key_value);
        int dec_char_value = data_value - key_value;

        if (dec_char_value < 0) {
            dec_char_value = dec_char_value + TOTAL_CHAR_OF_ALPHABET;
        }

        char c = (char)(dec_char_value + LETTER_OF_A);
        output_malloced_decrypted_string[idx_of_data] = c;

        if (idx_of_data <= i) {
            idx_of_data++;
        }
    }

    // Add a null terminating character
    *(output_malloced_decrypted_string + idx_of_data) = '\0';

    return output_malloced_decrypted_string;
}

// Create a streaming socket
int open_listener_socket(void) {
    int streaming_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (streaming_socket == -1) {
        fprintf(stderr, "Can't open the socket");
        exit(1);
    }
    return streaming_socket;
}

// Bind to a port
void bind_to_port(int socket, int port) {
    struct sockaddr_in the_socket;
    the_socket.sin_family = PF_INET;
    the_socket.sin_port = (in_port_t)htons(port);
    the_socket.sin_addr.s_addr = htonl(INADDR_ANY);

    // Reuse the socket to restart the server without problem
    int reuse = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) {
        fprintf(stderr, "Can't set the reuse option on the socket");
        exit(1);
    }

    int c = bind(socket, (struct sockaddr *) &the_socket, sizeof(the_socket));
    if (c == -1) {
        fprintf(stderr, "Can't bind to socket");
        exit(1);
    }
}

// Catch the signal
int catch_signal(int sig, void (*handler)(int)) {

    struct sigaction action;
    action.sa_handler = handler;
    // Use an empty mask
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    return sigaction (sig, &action, NULL);
}

// Encrypts the message sent from the client based on data length
char* start_encryption(int key_length_as_int, char* key_str, int plain_text_length_as_int, char* plain_text_string) {

    char* output_malloced_data_str = malloc(sizeof(char) * plain_text_length_as_int + 1);
    int idxOfData = 0;
    int idxOfKey = 0;

    for (int i = 0; i < plain_text_length_as_int - 1; i++) {

        int dataValue = plain_text_string[i] - LETTER_OF_A;
        idxOfKey = idxOfData % key_length_as_int;
        int keyValue = key_str[idxOfKey] - LETTER_OF_A;
        int encCharValue = dataValue + keyValue;

        // If the encrypted value is greater than 26, roll over the character
        if (encCharValue >= TOTAL_CHAR_OF_ALPHABET) {
            encCharValue = encCharValue % TOTAL_CHAR_OF_ALPHABET;
        }

        char c = (char)(encCharValue + LETTER_OF_A);
        output_malloced_data_str[idxOfData] = c;

        if (idxOfData < plain_text_length_as_int - 1) {
            idxOfData++;
        }
    }

    // Add a null terminating character
    *(output_malloced_data_str + idxOfData) = '\0';

    return output_malloced_data_str;
}

// Kills the process
void kill_the_process(void) {
    printf("Goodbye, I am ending this process now...\n");
    exit(1);
}

// Receive the data from a socket
int read_in(int socket, char* buf, int len) {

    char* buffer = buf;
    int slen = len;

    // Must receive at least once to start the while loop
    int received_data = recv(socket, buffer, slen, 0);
    while ((received_data > 0) && (buffer[received_data-1] != '\n')) {
        buffer += received_data;
        slen -= received_data;
        received_data = recv(socket, buffer, slen, 0);
    }

    if (received_data < 0) {
        return received_data;
        // Send back an empty string if there is an empty string
    } else if (received_data == 0) {
        buf[0] = '\0';
    } else {
        buffer[received_data - 1] = '\0';
    }
    return len - slen;
}

// Check for bad characters where false has the value of 0
int is_bad_plain_text_char(char* plain_text_string) {

    int is_bad_char = false;
    char* special_chars = "$*!(#*djs8301";
    int idx = 0;

    while (plain_text_string[idx]) {
        char each_char = plain_text_string[idx];
        idx++;
        if (strchr(special_chars, each_char) != NULL) {
            is_bad_char = true;
            break;
        }
    }
    return is_bad_char;
}
