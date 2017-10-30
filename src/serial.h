#ifndef SERIAL_H
#define SERIAL_H
int serialport_init(const char* serialport);
int serialport_write(const char* str);
void serial_close();

#endif
