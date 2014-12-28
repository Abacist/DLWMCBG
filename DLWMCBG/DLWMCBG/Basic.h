//include the definition of X, Y and basic function like cmp
#pragma once

#include<iostream>
#include<set>
#include<list>
#include<vector>
#include<stack>
#include<algorithm>

using namespace std;

// structure of a y vertex
struct Y
{
	int _id;	// id of x, assuming distinctive
	// Notes: the y vertex with _id=-1 denotes the empty y, which is used in msg transmission.
	int _w;


	//used for determine the edge
	bool operator<(const Y& y)
	{
		return this->_id < y._id;
	}
	bool operator<=(const Y& y)
	{
		return this->_id <= y._id;
	}
	bool operator>(const Y& y)
	{
		return this->_id > y._id;
	}
	bool operator>=(const Y& y)
	{
		return this->_id >= y._id;
	}
	bool operator==(const Y& y)
	{
		return this->_id == y._id;
	}
	bool operator!=(const Y& y)
	{
		return this->_id != y._id;
	}

};

ostream& operator<<(ostream& os, const Y& rhs);

// structure of an x vertex
struct X
{
	int _id;	// id of x, assuming distinctive
	// Notes: the x vertex with _id=-1 denotes the empty x, which is used in msg transmission.

	Y _s;	// the start of the interval of the neighbour of the x
	Y _e;	// the end of the interval of the neighbour of the x
	int _w;	// the weight of the x, assuming it is positive
	bool operator==(const X& x)
	{
		return this->_id == x._id;
	}
	bool operator!=(const X& x)
	{
		return !(*this == x);
	}
};

ostream& operator<<(ostream& os, const X& rhs);

// change Msg from structure to class to include various methods for Msg
class Msg
{
public:
	// (a_Z,b_Z;a_T,b_T;a_I,b_I;a_X,b_X;a_Y,b_Y ); 
	// if in a itme x.id=-1 or y.id=-1, that means the itme is empty.
	X _aX;
	X _bX;
	Y _aY;
	Y _bY;

	X _aMX;
	X _bMX;
	X _aTX;
	X _bTX;
	X _aIX;
	X _bIX;
	
	Y _aMY;
	Y _bMY;
	Y _aIY;
	Y _bIY;

	
	Msg();
	
};

bool cmpYIDInc(Y y1, Y y2);			
bool cmpYIDDec(Y, Y);
bool cmpYWeightInc(Y y1, Y y2);
bool cmpXEndInc(X x1, X x2);	
bool cmpXBeginDec(X, X);
bool cmpXWeightInc(X x1, X x2);
bool cmpXID(X x1, X x2);

void formGloverMatching(const vector<X> vX, const vector<Y> vY, vector<X>& vZ);	
void formPlaxtonMWM(const vector<X> vX, const vector<Y> vY, vector<X>& vZ, vector<Y>& vMY);