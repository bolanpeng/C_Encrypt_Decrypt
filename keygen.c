/***********************************************************************
 * This program creates a key file of specified length. The characters 
 * in the file generated will be any of the 27 allowed characters, 
 * generated using the standard UNIX randomization methods.
 * 
 * Syntax: keygen keylength
 **********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {
  
  // The 27 allowed characters for the key
  char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  
  // Length of the key
  int length = atoi(argv[1]);
  
  char *str = malloc(sizeof(char) * (length+1) );
  
  int i, key;
  srand(time(NULL));
  
  // Get a random character from the character set, and assign it to str
  for(i = 0; i < length; i++){
    key = rand() % 27;
    str[i] = charset[key];
  }
  str[i] = '\n';
  
  printf("%s", str);
  free(str);
  return 0;	
}
