
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <getopt.h>

#include "serial.h"

#define BAUD B9600
int fd = 0;

void serialClose(){
  close(fd);
}

int serialportWrite(const signed char* str, int len) {
  if(fd==-1) return -1;
  int n = write(fd, str, len);
  return ( n!=len)  ?  (-1) : (n);
}

// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// connects to that port with speed BAUD and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns 1 if sucessful, or -1 on error
int serialportInit(const char* serialport) {
  struct termios toptions;

  fd = open(serialport, O_RDWR | O_NOCTTY); // open port.

  if (fd == -1)  {
    perror("initSerialport: Unable to open port ");
    return -1;
  }

  // get term attributes. whatever those are.
  if (tcgetattr(fd, &toptions) < 0) {
    perror("initSerialport: Couldn't get term attributes");
    return -1;
  }

  // set baud rate.
  cfsetispeed(&toptions, BAUD);
  cfsetospeed(&toptions, BAUD);

  // honestly don't know what any of this does, but I'd rather not change it.
  // 8N1
  toptions.c_cflag &= ~PARENB;
  toptions.c_cflag &= ~CSTOPB;
  toptions.c_cflag &= ~CSIZE;
  toptions.c_cflag |= CS8;

  // no flow control
  toptions.c_cflag &= ~CRTSCTS;
  toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
  toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
  toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
  toptions.c_oflag &= ~OPOST; // make raw

  toptions.c_cc[VMIN]  = 0;
  toptions.c_cc[VTIME] = 20;


  // set attributes
  if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
    perror("initSerialport: Couldn't set term attributes");\
    fd = -1;
  }
  return 1;
}
