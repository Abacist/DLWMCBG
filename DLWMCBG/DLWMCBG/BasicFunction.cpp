#include"Basic.h"

ostream& operator<<(ostream& os, const Y& rhs)
{
	os << rhs._y;
	return os;
}

ostream& operator<<(ostream& os, const X& rhs)
{
	os << "X: " << rhs._id << " " << rhs._begin << " " << rhs._end << " " << rhs._w;
	return os;
}