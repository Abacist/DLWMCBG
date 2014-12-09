//implemention of Tree

#include<algorithm>
#include<fstream>
#include"Tree.h"
#pragma warning (disable:4018)

// Methods in TreeNode
TreeNode::TreeNode(vector<Y> vecY)
{
	sort(vecY.begin(), vecY.end(), cmpYInc);
	if (vecY[0]._id <= 0)
	{
		__debugbreak();
	}
	for (int i = 0; i < (int)vecY.size(); i++)
	{
		_Y.push_back(vecY[i]);
	}
	_leftChild = NULL;
	_rightChild = NULL;
	_parentNode = NULL;
}

Y TreeNode::getIntervalStart()
{
	return _Y[0];
}

// insert x.s or x.e in Y if it does not exist
void Tree::adjustXToProper(X x)
{
	vector<Y>::iterator it = find(_root->_Y.begin(), _root->_Y.end(), x._s);
	if (it == _root->_Y.end())
	{
		insertYinTree(x._s);
	}

	it = find(_root->_Y.begin(), _root->_Y.end(), x._e);
	if (it == _root->_Y.end())
	{
		insertYinTree(x._e);
	}
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
			_ZL.push_back(msg._aZ);
			vector<X>::iterator it = find(_Z.begin(), _Z.end(), msg._bZ);
			if (it != _Z.end())
			{
				vector<X> R;
				determineReachableSetinEE(msg._aZ, R, *new bool);
				R.push_back(msg._aZ);
				sort(R.begin(), R.end(), cmpXEndInc);
				X maxEndinR = R[R.size() - 1];
				it = find(_Z.begin(), _Z.end(), maxEndinR);
				_Z.erase(it);
				it = find(_ZL.begin(), _ZL.end(), maxEndinR);
				_ZL.erase(it);
				insertXintoESinNode(maxEndinR);


				/*_Z.erase(it);//has bug
				it = find(_ZL.begin(), _ZL.end(), msg._bZ);
				_ZL.erase(it);
				insertXintoESinNode(msg._aT);*/
			}
			else
			{
				vector<X> R;
				determineReachableSetinEE(msg._aZ, R, *new bool);//ES or EE? In Split or Msg Passing?
				R.push_back(msg._aZ);
				sort(R.begin(), R.end(), cmpXEndInc);
				X maxEndinR = R[R.size() - 1];
				if (maxEndinR._e < _rightChild->getIntervalStart())
				{
					//not transfer in P
					sort(R.begin(), R.end(), cmpXWeightIDInc);
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
			}
			else
			{
				//_bZ has already been preempted. in Split, we can directly use the ES structure of ZL
				//but how to do in the msg passing?
				/*vector<X> R;
				determineReachableSetinEE(msg._aZ, R, *new bool);//ES or EE? In Split or Msg Passing?
				R.push_back(msg._aZ);
				sort(R.begin(), R.end(), cmpXWeightIDInc);
				it = find(_Z.begin(), _Z.end(), R[0]);
				_Z.erase(it);
				it = find(_ZL.begin(), _ZL.end(), R[0]);
				_ZL.erase(it);
				_I.push_back(R[0]);*/
				vector<X> R;
				determineReachableSetinEE(msg._aZ, R, *new bool);//ES or EE? In Split or Msg Passing?
				//when the R is right, the below code is right
				R.push_back(msg._aZ);
				sort(R.begin(), R.end(), cmpXEndInc);
				X maxEndinR = R[R.size() - 1];
				if (maxEndinR._e < _rightChild->getIntervalStart())
				{
					//not transfer in P
					sort(R.begin(), R.end(), cmpXWeightIDInc);
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
	}


	/*TreeNode* leftChild = _leftChild;
	Msg msg;
	X tmpX;
	vector<X> tempMatched = _Z;
	_Z.clear();
	vector<X> tempMatched2 = _ZR;
	_ZR.clear();

	leftChild->_I = _I;//problem
	leftChild->_T = _T;

	for (int i = 0; i < (int)tempMatched.size(); i++)
	{
	tmpX = tempMatched[i];
	msg = leftChild->insertXintoESinNode(tmpX);		// insert into the left child node L
	switch (msg.flagInsertX())
	{
	case 0:	// matched
	{
	_Z.push_back(tmpX);
	} break;
	case 1:	// transferred, add a into matched, delete b from matched, and add b into ES
	{
	_Z.push_back(tmpX);
	vector<X>::iterator it = find(_Z.begin(), _Z.end(), msg._bZ);	// _bZ should be equal to _aT
	_Z.erase(it);
	insertXintoESinNode(msg._bZ);	// insert into the parent node P
	} break;
	case 2:
	{
	//no infeasible case
	} break;
	}
	}*/
}

// return the least tight piont greater than y in ES 
/*X TreeNode::findjInES(vector<Y>* pESY, Y y)
{
// assume y is between Y.s and Y.e
sort((*pESY).begin(), (*pESY).end(), cmpYInc);
sort(_ZR.begin(), _ZR.end(), cmpXEndInc);
//ZR must be no more greater than pESY
for (int i = 0; i < (int)_ZR.size(); i++)//why 1, not 0
{
if ((*pESY)[i] < y)
{
continue;
}
else if ((*pESY)[i] == _ZR[i]._e)
{
return _ZR[i];	// the least tight point greater or equal than y
}
}
//deal with the last element, i.e., the fake end
if (_ZR.size() == pESY->size())
{
return _ZR[_ZR.size()-1];
}
X x1;
x1._id = -1;
return x1;	// there is no tight point after y
}*/

// return the tightest point that is less than y; return y with y.id=-1 if there is no such one.
Y TreeNode::leftAlphaTightPoint(Y y, TreeNode * node)
{
	if (node == NULL)
	{
		vector<int> t; t.erase(t.begin());
	}
	vector<Y> YY = node->_Y;
	vector<X> ZZ = node->_Z;

	sort(YY.begin(), YY.end(), cmpYInc);
	sort(ZZ.begin(), ZZ.end(), cmpXEndInc);
	for (int i = (int)ZZ.size() - 1; i >= 0; i--)
	{
		if (ZZ[i]._e == YY[i] && YY[i] < y)
		{
			return YY[i];
		}
	}

	Y tmpY;
	tmpY._id = -1;
	return tmpY;
}

void TreeNode::determineReachableSetinES(X x, vector<X>& R, bool& isTight)
{
	vector<Y> ESY;
	if (_rightChild == NULL)
	{
		ESY = _Y;
	}
	else
	{
		ESY = _rightChild->_Y;
	}
	sort(ESY.begin(), ESY.end(), cmpYInc);
	if (x._e < ESY[0])
	{
		return;
	}
	// not necessay, since if x.e does not exist, add it before insert x.
	//else
	//{
	//	// if x.e does not exist in Y, set it as the greatest one less than x.e
	//	vector<Y>::iterator it = find(ESY.begin(), ESY.end(), x._e);
	//	if (it == ESY.end())
	//	{
	//		for (int i = (int)ESY.size() - 1; i >= 0; i--)
	//		{
	//			if (ESY[i] < x._e)
	//			{
	//				x._e = ESY[i];
	//				break;
	//			}
	//		}
	//	}
	//}

	sort(_ZR.begin(), _ZR.end(), cmpXEndInc);
	for (int i = 0; i < _ZR.size(); i++)
	{
		R.push_back(_ZR[i]);
		if (_ZR[i]._e == ESY[i] && ESY[i] >= x._e)
		{
			isTight = true;
			return;
		}
	}
	if (_ZR.size() == ESY.size())
	{
		isTight = true;
	}
	else
	{
		isTight = false;
	}
}

void TreeNode::determineReachableSetinEE(X x, vector<X>& R, bool& isTight)
{
	vector<Y> EEY;
	if (_leftChild == NULL)
	{
		EEY = _Y;
	}
	else
	{
		EEY = _leftChild->_Y;
	}
	sort(EEY.begin(), EEY.end(), cmpYDec);
	if (x._s > EEY[0])
	{
		return;
	}	

	sort(_ZL.begin(), _ZL.end(), cmpXBeginDec);

	for (int i = 0; i < _ZL.size(); i++)
	{
		R.push_back(_ZL[i]);
		if (_ZL[i]._s == EEY[i] && EEY[i] <= x._s)
		{
			isTight = true;
			return;
		}
	}
	if (_ZL.size() == EEY.size())
	{
		isTight = true;
	}
	else
	{
		isTight = false;
	}
}




// insert x in the tree node
Msg TreeNode::insertXintoESinNode(X x)
{
	Msg msg;
	msg._aX = x;
	msg._aZ = x;
	vector<X> R;
	bool isTight;
	determineReachableSetinES(x, R, isTight);
	if (isTight == false)	// insert Z successfully
	{
		_Z.push_back(x);
		_ZR.push_back(x);
		//return msg;
		// no need to add to _ZL
		//in leaf, x may be inserted to ZL, tbd
		//msg._aZ = x;
	}
	else
	{
		//insert fail
		R.push_back(x);
		sort(R.begin(), R.end(), cmpXEndInc);
		//transfer
		if (R[R.size() - 1]._e > _Y[_Y.size() - 1])
		{
			//R[R.size() - 1] into T
			_ZR.push_back(x);
			_Z.push_back(x);
			vector<X>::iterator it = find(_ZR.begin(), _ZR.end(), R[R.size() - 1]);
			_ZR.erase(it);
			it = find(_Z.begin(), _Z.end(), R[R.size() - 1]);
			_Z.erase(it);
			_T.push_back(R[R.size() - 1]);
			//msg._aZ = x;
			msg._bZ = R[R.size() - 1];
			msg._aT = R[R.size() - 1];
		}
		else
		{
			//infeasible
			X r = replaceMinWeightX(x);//including adjust ZL and ZR
			//msg._aZ = x;
			msg._bZ = r;
			msg._aI = r;
		}
	}
	/*
	else if (x._e > _Y[_Y.size() - 1])	// transferred
	{
	_T.push_back(x);
	msg._aT = x;
	}
	else // infeasible
	{
	vector<X>::iterator it = find(_Z.begin(), _Z.end(), jTP);
	_Z.erase(it);
	vector<X>::iterator it1 = find(_ZR.begin(), _ZR.end(), jTP);
	_ZR.erase(it1);
	msg._aZ = x;
	msg._bZ = jTP;

	_I.push_back(x);
	msg._aI = x;
	}*/

	return msg;
}

// only for the Leaf Node in Tree
Msg TreeNode::insertYintoESLeaf(Y y)
{
	Msg msg;
	msg._aY = y;
	//TreeNode * node = (this->_rightChild != NULL) ? this->_rightChild : this;

	Y lAlphaTP = leftAlphaTightPoint(y, this);

	//decide the X to be matched;	
	vector<X> backX;
	for (int j = 0; j < _I.size(); j++)
	{
		if (_I[j]._e > lAlphaTP)
		{
			backX.push_back(_I[j]);
		}
	}
	if (!backX.empty())
	{
		sort(backX.begin(), backX.end(), cmpXStandard);
		X x1 = backX[(int)backX.size() - 1];
		_Z.push_back(x1);
		_ZR.push_back(x1);
		vector<X>::iterator it = find(_I.begin(), _I.end(), x1);
		_I.erase(it);
	}
	else
	{
		//chose from T
		if (!_T.empty())
		{
			sort(_T.begin(), _T.end(), cmpXEndBeginIdInc);
			_Z.push_back(_T[0]);
			_ZR.push_back(_T[0]);
			_T.erase(_T.begin());
		}
	}

	return msg;
}


X TreeNode::replaceMinWeightX(X x)
{
	vector<X> ESR;
	vector<X> R;
	X r;
	determineReachableSetinES(x, R, *new bool);
	R.push_back(x);
	ESR = R;
	if (_rightChild == NULL)
	{
		//a leaf
		r = R[0];
		for (int i = 0; i < R.size(); i++)
		{
			if (cmpXWeightIDInc(R[i], r))
			{
				r = R[i];
			}
		}
		_Z.push_back(x);
		_ZR.push_back(x);
		vector<X>::iterator it = find(_Z.begin(), _Z.end(), r);
		_Z.erase(it);
		it = find(_ZR.begin(), _ZR.end(), r);
		_ZR.erase(it);
	}
	else
	{
		//internal node
		X minBeginX = R[0];
		for (int i = 0; i < R.size(); i++)
		{
			if (R[i]._s < minBeginX._s)
			{
				minBeginX = R[i];
			}
		}
		determineReachableSetinEE(minBeginX, R, *new bool);
		r = R[0];
		for (int i = 0; i < R.size(); i++)
		{
			if (cmpXWeightIDInc(R[i], r))
			{
				r = R[i];
			}
		}
		vector<X>::iterator it = find(_ZL.begin(), _ZL.end(), r);
		if (it != _ZL.end())
		{
			//r in _ZL, left
			//if r in ZL,then R must be added some elements in determineEE
			_Z.push_back(x);
			_ZR.push_back(x);
			vector<Y> EEY = _leftChild->_Y;
			sort(EEY.begin(), EEY.end(), cmpYDec);
			sort(_ZL.begin(), _ZL.end(), cmpXBeginDec);
			Y l2;
			l2._id = -1;
			for (int i = 0; i < _ZL.size(); i++)
			{
				if (EEY[i] == r._s)
				{
					break;
				}
				if (_ZL[i]._s == EEY[i])
				{
					l2 = EEY[i];
				}
			}
			if (l2._id == -1)
			{
				l2 = _rightChild->getIntervalStart();
			}
			vector<X> allBackX;//must have one, since r in ZL
			for (int i = 0; i < ESR.size(); i++)
			{
				if (ESR[i]._s < l2)
				{
					allBackX.push_back(ESR[i]);
				}
			}
			sort(allBackX.begin(), allBackX.end(), cmpXEndBeginIdInc);	// TBC: increaing start? should be decreasing?
			X backX = allBackX[0];
			it = find(_ZR.begin(), _ZR.end(), backX);
			_ZR.erase(it);
			_ZL.push_back(backX);
			it = find(_ZL.begin(), _ZL.end(), r);
			_ZL.erase(it);
			it = find(_Z.begin(), _Z.end(), r);
			_Z.erase(it);
		}
		else
		{
			//not in _ZL, x itself or in _ZR, right
			_Z.push_back(x);
			_ZR.push_back(x);
			it = find(_ZR.begin(), _ZR.end(), r);
			_ZR.erase(it);
			it = find(_Z.begin(), _Z.end(), r);
			_Z.erase(it);
		}

	}
	_I.push_back(r);
	return r;
}

void TreeNode::testInsertXintoNode(X x, int flag = 0)
{
	if (flag == 0)	// Z
	{
		_Z.push_back(x);
	}
	else if (flag == 1)	// T
	{
		_T.push_back(x);
	}
	else if (flag == 2)	// I
	{
		_I.push_back(x);
	}

}

int TreeNode::verifyNodeInvariants()
{

	sort(_Y.begin(), _Y.end(), cmpYInc);

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
		sort(_T.begin(), _T.end(), cmpXEndIncStartDec);

		for (int i = 0; i < (int)_Z.size(); i++)
		{
			if (_Z[i]._e > _T[0]._e)
			{
				X z = _Z[i];
				int j = 0;
				while (z._e > _T[j]._e)
				{
					vector<X> ZZ;
					for (int i = 0; i < (int)_Z.size(); i++)
					{
						ZZ.push_back(_Z[i]);
					}

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

	// invariant \phi_4: \nexists x\in I, Z+x-x'\in \I
	sort(_Z.begin(), _Z.end(), cmpXEndIncStartDec);
	if (!_I.empty() && _Z[_Z.size() - 1]._e > _Y[_Y.size() - 1])
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
				vector<X> Z2;
				for (int k = 0; k < (int)_Z.size(); k++)
				{
					Z2.push_back(_Z[k]);
				}
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
	tmpZ.clear();
	tmpX.clear();
	for (int i = 0; i < (int)_Z.size(); i++)
		tmpX.push_back(_Z[i]);
	for (int i = 0; i < (int)_I.size(); i++)
		tmpX.push_back(_I[i]);
	PlaxtonMWM(tmpX, _Y, &tmpZ);
	if (tmpZ.size() != _Z.size())
	{
		return 6;
	}
	sort(_Z.begin(), _Z.end(), cmpXStandard);
	sort(tmpZ.begin(), tmpZ.end(), cmpXStandard);
	for (int i = 0; i < (int)_Z.size(); i++)
	{
		if (!(_Z[i] == tmpZ[i]))
		{
			return 6;
		}
	}



	return 0;
}

void TreeNode::testPrintY()
{
	for (int i = 0; i < (int)_Y.size(); i++)
	{
		cout << _Y[i]._id << ', ';
	}
	cout << endl;
}



// Methods in Tree
Tree::Tree(vector<Y> vecY)
{
	_root = new TreeNode(vecY);
}

bool Tree::insertXinTree(X x)
{
	TreeNode* nodeP = locateLeaf(x);	// locate the leaf corresponding to x.begin

	//below is the whole implemention of the MSG passing rule
	Msg msg = nodeP->insertXintoESinNode(x);		// insert the x into the leaf
	nodeP->_X.push_back(x);
	/*if (msg.flagInsertX() == 2)
	{
	//msg._bZ = msg._aI = nodeP->replaceMinWeightX(msg);		// call replaceable algorithm
	}*/
	TreeNode* child = nodeP;
	nodeP = nodeP->_parentNode;

	while (nodeP != NULL)	//send msg until the root, the msg is from a node to its parent
	{
		// leaf is the current node; the msg comes from its own ES-Tree insert operation
		if (child == nodeP->_leftChild)	// the node is the left child
		{

			nodeP->_X.push_back(x);
			if (msg._aI._id == -1 && msg._aT._id == -1)//transfer to flags of msg later
			{
				//success
				nodeP->_Z.push_back(msg._aZ);
				nodeP->_ZL.push_back(msg._aZ);
				//msg keeps
			}

			else if (msg._aI._id == -1 && msg._aT._id != -1)
			{
				//transfer in left
				nodeP->_Z.push_back(msg._aZ);

				vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), msg._bZ);
				if (it != nodeP->_Z.end())
				{
					vector<X> R;
					nodeP->determineReachableSetinEE(msg._aZ, R, *new bool);
					R.push_back(msg._aZ);
					nodeP->_ZL.push_back(msg._aZ);
					sort(R.begin(), R.end(), cmpXEndInc);
					X maxEndinR = R[R.size() - 1];
					it = find(nodeP->_Z.begin(), nodeP->_Z.end(), maxEndinR);
					nodeP->_Z.erase(it);
					it = find(nodeP->_ZL.begin(), nodeP->_ZL.end(), maxEndinR);
					nodeP->_ZL.erase(it);
					Msg tempMsg = nodeP->insertXintoESinNode(maxEndinR);
					msg._aI = tempMsg._aI;
					msg._aT = tempMsg._aT;
					msg._bZ = tempMsg._bZ;
				}
				else
				{
					vector<X> R;
					nodeP->determineReachableSetinEE(msg._aZ, R, *new bool);//ES or EE? In Split or Msg Passing?
					//when the R is correct, the below code is right
					R.push_back(msg._aZ);
					nodeP->_ZL.push_back(msg._aZ);
					sort(R.begin(), R.end(), cmpXEndInc);
					X maxEndinR = R[R.size() - 1];
					if (maxEndinR._e < nodeP->_rightChild->getIntervalStart())
					{
						//not transfer in P
						sort(R.begin(), R.end(), cmpXWeightIDInc);
						vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), R[0]);
						nodeP->_Z.erase(it);
						it = find(nodeP->_ZL.begin(), nodeP->_ZL.end(), R[0]);
						nodeP->_ZL.erase(it);
						nodeP->_I.push_back(R[0]);
						Msg tempMsg;
						tempMsg._aX = msg._aX;
						tempMsg._aZ = msg._aZ;
						tempMsg._bZ = R[0];
						tempMsg._aI = R[0];
						msg = tempMsg;
					}
					else
					{
						vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), maxEndinR);
						nodeP->_Z.erase(it);
						it = find(nodeP->_ZL.begin(), nodeP->_ZL.end(), maxEndinR);
						nodeP->_ZL.erase(it);
						Msg tempMsg = nodeP->insertXintoESinNode(maxEndinR);
						msg._aI = tempMsg._aI;
						msg._aT = tempMsg._aT;
						msg._bZ = tempMsg._bZ;
						//_aZ, _aX keeps
					}

				}
			}
			else if (msg._aI._id != -1 && msg._aT._id == -1)
			{
				//infeasible in Left
				nodeP->_Z.push_back(msg._aZ);
				nodeP->_ZL.push_back(msg._aZ);
				vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), msg._bZ);
				if (it != nodeP->_Z.end())
				{
					nodeP->_Z.erase(it);
					it = find(nodeP->_ZL.begin(), nodeP->_ZL.end(), msg._bZ);
					nodeP->_ZL.erase(it);
					nodeP->_I.push_back(msg._aI);
					//msg keeps
				}
				else
				{
					vector<X>::iterator it = find(nodeP->_ZL.begin(), nodeP->_ZL.end(), msg._aZ);
					nodeP->_ZL.erase(it);
					vector<X> R;
					nodeP->determineReachableSetinEE(msg._aZ, R, *new bool);//ES or EE? In Split or Msg Passing?
					//when the R is right, the below code is right
					R.push_back(msg._aZ);
					nodeP->_ZL.push_back(msg._aZ);
					sort(R.begin(), R.end(), cmpXEndInc);
					X maxEndinR = R[R.size() - 1];
					if (maxEndinR._e < nodeP->_rightChild->getIntervalStart())
					{
						//not transfer in P
						sort(R.begin(), R.end(), cmpXWeightIDInc);
						vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), R[0]);
						nodeP->_Z.erase(it);
						it = find(nodeP->_ZL.begin(), nodeP->_ZL.end(), R[0]);
						nodeP->_ZL.erase(it);
						nodeP->_I.push_back(R[0]);
						Msg tempMsg;
						tempMsg._aX = msg._aX;
						tempMsg._aZ = msg._aZ;
						tempMsg._bZ = R[0];
						tempMsg._aI = R[0];
						msg = tempMsg;
					}
					else
					{
						vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), maxEndinR);
						nodeP->_Z.erase(it);
						it = find(nodeP->_ZL.begin(), nodeP->_ZL.end(), maxEndinR);
						nodeP->_ZL.erase(it);
						Msg tempMsg = nodeP->insertXintoESinNode(maxEndinR);
						msg._aI = tempMsg._aI;
						msg._aT = tempMsg._aT;
						msg._bZ = tempMsg._bZ;
						//_aZ, _aX keeps
					}
				}
			}
		}
		else // msg from the right child
		{
			nodeP->_X.push_back(x);
			if (msg._aT._id == -1 && msg._aI._id == -1)
			{
				// success in R
				Msg tempMsg = nodeP->insertXintoESinNode(msg._aZ);
				msg._aI = tempMsg._aI;
				msg._aT = tempMsg._aT;
				msg._bZ = tempMsg._bZ;
			}
			else if (msg._aI._id == -1 && msg._aT._id != -1)
			{
				// x' is replaced by x in Z and moved to T in R
				if (msg._aZ == msg._aT)		// x is transferred itself
				{
					nodeP->_T.push_back(msg._aT);
				}
				else
				{
					vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), msg._bZ);
					if (it != nodeP->_Z.end())
					{
						// x' \in T
						// in P, there may a new tight point between x.e and the y matched with x'
						vector<X> RinPES;
						nodeP->determineReachableSetinES(msg._aZ, RinPES, *new bool);
						it = find(RinPES.begin(), RinPES.end(), msg._bZ);
						if (it != RinPES.end())
						{
							nodeP->_Z.push_back(msg._aZ);
							nodeP->_ZR.push_back(msg._aZ);
							it = find(nodeP->_ZR.begin(), nodeP->_ZR.end(), msg._bZ);
							nodeP->_ZR.erase(it);
							it = find(nodeP->_Z.begin(), nodeP->_Z.end(), msg._bZ);
							nodeP->_Z.erase(it);
							nodeP->_T.push_back(msg._bZ);
						}
						else
						{
							Msg tempMsg = nodeP->insertXintoESinNode(msg._aZ);
							msg._aI = tempMsg._aI;
							msg._aT = tempMsg._aT;
							msg._bZ = tempMsg._bZ;
						}
					}
					else
					{
						// x' \notin T; reinsert x
						Msg tempMsg = nodeP->insertXintoESinNode(msg._aZ);
						msg._aI = tempMsg._aI;
						msg._aT = tempMsg._aT;
						msg._bZ = tempMsg._bZ;
					}
				}
			}
			else if (msg._aI._id != -1 && msg._aT._id == -1)
			{
				// x' is replaced by x in Z and moved to I in R
				if (msg._aZ == msg._aI)		// x is infeasible
				{
					nodeP->_I.push_back(msg._aI);
				}
				else
				{
					vector<X>::iterator it = find(nodeP->_Z.begin(), nodeP->_Z.end(), msg._bZ);
					if (it != nodeP->_Z.end())
					{
						// x' \in I
						nodeP->_Z.push_back(msg._aZ);
						nodeP->_ZR.push_back(msg._aZ);
						it = find(nodeP->_ZR.begin(), nodeP->_ZR.end(), msg._bZ);
						nodeP->_ZR.erase(it);
						it = find(nodeP->_Z.begin(), nodeP->_Z.end(), msg._bZ);
						nodeP->_Z.erase(it);
						nodeP->_I.push_back(msg._bZ);
					}
					else
					{
						// x' \notin I; reinsert x
						Msg tempMsg = nodeP->insertXintoESinNode(msg._aZ);
						msg._aI = tempMsg._aI;
						msg._aT = tempMsg._aT;
						msg._bZ = tempMsg._bZ;
					}
				}
			}
			else
			{
				vector<int> t; t.erase(t.begin());
			}
			//break;
		}
		child = nodeP;
		nodeP = nodeP->_parentNode;
	}

	return true;
}

