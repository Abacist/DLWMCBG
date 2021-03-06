#include<algorithm>
#include<fstream>
#include"Tree.h"
#include"Basic.h"
#pragma warning (disable:4018)

extern int verifyEachUpdate;


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
		/*if (verifyEachUpdate)
		{
			int flag = nodeP->verifyNodeInvariants();
			if (flag != 0)
			{
				__debugbreak();
			}
		}*/

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
		Y rbT = rightBetaTightPointforZL(msg._aY);
		if (rbT == msg._aY)
		{
			rbT._id++;
		}
		Y rbTBackUp = rbT;
		vector<X> TLI, leftI, rightI;
		determineNewInfeabileXOfTL(msg, TLI);
		determineNewInfeabileXOfLZRZ(msg, leftI, rightI);
		if (msg._bI._id != -1)
		{
			leftI.push_back(msg._bI);
		}
		vector<X> CSBase = TLI;
		for (int i = 0; i < leftI.size(); i++) CSBase.push_back(leftI[i]);
		for (int i = 0; i < rightI.size(); i++) CSBase.push_back(rightI[i]);
		sort(_ZL.begin(), _ZL.end(), cmpXEndInc);
		vector<X> ZLBackUp = _ZL, ZRBackUp = _ZR;

		vector<X> TML;
		/*vector<X> TMR;
		for (int i = 0; i < _ZR.size(); i++)
		{
			if (_ZR[i]._s < _rightChild->getIntervalStart())
			{
				TMR.push_back(_ZR[i]);
			}
		}*/
		for (int i = 0; i < _ZL.size(); i++)
		{
			if (_ZL[i]._e >= _rightChild->getIntervalStart())
			{
				TML.push_back(_ZL[i]);
			}
		}
		sort(TML.begin(), TML.end(), cmpXEndInc);
		vector<X> backX;//backX is a subset of TMR
		for (int i = 0; i < _ZR.size(); i++)
		{
			if (_ZR[i]._s < _rightChild->_Y[0] && _ZR[i]._s < rbT)
			{
				backX.push_back(_ZR[i]);
			}
		}
		if (!backX.empty())
		{
			//try to remove some b-Tight point in the left part, make the CS correct
			if (!TML.empty())
			{
				sort(backX.begin(), backX.end(), cmpXEndInc);
				Y laTSwitch = leftAlphaTightPointforZR(backX[0]._e);
				vector<X> forwardSwitch;
				for (int i = 0; i < TML.size(); i++)
				{
					if (laTSwitch._id == -1 || laTSwitch._id != -1 && TML[i]._e > laTSwitch)
					{
						forwardSwitch.push_back(TML[i]);
					}
				}
				if (!forwardSwitch.empty())
				{
					X maxBeginXForward;
					sort(forwardSwitch.begin(), forwardSwitch.end(), cmpXBeginDec);
					maxBeginXForward = forwardSwitch[0];
					X XBack = backX[0];
					_ZL.erase(find(_ZL.begin(), _ZL.end(), maxBeginXForward));
					_ZL.push_back(XBack);
					Y rbTNew = rightBetaTightPointforZL(msg._aY);
					if (rbTNew == msg._aY)
					{
						rbTNew._id++;
					}
					if (rbT < rbTNew)
					{
						rbT = rbTNew;
					}
				}


				//Y raTSwitch = rightAlphaTightPointforZR(TML[TML.size() - 1]._e);//by invariant 12
				//Y laTSwitch = leftAlphaTightPointforZR(TML[TML.size() - 1]._e);
				//vector<X> forwardSwitch;
				//for (int i = 0; i < TML.size(); i++)
				//{
				//	if (laTSwitch._id == -1 || laTSwitch._id != -1 && TML[i]._e > laTSwitch)
				//	{
				//		forwardSwitch.push_back(TML[i]);
				//	}
				//}
				//vector<X> backSwitch;
				//for (int i = 0; i < backX.size(); i++)
				//{
				//	if (raTSwitch._id == -1 || raTSwitch._id != -1 && backX[i]._e <= raTSwitch)
				//	{
				//		backSwitch.push_back(backX[i]);
				//	}
				//}
				//if (!backSwitch.empty())
				//{
				//	X maxBeginXForward, minBeginXBack;
				//	sort(forwardSwitch.begin(), forwardSwitch.end(), cmpXBeginDec);
				//	sort(backSwitch.begin(), backSwitch.end(), cmpXBeginDec);
				//	maxBeginXForward = forwardSwitch[0];
				//	minBeginXBack = backSwitch[backSwitch.size() - 1];
				//	if (minBeginXBack._s < maxBeginXForward._s && maxBeginXForward._s >= msg._aY)
				//	{
				//		_ZL.erase(find(_ZL.begin(), _ZL.end(), maxBeginXForward));
				//		_ZL.push_back(minBeginXBack);
				//		Y rbTNew = rightBetaTightPointforZL(msg._aY);
				//		if (rbT < rbTNew)
				//		{
				//			rbT = rbTNew;
				//		}

				//	}
				//}
			}
		}
		_ZL = ZLBackUp;
		vector<X> CS;
		for (int i = 0; i < CSBase.size(); i++)
		{
			if (CSBase[i]._s < rbT)
			{
				CS.push_back(CSBase[i]);
			}
		}
		if (!backX.empty())
		{
			sort(backX.begin(), backX.end(), cmpXEndInc);
			Y laT = leftAlphaTightPointforZR(backX[0]._e);
			for (int i = 0; i < CSBase.size(); i++)
			{
				if (CSBase[i]._e >= _rightChild->getIntervalStart() && CSBase[i]._e > laT &&
					find(CS.begin(), CS.end(), CSBase[i]) == CS.end())
				{
					CS.push_back(CSBase[i]);
				}
			}
		}
		//CS is complete now 
		if (CS.empty())
		{
			if (backX.empty())
			{
				return rMsg;
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
				if (msg._bT._id != -1 && it != _ZR.end())
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
			//there is a X in I can compense
			sort(CS.begin(), CS.end(), cmpXWeightInc);
			X cx = CS[CS.size() - 1];
			//add to left or right
			if (backX.empty())
			{
				//must add to left
				_I.erase(find(_I.begin(), _I.end(), cx));
				_Z.push_back(cx);
				_ZL.push_back(cx);
				rMsg._bI = cx;
				rMsg._aZ = cx;
				//return rMsg;
			}
			else
			{
				if (cx._s >= _rightChild->getIntervalStart())
				{
					//can only be add to right
					sort(backX.begin(), backX.end(), cmpXEndInc);
					_ZL.push_back(backX[0]);//by invariant 12
					_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
					_ZR.push_back(cx);
					_Z.push_back(cx);
					_I.erase(find(_I.begin(), _I.end(), cx));
					rMsg._bI = cx;
					rMsg._aZ = cx;
					//return rMsg;
				}
				else
				{
					if (cx._e < _rightChild->getIntervalStart())
					{
						//can only be add to left
						if (cx._s >= rbTBackUp)
						{
							//switch
							Y lbT = leftBetaTightPointforZL(cx._s);
							vector<X> cxR;
							for (int i = 0; i < TML.size(); i++)
							{
								if (TML[i]._s >= lbT)
								{
									cxR.push_back(TML[i]);
								}
							}
							sort(cxR.begin(), cxR.end(), cmpXEndInc);
							X forwardX = cxR[cxR.size() - 1];
							//Y raT = rightAlphaTightPointforZR(forwardX._e);
							//vector<X> backXFilter;
							//for (int i = 0; i < backX.size(); i++)
							//{
							//	if (raT._id == -1 || raT._id != -1 && backX[i]._e <= raT)
							//	{
							//		backXFilter.push_back(backX[i]);
							//	}
							//}
							//sort(backXFilter.begin(), backXFilter.end(), cmpXBeginDec);
							////Y rbTforcx = rightBetaTightPointforZL(backXFilter[backXFilter.size() - 1]._s);
							////the rbTBackUp will be removed!
							//vector<X> realBackX = backXFilter;
							///*for (int i = 0; i < backXFilter.size(); i++)
							//{
							//	if (backXFilter[i]._s < rbTforcx)
							//	{
							//		realBackX.push_back(backXFilter[i]);
							//	}
							//}*/
							//sort(realBackX.begin(), realBackX.end(), cmpXEndInc);
							_ZL.push_back(cx);
							_Z.push_back(cx);
							_I.erase(find(_I.begin(), _I.end(), cx));
							_ZL.erase(find(_ZL.begin(), _ZL.end(), forwardX));
							_ZR.push_back(forwardX);
							_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
							_ZL.push_back(backX[0]);
							rMsg._bI = cx;
							rMsg._aZ = cx;
							//return rMsg;
						}
						else
						{
							_I.erase(find(_I.begin(), _I.end(), cx));
							_Z.push_back(cx);
							_ZL.push_back(cx);
							rMsg._bI = cx;
							rMsg._aZ = cx;
							//return rMsg;
						}
					}
					else
					{
						//_s < r->Y[0] && _e>= r->Y[0]
						sort(backX.begin(), backX.end(), cmpXEndInc);
						Y laT = leftAlphaTightPointforZR(backX[0]._e);
						if (cx._e > laT && cx._s < rbTBackUp)
						{
							//can be add to both side
							/*X cxBackUp = cx;
							vector<X> RinZL;
							determineReachableSetinEE(cx, RinZL, *new bool);
							RinZL.push_back(cx);
							sort(RinZL.begin(), RinZL.end(), cmpXEndInc);
							if (cmpXEndInc(cx, RinZL[RinZL.size() - 1]))
							{
								_ZL.push_back(cx);
								_ZL.erase(find(_ZL.begin(), _ZL.end(), RinZL[RinZL.size() - 1]));
								cx = RinZL[RinZL.size() - 1];
							}*/
							_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
							if (cmpXEndInc(cx, backX[0]))
							{
								//for phi 12
								_ZL.push_back(cx);
								_ZR.push_back(backX[0]);
							}
							else
							{
								_ZL.push_back(backX[0]);
								_ZR.push_back(cx);
							}
							_I.erase(find(_I.begin(), _I.end(), cx));
							_Z.push_back(cx);
							rMsg._bI = cx;
							rMsg._aZ = cx;
							//return rMsg;
						}
						else if (cx._e <= laT &&  cx._s < rbTBackUp)
						{
							//add to left
							_I.erase(find(_I.begin(), _I.end(), cx));
							_Z.push_back(cx);
							_ZL.push_back(cx);
							rMsg._bI = cx;
							rMsg._aZ = cx;
							//return rMsg;
						}
						else if (cx._e > laT && cx._s >= rbTBackUp)
						{
							if (cx._s < rbT)
							{
								//two ways
								Y lbT = leftBetaTightPointforZL(cx._s);
								vector<X> cxR;
								for (int i = 0; i < TML.size(); i++)
								{
									if (TML[i]._s >= lbT)
									{
										cxR.push_back(TML[i]);
									}
								}
								sort(cxR.begin(), cxR.end(), cmpXEndInc);
								X forwardX = cxR[cxR.size() - 1];
								if (cmpXEndInc(forwardX, cx))
								{
									//x to right
									sort(backX.begin(), backX.end(), cmpXEndInc);
									_ZL.push_back(backX[0]);//by invariant 12
									_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
									_ZR.push_back(cx);
									_Z.push_back(cx);
									_I.erase(find(_I.begin(), _I.end(), cx));
									rMsg._bI = cx;
									rMsg._aZ = cx;
									//return rMsg;
								}
								else
								{
									//switch
									//Y raT = rightAlphaTightPointforZR(forwardX._e);
									//vector<X> backXFilter;
									//for (int i = 0; i < backX.size(); i++)
									//{
									//	if (raT._id  == -1 || raT._id != -1 && backX[i]._e <= raT)
									//	{
									//		backXFilter.push_back(backX[i]);
									//	}
									//}
									//sort(backXFilter.begin(), backXFilter.end(), cmpXBeginDec);
									////Y rbTforcx = rightBetaTightPointforZL(backXFilter[backXFilter.size() - 1]._s);
									//vector<X> realBackX = backXFilter;
									///*for (int i = 0; i < backXFilter.size(); i++)
									//{
									//	if (backXFilter[i]._s < rbTforcx)
									//	{
									//		realBackX.push_back(backXFilter[i]);
									//	}
									//}*/
									//sort(realBackX.begin(), realBackX.end(), cmpXEndInc);
									_Z.push_back(cx);
									_ZL.push_back(cx);
									_I.erase(find(_I.begin(), _I.end(), cx));
									_ZL.erase(find(_ZL.begin(), _ZL.end(), forwardX));
									_ZR.push_back(forwardX);
									_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
									_ZL.push_back(backX[0]);
									rMsg._bI = cx;
									rMsg._aZ = cx;
									//return rMsg;
								}
							}
							else
							{
								//only to right
								sort(backX.begin(), backX.end(), cmpXEndInc);
								_ZL.push_back(backX[0]);//by invariant 12
								_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
								_ZR.push_back(cx);
								_Z.push_back(cx);
								_I.erase(find(_I.begin(), _I.end(), cx));
								rMsg._bI = cx;
								rMsg._aZ = cx;
								//return rMsg;
							}
						}
						else
						{
							//cx._e <= laT && cx._s >= rbTBackUp
							//need switch
							Y lbT = leftBetaTightPointforZL(cx._s);
							vector<X> cxR;
							for (int i = 0; i < TML.size(); i++)
							{
								if (TML[i]._s >= lbT)
								{
									cxR.push_back(TML[i]);
								}
							}
							sort(cxR.begin(), cxR.end(), cmpXEndInc);
							X forwardX = cxR[cxR.size() - 1];
							//Y raT = rightAlphaTightPointforZR(forwardX._e);
							//vector<X> backXFilter;
							//for (int i = 0; i < backX.size(); i++)
							//{
							//	if (raT._id == -1 || raT._id != -1 && backX[i]._e <= raT)
							//	{
							//		backXFilter.push_back(backX[i]);
							//	}
							//}
							//sort(backXFilter.begin(), backXFilter.end(), cmpXBeginDec);
							////Y rbTforcx = rightBetaTightPointforZL(backXFilter[backXFilter.size() - 1]._s);
							//vector<X> realBackX = backXFilter;
							///*for (int i = 0; i < backXFilter.size(); i++)
							//{
							//	if (backXFilter[i]._s < rbTforcx)
							//	{
							//		realBackX.push_back(backXFilter[i]);
							//	}
							//}*/
							//sort(realBackX.begin(), realBackX.end(), cmpXEndInc);
							_ZL.push_back(cx);
							_Z.push_back(cx);
							_I.erase(find(_I.begin(), _I.end(), cx));
							_ZL.erase(find(_ZL.begin(), _ZL.end(), forwardX));
							_ZR.push_back(forwardX);
							_ZR.erase(find(_ZR.begin(), _ZR.end(), backX[0]));
							_ZL.push_back(backX[0]);
							rMsg._bI = cx;
							rMsg._aZ = cx;
							//return rMsg;
						}
					}
				}
			}
		}
		//adjust
		X testX = rMsg._aZ;
		if (testX._id != -1 && testX._s < _rightChild->getIntervalStart() && testX._e >= _rightChild->getIntervalStart())
		{
			//a TL element
			if (find(_ZL.begin(), _ZL.end(), testX) != _ZL.end())
			{
				vector<X> R;
				determineReachableSetinES(testX, R, *new bool);
				R.push_back(testX);
				sort(R.begin(), R.end(), cmpXBeginDec);
				X minBegin = R[R.size() - 1];
				if (cmpXEndInc(minBegin, testX))
				{
					vector<Y> YL;
					vector<Y> YR;
					for (int i = 0; i < _Y.size(); i++)
					{
						if (_Y[i] < _rightChild->getIntervalStart())
						{
							YL.push_back(_Y[i]);
						}
					}
					YL.push_back(rMsg._aY);
					for (int i = 0; i < _Y.size(); i++)
					{
						if (_Y[i] >= _rightChild->getIntervalStart())
						{
							YR.push_back(_Y[i]);
						}
					}
					vector<X> tempZL = _ZL;
					vector<X> tempZR = _ZR;
					tempZL.push_back(minBegin);
					tempZR.push_back(testX);
					tempZR.erase(find(tempZR.begin(), tempZR.end(), minBegin));
					tempZL.erase(find(tempZL.begin(), tempZL.end(), testX));
					
					vector<X> ZLNew, ZRNew;
					gloverMatching(tempZL, YL, &ZLNew);
					gloverMatching(tempZR, YR, &ZRNew);
					if (ZLNew.size() == _ZL.size() && ZRNew.size() == _ZR.size())
					{
						_ZL.push_back(minBegin);
						_ZR.push_back(testX);
						_ZR.erase(find(_ZR.begin(), _ZR.end(), minBegin));
						_ZL.erase(find(_ZL.begin(), _ZL.end(), testX));
					}
				}
			}
			else
			{
				vector<X> R;
				determineReachableSetinEE(testX, R, *new bool);
				R.push_back(testX);
				sort(R.begin(), R.end(), cmpXEndInc);
				X maxEnd = R[R.size() - 1];
				if (cmpXEndInc(testX, maxEnd))
				{
					vector<Y> YL;
					vector<Y> YR;
					for (int i = 0; i < _Y.size(); i++)
					{
						if (_Y[i] < _rightChild->getIntervalStart())
						{
							YL.push_back(_Y[i]);
						}
					}
					YL.push_back(rMsg._aY);
					for (int i = 0; i < _Y.size(); i++)
					{
						if (_Y[i] >= _rightChild->getIntervalStart())
						{
							YR.push_back(_Y[i]);
						}
					}
					vector<X> tempZL = _ZL;
					vector<X> tempZR = _ZR;
					tempZL.push_back(testX);
					tempZR.push_back(maxEnd);
					tempZR.erase(find(tempZR.begin(), tempZR.end(), testX));
					tempZL.erase(find(tempZL.begin(), tempZL.end(), maxEnd));

					vector<X> ZLNew, ZRNew;
					gloverMatching(tempZL, YL, &ZLNew);
					gloverMatching(tempZR, YR, &ZRNew);
					if (ZLNew.size() == _ZL.size() && ZRNew.size() == _ZR.size())
					{
						_ZL.push_back(testX);
						_ZR.push_back(maxEnd);
						_ZR.erase(find(_ZR.begin(), _ZR.end(), testX));
						_ZL.erase(find(_ZL.begin(), _ZL.end(), maxEnd));
					}
				}
			}
		}
		if (!backX.empty())
		{
			//adjust
			X testX = backX[0];
			if (testX._id != -1 && testX._s < _rightChild->getIntervalStart() && testX._e >= _rightChild->getIntervalStart())
			{
				//a TL element
				if (find(_ZL.begin(), _ZL.end(), testX) != _ZL.end())
				{
					vector<X> R;
					determineReachableSetinES(testX, R, *new bool);
					R.push_back(testX);
					sort(R.begin(), R.end(), cmpXBeginDec);
					X minBegin = R[R.size() - 1];
					if (cmpXEndInc(minBegin, testX))
					{
						vector<Y> YL;
						vector<Y> YR;
						for (int i = 0; i < _Y.size(); i++)
						{
							if (_Y[i] < _rightChild->getIntervalStart())
							{
								YL.push_back(_Y[i]);
							}
						}
						YL.push_back(rMsg._aY);
						for (int i = 0; i < _Y.size(); i++)
						{
							if (_Y[i] >= _rightChild->getIntervalStart())
							{
								YR.push_back(_Y[i]);
							}
						}
						vector<X> tempZL = _ZL;
						vector<X> tempZR = _ZR;
						tempZL.push_back(minBegin);
						tempZR.push_back(testX);
						tempZR.erase(find(tempZR.begin(), tempZR.end(), minBegin));
						tempZL.erase(find(tempZL.begin(), tempZL.end(), testX));

						vector<X> ZLNew, ZRNew;
						gloverMatching(tempZL, YL, &ZLNew);
						gloverMatching(tempZR, YR, &ZRNew);
						if (ZLNew.size() == _ZL.size() && ZRNew.size() == _ZR.size())
						{
							_ZL.push_back(minBegin);
							_ZR.push_back(testX);
							_ZR.erase(find(_ZR.begin(), _ZR.end(), minBegin));
							_ZL.erase(find(_ZL.begin(), _ZL.end(), testX));
						}
					}
				}
				else
				{
					vector<X> R;
					determineReachableSetinEE(testX, R, *new bool);
					R.push_back(testX);
					sort(R.begin(), R.end(), cmpXEndInc);
					X maxEnd = R[R.size() - 1];
					if (cmpXEndInc(testX, maxEnd))
					{
						vector<Y> YL;
						vector<Y> YR;
						for (int i = 0; i < _Y.size(); i++)
						{
							if (_Y[i] < _rightChild->getIntervalStart())
							{
								YL.push_back(_Y[i]);
							}
						}
						YL.push_back(rMsg._aY);
						for (int i = 0; i < _Y.size(); i++)
						{
							if (_Y[i] >= _rightChild->getIntervalStart())
							{
								YR.push_back(_Y[i]);
							}
						}
						vector<X> tempZL = _ZL;
						vector<X> tempZR = _ZR;
						tempZL.push_back(testX);
						tempZR.push_back(maxEnd);
						tempZR.erase(find(tempZR.begin(), tempZR.end(), testX));
						tempZL.erase(find(tempZL.begin(), tempZL.end(), maxEnd));

						vector<X> ZLNew, ZRNew;
						gloverMatching(tempZL, YL, &ZLNew);
						gloverMatching(tempZR, YR, &ZRNew);
						if (ZLNew.size() == _ZL.size() && ZRNew.size() == _ZR.size())
						{
							_ZL.push_back(testX);
							_ZR.push_back(maxEnd);
							_ZR.erase(find(_ZR.begin(), _ZR.end(), testX));
							_ZL.erase(find(_ZL.begin(), _ZL.end(), maxEnd));
						}
					}
				}
			}

		}
		
		return rMsg;
		
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
	for (int i = 0; i < leftI.size(); i++)
	{
		if (leftI[i]._e > laT && leftI[i]._e >= _rightChild->getIntervalStart())
		{
			cR.push_back(leftI[i]);
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
				if (find(cR.begin(), cR.end(), leftI[i]) == cR.end())
				{
					cL.push_back(leftI[i]);
				}
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
			}
		}
		for (int i = 0; i < rightI.size(); i++)
		{
			if (rightI[i]._s < rbT)
			{
				if (find(cR.begin(), cR.end(), rightI[i]) == cR.end())
				{
					cL.push_back(rightI[i]);
				}
			}
		}
	}
	if (cTLI.empty() && cL.empty() && cR.empty())
	{
		//try back switch
		//no need


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
			//return rMsg;
		}
		else
		{
			//pull the cTP[0] back
			_ZR.push_back(cTP[0]);
			_Z.push_back(cTP[0]);
			_T.erase(find(_T.begin(), _T.end(), cTP[0]));
			rMsg._bT = cTP[0];
			rMsg._aZ = cTP[0];
			//return rMsg;
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
		/*if (find(cL.begin(), cL.end(), cx) == cL.end())
		{*/
		if (!forwardX.empty())
		{
			if (laT._id == -1 && cx._e < _rightChild->getIntervalStart() || laT._id != -1 && cx._e <= laT)
			{
				//must need forwardX forward
				Y lbT = leftBetaTightPointforZL(cx._s);
				vector<X> realForwardX;
				for (int i = 0; i < forwardX.size(); i++)
				{
					if (forwardX[i]._s >= lbT)
					{
						realForwardX.push_back(forwardX[i]);
					}
				}
				forwardX = realForwardX;
				sort(forwardX.begin(), forwardX.end(), cmpXEndInc);
				_ZR.push_back(forwardX[forwardX.size() - 1]);
				_ZL.erase(find(_ZL.begin(), _ZL.end(), forwardX[forwardX.size() - 1]));
				_ZL.push_back(cx);
				_Z.push_back(cx);
				_I.erase(find(_I.begin(), _I.end(), cx));
				rMsg._bI = cx;
				rMsg._aZ = cx;
				//return rMsg;
			}
			else
			{
				//a new invariant for ZL and ZR
				//not (cx can only be add to left): 1. cx can be add to both 2. cx can only be add to right
				sort(forwardX.begin(), forwardX.end(), cmpXBeginDec);
				Y rbT = rightBetaTightPointforZL(forwardX[0]._s);
				if (cx._e >= _rightChild->getIntervalStart() && cx._s < _rightChild->getIntervalStart() && cx._s < rbT)
				{
					//1. cx can be add to both 
					//calculate the real forwardX
					Y lbT = leftBetaTightPointforZL(cx._s);
					vector<X> realForwardX;
					for (int i = 0; i < forwardX.size(); i++)
					{
						if (forwardX[i]._s >= lbT)
						{
							realForwardX.push_back(forwardX[i]);
						}
					}
					forwardX = realForwardX;
					sort(forwardX.begin(), forwardX.end(), cmpXEndInc);
					if (cmpXEndInc(cx, forwardX[forwardX.size() - 1]))
					{
						_ZR.push_back(forwardX[forwardX.size() - 1]);
						_ZL.erase(find(_ZL.begin(), _ZL.end(), forwardX[forwardX.size() - 1]));
						_ZL.push_back(cx);
						_Z.push_back(cx);
						_I.erase(find(_I.begin(), _I.end(), cx));
						rMsg._bI = cx;
						rMsg._aZ = cx;
						//return rMsg;
					}
					else
					{
						_I.erase(find(_I.begin(), _I.end(), cx));
						_Z.push_back(cx);
						_ZR.push_back(cx);
						rMsg._bI = cx;
						rMsg._aZ = cx;
					}

				}
				else
				{
					_I.erase(find(_I.begin(), _I.end(), cx));
					_Z.push_back(cx);
					_ZR.push_back(cx);
					rMsg._bI = cx;
					rMsg._aZ = cx;
				}
			}
			
		}
		else
		{
			_I.erase(find(_I.begin(), _I.end(), cx));
			_Z.push_back(cx);
			_ZR.push_back(cx);
			rMsg._bI = cx;
			rMsg._aZ = cx;
		}
		
		//return rMsg;
		//}
		//else
		//{
		//	//in left
		//	sort(forwardX.begin(), forwardX.end(), cmpXEndInc);
		//	_ZR.push_back(forwardX[forwardX.size()-1]);
		//	_ZL.erase(find(_ZL.begin(), _ZL.end(), forwardX[forwardX.size() - 1]));
		//	_ZL.push_back(cx);
		//	_Z.push_back(cx);
		//	_I.erase(find(_I.begin(), _I.end(), cx));
		//	rMsg._bI = cx;
		//	rMsg._aZ = cx;
		//	return rMsg;
		//}
	}
	//adjust
	//adjust
	X testX = rMsg._aZ;
	if (testX._id != -1 && testX._s < _rightChild->getIntervalStart() && testX._e >= _rightChild->getIntervalStart())
	{
		//a TL element
		if (find(_ZL.begin(), _ZL.end(), testX) != _ZL.end())
		{
			vector<X> R;
			determineReachableSetinES(testX, R, *new bool);
			R.push_back(testX);
			sort(R.begin(), R.end(), cmpXBeginDec);
			X minBegin = R[R.size() - 1];
			if (cmpXEndInc(minBegin, testX))
			{
				vector<Y> YL;
				vector<Y> YR;
				for (int i = 0; i < _Y.size(); i++)
				{
					if (_Y[i] < _rightChild->getIntervalStart())
					{
						YL.push_back(_Y[i]);
					}
				}
				YL.push_back(rMsg._aY);
				for (int i = 0; i < _Y.size(); i++)
				{
					if (_Y[i] >= _rightChild->getIntervalStart())
					{
						YR.push_back(_Y[i]);
					}
				}
				vector<X> tempZL = _ZL;
				vector<X> tempZR = _ZR;
				tempZL.push_back(minBegin);
				tempZR.push_back(testX);
				tempZR.erase(find(tempZR.begin(), tempZR.end(), minBegin));
				tempZL.erase(find(tempZL.begin(), tempZL.end(), testX));

				vector<X> ZLNew, ZRNew;
				gloverMatching(tempZL, YL, &ZLNew);
				gloverMatching(tempZR, YR, &ZRNew);
				if (ZLNew.size() == _ZL.size() && ZRNew.size() == _ZR.size())
				{
					_ZL.push_back(minBegin);
					_ZR.push_back(testX);
					_ZR.erase(find(_ZR.begin(), _ZR.end(), minBegin));
					_ZL.erase(find(_ZL.begin(), _ZL.end(), testX));
				}
			}
		}
		else
		{
			vector<X> R;
			determineReachableSetinEE(testX, R, *new bool);
			R.push_back(testX);
			sort(R.begin(), R.end(), cmpXEndInc);
			X maxEnd = R[R.size() - 1];
			if (cmpXEndInc(testX, maxEnd))
			{
				vector<Y> YL;
				vector<Y> YR;
				for (int i = 0; i < _Y.size(); i++)
				{
					if (_Y[i] < _rightChild->getIntervalStart())
					{
						YL.push_back(_Y[i]);
					}
				}
				YL.push_back(rMsg._aY);
				for (int i = 0; i < _Y.size(); i++)
				{
					if (_Y[i] >= _rightChild->getIntervalStart())
					{
						YR.push_back(_Y[i]);
					}
				}
				vector<X> tempZL = _ZL;
				vector<X> tempZR = _ZR;
				tempZL.push_back(testX);
				tempZR.push_back(maxEnd);
				tempZR.erase(find(tempZR.begin(), tempZR.end(), testX));
				tempZL.erase(find(tempZL.begin(), tempZL.end(), maxEnd));

				vector<X> ZLNew, ZRNew;
				gloverMatching(tempZL, YL, &ZLNew);
				gloverMatching(tempZR, YR, &ZRNew);
				if (ZLNew.size() == _ZL.size() && ZRNew.size() == _ZR.size())
				{
					_ZL.push_back(testX);
					_ZR.push_back(maxEnd);
					_ZR.erase(find(_ZR.begin(), _ZR.end(), testX));
					_ZL.erase(find(_ZL.begin(), _ZL.end(), maxEnd));
				}
			}
		}
	}

	return rMsg;

}