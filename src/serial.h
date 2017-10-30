#ifndef SERIAL_H
#define SERIAL_H
int serialport_init(const char* serialport);
int serialport_write(const signed char* str, int len);
void serial_close();

#endif
