#include<algorithm>
#include<fstream>
#include"Tree.h"
#include"Basic.h"
#pragma warning (disable:4018)


// Methods in TreeNode
TreeNode::TreeNode(vector<Y> vecY)
{
	sort(vecY.begin(), vecY.end(), cmpYInc);
	if (vecY[0]._id < 0)
	{
		throw new exception();
	}
	for (int i = 0; i < (int)vecY.size(); i++)
	{
		_Y.push_back(vecY[i]);
	}
	sort(_Y.begin(), _Y.end(), cmpYInc);	// sort Y after each _Y augmentation
	_leftChild = NULL;
	_rightChild = NULL;
	_parentNode = NULL;
}

Y TreeNode::getIntervalStart()
{
	//Y is always sorted
	return _Y[0];
}


// split a leaf into three nodes, a parent with two children.
void TreeNode::splitDSNode(X x)
{
	vector<Y> leftVecY, rightVecY;
	int i = 0;

	// split the values into two, one to left leaf, the other to right leaf
	while (_Y[i]._id < x._s._id)
	{
		leftVecY.push_back(_Y[i]);
		i++;
	}
	while (i < (int)_Y.size())
	{
		rightVecY.push_back(_Y[i]);
		i++;
	}

	// build the structure of the triple first, then insert the new variable
	TreeNode* leftChild = new TreeNode(leftVecY);
	TreeNode* rightChild = new TreeNode(rightVecY);

	//leftChild->_X = _X;	// copy the variables from the parent

	_leftChild = leftChild;
	_rightChild = rightChild;
	leftChild->_parentNode = this;
	rightChild->_parentNode = this;
	updateAuxSet4Split();
}

