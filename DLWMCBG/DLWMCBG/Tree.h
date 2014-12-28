#pragma once
//the definition of the whole Tree
#include"Basic.h"

class TreeNode
{
public:
	vector<X> _X;
	vector<Y> _Y;
	vector<X> _MX;
	vector<X> _IX;
	vector<X> _TX;
	vector<X> _MXR;	
	vector<X> _MXL;  
	vector<Y> _IY;
	vector<Y> _MY;

	TreeNode * _leftChild;
	TreeNode * _rightChild;
	TreeNode * _parent;
	
	
	TreeNode(vector<Y>);	//init

	Y maxY();
	Y minY();

	Y leftATforZR(Y y);
	Y rightATforZR(Y y);

	

};

class Tree
{
public:
	TreeNode* _root;
	
	Tree(vector<Y>);	// init

	void insertXinTree(X x);
	void insertYinTree(Y y);

};