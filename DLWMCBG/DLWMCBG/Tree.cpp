//implemention of Tree
#include"Tree.h"
#include<algorithm>

// Methods in TreeNode
TreeNode::TreeNode(vector<Y> vecY)
{
	sort(vecY.begin(), vecY.end(), cmpY);
	for (int i = 0; i < (int)vecY.size(); i++)
	{
		_Y.push_back(vecY[i]);
	}
	_left = NULL;
	_right = NULL;
	_parent = NULL;
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

	_left = leftChild;
	_right = rightChild;
	leftChild->_parent = this;
	rightChild->_parent = this;
}

// adjust the sets in the process of splitting
void TreeNode::updateAuxSet4Split()
{
	TreeNode* leftChild = _left;
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
		msg = leftChild->insertXinNode(tmpX);		// insert into the left child node L
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
			insertXinNode(msg._bZ);	// insert into the parent node P
		} break;
		case 2:
		{
			//no infeasible case				
		} break;
		}
	}
}

// return the least tight piont greater than y in ES 
X TreeNode::findjInES(vector<Y>* YR, Y y)
{
	// assume y is between Y.s and Y.e
	sort((*YR).begin(), (*YR).end(), cmpY);
	sort(_ZR.begin(), _ZR.end(), cmpXEndInc);
	for (int i = 1; i < (int)_ZR.size(); i++)
	{
		if ((*YR)[i] < y)
		{
			continue;
		}
		else if ((*YR)[i] == _ZR[i]._e)
		{
			return _ZR[i];	// the least tight point greater than y
		}
	}

	X x1;
	x1._id = -1;
	return x1;	// there is no tight point after y
}

// insert x in the tree node
Msg TreeNode::insertXinNode(X x)
{
	Msg msg;
	msg._aX = x;
	
	vector<Y>* pESValues;
	if (_right != NULL)
	{
		pESValues = &_right->_Y;
	}
	else
	{
		pESValues = &_Y;
	}
	
	Y tmpY = (x._e > _Y[_Y.size() - 1]) ? _Y[_Y.size() - 1] : x._e;	
	X jTP = findjInES(pESValues, tmpY);
	
	if (jTP._id == -1)	// insert Z successfully
	{
		_Z.push_back(x);
		_ZR.push_back(x);	
		msg._aZ = x;
	}
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
	}	
	
	return msg;
}

// insertedX is the x vertex added into the node P
X TreeNode::replaceMinWeightX(Msg msg)
{
	X x;
	x._id = -1;
	return x;
}


// Methods in Tree
Tree::Tree(vector<Y> vecY)
{
	_root = new TreeNode(vecY);
}

bool Tree::insertXinTree(X x)
{
	TreeNode* nodeP = locateLeaf(x);	// locate the leaf corresponding to x.begin
	Msg msg = nodeP->insertXinNode(x);		// insert the x into the leaf
	if (msg.flagInsertX() == 2)
	{
		msg._bZ = msg._aI = nodeP->replaceMinWeightX(msg);		// call replaceable algorithm		
	}
	TreeNode* child = nodeP;
	nodeP = nodeP->_parent;

	while (nodeP != NULL)	//send msg until the root, the msg is from a node to its parent
	{
		// leaf is the current node; the msg comes from its own ES-Tree insert operation
		if (child == nodeP->_left)	// the node is the left child
		{
			if (msg._aT._id == -1 && msg._aI._id == -1)	// msg._c == 0 // 1.success in L
			{
				nodeP->_parent->_Z.push_back(msg._aZ);	// msg._a
			}
			else if (msg._aZ._id == -1 || msg._aZ == msg._bZ)	// msg._a == msg._b 	// 2.non-sucessful, a = b
			{
				if (msg._aI._id != -1)
				{
					nodeP->_I.push_back(msg._aI);	//msg._b = msg._aI
				}
				else
				{
					Msg tempMsg = nodeP->insertXinNode(msg._aT);	//msg._b = msg._aT
					if (tempMsg._aI._id != -1)
					{
						tempMsg._aI = nodeP->replaceMinWeightX(tempMsg);		// call replacement algorithm
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
				nodeP->_parent->_I.push_back(msg._bZ);	// msg._b; assertion: msg._bZ == msg._aI
			}
			else if (msg.flagInsertX() == 1)	// msg._c == 1 // transferred
			{
				Msg tempMsg = nodeP->_parent->insertXinNode(msg._bZ);	// msg._b; // if tempMsg._b <> msg._b, then msg._b remains in the matched set of the parent
				//msg._b = tempMsg._b; msg._bEmpty = tempMsg._bEmpty; msg._c = tempMsg._c;
				msg._bZ = tempMsg._bZ;	
				msg._aT = tempMsg._aT;
				msg._aI = tempMsg._aI;
				
				if (tempMsg.flagInsertX() == 2)	// tempMsg._c == 2
				{
					msg._bZ = msg._aI = nodeP->_parent->replaceMinWeightX(tempMsg);		//msg._b // call replaceable algorithm
				}
			}
		}
		else // msg from the right child
		{
			

		}
		child = nodeP;
		nodeP = nodeP->_parent;
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
		if (node->_right != NULL)
		{
			if (x._s._id >= ((TreeNode*)node->_right)->getIntervalStart()._id)
			{
				node = (TreeNode*)node->_right;
			}
			else
			{
				node = (TreeNode*)node->_left;
			}
		}
		else // assert there is no case that right child is NULL and left child is not NULL.
		{
			node->splitDSNode(x);
			node->updateAuxSet4Split();
			node = (TreeNode*)node->_right;
		}
	}
	while (node->_left != NULL) // to leaf
	{
		node = node->_left;
	}

	TreeNode* tmp = node;
	while (tmp != NULL)
	{
		tmp->_X.push_back(x);
		tmp = tmp->_parent;
	}
	return node;
}