// adjust the sets in the process of splitting, call by the splited node, the standard process need to be discussed
void TreeNode::updateAuxSet4Split()
{
	//_leftChild->_T = _T;
	_leftChild->_X = _X;
	vector<X> restX = _Z;
	for (int i = 0; i < _I.size(); i++)
	{
		restX.push_back(_I[i]);
	}
	for (int i = 0; i < _T.size(); i++)
	{
		restX.push_back(_T[i]);
	}
	_ZR.clear();
	//_ZL.clear();
	_Z.clear();
	_I.clear();
	_T.clear();
	//part of left side msg passing, _ZL and left->Z keeps the same
	for (int i = 0; i < restX.size(); i++)
	{
		Msg msg = _leftChild->insertXintoESinNode(restX[i]);
		if (msg._aI._id == -1 && msg._aT._id == -1)
		{
			//success
			_Z.push_back(msg._aZ);
			_ZL.push_back(msg._aZ);
		}

		else if (msg._aI._id == -1 && msg._aT._id != -1)
		{
			//transfer in left
			_Z.push_back(msg._aZ);

			vector<X>::iterator it = find(_Z.begin(), _Z.end(), msg._bZ);
			if (it != _Z.end())
			{
				vector<X> R;
				determineReachableSetinEE(msg._aZ, R, *new bool);
				R.push_back(msg._aZ);
				_ZL.push_back(msg._aZ);
				sort(R.begin(), R.end(), cmpXEndInc);
				X maxEndinR = R[R.size() - 1];
				it = find(_Z.begin(), _Z.end(), maxEndinR);
				_Z.erase(it);
				it = find(_ZL.begin(), _ZL.end(), maxEndinR);
				_ZL.erase(it);
				insertXintoESinNode(maxEndinR);
			}
			else
			{
				vector<X> R;
				determineReachableSetinEE(msg._aZ, R, *new bool);//ES or EE? In Split or Msg Passing?
				//when the R is correct, the below code is right
				R.push_back(msg._aZ);
				_ZL.push_back(msg._aZ);
				sort(R.begin(), R.end(), cmpXEndInc);
				X maxEndinR = R[R.size() - 1];
				if (maxEndinR._e < _rightChild->getIntervalStart())
				{
					//not transfer in P
					sort(R.begin(), R.end(), cmpXWeightInc);
					vector<X>::iterator it = find(_Z.begin(), _Z.end(), R[0]);
					_Z.erase(it);
					it = find(_ZL.begin(), _ZL.end(), R[0]);
					_ZL.erase(it);
					_I.push_back(R[0]);
				}
				else
				{
					vector<X>::iterator it = find(_Z.begin(), _Z.end(), maxEndinR);
					_Z.erase(it);
					it = find(_ZL.begin(), _ZL.end(), maxEndinR);
					_ZL.erase(it);
					insertXintoESinNode(maxEndinR);
				}

			}
		}
		else if (msg._aI._id != -1 && msg._aT._id == -1)
		{
			//infeasible in Left
			_Z.push_back(msg._aZ);
			_ZL.push_back(msg._aZ);
			vector<X>::iterator it = find(_Z.begin(), _Z.end(), msg._bZ);
			if (it != _Z.end())
			{
				_Z.erase(it);
				it = find(_ZL.begin(), _ZL.end(), msg._bZ);
				_ZL.erase(it);
				_I.push_back(msg._aI);
				//msg keeps
			}
			else
			{
				vector<X>::iterator it = find(_ZL.begin(), _ZL.end(), msg._aZ);
				_ZL.erase(it);
				vector<X> R;
				determineReachableSetinEE(msg._aZ, R, *new bool);//ES or EE? In Split or Msg Passing?
				//when the R is right, the below code is right
				R.push_back(msg._aZ);
				_ZL.push_back(msg._aZ);
				sort(R.begin(), R.end(), cmpXEndInc);
				X maxEndinR = R[R.size() - 1];
				if (maxEndinR._e < _rightChild->getIntervalStart())
				{
					//not transfer in P
					sort(R.begin(), R.end(), cmpXWeightInc);
					vector<X>::iterator it = find(_Z.begin(), _Z.end(), R[0]);
					_Z.erase(it);
					it = find(_ZL.begin(), _ZL.end(), R[0]);
					_ZL.erase(it);
					_I.push_back(R[0]);
				}
				else
				{
					vector<X>::iterator it = find(_Z.begin(), _Z.end(), maxEndinR);
					_Z.erase(it);
					it = find(_ZL.begin(), _ZL.end(), maxEndinR);
					_ZL.erase(it);
					insertXintoESinNode(maxEndinR);
					//_aZ, _aX keeps
				}

			}
		}
	}

}



