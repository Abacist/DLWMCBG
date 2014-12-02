//include the definition of X, Y and basic function like cmp
#pragma once

#include<iostream>
#include<set>
#include<list>
#include<vector>
#include<stack>

using namespace std;

struct Y
{
	double _y;
	bool operator<(const Y& y)
	{
		return this->_y < y._y;
	}
	bool operator<=(const Y& y)
	{
		return this->_y <= y._y;
	}
	bool operator>(const Y& y)
	{
		return this->_y > y._y;
	}
	bool operator>=(const Y& y)
	{
		return this->_y >= y._y;
	}
	bool operator==(const Y& y)
	{
		return this->_y == y._y;
	}
	bool operator!=(const Y& y)
	{
		return this->_y != y._y;
	}

};

ostream& operator<<(ostream& os, const Y& rhs);

struct X
{
	int _id;
	Y _begin;
	Y _end;
	int _w;	// weight, assume it is positive
	bool operator==(const X& x)
	{
		return this->_id == x._id;
	}
};

ostream& operator<<(ostream& os, const X& rhs);

struct Msg
{
	//tbd	
};