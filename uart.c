/********************************
* Module:       UART            *
* Author:       Josh Chen       *
* Date:         2016/03/23      *
********************************/

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#include "uart.h"

int uart_open(struct uart *uart)
{
	struct termios old_tio, new_tio;
	tcflag_t cflag = CS8 | CLOCAL| CREAD;
	int oflag = O_RDWR | O_NOCTTY;

	if (uart->blocking_mode == 0)
		oflag |= O_NONBLOCK;

	uart->uart_fd = open(uart->uart_path, oflag);
	if (uart->uart_fd < 0)
		return 0;

	tcgetattr(uart->uart_fd, &old_tio);
	memset(&new_tio, 0, sizeof(new_tio));

	switch(uart->baud_rate) {
		case 1200:
			cflag |= B1200;		break;
		case 2400:
			cflag |= B2400;		break;
		case 4800:
			cflag |= B4800;		break;
		case 9600:
			cflag |= B9600;		break;
		case 19200:
			cflag |= B19200;	break;
		case 38400:
			cflag |= B38400;	break;
		case 57600:
			cflag |= B57600;	break;
		case 115200:
			cflag |= B115200;	break;
		default:
			close(uart->uart_fd);
			return 0;
	}

	if (uart->parity_check > 0)
	{
		cflag |= PARENB;
		if (uart->parity_check == 1)
			cflag |= PARODD;
	}

	if (uart->two_stop_bit == 1)
		cflag |= CSTOPB;

	new_tio.c_cflag = cflag;
	new_tio.c_iflag = IGNPAR;
	new_tio.c_oflag = 0;
	new_tio.c_lflag = 0;

	if (uart->blocking_mode == 0)
		new_tio.c_cc[VMIN] = 0;
	else
		new_tio.c_cc[VMIN] = 1;

	new_tio.c_cc[VTIME] = 0;
	tcflush(uart->uart_fd, TCIFLUSH);
	tcsetattr(uart->uart_fd, TCSANOW, &new_tio);

	return 1;
}

int uart_close(struct uart *uart)
{
	if (uart->uart_fd)
	{
		close(uart->uart_fd);
		uart->uart_fd = 0;
	}
	return 1;
}

int uart_recv(struct uart *uart, char *data, unsigned int data_len)
{
	unsigned int read_len = 0;
	unsigned int bit_per_char = 0, us_per_char = 0;

	if (uart->uart_fd == 0)
		return 0;

	if (uart->blocking_mode == 1)	//blocking mode
	{
		if (read(uart->uart_fd, data+read_len, 1) > 0)
		{
			if (data[read_len] != 0)
				read_len++;
		}
	}
	else	//non-blocking mode
	{
		bit_per_char = 8	//8 bit data
			+ ((uart->parity_check == 0) ? 0 : 1)	//0 or 1 parity check bit
			+ ((uart->two_stop_bit == 0) ? 1 : 2);	//1 or 2 stop bit
		us_per_char = (bit_per_char * 1000 * 1000 / uart->baud_rate) + 5;	//add 5 micro seconds buffer
		while (read(uart->uart_fd, data+read_len, 1) > 0)
		{
			if (data[read_len] != 0)
				read_len++;

			if (read_len >= (data_len - 1))
				break;

			usleep(us_per_char);	//wait next char arrival
		}
	}

	if (read_len <= 0)
		return 0;

	return read_len;
}

int uart_send(struct uart *uart, char *data, unsigned int data_len)
{
	int send_len = 0;
	if (uart->uart_fd == 0)
		return 0;

	send_len = write(uart->uart_fd, data, data_len);
	if (send_len != data_len)
		return 0;

	return 1;
}