int TreeNode::verifyNodeInvariants()
{

	sort(_Y.begin(), _Y.end(), cmpYInc);

	

	// invariant \phi_1: \nexists x\in T, x.e<=Y.e
	for (int i = 0; i < (int)_T.size(); i++)
	{
		if (_T[i]._e <= _Y[_Y.size() - 1])
		{
			return 1;
		}
	}

	// invariant \phi_2: \nexists x\in T, Z+x-x'\in \I
	if (_T.empty() != true)
	{
		sort(_T.begin(), _T.end(), cmpXEndInc);

		for (int i = 0; i < (int)_Z.size(); i++)
		{
			//if (_Z[i]._e > _T[0]._e)
			if (cmpXEndInc(_T[0], _Z[i]))
			{
				X z = _Z[i];
				int j = 0;
				//while (z._e > _T[j]._e)
				while (cmpXEndInc(_T[j], z))
				{
					vector<X> ZZ = _Z;
					vector<X>::iterator it = find(ZZ.begin(), ZZ.end(), z);
					ZZ.erase(it);
					ZZ.push_back(_T[j]);

					vector<X> tmpZ;
					gloverMatching(ZZ, _Y, &tmpZ);
					if (tmpZ.size() == ZZ.size())		// Z+x-x'\in \I
					{
						return 2;
					}
					j++;
					if (j >= _T.size())
					{
						break;
					}
				}
			}
		}
	}

	// invariant \phi_3: \nexists x\in I, x.e>Y.e
	for (int i = 0; i < (int)_I.size(); i++)
	{
		if (_I[i]._e > _Y[_Y.size() - 1])
		{
			return 3;
		}
	}

	// invariant \phi_4: \nexists x\in I, Z+x-x'\in \I, where x'.e>Y.e
	sort(_Z.begin(), _Z.end(), cmpXEndInc);
	if (!_I.empty() && _Z.size() > 0 && _Z[_Z.size() - 1]._e > _Y[_Y.size() - 1])
	{
		vector<X> Z1;
		for (int i = 0; i < (int)_Z.size(); i++)
		{
			if (_Z[i]._e > _Y[_Y.size() - 1])
			{
				Z1.push_back(_Z[i]);
			}
		}

		for (int i = 0; i < (int)Z1.size(); i++)
		{
			for (int j = 0; j < (int)_I.size(); j++)
			{
				vector<X> Z2 = _Z;
				vector<X>::iterator it = find(Z2.begin(), Z2.end(), Z1[i]);
				Z2.erase(it);
				Z2.push_back(_I[j]);

				vector<X> tmpZ;
				gloverMatching(Z2, _Y, &tmpZ);
				if (tmpZ.size() == Z2.size())		// Z+x-x'\in \I
				{
					return 4;
				}
			}
		}
	}

	// invariant \phi_5: Z of the maximum cardinaltiy
	vector<X> tmpZ;
	vector<X> tmpX;
	for (int i = 0; i < (int)_Z.size(); i++)
		tmpX.push_back(_Z[i]);
	for (int i = 0; i < (int)_T.size(); i++)
		tmpX.push_back(_T[i]);
	for (int i = 0; i < (int)_I.size(); i++)
		tmpX.push_back(_I[i]);
	gloverMatching(tmpX, _Y, &tmpZ);
	if (tmpZ.size() != _Z.size())
	{
		return 5;
	}

	// invariant \phi_6: Z of the maximum weight w.r.t. X-T
	vector<X> tmpZ2;
	vector<X> tmpX2;
	for (int i = 0; i < (int)_Z.size(); i++)
		tmpX2.push_back(_Z[i]);
	for (int i = 0; i < (int)_I.size(); i++)
		tmpX2.push_back(_I[i]);
	PlaxtonMWM(tmpX2, _Y, &tmpZ2);
	if (tmpZ2.size() != _Z.size())
	{
		return 6;
	}
	sort(_Z.begin(), _Z.end(), cmpXID);
	sort(tmpZ2.begin(), tmpZ2.end(), cmpXID);
	for (int i = 0; i < (int)_Z.size(); i++)
	{
		if (!(_Z[i] == tmpZ2[i]))
		{
			return 6;
		}
	}

	

	//X = Z+I+T
	vector<X> ZIT = _Z;
	for (int i = 0; i < _I.size(); i++)
	{
		ZIT.push_back(_I[i]);
	}
	for (int i = 0; i < _T.size(); i++)
	{
		ZIT.push_back(_T[i]);
	}
	if (ZIT.size() != _X.size())
	{
		return 7;
	}
	else
	{
		sort(ZIT.begin(), ZIT.end(), cmpXID);
		sort(_X.begin(), _X.end(), cmpXID);
		for (int i = 0; i < ZIT.size(); i++)
		{
			if (ZIT[i]._id != _X[i]._id)
			{
				return 7;
			}
		}
	}


	// invariant, for test insertY: |Y_P| = |Y_R| + |Y_L|
	if (_leftChild != NULL)
	{
		if (_Y.size() != _leftChild->_Y.size() + _rightChild->_Y.size())
		{
			return 8;
		}
	}

	if (_leftChild != NULL)
	{
		if (_ZL.size() != _leftChild->_Z.size())
		{
			return 9;
		}
	}

	//Z = ZL + ZR
	vector<X> Z1 = _Z;
	vector<X> Z2 = _ZL;
	for (int i = 0; i < _ZR.size(); i++)
	{
		Z2.push_back(_ZR[i]);
	}
	if (Z1.size() != Z2.size())
	{
		return 10;
	}
	else
	{
		sort(Z1.begin(), Z1.end(), cmpXID);
		sort(Z2.begin(), Z2.end(), cmpXID);
		for (int i = 0; i < Z1.size(); i++)
		{
			if (Z1[i] != Z2[i])
			{
				return 10;
			}
		}
	}
	
	//ZL and ZR are nice
	vector<X> tempZLR;
	vector<Y> tempY;
	if (_leftChild != NULL)
	{
		tempY = _leftChild->_Y;
		gloverMatching(_ZL, tempY, &tempZLR);
		if (tempZLR.size() != _ZL.size())
			return 11;
		tempY = _rightChild->_Y;
		gloverMatching(_ZR, tempY, &tempZLR);
		if (tempZLR.size() != _ZR.size())
			return 11;
	}


	return 0;
}


