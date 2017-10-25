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

int main(void){
  initServer();
  while(1);
}
