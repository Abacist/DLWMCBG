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
	vector<X> _ZR;	// corresponds to _matched2 in FAW	
	//ZL tbd

	TreeNode* _left;
	TreeNode *_right;
	TreeNode *_parent;
	
	//process to be implemented
	TreeNode(vector<Y>);	//init, params tbd;

	Y getIntervalStart();

	void splitDSNode(X);
	void updateAuxSet4Split();

	
	Msg insertXinNode(X x);	
	
	//calculate Replaceable Set and Compensable Set tbd
	Y findjInES(vector<Y>*, Y);		// return the least tight piont greater than y in ES 
	Y findlInEE(vector<Y>*, Y);		// return the greatest tight piont less than y in ES 


};

class Tree
{
public:
	TreeNode* _root;
	
	
	
	//process to be implemented
	Tree(vector<Y>);	// init

	TreeNode* locateLeaf(X x);	//include split
	TreeNode* locateLeaf(Y y);	
	bool insertXinTree(X x);
	
};