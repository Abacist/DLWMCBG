#include<fstream>
#include<Windows.h>
#include"Basic.h"


void generator(char* fileName, int MaxY, int UpdateRange)
{
	ofstream out(fileName);
	out << MaxY << endl;
	for (int i = 1; i <= MaxY; i++)
	{
		out << i << " ";
	}
	out << endl;
	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	srand(lpsystime.wMinute * 1000 + lpsystime.wMilliseconds);
	for (int i = 0; i < UpdateRange; i++)
	{
		out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 <<" "<< rand() % MaxY * 3 << endl;;
	}
	out << 1 << " " << 301 << " " << 50 <<" "<< 80 <<" "<< 100 << endl;
	out << '$' << endl;


	out.close();
}






Msg::Msg()
{
	X x;
	x._id = -1;
	Y y;
	y._id = -1;

	_aZ = _bZ = _aT = _bT = _aI = _bI = _aX = _bX = x;
	_aY = _bY = y;
}

// same as the format in FAW, i.e., int _c; //c==0, null; c==1, transferred; c==2, infeasible
int Msg::flagInsertX()
{
	if (_aT._id == -1 && _aI._id == -1)	// Neither T nor I is augmented.
	{
		return 0;
	}		
	else if (_aT._id != -1)	// T is augmented.
	{
		return 1;
	}		
	else if (_aI._id != -1)	// I is augmented.
	{
		return 2;
	}		
	else
	{
		return -1;
	}		
}

ostream& operator<<(ostream& os, const Y& rhs)
{
	os << rhs._id;
	return os;
}

ostream& operator<<(ostream& os, const X& rhs)
{
	os << "X: " << rhs._id << " " << rhs._s << " " << rhs._e << " " << rhs._w;
	return os;
}



// order y according to its index
bool cmpYInc(Y y1, Y y2)
{
	return y1._id < y2._id;
}

bool cmpYDec(Y y1, Y y2)
{
	return y1._id > y2._id;
}

// priority: increasing end
bool cmpXEndInc(X x1, X x2)	
{
	return x1._e < x2._e;
}

bool cmpXBeginDec(X x1, X x2)
{
	return x1._s > x2._s;
}

bool cmpXWeightIDInc(X x1, X x2)
{
	if (x1._w < x2._w || x1._w == x2._w && x1._id < x2._id)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool cmpXID(X x1, X x2)
{
	return x1._id < x2._id;
}

bool cmpXEndBeginIdInc(X x1, X x2)
{
	if (x1._e < x2._e)
	{
		return true;
	}
	if (x1._e == x2._e && x1._s < x2._s)
	{
		return true;
	}
	if (x1._e == x2._e && x1._s == x2._s && x1._id < x2._id)
	{
		return true;
	}
	return false;
}