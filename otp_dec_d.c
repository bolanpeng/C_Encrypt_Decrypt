/***********************************************************************
 * This performs exactly like otp_enc_d, except it deciphers the text
 * that is provided by the client.
 * 
 * Syntax: otp_dec_d listening_port
 **********************************************************************/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void decrypt(char *, char *, size_t);
 
 int main(int argc, char **argv) {

  // Create network endpoint with socket()
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }	 


  // Setting up an address
  int port_num = atoi(argv[1]);
  
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port_num);
  server.sin_addr.s_addr = INADDR_ANY;


  // Bind socket to a port
  if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
    perror("bind call failed");
    exit(1);
  }
  
  
  // Start listening for connections, max connection is 5
  if (listen(sockfd, 5) == -1) {
    perror("listen call failed");
    exit(1);
  }
  
  
  // Loop and accept connections
  int client_sockfd, client_type, n;
  struct sockaddr_in client_add;
  socklen_t client_len = sizeof(client_add);
  int server_type = 2;
  size_t msg_length;
  char *msg_buffer, *key_buffer;
  
  while (1) {
    client_sockfd = accept(sockfd, (struct sockaddr *) &client_add, &client_len);
    if (client_sockfd == -1) {
      perror("accept call failed");
    }
    
    /*******************************************************************
     * Fork off to process the client
     * 1. VALIDATE it's the dec client
     * 2. READ ciphertext and key from client
     * 3. DECRYPT the ciphertext with the key (in another function)
     * 4. WRITE back the decrpyted text to client
     ******************************************************************/
    pid_t childPid = fork();
    if (childPid == 0) {
		
      // read the client type 
      n = read(client_sockfd, &client_type, sizeof(int));
      if (n < 0) {
	    perror("ERROR reading client type from socket");
      }
      
      // write the server type
      n = write(client_sockfd, &server_type, sizeof(int));
      if (n < 0) {
	    perror("ERROR writing server type to socket");
      }
      
      // validate client type matches the server type
      if (client_type != server_type) {
	    fprintf(stderr, "ERROR wrong type of client, connection rejected.\n");
	    close(client_sockfd);
	    exit(1);
      }
    
      // read in the length of the message
      n = read(client_sockfd, &msg_length, sizeof(size_t));
      if (n < 0) {
	    perror("ERROR reading file length from socket");
      }
    
      // allocate space for the message and key buffer, they will be the same length
      msg_buffer = malloc(msg_length);
      key_buffer = malloc(msg_length);
    
      // read in the message
      n = read(client_sockfd, msg_buffer, msg_length);
      if (n < 0) {
	    perror("ERROR reading text file from socket");
      }
    
      // read in the key
      n = read(client_sockfd, key_buffer, msg_length);
      if (n < 0) {
	    perror("ERROR reading key file from socket");
      }
    
      // decrypt
      decrypt(msg_buffer, key_buffer, msg_length);
    
      // write
      n = write(client_sockfd, msg_buffer, msg_length);
      if (n < 0) {
	    perror("ERROR writing from client to socket");	
	  }
	  
	  
	  free(msg_buffer);
	  free(key_buffer);
	  close(client_sockfd);
	  exit(0);
    }
    else {
	  continue;  // parent do nothing	
	}
  }
  
  close(sockfd);
  return 0;
}


// deciphers the passed in ciphertext
void decrypt(char *message, char *key, size_t length) {
  int i, a, b;
  
  for (i = 0; i < length; i++) {
	if (message[i] == ' ') {
      message[i] = '@';
    }
	if (key[i] == ' ') {
	  key[i] = '@';	
	}
	  
    // Find the position of the character, n characters away from '@'
    a = message[i] - '@';
    b = key[i] - '@';
    
    // If a - b returns a negative result, then 27 is added to make the number zero or higher
    if (a - b < 0)
      message[i] = ( (a - b + 27) % 27) + '@';
    // If a - b equals 0, which means we are at the beginning of the sequence, that means this character was supposed to be SPACE, which we changed to '@' during encoding
    else if (a - b == 0)
      message[i] = ' ';
    else
      // Message - Key (mod 27), and add the ASCII value of '@' back to find the resulting character
      message[i] = ( (a - b) % 27 ) + '@';
  } 	
}
