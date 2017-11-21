#include <sys/types.h>
#include <string.h>
#include "comport.h"
#include "platform.h"
#include "recvframe.h"
#include "sendframe.h"

static ComPort com;
static SendFrame send;
static TimeOperator t;
static TimeOperator s;
static unsigned char invokeid = 0x01;

bool Token(RecvFrame &frame)
{
	unsigned char dst = frame.GetFrameDst();
	unsigned char src = frame.GetFrameSrc();
	send.ReadReal(src, dst, invokeid++, 102);
	com.Send(send.data(), (int)send.length());
	printf("[%5d].send:", t.mdiff());
	send.showhex();
	return true;
}
bool PollMaster(RecvFrame &frame)
{
	unsigned char dst = frame.GetFrameDst();
	unsigned char src = frame.GetFrameSrc();

	frame.SetFrameType(FrameTypePollMasterAck);
	frame.SetFrameDst(src);
	frame.SetFrameSrc(dst);
	frame.SetFrameHeadCrc(frame.CalcHeadCrc());
	com.Send(frame.data(), frame.headlength());
	printf("[%5d].send:", t.mdiff());
	frame.ShowFrame();
	return true;
}
bool PollMasterAck(RecvFrame &frame)
{
	unsigned char dst = frame.GetFrameDst();
	unsigned char src = frame.GetFrameSrc();

	frame.SetFrameType(0x02);
	frame.SetFrameDst(src);
	frame.SetFrameSrc(dst);
	frame.SetFrameHeadCrc(frame.CalcHeadCrc());
	return true;
}
bool TestRequest(RecvFrame &frame)
{
	return false;
}
bool TestResponse(RecvFrame &frame)
{
	return false;
}
bool DataRequest(RecvFrame &frame)
{
	return false;
}
bool DataResponse(RecvFrame &frame)
{
	return false;
}

bool ProcFrame(RecvFrame& frame)
{
	switch( frame.GetFrameType() )
	{
	case FrameTypeToken:
		return Token(frame);

	case FrameTypePollMaster:
		return PollMaster(frame);

	case FrameTypePollMasterAck:
		return PollMasterAck(frame);

	case FrameTypeTestRequest:
		return TestRequest(frame);

	case FrameTypeTestRequestAck:
		return TestResponse(frame);

	case FrameTypeDataRequest:
		return DataRequest(frame);

	case FrameTypeDataRequestAck:
		return DataResponse(frame);
	}
	return false;
}
bool DoPollMaster(RecvFrame &frame)
{
	static char master = 0x01;

	if( frame.mdiff() > 1000 )
	{
		unsigned char buf[] = {
			0x55, 0xff, 0x01, master++ % 0x7F, 0x7F, 0, 0, 0
		};
		frame.clear();
		frame.push(buf, sizeof(buf));
		frame.SetFrameHeadCrc(frame.CalcHeadCrc());
		return true;
	}
	return false;
}

int main(int argc, char **argv)
{
	RecvFrame frame;
	u8 buf[32] = {0};
	int len = sizeof(buf);
	string dev = "/dev/ttySX0";

	if( argc > 1 )
	{
		dev = argv[1];
	}
	if( com.Open(dev) == false )
	{
		printf("open failed!\n");
		return -1;
	}
	if( com.Set(9600, 0, 8, 1) == false )
	{
		printf("set failed!\n");
		return -1;
	}
	com.Block(false);

	while(1)
	{
		len = com.Recv(buf, sizeof(buf));
		for(int i = 0; i < len; i++)
		{
			frame.push((unsigned char)buf[i]);
			if( frame.Check() )
			{
				printf("[%5d].recv:", t.mdiff());
				frame.ShowFrame();
				if( ProcFrame(frame) )
				{
				}
				frame.clear();
				t.init();
				continue;
			}
		}
		if( DoPollMaster(frame) )
		{
			com.Send(frame.data(), frame.headlength());
			printf("[%5d].send:", s.mdiff());
			frame.ShowFrame();
			frame.clear();
			s.init();
		}
		usleep(100);
	}

	return 0;
}
