#include "bacnet.h"

Bacnet::Bacnet(void):master(0x7f),invokeid(0),pollindex(0),tokenhold(0)
{
	for(unsigned char i = 0; i < 255; i++)
	{
		devices[i].init();
	}
}
void Bacnet::SetMaster(unsigned char m)
{
	master = m;
}
RecvFrame& Bacnet::GetRecvFrame(void)
{
	return rframe;
}
void Bacnet::Run(void)
{
	RecvCom();
	PollDevice();
	if( 0 == tokenhold )
	{
		return;	
	}
	if( sendtime.mdiff() <  100 )
	{
		return;
	}
	DoRequest();
}
bool Bacnet::Empty(void)
{
	return instancelist.empty();
}
void Bacnet::PushBack(const Instance& i)
{
	instancelist.push_back(i);
}
void Bacnet::PushFront(const Instance& i)
{
	instancelist.push_front(i);
}
void Bacnet::DoRequest(void)
{
	if( instancelist.empty() )
	{
		return;
	}

	SendFrame frame;
	Instance& i = instancelist.front();

	switch(i.mode + i.type)
	{
		case 'r' + 'A':
		case 'R' + 'A':
		case 'r' + 'B':
		case 'R' + 'B':
		case 'r' + 'M':
		case 'R' + 'M':
			frame.ReadReal(i.dev, master, invokeid, (int)i.instance);
			break;
		case 'w' + 'A':
		case 'W' + 'A':
			frame.WriteReal(i.dev, master, invokeid, (int)i.instance, i.analog);
			break;
		case 'w' + 'B':
		case 'W' + 'B':
			frame.WriteEnum(i.dev, master, invokeid, (int)i.instance, i.binary);
			break;
	}
	instancelist.pop_front();
	SendCom(frame);
	if( invokeid > 127 )
	{
		invokeid = 0;
	}
}
bool Bacnet::OpenCom(const string& dev)
{
	if( comport.Open(dev) )
	{
		return true;
	}
	return false;
}
bool Bacnet::SetCom(int baud, char parity, int bsize, int stop)
{
	return comport.Set(baud, parity, bsize, stop);
}
void Bacnet::RecvCom(void)
{
	unsigned char c = 0;

	if( comport.Recv(&c, 1) == 1 )
	{
		rframe.push(c);
	}
	if( rframe.Check() )
	{
		ProcFrame();
		printf("[%3d].recv:", recvtime.mdiff());
		rframe.showhex();
		rframe.clear();
		recvtime.init();
	}
}
bool Bacnet::ProcFrame(void)
{
	u8 src = rframe.GetFrameSrc();
	u8 dst = rframe.GetFrameDst();

	devices[src].init();

	if( dst == 0xff )
	{
		return Dispatch();
	}
	if( dst == master )
	{
		return Dispatch();
	}
	if( devices[dst].sdiff() > 10 )
	{
		return Dispatch();
	}
	return false;
}
bool Bacnet::Dispatch(void)
{
	switch( rframe.GetFrameType() )
	{
		case FrameTypeToken:
			return Token();

		case FrameTypePollMaster:
			return PollMaster();

		case FrameTypePollMasterAck:
			return PollMasterAck();

		case FrameTypeTestRequest:
			return TestRequest();

		case FrameTypeTestRequestAck:
			return TestRequestAck();

		case FrameTypeDataRequest:
			return DataRequest();

		case FrameTypeDataRequestAck:
			return DataRequestAck();
	}
	return false;
}
bool Bacnet::SendCom(SendFrame& frame)
{
	printf("[%3d].send:", sendtime.mdiff());
	frame.showhex();
	sendtime.init();
	return comport.Send(frame.data(), frame.length()) > 0;
}
bool Bacnet::SendCom(RecvFrame& frame)
{
	printf("[%3d].send:", sendtime.mdiff());
	frame.showhex();
	sendtime.init();
	return comport.Send(frame.data(), frame.length()) > 0;
}
void Bacnet::PollDevice(void)
{
	if( tokentime.sdiff() < 3 )
	{
		return;
	}
	if( recvtime.mdiff() < 10 * master )
	{
		return;
	}
	if( sendtime.mdiff() < 100 )
	{
		return;
	}
	RecvFrame frame;
	
	frame.push(0x55);//head-pre0
	frame.push(0xff);//head-pre1
	frame.push(0x01);//frame-type
	frame.push(0x00);//frame-src
	frame.push(0x00);//frame-dst
	frame.push(0x00);//frame-data-len-hig
	frame.push(0x00);//frame-data-len-low
	frame.push(0x00);//frame-head-crc

	frame.SetFrameSrc(master);
	frame.SetFrameDst(pollindex++%128);
	frame.SetFrameHeadCrc(frame.CalcHeadCrc());
	SendCom(frame);
}
bool Bacnet::Token(void)
{
	tokenhold = 1;
	tokentime.init();
	DoRequest();
	return true;
}
bool Bacnet::PollMaster(void)
{
	const u8 src = rframe.GetFrameSrc();
	const u8 dst = rframe.GetFrameDst();

	if( dst == master || devices[dst].sdiff() > 10 )
	{
		rframe.SetFrameType(FrameTypePollMasterAck);
		rframe.SetFrameDst(src);
		rframe.SetFrameSrc(dst);
		rframe.SetFrameHeadCrc(rframe.CalcHeadCrc());
		SendCom(rframe);
	}
	tokenhold = 0;

	return true;
}
bool Bacnet::PollMasterAck(void)
{
	u8 src = rframe.GetFrameSrc();
	u8 dst = rframe.GetFrameDst();

	devices[src].init();

	rframe.SetFrameType(FrameTypeToken);
	rframe.SetFrameSrc(master);
	rframe.SetFrameDst(pollindex++%128);
	rframe.SetFrameHeadCrc(rframe.CalcHeadCrc());
	rframe.showhex();
	SendCom(rframe);

	return true;
}
bool Bacnet::TestRequest(void)
{
	return false;
}
bool Bacnet::TestRequestAck(void)
{
	return false;
}
bool Bacnet::DataRequest(void)
{
	return false;
}
bool Bacnet::DataRequestAck(void)
{
	return true;
}
