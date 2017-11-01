/* Whats getting sent where.
            ANDROID -- the Client.
            |   ^
joysticks   |   |   Video (bitmaps)
            v   |
            RPI     -- THIS Program.
            |
joysticks   |
            v
            ARDUINO -- The other end of serial.
*/

#include "server.h"
#include "serial.h"

int main(void){
  serialportInit("/dev/ttyACM0");
  initServer();
  while(1); // keep program running because of server running in thread.
  serialClose(); // totally redunant but if I decide to check for
                 // termination later this Would be nice call.
                 // probably want a similar thing for the server.
}
