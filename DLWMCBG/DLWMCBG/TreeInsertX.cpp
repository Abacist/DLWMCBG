//implemention of Tree

#include<algorithm>
#include<fstream>
#include"Tree.h"
#include"Basic.h"
#pragma warning (disable:4018)




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









