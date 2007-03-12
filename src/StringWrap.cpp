#include <ctype.h>
#include "StringWrap.h"

#ifdef _UNICODE
# define STRCHR wcschr
# define STRLEN wcslen
# define ISDIGIT iswdigit
#else
# define STRCHR strchr
# define STRLEN strlen
# define ISDIGIT isdigit
#endif

void String::Printf(const TCHAR *Format,...)
{
	va_list Args;

	va_start(Args,Format);
	VPrintf(Format,Args);
	va_end(Args);
}

void String::VPrintf(const TCHAR *Format,va_list Args)
{
	TCHAR *PercentPtr,*Start,*AddStr;
	unsigned long AddNum;
	int OutLen,StrLen;
	int Flags,FieldWidth,Precision,Base;
	TCHAR Qualifier,Type;
	bool FinishedFlags,Numeric;
	
	PercentPtr=(TCHAR *)Format;
	Start=(TCHAR *)Format;
	while((PercentPtr=STRCHR(Start,'%'))!=NULL){
		OutLen=(int)(PercentPtr-Start);
		// Output string up to %
		if(OutLen) ThisString.append(Start,OutLen);
		// Get flags
		Flags=0;
		FinishedFlags=false;
		do{
			++PercentPtr;
			switch(*PercentPtr){
			case '-':
				Flags|=LEFT;
				break;
			case '+':
				Flags|=PLUS;
				break;
			case ' ':
				Flags|=SPACE;
				break;
			case '#':
				Flags|=SPECIAL;
				break;
			case '0':
				Flags|=ZEROPAD;
				break;
			default:
				FinishedFlags=true;
				break;
			}
		}while(!FinishedFlags);
		// Get field width
		FieldWidth=0;
		if(ISDIGIT(*PercentPtr))
			FieldWidth=GetNumber(&PercentPtr);
		else if(*PercentPtr=='*'){
			++PercentPtr;
			FieldWidth=va_arg(Args,int);
			if(FieldWidth<0){
				FieldWidth=-FieldWidth;
				Flags|=LEFT;
			}
		}
		// Get precision specifier
		Precision=0;
		if(*PercentPtr=='.'){
			++PercentPtr;
			if(isdigit(*PercentPtr))
				Precision=GetNumber(&PercentPtr);
			else if(*PercentPtr=='*'){
				Precision=va_arg(Args,int);
			}
			if(Precision<0) Precision=0;
		}
		// Get conversion qualifier
		Qualifier=' ';
		if(*PercentPtr=='h' || *PercentPtr=='l' || *PercentPtr=='L'){
			Qualifier=*PercentPtr;
			++PercentPtr;
		}
		// Get arg format
		Base=10;
		Numeric=false;
		Type=*PercentPtr;
		switch(Type){
		case 'c': // Character
			if(!(Flags&LEFT) && FieldWidth>1){
				AddBlank(FieldWidth-1);
				FieldWidth=0;
			}
			ThisString+=(TCHAR) va_arg(Args,int);
			if(FieldWidth>1) AddBlank(FieldWidth-1);
			break;
		case 's': // String
			AddStr=va_arg(Args,TCHAR *);
			if(!AddStr) AddStr=TEXT("<NULL>");
			StrLen=(int) STRLEN(AddStr);
			if(!(Flags&LEFT) && FieldWidth>StrLen){
				AddBlank(FieldWidth-StrLen);
				FieldWidth=0;
			}
			ThisString.append(AddStr,StrLen);
			if(FieldWidth>StrLen){
				AddBlank(FieldWidth-StrLen);
			}
			break;
		case 'p': // Pointer
			if(FieldWidth==0){
				FieldWidth=2*sizeof(void *);
				Flags|=ZEROPAD;
			}
			AddNumber(*((unsigned long *) va_arg(Args,void *)),16,FieldWidth,Precision,Flags);
			break;
		case '%': // Percentage symbol
			ThisString+='%';
			break;
		case 'o': // Octal number
			Base=8;
			Numeric=true;
			break;
		case 'X': // Uppercase hex
			Flags|=LARGE;
		case 'x': // Lowercase hex
			Base=16;
			Numeric=true;
			break;
		case 'd': // Signed decimal
		case 'i': // Signed decimal
			Flags|=SIGN;
		case 'u': // Unsigned decimal
			Numeric=true;
			break;
		case 'f': // floating point
			Flags|=SIGN;
			Numeric=true;
			break;
		default: // Something wrong...
			ThisString+='%';
			ThisString+=*PercentPtr;
			break;
		}
		PercentPtr++;
		if(Numeric){
			if(Type=='f'){
				AddDouble(va_arg(Args,double),FieldWidth,Precision,Flags);
			}
			else{
				switch(Qualifier){
				case 'l':
				case 'L':
					if(Flags&SIGN) AddNum=va_arg(Args,long);
					else AddNum=va_arg(Args,unsigned long);
					break;
				case 'h':
					if(Flags&SIGN) AddNum=(short) va_arg(Args,int);
					else AddNum=(unsigned short) va_arg(Args,int);
					break;
				default:
					if(Flags&SIGN) AddNum=va_arg(Args,int);
					else AddNum=(unsigned int) va_arg(Args,int);
					break;
				}
				AddNumber(AddNum,Base,FieldWidth,Precision,Flags);
			}
		}
		Start=PercentPtr;
	}
	if(*Start!='\x0') ThisString.append(Start);
}

