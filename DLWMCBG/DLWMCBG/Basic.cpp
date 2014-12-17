#include<fstream>
#include<Windows.h>
#include"Basic.h"
#pragma warning (disable:4018)

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
		out << 1 << " " << i + 1 << " " << s << " " << e << " " << 0/*rand() % WeightRange*/ << endl;

		//out << 1 << " " << i + 1 << " " << 1 << " " << rand() % MaxY + 1 << " " << rand() % MaxY * 10 << endl;;
		//out << 1 << " " << i + 2 << " " << 3 << " " << rand() % MaxY + 1 << " " << rand() % WeightRange  + 1 << endl;
	}
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
	if (x1._e < x2._e)
	{
		return true;
	}
	if (x1._e == x2._e && x1._id < x2._id)
	{
		return true;
	}
	return false;//x1._e < x2._e;
}

bool cmpXBeginDec(X x1, X x2)
{
	if (x1._s > x2._s)
	{
		return true;
	}
	if (x1._s == x2._s && x1._id > x2._id)
	{
		return true;
	}
	return false;//x1._e < x2._e;
	//return x1._s > x2._s;

}

// increasing start
/*bool cmpXStartInc(X x1, X x2)
{
	return x1._s < x2._s;
}*/

bool cmpXWeightInc(X x1, X x2)
{
	if (x1._w < x2._w)
	{
		return true;
	}
	if (x1._w == x2._w)
	{
		return !cmpXEndInc(x1, x2);
	}
	return false;
}

bool cmpXID(X x1, X x2)
{
	return x1._id < x2._id;
}

/*bool cmpXEndBeginIdInc(X x1, X x2)
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
}*/

// weight decreasing, end increasing, begin decreasing, id increasing
/*bool cmpXStandard(X x1, X x2)
{
	if (x1._w > x2._w)
	{
		return true;
	}
	else if (x1._w == x2._w && x1._id > x2._id)
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
}*/

// end increasing, begin decreasing, increasing id
/*bool cmpXEndIncStartDec(X x1, X x2)
{
	if (x1._e < x2._e)
	{
		return true;
	}
	else if (x1._e == x2._e && x1._s > x2._s)
	{
		return true;
	}
	else if (x1._e == x2._e && x1._s == x2._s && x1._id < x2._id)
	{
		return true;
	}
	return false;
}*/

// For Test
/*struct TestX
{
	X _X;
	int flag = 0;
	bool operator==(const TestX& x)
	{
		return this->_X._id == x._X._id;
	}
};*/

// increasing start
/*bool testCmpXStartInc(TestX x1, TestX x2)
{
	return x1._X._s < x2._X._s;
}*/

// weight increasing, end increasing, begin decreasing, id increasing
/*bool testCmpXEndBeg(TestX x1, TestX x2)
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
}*/

// weight decreasing, end increasing, begin decreasing, id increasing
/*bool testCmpXStandard(TestX x1, TestX x2)
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
	if (x1._X._w > x2._X._w)
	{
		return true;
	}
	else if (x1._X._w == x2._X._w && x1._X._id > x2._X._id)
	{
		return true;
	}
	return false;
}*/

// weight decreasing, end increasing, begin decreasing, id increasing
/*bool testCmpXStandard_TEMP(TestX x1, TestX x2)
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
}*/

// return the OIS in the glover mathcing of a CBG
void gloverMatching(const vector<X>& vX, const vector<Y>& vY, vector<X>* vZ)
{
	vector<X> XX = vX;
	vector<Y> YY = vY;
	vZ->clear();

	for (int i = 0; i < YY.size(); i++)
	{
		vector<X> selectedX;
		for (int j = 0; j < XX.size(); j++)
		{
			if (XX[j]._s <= YY[i] && XX[j]._e >= YY[i])
			{
				selectedX.push_back(XX[j]);
			}
		}

		if (selectedX.size() == 0)
		{
			continue;
		}
		else
		{
			X x = selectedX[0];
			for (int j = 0; j < selectedX.size(); j++)
			{
				if (cmpXEndInc(selectedX[j],x))
					//(selectedX[j]._e == x._e && selectedX[j]._s < x._s)
					//|| (selectedX[j]._e == x._e && selectedX[j]._s == x._s && selectedX[j]._id < x._id))
				{
					x = selectedX[j];
				}
			}

			vZ->push_back(x);
			vector<X>::iterator it = find(XX.begin(),XX.end(), x);
			XX.erase(it);
			if (vZ->size() == vX.size() || vZ->size() == vY.size())
			{
				return;
			}
		}
	}
	return;
}

// return the OIS in the plaxton MWM of a LWCBG
void PlaxtonMWM(const vector<X>& vX, const vector<Y>& vY, vector<X>* vZ)
{
	vector<X> XX = vX;
	vector<Y> YY = vY;
	
	sort(XX.begin(), XX.end(), cmpXWeightInc);
	sort(YY.begin(), YY.end(), cmpYInc);

	vZ->clear();
	for (int i = XX.size() - 1; i >= 0;  i--)
	{
		vector<X> tmpZ;
		vZ->push_back(XX[i]);
		gloverMatching(*vZ, YY, &tmpZ);
		if (tmpZ.size() < vZ->size())
		{
			vector<X>::iterator it = find(vZ->begin(), vZ->end(), XX[i]);
			vZ->erase(it);
		}
		if (vZ->size() == vX.size() || vZ->size() == vY.size())
		{
			return;
		}
	}
}