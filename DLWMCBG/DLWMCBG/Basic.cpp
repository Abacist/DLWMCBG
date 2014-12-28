#include<fstream>
#include<Windows.h>
#include"Basic.h"
#pragma warning (disable:4018)

void generator(char* fileName, int MaxY, int UpdateRange, int WeightRange)
{
	ofstream out(fileName);
	/*out << MaxY+1 << endl;
	for (int i = 0; i <= MaxY; i++)
	{
		out << i << " ";
	}
	out << endl;*/
	out << 1 << endl << 0 << endl;
	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	srand(lpsystime.wMinute * 1000 + lpsystime.wMilliseconds);
	int i = 0;
	for (; i < UpdateRange; i++)
	{
		if ((rand() % 2) == 0)		// insert x or insert y
		{
			int s = rand() % MaxY + 1;
			int e = rand() % (MaxY*2) + 1;
			if (s > e)
			{
				int temp = e;
				e = s;
				s = temp;
			}
			
			out << 1 << " " << i + 1 << " " << 0 << " " << e << " " << rand() % WeightRange << endl;
		}
		else
		{
			out << 3 << " " << rand() % MaxY + 1 <<" "<<rand()%WeightRange<< endl;
		}
		
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

	_aX = _bX = _aMX = _bMX = _aIX = _bIX = _aTX = _bTX = x;
	_aY = _bY = _aMY = _bMY = _aIY = _bIY = y;
}

ostream& operator<<(ostream& os, const Y& rhs)
{
	os << rhs._id<<rhs._w;
	return os;
}

ostream& operator<<(ostream& os, const X& rhs)
{
	os << "X: " << rhs._id << " " << rhs._s << " " << rhs._e << " " << rhs._w;
	return os;
}



// order y according to its index
bool cmpYIDInc(Y y1, Y y2)
{
	return y1._id < y2._id;
}

bool cmpYIDDec(Y y1, Y y2)
{
	return y1._id > y2._id;
}

bool cmpYWeightInc(Y y1, Y y2)
{
	if (y1._w < y2._w)
	{
		return true;
	}
	if (y1._w == y2._w && y1._id < y2._id)
	{
		return true;
	}
	return false;
}

// priority: increasing end
bool cmpXEndInc(X x1, X x2)
{
	if (x1._e < x2._e)
	{
		return true;
	}
	if (x1._e == x2._e && x1._s <x2._s)
	{
		return true;
	}
	if (x1._e == x2._e && x1._s == x2._s && x1._id < x2._id)
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



// return the OIS in the glover mathcing of a CBG
void formGloverMatching(const vector<X> vX, const vector<Y> vY, vector<X>& vZ)
{
	vector<X> XX = vX;
	vector<Y> YY = vY;
	vZ.clear();

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

			vZ.push_back(x);
			vector<X>::iterator it = find(XX.begin(),XX.end(), x);
			XX.erase(it);
			if (vZ.size() == vX.size() || vZ.size() == vY.size())
			{
				return;
			}
		}
	}
	return;
}

// return the OIS in the plaxton MWM of a LWCBG
void formPlaxtonMWM(const vector<X> vX, const vector<Y> vY, vector<X>& vZ, vector<Y>& vMY)
{
	vector<X> XX = vX;
	vector<Y> YY = vY;
	
	sort(XX.begin(), XX.end(), cmpXWeightInc);
	sort(YY.begin(), YY.end(), cmpYIDInc);

	vZ.clear();
	for (int i = XX.size() - 1; i >= 0;  i--)
	{
		vector<X> tmpZ;
		vZ.push_back(XX[i]);
		formGloverMatching(vZ, YY, tmpZ);
		if (tmpZ.size() < vZ.size())
		{
			vector<X>::iterator it = find(vZ.begin(), vZ.end(), XX[i]);
			vZ.erase(it);
		}
		if (vZ.size() == vX.size() || vZ.size() == vY.size())
		{
			break;
		}
	}
	//vZ finished
	vMY.clear();
	sort(YY.begin(), YY.end(), cmpYWeightInc);
	for (int i = YY.size() - 1; i >= 0; i--)
	{
		vector<X> tempZ;
		vMY.push_back(YY[i]);
		formGloverMatching(vZ, vMY, tempZ);
		if (tempZ.size() < vMY.size())
		{
			vMY.erase(find(vMY.begin(), vMY.end(), YY[i]));
		}
		if (vMY.size() == vZ.size())
		{
			break;
		}
	}
}