#define ADDCHARS(CharPtr,Number) for(int i=0;i<(Number);i++) ThisString.append((TCHAR *)((CharPtr)+i))

void String::AddDouble(double Number,int Size,int Precision,int Type)
{
	int Decimal,DSign,Digits;
	char *FloatPtr,*FracPtr;
	TCHAR Sign=' ',FillChar=' ';

	if(Type&LEFT) Type&=~ZEROPAD;
	if(Type&ZEROPAD) FillChar='0';
	// Convert to a string
	FloatPtr=fcvt(Number,Precision,&Decimal,&DSign);
	// Work out sign
	if(Type&SIGN){
		if(DSign){
			Sign='-';
			Number=-Number;
			Size--;
		}
		else if(Type&PLUS){
			Sign='+';
			Size--;
		}
		else if(Type&SPACE){
			Sign=' ';
			Size--;
		}
	}
	Digits=(int) strlen(FloatPtr);
	if(Decimal<0){
		// 0.(abs(Decimal)*'0')[FloatPtr]
		Size-=Digits+(-Decimal)+2;
	}
	else if(Decimal==0){
		// 0.[FloatPtr]
		Size-=Digits+2;
	}
	else{
		// [FloatPtr,,Decimal-1].[FloatPtr,Decimal]
		if(Decimal<Digits) Size-=Digits+1;
		else Size-=Digits;
	}
	if(Size>=0 && !(Type&(ZEROPAD+LEFT))) AddBlank(Size,' ');
	if(Sign!=' ') ThisString+=Sign;
	if(Size>=0 && !(Type&LEFT)){
		AddBlank(Size,FillChar);
		Size=0;
	}
	if(Decimal<0){
		// 0.(abs(Decimal)*'0')[FloatPtr]
		ThisString+=TEXT("0.");
		AddBlank(-Decimal,'0');
		ADDCHARS(FloatPtr,Digits);
	}
	else if(Decimal==0){
		// 0.[FloatPtr]
		ThisString+=TEXT("0.");
		ADDCHARS(FloatPtr,Digits);
	}
	else{
		// [FloatPtr,,Decimal-1].[FloatPtr,Decimal]
		ADDCHARS(FloatPtr,Decimal);
		FracPtr=FloatPtr+Decimal;
		if(Digits-Decimal>0){
			ThisString+='.';
			ADDCHARS(FracPtr,Digits-Decimal);
		}
	}
}

void String::AddNumber(long Number,int Base,int Size,int Precision,int Type)
{
	const TCHAR *Digits=TEXT("0123456789abcdefghijklmnopqrstuvwxyz");
	TCHAR FillChar=' ',Sign=' ',Tmp[66];
	unsigned long AbsNumber;
	int Loop;
	
	if(Base>=2 && Base<=36){
		if(Type&LARGE) Digits=TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		if(Type&LEFT) Type&=~ZEROPAD;
		if(Type&ZEROPAD) FillChar='0';
		// Work out sign
		AbsNumber=Number;
		if(Type&SIGN){
			if(Number<0){
				Sign='-';
				AbsNumber=-Number;
				Size--;
			}
			else if(Type&PLUS){
				Sign='+';
				Size--;
			}
			else if(Type&SPACE){
				Sign=' ';
				Size--;
			}
		}
		// Special prefix space
		if(Type&SPECIAL){
			if(Base==16) Size-=2;
			else if(Base==8) Size--;
		}
		Loop=0;
		if(Number==0) Tmp[Loop++]='0';
		else{
			while(AbsNumber!=0){
				Tmp[Loop++]=Digits[NumberDivide(&AbsNumber,Base)];
			}
		}
		if(Loop>Precision) Precision=Loop;
		Size-=Precision;
		if(Size>0 && !(Type&(ZEROPAD|LEFT))) AddBlank(Size);
		if(Sign!=' ') ThisString+=Sign;
		if(Type&SPECIAL){
			if(Base==8) ThisString+='0';
			else if(Base==16){
				// Add 0x or 0X
				ThisString+='0';
				ThisString+=Digits[33];
			}
		}
		if(Size>0 && !(Type&LEFT)){
			AddBlank(Size,FillChar);
			Size=0;
		}
		if(Precision-Loop>0) AddBlank(Precision-Loop);
		Precision-=Loop;
		while(Loop-- > 0) ThisString+=Tmp[Loop];
		if(Size>0) AddBlank(Size);
		Size=0;
	}
}

int String::NumberDivide(unsigned long *Number,int Base)
{
	int Result;
	
	Result=((unsigned long)(*Number))%(unsigned)Base;
	*Number=((unsigned long)(*Number))/(unsigned)Base;
	return Result;
}

int String::GetNumber(TCHAR **StringPtr)
{
	int Number=0;
	
	while(isdigit(**StringPtr)) Number=Number*10+*((*StringPtr)++)-'0';
	return Number;
}

void String::AddBlank(int Number,TCHAR Char)
{
	ThisString.append(Number,Char);
}
