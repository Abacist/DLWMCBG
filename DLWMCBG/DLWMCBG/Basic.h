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
};

ostream& operator<<(ostream& os, const X& rhs);

// change Msg from structure to class to include various methods for Msg
class Msg
{
public:
	// (a_Z,b_Z;a_T,b_T;a_I,b_I;a_X,b_X;a_Y,b_Y ); 
	// if in a itme x.id=-1 or y.id=-1, that means the itme is empty.
	X _aZ;
	X _bZ;
	X _aT;
	X _bT;
	X _aI;
	X _bI;
	X _aX;
	X _bX;
	Y _aY;
	Y _bY;

	//// the FAW's format 
	//bool _aEmpty; //true: _a is empty
	//X _a;
	//bool _bEmpty;
	//X _b;
	//int _c; //c==0, null; c==1, transferred; c==2, infeasible

	// processes
	Msg();
	int flagInsertX();	
};

bool cmpYInc(Y y1, Y y2);			// in the increasing y._id ording
bool cmpYDec(Y, Y);
bool cmpXEndInc(X x1, X x2);	// in the increasing x._e ording
bool cmpXBeginDec(X, X);
//bool cmpXStartInc(X x1, X x2);	// increasing start
bool cmpXWeightInc(X x1, X x2);
bool cmpXID(X x1, X x2);
//bool cmpXEndBeginIdInc(X x1, X x2);
//bool cmpXEndIncStartDec(X x1, X x2);	// end increasing, start decreasing
//bool cmpXStandard(X x1, X x2);	// weight decreasing, end increasing, start decreasing, id increasing

void gloverMatching(const vector<X>& vX, const vector<Y>& vY, vector<X>* vZ);	// return the OIS in the glover mathcing of a CBG
void PlaxtonMWM(const vector<X>& vX, const vector<Y>& vY, vector<X>* vZ);	// return the OIS in the plaxton MWM of a LWCBG