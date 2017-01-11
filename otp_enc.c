/***********************************************************************
 * This is the client, it connects to otp_enc_d, and asks it to perform
 * encryption.
 * 
 * Syntax: otp_enc plaintext key port
 **********************************************************************/

#include <ctype.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int sizeOfFile(char *);

int main(int argc, char **argv) {
  
  // Read in the plaintext, return the length of the file
  size_t text_length = sizeOfFile(argv[1]);
  
  // Read in the key, return the length of the file
  size_t key_length = sizeOfFile(argv[2]);
  
  // Compare the length of the two files, if key is shorter than plaintext, exit with error 1
  if (key_length < text_length) {
	fprintf(stderr, "ERROR: key '%s' is too short.\n", argv[2]);
	exit(1);  
  }
  
  // Create buffers to store the file data, up to the size of the text file
  char *msg_buffer = malloc(text_length);
  char *key_buffer = malloc(text_length);
  
  // Read plaintext into buffer
  int msg_fp, n;
  msg_fp = open(argv[1], O_RDONLY);
  n = read(msg_fp, msg_buffer, text_length);
  if (n == -1) {
    perror("ERROR reading the textfile");  
  }
  
  // Read key into buffer
  int key_fp;
  key_fp = open(argv[2], O_RDONLY);
  n = read(key_fp, key_buffer, text_length);
  if (n == -1) {
    perror("ERROR reading the key file");  
  }
  
  close(msg_fp);
  close(key_fp);
  
  
  // Create the socket endpoint
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }	
  
  
  // Setting up the address
  struct hostent *server_ip_add;
  server_ip_add = gethostbyname("localhost");
  if (server_ip_add == NULL) {
    fprintf(stderr, "could not resolve server host name\n");
    exit(1);
  }
  
  int port_num = atoi(argv[3]);
  
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port_num);
  memcpy(&server.sin_addr, server_ip_add->h_addr, server_ip_add->h_length);
 
 
  // Connect the socket to the server, if connection fails, exit with error 2
  if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
    fprintf(stderr, "ERROR: could not connect on port %d\n", port_num);
    exit(2);
  }

  // Verify the client type matches the server type
  int client_type = 1;
  int server_type;
  
  // Write client type
  n = write(sockfd, &client_type, sizeof(int));
  if (n < 0) {
    perror("ERROR writing client type to socket");  
  }
  
  // Read server type
  n = read(sockfd, &server_type, sizeof(int));
  if (n < 0) {
    perror("ERROR reading server type from socket");  
  }
  
  if (client_type != server_type) {
	close(sockfd);
	exit(1);
  }
  
  /*********************************************************************
   * Write the data in the plaintext and key file to the server
   * 1. Write the length of the plaintext
   * 2. Write the plaintext
   * 3. Write the key
   ********************************************************************/ 
  // length of the plaintext
  n = write(sockfd, &text_length, sizeof(size_t));
  if (n < 0) {
    perror("ERROR writing file length to socket");
  }
  
  // plaintext
  n = write(sockfd, msg_buffer, text_length);
  if (n < 0) {
    perror("ERROR writing text file to socket");
  }
  
  // key
  n = write(sockfd, key_buffer, text_length);
  if (n < 0) {
    perror("ERROR writing key file to socket");
  }
  
  // Receive the encrypted text from the server, and output to stdout
  n = read(sockfd, msg_buffer, text_length);
  if (n < 0) {
    perror("ERROR reading text file from socket");
  }
  write(1, msg_buffer, text_length);
  printf("\n");
  
  close(sockfd);
  return 0;	
}

// Check the input file for bad characters, and keep track of the file's length
int sizeOfFile(char * arg) {
  FILE *fp = fopen(arg, "r");
  char ch;
  int length = 0;
  
  // Read the file character by character
  while ((ch = fgetc(fp)) != '\n') {
	// If it's a bad character, exit with error 1
	if (ch == '$') {
      fprintf(stderr, "ERROR: Bad character in %s\n", arg);
      exit(1);
    }
    length++;
  }
  
  fclose(fp);
  return length;
}
