#ifndef ENC_CLIENT
#define ENC_CLIENT

// This is the declaration of the encryption client.

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BYTES_64_OF_FILENAME 64
#define BYTES_OF_DATA_100000 100000
#define BYTES_TO_BE_READ_100000 100000
#define HOSTNAME "127.0.0.1"
#define IS_NONE -1
#define NULL_TERM_CHAR 1
#define UNNECESSARY_FIRST_NULL_CHAR 1

int count_num_of_chars_in_key_file(char* key_file);
int count_num_of_chars_in_plain_text_file(char* plain_text_file);
char* make_key_str(char* key_file, int num_of_file_chars);
char* make_plain_file_str(char* my_file, int num_of_file_chars);
char* num_2_key_str(int num);
char* num_2_plain_text_str(int num);
char* read_contents_of_key_file(char* key_file);
char* read_contents_of_text_file(char* my_plain_text);
char* save_encrypted_str_into_dir(char* encrypted_str);
void setup_address_struct(struct sockaddr_in* address, int port_number, char* hostname);

#endif