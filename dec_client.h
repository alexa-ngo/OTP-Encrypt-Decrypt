#ifndef DEC_CLIENT
#define DEC_CLIENT

// This is the declaration of the decryption client.

#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define BYTES_OF_DATA_100000 100000
#define DOES_NOT_EXIST -1
#define IS_NONE -1
#define HOSTNAME "127.0.0.1"
#define MAX_5_NUM_OF_RUNNING_PROCESSES 5
#define NULL_TERM_CHAR 1

void bind_to_port(int socket, int port);
int catch_signal(int sig, void (*handler)(int));
int count_num_of_chars_in_key_file(char* key_file);
int count_num_of_chars_in_plain_text_file(char* plain_text_file);
char* get_key_file_data(char* key_filename);
char* get_encrypted_data(char* encrypted_data);
char* num_2_key_str(int num);
char* num_2_plain_text_str(int num);
int open_listener_socket(void);
void setup_address_struct(struct sockaddr_in* address, int port_number, char* hostname);


#endif
