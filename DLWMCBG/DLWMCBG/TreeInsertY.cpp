#include<algorithm>
#include<fstream>
#include"Tree.h"
#include"Basic.h"
#pragma warning (disable:4018)


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



Msg TreeNode::insertYintoLeaf(Y y)
{
	//TBM
	Msg msg;
	msg._aY = y;

	Y laT = leftAlphaTightPoint(y);
	vector<X> ICS, TCS;
	for (int i = 0; i < _I.size(); i++)
	{
		if (_I[i]._e > laT)
		{
			ICS.push_back(_I[i]);
		}
	}
	if (ICS.empty())
	{
		if (!_T.empty())
		{
			sort(_T.begin(), _T.end(), cmpXEndInc);
			X cx = _T[0];
			_Z.push_back(cx);
			_ZR.push_back(cx);
			vector<X>::iterator it = find(_T.begin(), _T.end(), cx);
			_T.erase(it);
			msg._bT = cx;
			msg._aZ = cx;
		}

	}
	else
	{
		sort(ICS.begin(), ICS.end(), cmpXWeightInc);
		X cx = ICS[ICS.size()-1];
		_Z.push_back(cx);
		_ZR.push_back(cx);
		vector<X>::iterator it = find(_I.begin(), _I.end(), cx);
		_I.erase(it);
		msg._bI = cx;
		msg._aZ = cx;
	}


	return msg;
}
