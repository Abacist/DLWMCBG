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
			msg = nodeP->insertYintoInternalNodeL(msg);
		}
		else
		{
			// msg from the right child			
			msg = nodeP->insertYintoInternalNodeR(msg);
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

	Y laT = leftAlphaTightPointforZR(y);
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

void TreeNode::determineNewInfeabileXOfTL(Msg msg, vector<X>& TLinPI)
{
	if (_leftChild == NULL)
	{
		throw new exception();
	}
	else
	{
		TLinPI.clear();
		vector<X> TL = _leftChild->_T;
		//roll back
		if (msg._bT._id != -1&& msg._aY < _rightChild->getIntervalStart())
		{
			TL.push_back(msg._bT);
		}
		for (int i = 0; i < TL.size(); i++)
		{
			vector<X>::iterator it = find(_I.begin(), _I.end(), TL[i]);
			if (it != _I.end())
			{
				TLinPI.push_back(TL[i]);
			}
		}
	}
}

void TreeNode::determineNewInfeabileXOfLZRZ(Msg curMsg, vector<X>& leftPart, vector<X>& rightPart)
{
	if (_leftChild == NULL)
	{
		throw new exception();
	}
	vector<X>::iterator it;
	vector<X> LZ = _leftChild->_Z;
	vector<X> RZ = _rightChild->_Z;
	leftPart.clear();
	rightPart.clear();
	if (curMsg._aY < _rightChild->_Y[0])
	{
		//msg from left
		if (curMsg._aZ._id != -1)
		{
			vector<X>::iterator it = find(LZ.begin(), LZ.end(), curMsg._aZ);
			LZ.erase(it);
		}
	}
	else
	{
		//msg from right	
		if (curMsg._aZ._id != -1)
		{
			it = find(RZ.begin(), RZ.end(), curMsg._aZ);
			RZ.erase(it);
		}
	}
	for (int i = 0; i < LZ.size(); i++)
	{
		if (find(_I.begin(), _I.end(), LZ[i]) != _I.end())
		{
			leftPart.push_back(LZ[i]);
		}
	}
	for (int i = 0; i < RZ.size(); i++)
	{
		if (find(_I.begin(), _I.end(), RZ[i]) != _I.end())
		{
			rightPart.push_back(RZ[i]);
		}
	}
}


//void TreeNode::determineCompensableSetOfPI(Msg curMsg, vector<X>& CS)
//{
//	CS.clear();
//}

Msg TreeNode::insertYintoInternalNodeL(Msg msg)
{
	Msg rMsg;
	rMsg._aY = msg._aY;
	if (msg._aZ._id == -1)
	{
		//msg keeps
		return msg;
	}
	else if (msg._bT._id != -1 && find(_T.begin(), _T.end(), msg._bT) != _T.end())
	{
		_T.erase(find(_T.begin(), _T.end(), msg._bT));
		_ZL.push_back(msg._bT);
		_Z.push_back(msg._bT);
		return msg;
	}
	else
	{
		//cx is from T in L and is matched or infeasible in P
		//cx is from I in L
		vector<X> TLI, leftI, rightI;
		determineNewInfeabileXOfTL(msg, TLI);
		determineNewInfeabileXOfLZRZ(msg, leftI, rightI);
		Y rbT = rightBetaTightPointforZL(msg._aY);
		vector<X> cTLI;
		for (int i = 0; i < TLI.size(); i++)
		{
			if (TLI[i]._s < rbT)
			{
				cTLI.push_back(TLI[i]);
			}
		}
		if (msg._bI._id != -1)
		{
			leftI.push_back(msg._bI);
		}
		vector<X> cL;
		for (int i = 0; i < leftI.size(); i++)
		{
			if (leftI[i]._s < rbT)
			{
				cL.push_back(leftI[i]);
			}
		}
		vector<X> backX, cR;
		for (int i = 0; i < _ZR.size(); i++)
		{
			if (_ZR[i]._s < _rightChild->_Y[0] && _ZR[i]._s < rbT)
			{
				backX.push_back(_ZR[i]);
			}
		}
		if (!backX.empty())
		{
			sort(backX.begin(), backX.end(), cmpXEndInc);
			Y laT = leftAlphaTightPointforZR(backX[0]._e);
			for (int i = 0; i < rightI.size(); i++)
			{
				if (rightI[i]._e > laT)
				{
					cR.push_back(rightI[i]);
				}
			}
			for (int i = 0; i < TLI.size(); i++)
			{
				if (TLI[i]._e > laT)
				{
					if (find(cTLI.begin(), cTLI.end(), TLI[i]) == cTLI.end())
					{
						cR.push_back(TLI[i]);
					}
					//cTLI or CL?
				}
			}
		}
		if (cTLI.empty() && cL.empty() && cR.empty())
		{
			//find in P.T
			if (backX.empty())
			{
				return rMsg;//Y bT = rightBetaTightPointforZR(_leftChild->_Y[_leftChild->_Y.size() - 1]);
			}
			Y bT = rightBetaTightPointforZR(/*_leftChild->_Y[_leftChild->_Y.size() - 1]);*/_rightChild->_Y[0]);
			vector<X> cTP;
			for (int i = 0; i < _T.size(); i++)
			{
				if (_T[i]._s < bT)
				{
					cTP.push_back(_T[i]);
				}
			}
			sort(cTP.begin(), cTP.end(), cmpXEndInc);
			if (cTP.empty())
			{
				vector<X>::iterator it = find(_ZR.begin(), _ZR.end(), msg._bT);
				if (msg._bT._id != -1 &&  it != _ZR.end())
				{
					_ZR.erase(it);
					_ZL.push_back(msg._bT);
				}
				return rMsg;
			}
			else
			{
				//pull the cTP[0] back
				_ZL.push_back(backX[0]);
				_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
				_ZR.push_back(cTP[0]);
				_Z.push_back(cTP[0]);
				_T.erase(find(_T.begin(), _T.end(), cTP[0]));
				rMsg._bT = cTP[0];
				rMsg._aZ = cTP[0];
				return rMsg;
			}
		}
		else
		{
			vector<X> cxinI = cTLI;
			for (int i = 0; i < cL.size(); i++)
			{
				cxinI.push_back(cL[i]);
			}
			for (int i = 0; i < cR.size(); i++)
			{
				cxinI.push_back(cR[i]);
			}
			sort(cxinI.begin(), cxinI.end(), cmpXWeightInc);
			X cx = cxinI[cxinI.size() - 1];
			if (find(rightI.begin(), rightI.end(), cx) == rightI.end())
			{
				//not in right
				//in TIL or leftI
				_I.erase(find(_I.begin(), _I.end(), cx));
				_Z.push_back(cx);
				_ZL.push_back(cx);
				rMsg._bI = cx;
				rMsg._aZ = cx;
				return rMsg;
			}
			else
			{
				_ZL.push_back(backX[0]);
				_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
				_ZR.push_back(cx);
				_Z.push_back(cx);
				_I.erase(find(_I.begin(), _I.end(), cx));
				rMsg._bI = cx;
				rMsg._aZ = cx;
				return rMsg;
			}
		}
	}
}


Msg TreeNode::insertYintoInternalNodeR(Msg msg)
{
	Msg rMsg;
	rMsg._aY = msg._aY;

	vector<X> TLI, leftI, rightI;
	determineNewInfeabileXOfTL(msg, TLI);
	determineNewInfeabileXOfLZRZ(msg, leftI, rightI);
	Y laT = leftAlphaTightPointforZR(msg._aY);
	vector<X> cTLI;
	for (int i = 0; i < TLI.size(); i++)
	{
		if (TLI[i]._e > laT)
		{
			cTLI.push_back(TLI[i]);
		}
	}
	if (msg._bI._id != -1)
	{
		rightI.push_back(msg._bI);
	}
	vector<X> cR;
	for (int i = 0; i < rightI.size(); i++)
	{
		if (rightI[i]._e > laT)
		{
			cR.push_back(rightI[i]);
		}
	}
	vector<X> forwardX, cL;
	for (int i = 0; i < _ZL.size(); i++)
	{
		if (_ZL[i]._e >= _rightChild->_Y[0] && _ZL[i]._e > laT)
		{
			forwardX.push_back(_ZL[i]);
		}
	}
	if (!forwardX.empty())
	{
		sort(forwardX.begin(), forwardX.end(), cmpXBeginDec);
		Y rbT = rightBetaTightPointforZL(forwardX[0]._s);
		for (int i = 0; i < leftI.size(); i++)
		{
			if (leftI[i]._s < rbT)
			{
				cL.push_back(leftI[i]);
			}
		}
		for (int i = 0; i < TLI.size(); i++)
		{
			if (TLI[i]._s < rbT)
			{
				if (find(cTLI.begin(), cTLI.end(), TLI[i]) == cTLI.end())
				{
					cL.push_back(TLI[i]);
				}
				//cTLI or CL?
			}
		}
	}
	if (cTLI.empty() && cL.empty() && cR.empty())
	{
		//find in P.T
		Y bT = rightBetaTightPointforZR(msg._aY);/*_leftChild->_Y[_leftChild->_Y.size() - 1]);*/
		vector<X> cTP;
		for (int i = 0; i < _T.size(); i++)
		{
			if (_T[i]._s < bT)
			{
				cTP.push_back(_T[i]);
			}
		}
		sort(cTP.begin(), cTP.end(), cmpXEndInc);
		if (cTP.empty())
		{
			return rMsg;
		}
		else
		{
			//pull the cTP[0] back
			_ZR.push_back(cTP[0]);
			_Z.push_back(cTP[0]);
			_T.erase(find(_T.begin(), _T.end(), cTP[0]));
			rMsg._bT = cTP[0];
			rMsg._aZ = cTP[0];
			return rMsg;
		}
	}
	else
	{
		vector<X> cxinI = cTLI;
		for (int i = 0; i < cL.size(); i++)
		{
			cxinI.push_back(cL[i]);
		}
		for (int i = 0; i < cR.size(); i++)
		{
			cxinI.push_back(cR[i]);
		}
		sort(cxinI.begin(), cxinI.end(), cmpXWeightInc);
		X cx = cxinI[cxinI.size() - 1];
		if (find(cL.begin(), cL.end(), cx) == cL.end())
		{
			//not in left
			//in TIL or right
			_I.erase(find(_I.begin(), _I.end(), cx));
			_Z.push_back(cx);
			_ZR.push_back(cx);
			rMsg._bI = cx;
			rMsg._aZ = cx;
			return rMsg;
		}
		else
		{
			//in left
			sort(forwardX.begin(), forwardX.end(), cmpXEndInc);
			_ZR.push_back(forwardX[forwardX.size()-1]);
			_ZL.erase(find(_ZL.begin(), _ZL.end(), forwardX[forwardX.size() - 1]));
			_ZL.push_back(cx);
			_Z.push_back(cx);
			_I.erase(find(_I.begin(), _I.end(), cx));
			rMsg._bI = cx;
			rMsg._aZ = cx;
			return rMsg;
		}
	}

}