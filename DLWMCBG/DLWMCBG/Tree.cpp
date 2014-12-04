//implemention of Tree

#include<algorithm>
#include"Tree.h"

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

	leftChild->_X = _X;	// copy the variables from the parent

	_leftChild = leftChild;
	_rightChild = rightChild;
	leftChild->_parentNode = this;
	rightChild->_parentNode = this;
}

// adjust the sets in the process of splitting
void TreeNode::updateAuxSet4Split()
{
	TreeNode* leftChild = _leftChild;
	Msg msg;
	X tmpX;
	vector<X> tempMatched = _Z;
	_Z.clear();
	vector<X> tempMatched2 = _ZR;
	_ZR.clear();

	leftChild->_I = _I;
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
	}
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

void TreeNode::determineReachableSetinES(X x, vector<X>& R, bool& isTight)
{
	vector<Y>* pESY;
	if (_rightChild == NULL)
	{
		pESY = &_Y;
	}
	else
	{
		pESY = &_rightChild->_Y;
	}
	sort((*pESY).begin(), (*pESY).end(), cmpYInc);
	sort(_ZR.begin(), _ZR.end(), cmpXEndInc);
	for (int i = 0; i < _ZR.size(); i++)
	{
		R.push_back(_ZR[i]);
		if (_ZR[i]._e == _Y[i] && _Y[i] >= x._e)
		{
			isTight = true;
			return;
		}
	}
	if (_ZR.size() == _Y.size())
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
	vector<X> R;
	bool isTight;
	determineReachableSetinES(x, R, isTight);
	if (isTight == false)	// insert Z successfully
	{
		_Z.push_back(x);
		_ZR.push_back(x);	
		// no need to add to _ZL
		//in leaf, x may be inserted to ZL, tbd
		msg._aZ = x;
	}
	else
	{
		//insert fail
		//transfer
		if (R[R.size() - 1]._e > _Y[_Y.size() - 1] || x._e >_Y[_Y.size() - 1])//R is sorted by end inc
		{
			if (R[R.size() - 1]._e > x._e)
			{
				//R[R.size() - 1] into T
				vector<X>::iterator it = find(_ZR.begin(), _ZR.end(), R[R.size() - 1]);
				_ZR.erase(it);
				it = find(_Z.begin(), _Z.end(), R[R.size() - 1]);
				_Z.erase(it);
				_ZR.push_back(x);
				_Z.push_back(x);
				_T.push_back(R[R.size() - 1]);
				msg._aZ = x;
				msg._bZ = R[R.size() - 1];
				msg._aT = R[R.size() - 1];
			}
			else
			{
				//x into T
				_T.push_back(x);
				msg._aT = x;
			}
		}
		else
		{
			
			//infeasible
			X r = replaceMinWeightX(x);
			msg._aZ = x;
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


X TreeNode::replaceMinWeightX(X x)
{
	vector<X> R;
	X r;
	determineReachableSetinES(x, R, *new bool);
	R.push_back(x);
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
		//tbd
	}
	return r;
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
	if (msg.flagInsertX() == 2)
	{
		//msg._bZ = msg._aI = nodeP->replaceMinWeightX(msg);		// call replaceable algorithm		
	}
	TreeNode* child = nodeP;
	nodeP = nodeP->_parentNode;

	while (nodeP != NULL)	//send msg until the root, the msg is from a node to its parent
	{
		// leaf is the current node; the msg comes from its own ES-Tree insert operation
		if (child == nodeP->_leftChild)	// the node is the left child
		{
			if (msg._aT._id == -1 && msg._aI._id == -1)	// msg._c == 0 // 1.success in L
			{
				nodeP->_parentNode->_Z.push_back(msg._aZ);	// msg._a
			}
			else if (msg._aZ._id == -1 || msg._aZ == msg._bZ)	// msg._a == msg._b 	// 2.non-sucessful, a = b
			{
				if (msg._aI._id != -1)
				{
					nodeP->_I.push_back(msg._aI);	//msg._b = msg._aI
				}
				else
				{
					Msg tempMsg = nodeP->insertXintoESinNode(msg._aT);	//msg._b = msg._aT
					if (tempMsg._aI._id != -1)
					{
						//tempMsg._aI = nodeP->replaceMinWeightX(tempMsg);		// call replacement algorithm
					}
					// msg._b = tempMsg._b; msg._bEmpty = tempMsg._bEmpty; msg._c = tempMsg._c;
					msg._bZ = tempMsg._bZ;
					msg._aT = tempMsg._aT;
					msg._aI = tempMsg._aI;
				}
			}
			else   // 3.non-sucessful, a != b
			{
			}

			if (msg.flagInsertX() == 2)	// msg._c == 2 // infeasible
			{
				nodeP->_parentNode->_I.push_back(msg._bZ);	// msg._b; assertion: msg._bZ == msg._aI
			}
			else if (msg.flagInsertX() == 1)	// msg._c == 1 // transferred
			{
				Msg tempMsg = nodeP->_parentNode->insertXintoESinNode(msg._bZ);	// msg._b; // if tempMsg._b <> msg._b, then msg._b remains in the matched set of the parent
				//msg._b = tempMsg._b; msg._bEmpty = tempMsg._bEmpty; msg._c = tempMsg._c;
				msg._bZ = tempMsg._bZ;	
				msg._aT = tempMsg._aT;
				msg._aI = tempMsg._aI;
				
				if (tempMsg.flagInsertX() == 2)	// tempMsg._c == 2
				{
					//msg._bZ = msg._aI = nodeP->_parent->replaceMinWeightX(tempMsg);		//msg._b // call replaceable algorithm
				}
			}
		}
		else // msg from the right child
		{
			

		}
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
		if (node->_rightChild != NULL)
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
			node->updateAuxSet4Split();
			node = (TreeNode*)node->_rightChild;
		}
	}
	while (node->_leftChild != NULL) // to leaf
	{
		node = node->_leftChild;
	}

	TreeNode* tmp = node;
	while (tmp != NULL)
	{
		tmp->_X.push_back(x);
		tmp = tmp->_parentNode;
	}
	return node;
}


