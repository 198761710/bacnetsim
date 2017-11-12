#include <stdio.h>
#include "comport.h"


int main(void)
{
	ComPort com;

	com.Open("/dev/ttyS0");
	com.SetMode(Mode_485);
	com.Set(9600, 'n', 8, 1);
	com.Block(1);

	while( com.IsOpen() )
	{
		int len = 0;
		char buf[32] = {0};

		com.Send("\x01\x03\x00\x00\x00\x02\xc4\x0b", 8);
		usleep(10*1000);
		len = com.Recv(buf, sizeof(buf));
		for(int i = 0; i < len; )
		{
			printf("%02X ", 0xff & buf[i]);
			if( ++i >= len )
			{
				printf("\n");
			}
		}
		usleep(10*1000);
	}//while
	return 0;
}
