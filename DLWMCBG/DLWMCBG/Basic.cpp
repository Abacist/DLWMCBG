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
		out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 << " " << rand() % MaxY * 3 << endl;;
	}
	out << 1 << " " << 301 << " " << 50 << " " << 80 << " " << 100 << endl;
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

// increasing start
bool cmpXStartInc(X x1, X x2)
{
	return x1._s < x2._s;
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

// weight increasing, end increasing, begin decreasing, id increasing
bool cmpXStandard(X x1, X x2)
{
	if (x1._w < x2._w)
	{
		return true;
	}
	else if (x1._w == x2._w && x1._e < x2._e)
	{
		return true;
	}
	else if (x1._w == x2._w && x1._e == x2._e && x1._s > x2._s)
	{
		return true;
	}
	else if (x1._w == x2._w && x1._e == x2._e && x1._s == x2._s && x1._id < x2._id)
	{
		return true;
	}
	return false;
}

// end increasing, begin decreasing
bool cmpXEndIncStartDec(X x1, X x2)
{
	if (x1._e < x2._e)
	{
		return true;
	}
	if (x1._e == x2._e && x1._s > x2._s)
	{
		return true;
	}
	return false;
}

// are all X matched w.r.t. Y?
bool isXPerfectMatching(const vector<X>& vX, const vector<Y>& vY)
{
	vector<X> XX;
	vector<Y> YY;
	for (int i = 0; i < (int)vX.size(); i++)
	{
		XX.push_back(vX[i]);
	}
	for (int i = 0; i < (int)vY.size(); i++)
	{
		YY.push_back(vY[i]);
	}
	sort(XX.begin(), XX.end(), cmpXEndIncStartDec);
	sort(YY.begin(), YY.end(), cmpYInc);

	for (int i = 0; i < (int)XX.size(); i++)
	{
		if (XX[i]._s > YY[i] || XX[i]._e < YY[i])
		{
			return false;
		}
	}

	return true;
}



// For Test
struct SX
{
	X _X;
	int flag = 0;
	bool operator==(const SX& x)
	{
		return this->_X._id == x._X._id;
	}
};

// increasing start
bool testCmpXStartInc(SX x1, SX x2)
{
	return x1._X._s < x2._X._s;
}

// weight increasing, end increasing, begin decreasing, id increasing
bool testCmpXEndBeg(SX x1, SX x2)
{
	if (x1._X._e < x2._X._e)
	{
		return true;
	}
	else if (x1._X._e == x2._X._e && x1._X._s > x2._X._s)
	{
		return true;
	}
	return false;
}

// return the OIS in the glover mathcing of a CBG
void gloverMatching(const vector<X>& vX, const vector<Y>& vY, vector<X>* vZ)
{	
	vector<SX> XX;
	vector<Y> YY;
	for (int i = 0; i < (int)vX.size(); i++)
	{
		SX sx;
		sx._X = vX[i];		
		XX.push_back(sx);
	}
	for (int i = 0; i < (int)vY.size(); i++)
	{
		YY.push_back(vY[i]);
	}
	sort(XX.begin(), XX.end(), testCmpXStartInc);
	sort(YY.begin(), YY.end(), cmpYInc);
		
	vZ->clear();
	for (int i = 0; i < (int)YY.size(); i++)
	{
		vector<SX> W;	// the x set that may match with YY[i]
		int j = 0;
		while (j < (int)XX.size() && XX[j]._X._s <= YY[i])
		{
			if (XX[j]._X._e >= YY[i])
			{
				W.push_back(XX[j]);
			}
			j++;
		}
		sort(W.begin(), W.end(), testCmpXEndBeg);
		for (int k = 0; k < (int)W.size(); k++)
		{
			if (W[k].flag == 0)	// get the unmatched x which has the smallest end
			{
				vZ->push_back(W[k]._X);
				vector<SX>::iterator it = find(XX.begin(), XX.end(), W[k]);
				it->flag = 1;
				break;
			}
		}
	}

}

