#ifndef __TIME_OPERATOR_H__
#define __TIME_OPERATOR_H__
#include <sys/time.h>

class TimeOperator
{
protected:
	struct timeval t;
public:
	TimeOperator(void);

public:
	int init(void);
	int sdiff(void);
	int mdiff(void);
	int udiff(void);
};

#endif//__TIME_OPERATOR_H__
