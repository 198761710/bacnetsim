#include <stdio.h>
#include "bacnet.h"

void InitBacnet(Bacnet& b)
{
	b.PushBack(Instance(1, 'r', 'A', 102, 0, 0.0));
	b.PushBack(Instance(1, 'r', 'A', 103, 0, 0.0));
	b.PushBack(Instance(1, 'r', 'A', 104, 0, 0.0));
	b.PushBack(Instance(1, 'r', 'A', 1013, 0, 0.0));
	b.PushBack(Instance(1, 'r', 'A', 1015, 0, 0.0));
	b.PushBack(Instance(1, 'r', 'B', 203, 0, 0.0));
	b.PushBack(Instance(1, 'r', 'B', 103, 0, 0.0));
}

int main(int argc, char **argv)
{
	Bacnet bacnet;
	string comdev = "/dev/ttySX0";

	if( bacnet.OpenCom(comdev) == false )
	{
		return -printf("OpenCom(%s) failed\n", comdev.data());
	}
	if( bacnet.SetCom(9600, 0, 8, 1)  == false )
	{
		return -printf("SetCom(9600, N, 8, 1) failed\n");
	}

	while(1)
	{
		if( bacnet.Empty() )
		{
			InitBacnet(bacnet);
		}
		bacnet.Run();
		usleep(100);
	}

	return 0;
}
