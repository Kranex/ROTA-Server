/*
            ANDROID -- client
            |   ^
joysticks   |   |   Video (bitmaps)
            v   |
            RPI     -- server
            |
joysticks   |
            v
            ARDUINO -- program



*/

#include "server.h"
#include "serial.h"

int main(void){
  serialport_init("/dev/ttyACM0");

  initServer();
  while(1);
  serial_close();
}
