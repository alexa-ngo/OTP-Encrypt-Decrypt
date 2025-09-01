#include "dec_client.h"

// Count the number of characters in the key file
int count_num_of_chars_in_key_file(char* key_file) {

    int count = 0;
    char each_char;
    FILE* fp = fopen(key_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: cannot open the file");
        exit(1);
    }
    while ((each_char = fgetc(fp)) != EOF) {
        count++;
    }
    fclose(fp);

    return count;
}

// Count the number of characters in the plain text file
int count_num_of_chars_in_plain_text_file(char* plain_text_file) {

    int count = 0;
    char each_char;
    FILE* fp = fopen(plain_text_file, "r");

    if (fp == NULL) {
        fprintf(stderr, "ERROR: cannot open the file");
        exit(1);
    }
    while ((each_char = fgetc(fp)) != EOF) {
        count++;
    }
    fclose(fp);

    return count;
}

// Make a pointer to a string from the key file
char* make_key_str(char* key_file, int num_of_key_file_content_char) {

    char* key_str = malloc(sizeof(char) * num_of_key_file_content_char);
    char each_char;
    int idx = 0;

    FILE* fp = fopen(key_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: cannot open the file");
        exit(1);
    }

    while ((each_char = fgetc(fp)) != EOF) {
        *(key_str + idx) = each_char;
        idx++;
    }

    // Add a null terminating character
    *(key_str + idx - NULL_TERM_CHAR) = '\0';

    fclose(fp);
    return key_str;
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

// Get the data from the encrypted file
char* get_encrypted_data(char* encrypted_data_filename) {
    char* encrypted_data = malloc(sizeof(char) * BYTES_OF_DATA_100000);
    FILE* fp = fopen(encrypted_data_filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: ");
        exit(1);
    }
    int idx = 0;
    char each_char;
    while ((each_char = fgetc(fp)) != EOF) {
        *(encrypted_data + idx) = each_char;
        idx++;
    }
    *(encrypted_data + idx - NULL_TERM_CHAR) = '\0';
    fclose(fp);
    return encrypted_data;
}

// Get from key file
char* get_key_file_data(char* key_filename) {
    char* key_data = malloc(sizeof(char) * BYTES_OF_DATA_100000);
    FILE* fp = fopen(key_filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: ");
        exit(1);
    }
    int idx = 0;
    char each_char;
    while ((each_char = fgetc(fp)) != EOF) {
        *(key_data + idx) = each_char;
        idx++;
    }
    *(key_data + idx - NULL_TERM_CHAR) = '\0';
    fclose(fp);
    return key_data;
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


void setup_address_struct(struct sockaddr_in* address, int port_number, char* hostname) {

    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    address->sin_port = htons(port_number);

    // Get the DNS entry for this hostname
    struct hostent* hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(1);
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_dr
    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}


// Num2Str: Convert a number to a string
// Memory is malloc'd and also freed in here
// so don't use strdup
char* num_2_key_str(int num) {
    int idx = 0;
    char* buffer = malloc(sizeof(char) * 7);		// just enough for 7 digits

    int quotient = num;
    while (quotient > 0) {
        int digit = quotient % 10;

        char v = '0' + digit;
        buffer[idx] = v;
        idx++;
        quotient = quotient / 10;
    }
    buffer[idx] = '\0';		// don't forget the null terminating character

    // Reverse the string because the "number" is now backwards.
    int buffer_length = idx;
    for (int i = 0, j = buffer_length -1; i < j; i++, j--) {
        char t = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = t;
    }
    return buffer;
}

// Convert the integer of characters for plain text to a string
char* num_2_plain_text_str(int num) {
    int idx = 0;
    char* buffer = malloc(sizeof(char) * 7);		// just enough for 7 digits

    int quotient = num;
    while (quotient > 0) {
        int digit = quotient % 10;
        char v = '0' + digit;
        buffer[idx] = v;
        idx++;
        quotient = quotient / 10;
    }
    buffer[idx] = '\0';		// don't forget the null terminating character

    // Reverse the string because the "number" is now backwards.
    int buffer_length = idx;
    for (int i = 0, j = buffer_length -1; i < j; i++, j--) {
        char t = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = t;
    }
    return buffer;
}

