#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

struct uart {
	int uart_fd;
	char *uart_path;
	unsigned int baud_rate;		//UART baudrate: 1200, 2400 4800, 9600, 19200, 38400, 57600, 115200
	unsigned char parity_check;	//Non Parity = 0, Odd Parity = 1, Even Parity = 2
	unsigned char two_stop_bit;	//1 Stop Bit = 0, 2 Stop Bit = 1
	unsigned char blocking_mode;	//Non-Blocking = 0, Blocking = 1
};

int uart_open(struct uart *uart);
int uart_close(struct uart *uart);
int uart_recv(struct uart *uart, char *data, unsigned int data_len);	//return recv data number
int uart_send(struct uart *uart, char *data, unsigned int data_len);	//return send data number

#ifdef __cplusplus
}
#endif

#endif
