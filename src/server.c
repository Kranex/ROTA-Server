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

#include "serial.h"

#define TRUE   1
#define FALSE  0
#define PORT 55455;
int initalised;
pthread_t serverThread;

void * server(void *args){
  int opt = TRUE;
  int master_socket, addrlen, new_socket, client_socket, valread;
  struct sockaddr_in address;

  signed char buffer[128];  //data buffer of 128 signed bytes

  //initialise client socket to 0.
  client_socket = 0;

  //create a master socket
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
    perror("master_socket: master socket failed");
    exit(EXIT_FAILURE);
  }

  //set master socket to allow multiple connections ,
  //this is just a good habit, it will work without this
  // also totally redundant as we only want one controller, however
  // it doesn't really matter.
  if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
  sizeof(opt)) < 0 ) {
    perror("setsockopt: failed to allow multiple connections");
    exit(EXIT_FAILURE);
  }

  //type of socket created. More overly complicated c socket stuff.
  // seriously java is 200x better at this.
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  //bind the socket to localhost:PORT
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
    perror("bind: failed");
    exit(EXIT_FAILURE);
  }

  addrlen = sizeof(address);
  // initalised flag to unblock initServer.
  initalised = 1;

  // no real stop flag. catching terminate signals is a pain.
  // Currently doesn't block the port so whatever.
  while(TRUE) {
    // If something happened on the master socket,
    // then its an incoming connection.
    // i have no idea what will happen if a second connection is
    // made and I'd rather not find out.
    if (client_socket == 0) {
      if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
          perror("accept: new_socket accept failed.");
          exit(EXIT_FAILURE);
      }

        //print the connection information. useful for checking connections.
        printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
        new_socket , inet_ntoa(address.sin_addr) , ntohs
        (address.sin_port));

        client_socket = new_socket;
    }else{
        //Check if it was for closing , and also read the
        //incoming message
        if ((valread = read(client_socket, buffer, 5)) == 0) {
          //client has disconnected , get his details and print
          getpeername(client_socket, (struct sockaddr*)&address , \
          (socklen_t*)&addrlen);
            printf("User disconnected , ip %s , port %d \n" ,
            inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

          //Close the socket and mark as 0 for reuse
          close(client_socket);
          client_socket = 0;
        } else {

          //we're expecting 5 bytes,{undefined, x1, y1, x2 and y2}
          signed char dat[5] = {0};

          // loop through buffer, add 127 to it as serial doesn't like
          // signed bytes >.<
          for(int i = 0; i < valread; i++){
            dat[i] = buffer[i]+127;
          }

          // write to arduino.
          serialportWrite(dat, 5);
        }
      }
    }
  }
  /* initialise server thread */
  void initServer(){
    pthread_create(&serverThread, NULL, server, NULL);
    while(!initalised);
  }
