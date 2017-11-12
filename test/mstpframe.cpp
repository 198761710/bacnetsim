#include <stdio.h>
#include "mstpframe.h"


void frame1(void)
{
	unsigned char buf[] = 
	{
		0x55, 0xFF, 0x06, 0xFF, 0x08, 0x00, 0x13, 0xD8, 
		0x01, 0x20, 0xFF, 0xFF, 0x00, 0xFF, 0x10, 0x00, 
		0xC4, 0x02, 0x00, 0x00, 0x08, 0x21, 0xCE, 0x91, 
		0x03, 0x21, 0x11, 0xE4, 0x41
	};
	MstpFrame frame;
	frame.push(buf, sizeof(buf));
	frame.SetFrameHeadCrc(frame.CalcHeadCrc());
	frame.ShowHead();
	frame.ShowData();
	printf("%s.CheckHead(%d)\n", __func__, frame.CheckHead());
	printf("%s.CheckData(%d)\n", __func__, frame.CheckData());
}

void frame2(void)
{
	unsigned char buf[] = 
	{
		0x55, 0xFF, 0x00, 0x08, 0x7F, 0x00, 0x00, 0x00
	};
	MstpFrame frame;
	frame.push(buf, sizeof(buf));
	frame.SetFrameHeadCrc(frame.CalcHeadCrc());
	frame.ShowHead();
	frame.ShowData();
	printf("%s.CheckHead(%d)\n", __func__, frame.CheckHead());
	printf("%s.CheckData(%d)\n", __func__, frame.CheckData());
}

int main(void)
{
	frame1();
	frame2();

	return 0;
}
