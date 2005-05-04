#ifndef MY_STRING_H_INCLUDED
#define MY_STRING_H_INCLUDED

#include <xstring>
#include <windows.h>

#define ZEROPAD	0x01		/* pad with zero */
#define SIGN	0x02		/* unsigned/signed long */
#define PLUS	0x04		/* show plus */
#define SPACE	0x08		/* space if plus */
#define LEFT	0x10		/* left justified */
#define SPECIAL	0x20		/* 0x */
#define LARGE	0x40		/* use 'ABCDEF' instead of 'abcdef' */

using namespace std;

class String
{
private:
#ifdef _UNICODE
	wstring ThisString;
#else
	string ThisString;
#endif
	void AddBlank(int,const TCHAR=' ');
	void AddDouble(double,int,int,int);
	void AddNumber(long,int,int,int,int);
	int NumberDivide(unsigned long *,int);
	int GetNumber(TCHAR **);
public:
	void String::Printf(const TCHAR *,...);
	String& operator=(const String& _X)		{ThisString.assign(_X.ThisString); return *this;}
	String& operator=(const TCHAR *_S)		{ThisString.assign(_S); return *this;}
	String& operator=(TCHAR _C)				{ThisString.assign(1, _C); return *this;}
	String& operator+=(const String& _X)	{ThisString.append(_X.ThisString); return *this;}
	String& operator+=(const TCHAR *_S)		{ThisString.append(_S); return *this;}
	String& operator+=(TCHAR _C)			{ThisString.append(1, _C); return *this;}
	const TCHAR *CStr()						{return ThisString.c_str();}
	void Erase(int _P=0,int _M=-1)			{ThisString.erase(_P,_M);}
	int Length()							{return (int) ThisString.size();}
	int FindLastOf(TCHAR _C)				{return (int) ThisString.find_last_of(_C);}
};

#endif