bool Tree::insertYinTree(Y y)
{
	TreeNode* nodeP = locateLeaf(y);	// locate the leaf corresponding to x.begin
	// if y is in Y, return
	vector<Y>::iterator it = find(nodeP->_Y.begin(), nodeP->_Y.end(), y);
	if (it != nodeP->_Y.end())
	{
		return false;
	}

	//below is the whole implemention of the MSG passing rule
	Msg msg = nodeP->insertYintoESLeaf(y);		// insert the y into the leaf
	nodeP->_Y.push_back(y);

	TreeNode* child = nodeP;
	nodeP = nodeP->_parentNode;

	while (nodeP != NULL)	//send msg until the root, the msg is from a node to its parent
	{
	}
	return true;
}

TreeNode* Tree::locateLeaf(X x)
{
	// assert: x's begin and end have been adjusted to existing value;
	TreeNode* node = _root;

	// find the node which the start of y is equal to x.begin
	// if there is no such node, i.e., reaching a leaf which its start is not equal to the x.begin, split the leaf into two leaf.
	while (x._s._id != node->getIntervalStart()._id)
	{
		if (node->_rightChild != NULL)//not a leaf
		{
			if (x._s._id >= ((TreeNode*)node->_rightChild)->getIntervalStart()._id)
			{
				node = (TreeNode*)node->_rightChild;
			}
			else
			{
				node = (TreeNode*)node->_leftChild;
			}
		}
		else // assert there is no case that right child is NULL and left child is not NULL.
		{
			node->splitDSNode(x);
			//node->updateAuxSet4Split();
			node = (TreeNode*)node->_rightChild;
			//test code for split==========================
			/*vector<X> Z = this->_root->_Z;
			ofstream out("Split-test.txt");
			sort(Z.begin(), Z.end(), cmpXID);
			for (int i = 0; i < Z.size(); i++)
			{
			out << Z[i]._id << endl;
			}
			out << "============T in root==================" << endl;
			vector<X> T = this->_root->_T;
			sort(T.begin(), T.end(), cmpXEndInc);
			for (int i = 0; i < T.size(); i++)
			{
			out << T[i]._id << "\t" << T[i]._e << endl;
			}
			out << "============I in root==================" << endl;
			vector<X> I = this->_root->_I;
			sort(I.begin(), I.end(), cmpXEndInc);
			for (int i = 0; i < I.size(); i++)
			{
			out << I[i]._id << "\t" << I[i]._e << endl;
			}
			out << "============Z in left==================" << endl;
			Z = this->_root->_leftChild->_Z;
			sort(Z.begin(), Z.end(), cmpXID);
			for (int i = 0; i < Z.size(); i++)
			{
			out << Z[i]._id << endl;
			}
			out << "============T in left==================" << endl;
			T = this->_root->_leftChild->_T;
			sort(T.begin(), T.end(), cmpXEndInc);
			for (int i = 0; i < T.size(); i++)
			{
			out << T[i]._id << "\t" << T[i]._e << endl;
			}
			out << "============I in left==================" << endl;
			I = this->_root->_leftChild->_I;
			sort(I.begin(), I.end(), cmpXEndInc);
			for (int i = 0; i < I.size(); i++)
			{
			out << I[i]._id << "\t" << I[i]._e << endl;
			}
			out.close();
			int a = 1;*/
			//=========================	
		}
	}
	while (node->_leftChild != NULL) // to leaf
	{
		node = node->_leftChild;
	}
	/*//add into _X in the path
	TreeNode* tmp = node;
	while (tmp != NULL)
	{
	tmp->_X.push_back(x);
	tmp = tmp->_parentNode;
	}*/
	return node;
}

