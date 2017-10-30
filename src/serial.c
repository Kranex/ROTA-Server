/*

* Arduino-serial

* --------------

*

* A simple command-line example program showing how a computer can

* communicate with an Arduino board. Works on any POSIX system (Mac/Unix/PC)

*

*

* Compile with something like:

* gcc -o arduino-serial arduino-serial.c

* Created 5 December 2006

* Copyleft (c) 2006, Tod E. Kurt, tod@todbot.com

 Updated 8 December 2006:
*  Justin McBride discoevered B14400 & B28800 aren't in Linux's termios.h.
*  I've included his patch, but commented out for now.  One really needs a
*  real make system when doing cross-platform C and I wanted to avoid that
*  for this little program. Those baudrates aren't used much anyway. :)

Updated 26 December 2007:
*  Added ability to specify a delay (so you can wait for Arduino Diecimila)
*  Added ability to send a binary byte number
*/


#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

#include "serial.h"
int fd = 0;

void serial_close(){
  close(fd);
}

int serialport_write(const signed char* str, int len) {
  if(fd==-1) return -1;
  int n = write(fd, str, len);
  return ( n!=len)  ?  (-1) : (n);
}

// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error

void serialport_init(const char* serialport) {
  struct termios toptions;
  //fprintf(stderr,"init_serialport: opening port %s @ %d bps\n",
  //        serialport,baud);
  //fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);

  fd = open(serialport, O_RDWR | O_NOCTTY);

  if (fd == -1)  {
    perror("init_serialport: Unable to open port ");
    return -1;
  }

  if (tcgetattr(fd, &toptions) < 0) {
    perror("init_serialport: Couldn't get term attributes");
    return -1;
  }

  cfsetispeed(&toptions, B9600);
  cfsetospeed(&toptions, B9600);

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

  // see: http://unixwiz.net/techtips/termios-vmin-vtime.html

  toptions.c_cc[VMIN]  = 0;
  toptions.c_cc[VTIME] = 20;



  if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
    perror("init_serialport: Couldn't set term attributes");\
    fd = -1;
  }
}