// Methods in Tree
Tree::Tree(vector<Y> vecY)
{
	_root = new TreeNode(vecY);
}

int Tree::verifyInvariantsRecur()
{
	return verifyInvariantsRecur(_root);
}

int Tree::verifyInvariantsRecur(TreeNode* curRoot)
{
	if (curRoot->_leftChild == NULL)
	{
		return curRoot->verifyNodeInvariants();
	}
	else
	{
		int flagL = verifyInvariantsRecur(curRoot->_leftChild);
		int flagR = verifyInvariantsRecur(curRoot->_rightChild);
		if (flagL == 0 && flagR == 0)
		{
			return curRoot->verifyNodeInvariants();
		}
		else
		{
			return max(flagL, flagR);
		}
	}
}


int Tree::verifyInvariantsInUnweightedCase()
{
	return verifyInvariantsInUnweightedCase(_root);
}

int Tree::verifyInvariantsInUnweightedCase(TreeNode* curRoot)
{
	if (curRoot->_leftChild != NULL)
	{
		int flagL = verifyInvariantsInUnweightedCase(curRoot->_leftChild);
		int flagR = verifyInvariantsInUnweightedCase(curRoot->_rightChild);
		if (flagL != 0 || flagR != 0)
		{
			return 1;
		}
		vector<X> PZL = curRoot->_ZL;
		vector<X> LZ = curRoot->_leftChild->_Z;
		sort(PZL.begin(), PZL.end(), cmpXID);
		sort(LZ.begin(), LZ.end(), cmpXID);
		if (PZL.size() != LZ.size())
		{
			return 1;
		}
		for (int i = 0; i < PZL.size(); i++)
		{
			if (PZL[i] != LZ[i])
			{
				return 1;
			}
		}
	}


	return 0;
}


// Note: if there is no such tight point, return y with y.id=Y.e+1
Y TreeNode::rightBetaTightPointforZL(Y y)
{
	//before the y is inserted
	vector<X> tempZ;
	vector<Y> tempY;
	if (this->_rightChild != NULL)
	{
		tempZ = _ZL;
		for (int i = 0; i < _Y.size(); i++)
		{
			if (_Y[i] < _rightChild->getIntervalStart())
			{
				tempY.push_back(_Y[i]);
			}
		}
	}
	else
	{
		tempZ = _Z;//equals to _ZR
		tempY = _Y;
	}
	sort(tempY.begin(), tempY.end(), cmpYDec);
	sort(tempZ.begin(), tempZ.end(), cmpXBeginDec);
	Y tY;
	tY._id = -1;

	for (int i = 0; i < tempZ.size(); i++)
	{
		if (tempY[i] == tempZ[i]._s && tempY[i] > y)
		{
			tY = tempY[i];
		}
	}
	if (tY._id == -1)
	{
		tY._id = tempY[0]._id + 1;
	}
	return tY;
}

