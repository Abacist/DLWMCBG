#include"Basic.h"

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
bool cmpY(Y y1, Y y2)
{
	return y1._id < y2._id;
}

// priority: increasing end
bool cmpXEndInc(X x1, X x2)	
{
	return x1._e <= x2._e;
}