//the definition of the whole Tree
#include"Basic.h"

class TreeNode
{
public:
	vector<X> _X;
	vector<Y> _Y;
	vector<X> _Z;
	vector<X> _I;
	vector<X> _T;
	//ZL,ZR tbd
	TreeNode* _left;
	TreeNode *_right;
	TreeNode *_parent;
	
	//process to be implemented
	Msg insertXinNode(X x);
	TreeNode();//params tbd;
	void splitTreeNode(X x);//include maintain the sets
	//calculate Replaceable Set and Compensable Set tbd
};

class Tree
{
public:
	TreeNode* _root;
	
	
	
	//process to be implemented
	TreeNode* locateLeaf(X x);//include split
	TreeNode* locateLeaf(Y y);
	Tree();
	bool insertXinTree(X x);
	
};