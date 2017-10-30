#ifndef SERIAL_H
#define SERIAL_H
void serialport_init(const char* serialport);
int serialport_write(const signed char* str, int len);
void serial_close();

#endif