TreeNode* Tree::locateLeaf(Y y)
{
	// assume that y with id=0 has been inserted and there is no split needed

	TreeNode* node = _root;
	// find the node in which y in Y	
	while (node->_rightChild != NULL)
	{
		if (node->_rightChild != NULL && y < node->_rightChild->_Y[0])
		{
			node = node->_leftChild;
		}
		else
		{
			node = node->_rightChild;
		}
	}

	return node;
}


int Tree::verifyTreeInvariants()
{
	TreeNode *node = this->_root;
	stack<TreeNode *> stk;
	int flag = 0;

	// dfs	- preorder
	do
	{
		if (stk.empty() != true)
		{
			node = stk.top()->_rightChild;
			stk.pop();
		}
		flag = node->verifyNodeInvariants();
		if (flag != 0)
		{
			return flag;
		}
		while (node->_leftChild != NULL)
		{
			if (node->_rightChild != NULL)
			{
				stk.push(node);
			}
			node = node->_leftChild;
			// visit
			//node->testPrintY();
			flag = node->verifyNodeInvariants();
			if (flag != 0)
			{
				return flag;
			}
		}
	} while (stk.empty() != true);

	//// dfs	- inorder	
	//cout << endl << endl << "inorder" << endl;
	//node = this->_root;	// assertion: root is not NULL
	//do
	//{
	//	do
	//	{
	//		stk.push(node);
	//		node = node->_leftChild;
	//	} while (node != NULL);

	//	do
	//	{
	//		node = stk.top();
	//		stk.pop();
	//		node->testPrintY();
	//	} while (!stk.empty() && node->_rightChild == NULL);

	//	node = node->_rightChild;
	//} while (stk.empty() != true || node != NULL);

	//// dfs	- postorder	
	//cout << endl << endl << "post order" << endl;
	//node = this->_root;	// assertion: root is not NULL

	//stk.push(node);
	//do
	//{
	//	while (node->_leftChild != NULL)
	//	{
	//		node = node->_leftChild;
	//		stk.push(node);
	//	}

	//	if (node->_rightChild != NULL)
	//	{
	//		node = node->_rightChild;
	//		stk.push(node);
	//		continue;
	//	}
	//	else
	//	{
	//		do
	//		{
	//			node = stk.top();
	//			stk.pop();
	//			node->testPrintY();
	//		} while (!stk.empty() && (node == stk.top()->_rightChild || (node == stk.top()->_leftChild && stk.top()->_rightChild == NULL)));
	//		if (!stk.empty())
	//		{
	//			node = stk.top()->_rightChild;
	//			stk.push(node);
	//		}
	//	}
	//} while (!stk.empty());


	return 0;
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
			return flagL;
		}
	}
}

// only verify the top three nodes
int Tree::verifyTreeInvariantsSimple()
{
	TreeNode* node = _root;
	int flag = node->verifyNodeInvariants();
	int flagL = 0, flagR = 0;
	if (node->_rightChild != NULL)
	{
		flagL = node->_leftChild->verifyNodeInvariants();
		flagR = node->_rightChild->verifyNodeInvariants();
	}

	if (flag != 0)
	{
		return flag;
	}
	else if (flagL != 0 || flagR != 0)
	{
		return (flagL > flagR) ? flagL : flagR;
	}
	else
	{
		return 0;
	}
}