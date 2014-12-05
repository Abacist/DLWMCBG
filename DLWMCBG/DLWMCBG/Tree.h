//the definition of the whole Tree
#include"Basic.h"

class TreeNode
{
public:
	//vector<X> _X;
	vector<Y> _Y;
	vector<X> _Z;
	vector<X> _I;
	vector<X> _T;
	vector<X> _ZR;	// // corresponds to _matched2 in FAW	
	vector<X> _ZL;  

	TreeNode* _leftChild;
	TreeNode *_rightChild;
	TreeNode *_parentNode;
	
	//process to be implemented
	TreeNode(vector<Y>);	//init, params tbd;

	Y getIntervalStart();

	void splitDSNode(X);
	void updateAuxSet4Split();

	
	Msg insertXintoESinNode(X x);
	
	//calculate Replaceable Set and Compensable Set tbd
	//X findjInES(vector<Y>*, Y);		// return x where x.e is the least tight piont greater than y in ES 
	//X findlInEE(vector<Y>*, Y);		// return x where x.s is the greatest tight piont less than y in EE

	void determineReachableSetinES(X x, vector<X>& R, bool& isTight);//no insert, just check
	void determineReachableSetinEE(X x, vector<X>& R, bool& isTight);
	
	X replaceMinWeightX(X x);	// compute the replaceable set in this node and do the repalcement
	//X determineMinWeightX(X, X, TreeNode*&);
	TreeNode * pullBackATransferredXInWeightProcess(TreeNode *, X, Msg, bool&, X&);


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