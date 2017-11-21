#ifndef __BACNET_H__
#define __BACNET_H__
#include <map>
#include <list>
#include "comport.h"
#include "sendframe.h"
#include "recvframe.h"
#include "timeoperator.h"
using namespace std;

class Instance
{
public:
	u8 dev;
	u8 mode;
	u8 type;
	u16 instance;
	s32 binary;
	float analog;
public:
	Instance(u8 d, u8 m, u8 t, u16 i, s32 b, float a):dev(d),mode(m),type(t),instance(i),binary(b),analog(a){}
};

class Bacnet
{
protected:
	ComPort comport;
	RecvFrame rframe;
	TimeOperator tokentime;
	TimeOperator recvtime;
	TimeOperator sendtime;
	list<Instance> instancelist;
	map<unsigned char,TimeOperator> devices;
	unsigned char master;
	unsigned char invokeid;
	unsigned char pollindex;
	unsigned char tokenhold;

public:
	Bacnet(void);

public:
	void SetMaster(unsigned char);
	RecvFrame& GetRecvFrame(void);

public:
	void Run(void);
	bool OpenCom(const string& dev);
	bool SetCom(int baud, char parity, int bsize, int stop);
	void RecvCom(void);
	bool SendCom(SendFrame& frame);
	bool SendCom(RecvFrame& frame);
	void PushBack(const Instance& i);
	void PushFront(const Instance& i);
	bool Empty(void);
	virtual void DoRequest(void);

protected:
	bool ProcFrame(void);
	bool Dispatch(void);
	void PollDevice(void);

protected:
	virtual bool Token(void);
	virtual bool PollMaster(void);
	virtual bool PollMasterAck(void);
	virtual bool TestRequest(void);
	virtual bool TestRequestAck(void);
	virtual bool DataRequest(void);
	virtual bool DataRequestAck(void);
};

#endif//__BACNET_H__