// Note: if there is no such tight point, return y with y.id=Y.e+1
Y TreeNode::rightBetaTightPointforZR(Y y)
{
	//before the y is inserted
	vector<X> tempZ;
	vector<Y> tempY;
	if (this->_rightChild != NULL)
	{
		tempZ = _ZR;
		for (int i = 0; i < _Y.size(); i++)
		{
			if (_Y[i] >= _rightChild->getIntervalStart())
			{
				tempY.push_back(_Y[i]);
			}
		}
	}
	else
	{
		tempZ = _Z;//equals to _ZR
		tempY = _Y;
	}
	sort(tempY.begin(), tempY.end(), cmpYDec);
	sort(tempZ.begin(), tempZ.end(), cmpXBeginDec);
	Y tY;
	tY._id = -1;

	for (int i = 0; i < tempZ.size(); i++)
	{
		if (tempY[i] == tempZ[i]._s && tempY[i] > y)
		{
			tY = tempY[i];
		}
	}
	if (tY._id == -1)
	{
		tY._id = tempY[0]._id + 1;
	}
	return tY;
}

// Note: if there is no such tight point, return the least y in P._Y
Y TreeNode::leftBetaTightPointforZL(Y y)
{
	vector<X> tempZ;
	vector<Y> tempY;
	if (this->_rightChild != NULL)
	{
		tempZ = _ZL;
		for (int i = 0; i < _Y.size(); i++)
		{
			if (_Y[i] < _rightChild->getIntervalStart())
			{
				tempY.push_back(_Y[i]);
			}
		}
	}
	else
	{
		throw new exception();
	}
	sort(tempY.begin(), tempY.end(), cmpYDec);
	sort(tempZ.begin(), tempZ.end(), cmpXBeginDec);
	Y tY;
	tY._id = -1;
	
	for (int i = tempZ.size() - 1; i >= 0; i--)
	{
		if (tempY[i] == tempZ[i]._s && tempY[i] <= y)
		{
			tY = tempY[i];
		}
	}
	if (tY._id == -1 && _ZR.size() == tempY.size())
	{
		return tempY[0];
	}
	return tY;
}

// Note: if there is no such tight point, return the greatest y in P._Y
Y TreeNode::rightAlphaTightPointforZR(Y y)
{
	vector<X> tempZ;
	vector<Y> tempY;
	if (this->_rightChild != NULL)
	{
		tempZ = _ZR;
		for (int i = 0; i < _Y.size(); i++)
		{
			if (_Y[i] >= _rightChild->getIntervalStart())
			{
				tempY.push_back(_Y[i]);
			}
		}
	}
	else
	{
		tempZ = _Z;//equals to _ZR
		tempY = _Y;
	}
	sort(tempY.begin(), tempY.end(), cmpYInc);
	sort(tempZ.begin(), tempZ.end(), cmpXEndInc);
	Y tY;
	tY._id = -1;

	// if the ES part is full, return the last one.
	// Note: in this case, it maybe not a real tight point.
	if (_ZR.size() == tempY.size())
	{
		return tempY[tempY.size() - 1];
	}
	for (int i = tempZ.size() - 1; i >= 0; i--)
	{
		if (tempY[i] == tempZ[i]._e && tempY[i] >= y)
		{
			tY = tempY[i];
		}
	}
	return tY;
}

// return the \alpha tightest point that is less than y; return y with y.id=-1 if there is no such one.
Y TreeNode::leftAlphaTightPointforZR(Y y)
{
	vector<X> tempZ;
	vector<Y> tempY;
	if (this->_rightChild != NULL)
	{
		tempZ = _ZR;
		for (int i = 0; i < _Y.size(); i++)
		{
			if (_Y[i] >= _rightChild->getIntervalStart())
			{
				tempY.push_back(_Y[i]);
			}
		}
	}
	else
	{
		tempZ = _Z;//equals to _ZR
		tempY = _Y;
	}
	sort(tempY.begin(), tempY.end(), cmpYInc);
	sort(tempZ.begin(), tempZ.end(), cmpXEndInc);
	Y tY;
	tY._id = -1;

	for (int i = 0; i < tempZ.size(); i++)
	{
		if (tempY[i] == tempZ[i]._e && tempY[i] < y)
		{
			tY = tempY[i];
		}
	}
	return tY;
}


