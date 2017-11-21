#include <sys/ioctl.h>
#include <fcntl.h>
#include "comport.h"

bool ComPort::IsOpen(void)
{
	return (-1 != fd);
}
bool ComPort::Open(const string& path)
{
	if( -1 == fd )
	{
		fd = uart_open(path.data());
	}
	return (-1 != fd);
}
bool ComPort::Set(int baud, char parity, int bsize, int stop)
{
	switch(parity)
	{
	case 0:
	case 'n':
	case 'N':
		parity = 'N';
		break;
	case 1:
	case 'o':
	case 'O':
		parity = 'O';
		break;
	case 2:
	case 'e':
	case 'E':
		parity = 'E';
		break;
	}
	return (uart_config(fd, baud, parity, bsize, stop) == 0);
}
int ComPort::Recv(unsigned char *buf, const int len)
{
	return uart_read(fd, (char *)buf, len);
}
int  ComPort::Send(const unsigned char *data, const int len)
{
	return uart_write(fd, (const char*)data, len);
}
bool ComPort::Block(bool b)
{
	if( b )
	{
		return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
	}
	else
	{
		return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
	}
}
bool ComPort::Close(void)
{
	uart_close(fd);
	fd = -1;
}
bool ComPort::SetMode(int mode)
{
	switch(mode)
	{
	case Mode_422:
	case Mode_485:
	case Mode_232:
		break;
	default:
		mode = Mode_485;
	}
	return ioctl(fd, TIOCSERMODE, Mode_485) == 0;
}
