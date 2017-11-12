#ifndef __XSTRING_H__
#define __XSTRING_H__
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
using namespace std;

class xstring : public std::string
{
public:
	xstring(void){}
	xstring(const char* s){ if(s)format("%s",s); }
public:
	int format(const char* fmt,...)
	{
		int r = 0;
		va_list ap;
		char buf[4096] = {0};

		va_start(ap, fmt);
		r = vsnprintf(buf, sizeof(buf), fmt, ap);
		va_end(ap);
		buf[sizeof(buf)-1] = 0;
		*this = buf;

		return r;
	}
	xstring& operator=(const char* s)
	{
		if( s )
		{
			*(std::string*)this = s;
		}
		else
		{
			*(std::string*)this = "";
		}
		return *this;
	}
	unsigned char xdigit(int i)
	{
		if( i < length() )
		{
			switch( (*(this))[i] )
			{
			case '0':
				return 0x00;
			case '1':
				return 0x01;
			case '2':
				return 0x02;
			case '3':
				return 0x03;
			case '4':
				return 0x04;
			case '5':
				return 0x05;
			case '6':
				return 0x06;
			case '7':
				return 0x07;
			case '8':
				return 0x08;
			case '9':
				return 0x09;
			case 'a':
			case 'A':
				return 0x0A;
			case 'b':
			case 'B':
				return 0x0B;
			case 'c':
			case 'C':
				return 0x0C;
			case 'd':
			case 'D':
				return 0x0D;
			case 'e':
			case 'E':
				return 0x0E;
			case 'f':
			case 'F':
				return 0x0F;
			}
		}//siwtch
		return 0;
	}//xdigit
	
	unsigned char tochar(void)
	{
		return 0xff & toint();
	}
	unsigned char toshort(void)
	{
		return 0xffff &toint();
	}
	int toint(void)
	{
		if( empty() )
		{
			return 0;
		}
		return atoi(data());
	}
	int xtoint(void)
	{
		int len = length();

		if( len < 1 )
		{
			return 0;
		}
		if( len < 3 )
		{
			return strtol(data(), 0, 16);
		}
		if( data()[2] == 'x' )
		{
			return strtol(data()+2, 0, 16);
		}
	}
	xstring operator+(const xstring& s)
	{
		xstring tmp = s;
		tmp += *this;
		return tmp;
	}
};

#endif//__XSTRING_H__
