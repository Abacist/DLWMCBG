#include"Tree.h"

TreeNode::TreeNode(vector<Y> vY)
{
	_Y = vY;
	_IY = vY;
	_rightChild = NULL;
	_leftChild = NULL;
	_parent = NULL;
}

Tree::Tree(vector<Y> vY)
{
	_root = new TreeNode(vY);
}

Y TreeNode::maxY()
{
	sort(_Y.begin(), _Y.end(), cmpYIDDec);
	return _Y[0];
}

Y TreeNode::minY()
{
	sort(_Y.begin(), _Y.end(), cmpYIDInc);
	return _Y[0];
}