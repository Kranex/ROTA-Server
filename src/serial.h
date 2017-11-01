#ifndef SERIAL_H
#define SERIAL_H
int serialportInit(const char* serialport);
int serialportWrite(const signed char* str, int len);
void serialClose();

#endif
