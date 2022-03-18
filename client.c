#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

int open_TCP(char *name, int port) {
  // The client's socket file descriptor.
  int socketfd;

  struct sockaddr_in serveraddr;

  printf("Client is creating a socket.\n");

  // Step 1: Open a TCP stream socket
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  } 

  if (name == NULL) { 
    printf("Improper host\n");
    return -1;
  }

  // set structure
  bzero((char *)&serveraddr, sizeof(serveraddr));
  // Record family 
  serveraddr.sin_family = AF_INET;
  // Convert the port
  serveraddr.sin_port = htons(port);
  // Copy the IP address to server
  inet_pton(serveraddr.sin_family, name, &serveraddr.sin_addr.s_addr);

  // Step 2: Connect to the remote server on the specified port number
  if (connect(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
    return -1;
  }
  
  printf("Client connected.\n");

  // Return the connected file descriptor.
  return socketfd;
}

int main(int argc, char *argv[]) {
  int socketfd;
  int port;
  char Id[256];
  char *hostIP;
  char prob[256];
  char ans[256];
  
  // initialize arrays
  memset(Id, 0, 256); memset(prob, 0, 256); memset(ans, 0, 256);

  // check arguments 
  if (argc != 4) {
    fprintf(stderr, "usage: %s <Identification> <Port> <Host>\n", argv[0]);
    exit(0);
  } 
  
  // set arguments
  hostIP = argv[3];
  port = atoi(argv[2]);
  strcpy(Id, "cs230 HELLO "); strcat(Id, argv[1]); strcat(Id, "\n");
 
  // open socket & connect
  socketfd = open_TCP(hostIP, port);

  // check socket
  if (socketfd < 0) {
    printf("Socket did not open properly\n");
    exit(1);
  }

  // Step 3: Send your SPIRE ID to the server in the following format: NETID@umass.edu
  ssize_t bs = 0;
  ssize_t br = 0;
  bs = send(socketfd, Id, strlen(Id), 0);
  printf("bytes: %ld, %s", bs, Id);

  // Step 4 & 5: Receive problems, send solutions
  while (1) {
    int n1 = 0, n2 = 0, result = 0;
    char *token;
    char copy[256]; memset(copy, 0, 256);

    // get problem
    br = recv(socketfd, prob, 256, 0);
    
    // print problem
    printf("bytes: %ld, %s", br, prob);

    // get answer
    // separate string into elements
    strcpy(copy, prob);
    token = strtok(copy, " ");
    token = strtok(NULL, " ");

    // check if computations complete base on status
    if (strcmp(token, "STATUS") != 0) {
      break;
    }
    token = strtok(NULL, " ");
    n1 = atoi(token);
    token = strtok(NULL, " ");

    // get operator
    int i = 0;
    if (strcmp(token,"+") == 0) {
      i = 1;
    } else if (strcmp(token,"-") == 0) {
      i = 2;
    } else if (strcmp(token,"*") == 0) {
      i = 3;
    } else if (strcmp(token,"/") == 0) {
      i = 4;
    } else if (strcmp(token,"%") == 0) {
      i = 5;
    }
    
    // compute result
    token = strtok(NULL, " ");
    n2 = atoi(token); 
    switch(i) {
      case 1 :
        result = (n1) + (n2);
        break;
      case 2 :
        result = (n1) - (n2);
        break;
      case 3 :
        result = (n1) * (n2);
        break;
      case 4 :
        result = (n1) / (n2);
        break;
      case 5 :
        result = (n1) % (n2);
        break;
      default :
        printf("Invalid operator: %s.\n", token);
    }
    
    // convert number to string
    char str[256]; memset(str, 0, 256);
    sprintf(str, "%d", result);
    strcpy(ans, "cs230 "); strcat(ans, str); strcat(ans, "\n");
   
    // send answer to server
    bs = send(socketfd, ans, strlen(ans), 0);
    printf("bytes: %ld, %s\n", bs, ans);
  }

  // print flag
  printf("%s\n", prob);
  
  // close & exit
  close(socketfd);
  exit(0);
  return 0;
}
