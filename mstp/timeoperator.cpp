#include "timeoperator.h"

TimeOperator::TimeOperator(void)
{
	init();
}
int TimeOperator::init(void)
{
	return gettimeofday(&t, 0);
}
int TimeOperator::sdiff(void)
{
	struct timeval n;

	if( gettimeofday(&n, 0) )
	{
		return 0;
	}
	return n.tv_sec - t.tv_sec;
}
int TimeOperator::mdiff(void)
{
	struct timeval n;

	if( gettimeofday(&n, 0) )
	{
		return 0;
	}
	return (n.tv_sec - t.tv_sec) * 1000 + (n.tv_usec - t.tv_usec) / 1000;
}
int TimeOperator::udiff(void)
{
	struct timeval n;

	if( gettimeofday(&n, 0) )
	{
		return 0;
	}
	return (n.tv_sec - t.tv_sec) * 1000 * 1000 + (n.tv_usec - t.tv_usec);
}
