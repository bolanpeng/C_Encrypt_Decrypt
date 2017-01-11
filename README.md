#Definitions

**Plaintext** is information that needs to be encrypted and protected. It is human readable.

**Ciphertext** is the term for the plaintext after it has been encrypted by the program. Ciphertext is not human readable.

**Key** is a random sequence of characters that will be used to convert Plaintext to Ciphertext, and back again. It must 
not be re-used.


#How to Run

Run **./compileall** in bash shell(recommended) to compile all five programs. 

**keygen _keylength_** is used to generate a new random key. It outputs to *stdout*.

**otp_enc_d _listening_port_** runs in the background to perform the actual encoding. It is able to support up to five 
concurrent socket connections.

**otp_enc _plaintext key port_** connects to *opt_enc_d*, and asks it to perform a one-time-pad style encryption. When 
*opt_enc* receives the ciphertext back from *opt_enc_d*, it outputs to *stdout*.

**otp_dec_d _listening_port_** performs exactly like *opt_enc_d*, in syntax and usage, except it decodes the ciphertext 
it is given. 

**otp_dec _ciphertext key port_** connects to *opt_dec_d* and asks it to decrypt ciphertext. 



#Example

Following is an example of usage, if the program is being tested from the command line:

```
$ cat plaintext1
THE RED GOOSE FLIES AT MIDNIGHT
$ otp_enc_d 57171 &
$ opt_dec_d 57172 &
$ keygen 1024 > mykey
$ opt_enc plaintext1 mykey 57171 > ciphertext1
$ cat ciphertext1
GU WIRGEWOMGRIFOENBYIWUG T WOFL
$ otp_dec ciphertext1 mykey 57172 > plaintext1_a
$ cat plaintext1_a
THE RED GOOSE FLIES AT MIDNIGHT
```
