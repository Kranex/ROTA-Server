#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <pthread.h>

#define TRUE   1
#define FALSE  0

char port[6] = "55455";
int initalised;
pthread_t serverThread;

void * server(void *args){
  int opt = TRUE;
  int master_socket, addrlen, new_socket, client_socket, activity, valread, sd;
  int max_sd;
  struct sockaddr_in address;

  char buffer[128];  //data buffer of 1K

  //set of socket descriptors
  fd_set readfds;
  //initialise all client_socket[] to 0 so not checked
  client_socket = 0;

  //create a master socket
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
    //  perror("socket failed");
    exit(EXIT_FAILURE);
  }

  //set master socket to allow multiple connections ,
  //this is just a good habit, it will work without this
  if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
  sizeof(opt)) < 0 ) {
    //perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  //type of socket created
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(port));

  //bind the socket to localhost port 8888
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
    //perror("bind failed");
    exit(EXIT_FAILURE);
  }
  //  printf("Listener on port %d \n", PORT);

  //try to specify maximum of 3 pending connections for the master socket
  if (listen(master_socket, 3) < 0) {
    //perror("listen");
    exit(EXIT_FAILURE);
  }

  //accept the incoming connection
  addrlen = sizeof(address);
  //  puts("Waiting for connections ...");
  initalised = 1;
  while(TRUE) {
    //If something happened on the master socket ,
    //then its an incoming connection
    if (client_socket == 0) {
      if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
          //perror("accept");
          exit(EXIT_FAILURE);
      }

        //inform user of socket number - used in send and receive commands
        printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
        new_socket , inet_ntoa(address.sin_addr) , ntohs
        (address.sin_port));

        //send new connection greeting message
        //if( send(new_socket, message, strlen(message), 0) != strlen(message)) {
        //  //perror("send");
        //  }

        //  puts("Welcome message sent successfully");
        client_socket = new_socket;
          //snprintf(names[i], 16, "%s%d", "User", users);
          //printf("Adding to list of sockets as %d\n" , i);
    }else{
        //Check if it was for closing , and also read the
        //incoming message
        if ((valread = read(client_socket, buffer, 5)) == 0) {
          //Somebody disconnected , get his details and print
          getpeername(client_socket, (struct sockaddr*)&address , \
          (socklen_t*)&addrlen);
            printf("User disconnected , ip %s , port %d \n" ,
            inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

          //Close the socket and mark as 0 in list for reuse
          close(client_socket);
          client_socket = 0;
        } else {
          //Echo back the message that came in
          //set the string terminating NULL byte on the end
          //of the data read
          buffer[valread] = '\0';
          //char msg[128+16];
          printf("%d", buffer[1]);
          for(int i = 2; i < valread; i++){
            printf(" %d", buffer[i]);
          }
          printf("\n");
          //snprintf(msg, 128+16, "<%s> %s", names[i], buffer);
          //send(client_socket, msg , strlen(msg) , 0 );
        }
      }
    }
  }
  void initServer(){
    pthread_create(&serverThread, NULL, server, NULL);
    while(!initalised);
  }
