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
Y TreeNode::findjInES(vector<Y>* YR, Y y)
{
	sort((*YR).begin(), (*YR).end(), cmpY);
	sort(_ZR.begin(), _ZR.end(), cmpXEndInc);
	for (int i = 1; i < (int)_ZR.size(); i++)
	{
		if ((*YR)[i] == _ZR[i]._e)
		{
			return (*YR)[i];
		}
	}

	Y y1;
	y1._id = -1;
	return y1;
}

// insert x in the tree node
Msg TreeNode::insertXinNode(X x)
{
	Msg msg;

	vector<Y>* pESValues;
	if (_right != NULL)
	{
		pESValues = &_right->_Y;
	}
	else
	{
		pESValues = &_Y;
	}

	Y jTP;
	if (x._e > _Y[_Y.size() - 1])
	{
		jTP = findjInES(pESValues, _Y[_Y.size() - 1]);
	}
	else
	{
		jTP = findjInES(pESValues, x._e);
	}
	
	if (jTP._id == -1)
	{
		_Z.push_back(x);
		_ZR.push_back(x);
		msg._aZ = x;
	}
	else if (x._e > _Y[_Y.size() - 1])
	{
		_T.push_back(x);
		msg._aT = x;
	}
	else
	{
		_I.push_back(x);
		msg._aI = x;
	}
	msg._aX = x;
	
	return msg;
}

// Methods in Tree
Tree::Tree(vector<Y> vecY)
{
	_root = new TreeNode(vecY);
}

bool Tree::insertXinTree(X x)
{
	TreeNode* leaf = locateLeaf(x);	// locate the leaf corresponding to x.begin

	return false;
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


