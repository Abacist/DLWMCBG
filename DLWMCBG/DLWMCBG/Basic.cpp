#include<fstream>
#include<Windows.h>
#include"Basic.h"


void generator(char* fileName, int MaxY, int UpdateRange, int WeightRange)
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
	//out << 1 << " " << 1000 << " " << 40<< " " << MaxY-1 << " " << 100 << endl;
	//out << 1 << " " << 1001 << " " << 9<< " " << MaxY - 1 << " " << 100 << endl;
	//out << 1 << " " << 1002 << " " << 20<< " " << MaxY - 1 << " " << 100 << endl;
	//out << 1 << " " << 1003 << " " << 5 << " " << MaxY - 1 << " " << 100 << endl;

	//out << 1 << " " << 1 << " " << 3 << " " << rand() % MaxY + 1 << " " << rand() % WeightRange + 1 << endl;

	int i = 0;
	for (; i < UpdateRange; i++)
	{
		int s = rand() % MaxY + 1;
		int rest = MaxY - s;
		int e;
		if (rest == 0)
			e = s;
		else
		{
			int diff = rand() % rest;
			e = s + diff;
		}
		out << 1 << " " << i + 1 << " " << s << " " << e << " " << rand() % WeightRange << endl;

		//out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 << " " << rand() % MaxY * 10 << endl;;
		//out << 1 << " " << i + 2 << " " << 3 << " " << rand() % MaxY + 1 << " " << rand() % WeightRange  + 1 << endl;
	}
	/*out << 1 << " " << 1000 << " " << 40 << " " << 80 << " " << 100 << endl;
	for (int j = 0; j < UpdateRange; j++, i++)
	{
	out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 << " " << rand() % MaxY * 10 << endl;;
	}
	out << 1 << " " << 1001 << " " << 30 << " " << 80 << " " << 100 << endl;
	for (int j = 0; j < UpdateRange; j++, i++)
	{
	out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 << " " << rand() % MaxY * 10 << endl;;
	}
	out << 1 << " " << 1002 << " " << 20 << " " << 80 << " " << 100 << endl;
	for (int j = 0; j < UpdateRange; j++, i++)
	{
	out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 << " " << rand() % MaxY * 10 << endl;;
	}
	out << 1 << " " << 1003 << " " << 10 << " " << 80 << " " << 100 << endl;
	for (int j = 0; j < UpdateRange; j++, i++)
	{
	out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 << " " << rand() % MaxY * 10 << endl;;
	}*/
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

// weight decreasing, end increasing, begin decreasing, id increasing
bool cmpXStandard(X x1, X x2)
{
	if (x1._w > x2._w)
	{
		return true;
	}
	else if (x1._w == x2._w && x1._id > x2._id)
	{
		return true;
	}
	/*else if (x1._w == x2._w && x1._e < x2._e)
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
	}*/
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

// For Test
struct TestX
{
	X _X;
	int flag = 0;
	bool operator==(const TestX& x)
	{
		return this->_X._id == x._X._id;
	}
};

// increasing start
bool testCmpXStartInc(TestX x1, TestX x2)
{
	return x1._X._s < x2._X._s;
}

// weight increasing, end increasing, begin decreasing, id increasing
bool testCmpXEndBeg(TestX x1, TestX x2)
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

// weight decreasing, end increasing, begin decreasing, id increasing
bool testCmpXStandard(TestX x1, TestX x2)
{
	/*if (x1._X._w > x2._X._w)
	{
	return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._e < x2._X._e)
	{
	return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._e == x2._X._e && x1._X._s > x2._X._s)
	{
	return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._e == x2._X._e && x1._X._s == x2._X._s && x1._X._id < x2._X._id)
	{
	return true;
	}
	return false;*/
	if (x1._X._w > x2._X._w)
	{
		return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._id > x2._X._id)
	{
		return true;
	}
	return false;
}

// weight decreasing, end increasing, begin decreasing, id increasing
bool testCmpXStandard_TEMP(TestX x1, TestX x2)
{
	if (x1._X._w > x2._X._w)
	{
		return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._e < x2._X._e)
	{
		return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._e == x2._X._e && x1._X._s > x2._X._s)
	{
		return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._e == x2._X._e && x1._X._s == x2._X._s && x1._X._id < x2._X._id)
	{
		return true;
	}
	return false;
}

// return the OIS in the glover mathcing of a CBG
void gloverMatching(const vector<X>& vX, const vector<Y>& vY, vector<X>* vZ)
{
	vector<TestX> XX;
	vector<Y> YY;
	for (int i = 0; i < (int)vX.size(); i++)
	{
		TestX sx;
		sx._X = vX[i];
		XX.push_back(sx);
	}
	YY = vY;
	/*for (int i = 0; i < (int)vY.size(); i++)
	{
	YY.push_back(vY[i]);
	}*/
	sort(XX.begin(), XX.end(), testCmpXStandard);
	sort(YY.begin(), YY.end(), cmpYInc);

	vZ->clear();
	for (int i = 0; i < (int)YY.size(); i++)
	{
		vector<TestX> W;	// the unmatched x set that may match with YY[i]
		for (int j = 0; j < (int)XX.size(); j++)
		{
			if (XX[j]._X._e >= YY[i] && XX[j]._X._s <= YY[i] && XX[j].flag == 0)
			{
				W.push_back(XX[j]);
			}
		}

		if ((int)W.size() > 0)
		{
			sort(W.begin(), W.end(), testCmpXEndBeg);
			vZ->push_back(W[0]._X);
			vector<TestX>::iterator it = find(XX.begin(), XX.end(), W[0]);
			it->flag = 1;
		}
	}
}

// return the OIS in the plaxton MWM of a LWCBG
void PlaxtonMWM(const vector<X>& vX, const vector<Y>& vY, vector<X>* vZ)
{
	vector<TestX> XX;
	vector<Y> YY;
	for (int i = 0; i < (int)vX.size(); i++)
	{
		TestX sx;
		sx._X = vX[i];
		XX.push_back(sx);
	}
	YY = vY;
	/*for (int i = 0; i < (int)vY.size(); i++)
	{
	YY.push_back(vY[i]);
	}*/
	sort(XX.begin(), XX.end(), testCmpXStandard);
	sort(YY.begin(), YY.end(), cmpYInc);

	vZ->clear();
	for (int i = 0; i < (int)XX.size(); i++)
	{
		vector<X> tmpZ;
		vZ->push_back(XX[i]._X);
		gloverMatching(*vZ, YY, &tmpZ);
		if (tmpZ.size() < vZ->size())
		{
			vector<X>::iterator it = find(vZ->begin(), vZ->end(), XX[i]._X);
			vZ->erase(it);
		}
	}
}