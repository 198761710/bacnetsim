#include <sys/types.h>
#include <string.h>
#include "mstp.h"
#include "comport.h"
#include "mstpframe.h"
#include "platform.h"

static Mstp mstp;
static ComPort com;
static TimeOperator t;
static TimeOperator s;
static unsigned char invokeid = 0x01;

bool ProcFrameTypeToken(MstpFrame &frame)
{
	unsigned char dst = frame.GetFrameDst();
	unsigned char src = frame.GetFrameSrc();
	mstp.ReadReal(src, dst, invokeid++, 102);
	com.Send(mstp.data(), (int)mstp.length());
	printf("[%5d].send:", t.mdiff());
	mstp.showhex();
	return true;
}
bool ProcFrameTypePollMaster(MstpFrame &frame)
{
	unsigned char dst = frame.GetFrameDst();
	unsigned char src = frame.GetFrameSrc();

	frame.SetFrameType(FrameTypePollMasterAck);
	frame.SetFrameDst(src);
	frame.SetFrameSrc(dst);
	frame.SetFrameHeadCrc(frame.CalcHeadCrc());
	com.Send(frame.frame(), frame.headlength());
	printf("[%5d].send:", t.mdiff());
	frame.ShowFrame();
	return true;
}
bool ProcFrameTypePollMasterAck(MstpFrame &frame)
{
	unsigned char dst = frame.GetFrameDst();
	unsigned char src = frame.GetFrameSrc();

	frame.SetFrameType(0x02);
	frame.SetFrameDst(src);
	frame.SetFrameSrc(dst);
	frame.SetFrameHeadCrc(frame.CalcHeadCrc());
	return true;
}
bool ProcFrameTypeTestRequest(MstpFrame &frame)
{
	return false;
}
bool ProcFrameTypeTestResponse(MstpFrame &frame)
{
	return false;
}
bool ProcFrameTypeDataRequest(MstpFrame &frame)
{
	return false;
}
bool ProcFrameTypeDataResponse(MstpFrame &frame)
{
	return false;
}

bool ProcFrame(MstpFrame& frame)
{
	switch( frame.GetFrameType() )
	{
	case FrameTypeToken:
		return ProcFrameTypeToken(frame);
	case FrameTypePollMaster:
		return ProcFrameTypePollMaster(frame);
	case FrameTypePollMasterAck:
		return ProcFrameTypePollMasterAck(frame);
	case FrameTypeTestRequest:
		return ProcFrameTypeTestRequest(frame);
	case FrameTypeTestResponse:
		return ProcFrameTypeTestResponse(frame);
	case FrameTypeDataRequest:
		return ProcFrameTypeDataRequest(frame);
	case FrameTypeDataResponse:
		return ProcFrameTypeDataResponse(frame);
	}
	return false;
}
bool PollMaster(MstpFrame &frame)
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
	MstpFrame frame;
	char buf[32] = {0};
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
		if( PollMaster(frame) )
		{
			com.Send(frame.frame(), frame.headlength());
			printf("[%5d].send:", s.mdiff());
			frame.ShowFrame();
			frame.clear();
			s.init();
		}
		usleep(100);
	}

	return 0;
}
