//implemention of Tree

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

// insert x.s or x.e in Y if it does not exist
void Tree::adjustXToProper(X x)
{
	//to be disscussed: delete a y with such that y == some x's begin or end
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
			if (cmpXWeightInc(R[i], r))
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
			if (cmpXWeightInc(R[i], r))
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
			//			sort(allBackX.begin(), allBackX.end(), cmpXEndBeginIdInc);	// TBC: increaing start? should be decreasing?
			sort(allBackX.begin(), allBackX.end(), cmpXEndInc);
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

	// invariant, for test insertY: |Y_P| = |Y_R| + |Y_L|
	if (_leftChild != NULL)
	{
		if (_Y.size() != _leftChild->_Y.size() + _rightChild->_Y.size())
		{
			return 7;
		}
	}


	return 0;
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
	TreeNode* child = nodeP;
	nodeP = nodeP->_parentNode;
	/*if (nodeP->verifyNodeInvariants() != 0)
	{
		throw new exception();
	}*/
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
						sort(R.begin(), R.end(), cmpXWeightInc);
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
						sort(R.begin(), R.end(), cmpXWeightInc);
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
						/*vector<X> RinPES;
						nodeP->determineReachableSetinES(msg._aZ, RinPES, *new bool);
						it = find(RinPES.begin(), RinPES.end(), msg._bZ);
						if (it != RinPES.end())
						{*/
						nodeP->_Z.push_back(msg._aZ);
						nodeP->_ZR.push_back(msg._aZ);
						it = find(nodeP->_ZR.begin(), nodeP->_ZR.end(), msg._bZ);
						nodeP->_ZR.erase(it);
						it = find(nodeP->_Z.begin(), nodeP->_Z.end(), msg._bZ);
						nodeP->_Z.erase(it);
						nodeP->_T.push_back(msg._bZ);
						/*}
						else
						{
						throw new exception();
						Msg tempMsg = nodeP->insertXintoESinNode(msg._aZ);
						msg._aI = tempMsg._aI;
						msg._aT = tempMsg._aT;
						msg._bZ = tempMsg._bZ;
						}*/
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
		/*if (nodeP->verifyNodeInvariants() != 0)
		{
			throw new exception();
		}*/
		child = nodeP;
		nodeP = nodeP->_parentNode;
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
			int flag = verifyInvariantsRecur(node);
			if (flag != 0)
			{
				throw new exception("Split Error");
			}


			//node->updateAuxSet4Split();
			node = node->_rightChild;

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

	// TBD: y.id=0 to be inserted?
	/*if (_root->_Y[0]._id != 0)
	{
	throw new exception();
	}*/

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
			return max(flagL,flagR);
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

Msg TreeNode::insertYintoLeaf(Y y)
{
	//TBM
	Msg msg;
	msg._aY = y;

	return msg;
}


bool Tree::insertYinTree(Y y)
{
	//TBM
	TreeNode* nodeP = locateLeaf(y);	// locate the leaf corresponding to x.begin
	// if y is in Y, return
	vector<Y>::iterator it = find(nodeP->_Y.begin(), nodeP->_Y.end(), y);
	if (it != nodeP->_Y.end())
	{
		//already inserted
		return false;
	}

	//below is the whole implemention of the MSG passing rule
	Msg msg = nodeP->insertYintoLeaf(y);		// insert the y into the leaf
	nodeP->_Y.push_back(y);
	sort(nodeP->_Y.begin(), nodeP->_Y.end(), cmpYInc);	// sort Y after each _Y augmentation

	TreeNode* child = nodeP;
	nodeP = nodeP->_parentNode;

	while (nodeP != NULL)	//send msg until the root, the msg is from a node to its parent
	{
		if (child == nodeP->_leftChild)
		{
			// msg from the left child
			
		}
		else
		{
			// msg from the right child			
			
		}
		nodeP->_Y.push_back(y);	// add y in Y
		sort(nodeP->_Y.begin(), nodeP->_Y.end(), cmpYInc);	// sort Y after each _Y augmentation

		child = nodeP;
		nodeP = nodeP->_parentNode;
	}
	return true;
}



// Note: if there is no such tight point, return y with y.id=-1
Y TreeNode::rightBetaTightPoint(Y y)
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
	return tY;
}

// Note: if there is no such tight point, return the least y in P._Y
Y TreeNode::leftBetaTightPoint(Y y)
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
	sort(tempY.begin(), tempY.end(), cmpYDec);
	sort(tempZ.begin(), tempZ.end(), cmpXBeginDec);
	Y tY;
	tY._id = -1;

	// if the ES part is full, return the first one.
	// Note: in this case, it maybe not a real tight point.
	if (_ZR.size() == tempY.size())
	{
		return tempY[0];
	}
	for (int i = tempZ.size() - 1; i >= 0; i--)
	{
		if (tempY[i] == tempZ[i]._s && tempY[i] <= y)
		{
			tY = tempY[i];
		}
	}
	return tY;
}

// Note: if there is no such tight point, return the greatest y in P._Y
Y TreeNode::rightAlphaTightPoint(Y y)
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
Y TreeNode::leftAlphaTightPoint(Y y)
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


