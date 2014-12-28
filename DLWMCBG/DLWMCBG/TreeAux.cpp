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