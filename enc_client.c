#include "enc_client.h"

// Save the encrypted string into a file in the current directory
char* save_encrypted_str_into_dir(char* encrypted_str) {

    pid_t process_id;
    process_id = getpid();
    char* encrypted_word_label = "encrypted_word_";
    char* pid_str = num_2_key_str(process_id);
    char* dot_txt = ".txt";

    char unique_filename[100] = "";
    sprintf(unique_filename, "%s%s%s", encrypted_word_label, pid_str, dot_txt);

    FILE* fp = fopen(unique_filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: ");
        exit(1);
    }

    // Write the formatted data to the file
    fprintf(stderr, "%s\n", encrypted_str);
    free(pid_str);
    fclose(fp);

    char* the_filename = strdup(unique_filename);
    return the_filename;
}

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


// Make a pointer to a string from the plain file
char* make_plain_file_str(char* my_plain_text, int num_of_key_file_content_char) {

    char* plain_file_str = malloc(sizeof(char) * num_of_key_file_content_char);

    char each_char;
    int idx = 0;

    FILE* fp = fopen(my_plain_text, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: cannot open the file");
        exit(1);
    }

    while ((each_char = fgetc(fp)) != EOF) {
        *(plain_file_str + idx) = each_char;
        idx++;
    }

    // Add a null terminating character
    *(plain_file_str + idx - NULL_TERM_CHAR) = '\0';

    fclose(fp);
    return plain_file_str;
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

// Read the contents of the key file and stores the data in a buffer
char* read_contents_of_key_file(char* key_file) {

    // Read the contents of the key file
    FILE* key_file_ptr = fopen(key_file, "r");
    char data_to_be_read[BYTES_OF_DATA_100000];
    if (key_file_ptr == NULL) {
        fprintf(stderr, "ERROR: cannot open the file");
        exit(1);
    } else {
        while (fgets(data_to_be_read, BYTES_TO_BE_READ_100000, key_file_ptr) != NULL); {
            // Read the data from the key file and place the data into a buffer
        }
    }
    fclose(key_file_ptr);

    // Copy the key pointer into a buffer
    char* key_data_buffer = malloc(sizeof(char) * BYTES_OF_DATA_100000);
    key_data_buffer = strdup(data_to_be_read);
    return key_data_buffer;
}

// Read the contents of the text file and stores the data in a buffer
char* read_contents_of_text_file(char* my_plain_text) {

    // Read the contents of the text file
    FILE* text_file_ptr = fopen(my_plain_text, "r");
    char data_to_be_read[BYTES_OF_DATA_100000];
    if (text_file_ptr == NULL) {
        fprintf(stderr, "ERROR: cannot open the file");
        exit(1);
    } else {
        while (fgets(data_to_be_read, BYTES_TO_BE_READ_100000, text_file_ptr) != NULL); {
            // Read the data from the text file and place the data into a buffer
        }
    }
    fclose(text_file_ptr);

    // Copy the data to a pointer
    char* text_file_data_str = strdup(data_to_be_read);
    return text_file_data_str;
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
    // Copy the first IP addresss from the DNS entry to sin_addr.s_dr
    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}
