#ifndef _INCLUDE_SERIAL_H_
#define _INCLUDE_SERIAL_H_

void serial_configure_baud_rate(unsigned short com, unsigned short divisor);
void serial_configure_line(unsigned short com);
void serial_configure_buffer(unsigned short com);
void serial_configure_modem(unsigned short com);
int serial_is_transmit_fifo_empty(unsigned short com);
void serial_write(char *buf, unsigned int len);

#endif