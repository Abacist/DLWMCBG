#include<iostream>
#include<fstream>
#include"Basic.h"
#include"Tree.h"

#pragma warning (disable:4018)

using namespace std;

const int mY =			10;
const int ur =			40;
int verifyEachUpdate =	1;//0 not verify, !0 verify
int gen =				1;

void generator(char* fileName = "input.txt", int MaxY = mY, int UpdateRange = ur, int WeightRange = 1000);

int main()
{
	int cases = 1;
	
	while (true)
	{
		if (gen)
		{
			generator(); //generator need to be fit the format
		}
		ifstream in("input.txt");
		//ofstream out("output.txt");
		if (!in)
		{
			cout << "input file open error" << endl;
			return 0;
		}
		//get the range of Y
		int rangeOfY;
		in >> rangeOfY;
		//cout << rangeOfY << endl;

		// init Tree
		vector<Y> tempVecY;
		for (int i = 0; i < rangeOfY; i++)
		{
			Y temp;
			in >> temp._id;
			tempVecY.push_back(temp);
		}
		// TBD: add y with id=0 to avoid split in insert y ops.
		Tree* pTree = new Tree(tempVecY);

		char command;
		while (!in.eof())
		{
			in >> command;
			if (command == '#')
			{
				char s[20];
				in.get(s, 20, '\n');
				continue;
			}
			if (command == '$')
			{
				break;
			}

			switch (command)
			{
				//insert an X
			case '1':
			{
				X x;
				in >> x._id >> x._s._id >> x._e._id >> x._w;	// divide by Space

				// if x.s or x.e is not in _Y, insert it.	
				pTree->adjustXToProper(x);
				if (x._id == 9)
				{
					int a = 1;
				}
				pTree->insertXinTree(x);
				
				if (verifyEachUpdate)
				{
					int flag = pTree->verifyInvariantsRecur();
					if (flag == 0)
					{
						//cout << "Insert X pass, id " << x._id << endl;
					}
					else
					{
						cout << endl << endl << endl << endl << "After InsertX, Not satify, please check! Error code: " << flag << endl
							<< "X id:" << x._id << endl;
						goto End;
					}
				}
				

			}break;


			case '2':
			{

			}break;
			case '3':
			{
				Y y;
				in >> y._id;	
				if (y._id == 3)
				{
					int a = 1;
				}
				vector<Y>::iterator it = find(pTree->_root->_Y.begin(), pTree->_root->_Y.end(), y);
				if (it == pTree->_root->_Y.end())
				{
					

					pTree->insertYinTree(y);

					if (verifyEachUpdate)
					{
						int flag = pTree->verifyInvariantsRecur();
						if (flag == 0)
						{
							//cout << "Insert Y pass, id " << y._id << endl;
						}
						else
						{
							cout << endl << endl << endl << endl << "After InsertY, Not satify, please check! Error code: " << flag << endl
								<< "Y id:" << y._id << endl;
							goto End;
						}
					}
					
				}
				else
				{
					// cout <<"Y: "<<y._id << " already exists." << endl;
					//throw new exception();
				}



			}break;
			case '4':
			{

			}break;
			case '5':
			{

			}break;
			case '6':
			{
				X x;
				in >> x._id >> x._s._id >> x._e._id;
			}break;
			case '7':
			{

			}break;
			case '8':
			{
				Y y;
				in >> y._id;

			}break;
			}


		}
		in.close();
		//out.close();
		//verify
//		int flag = pTree->verifyTreeInvariantsSimple();
		int flag = pTree->verifyInvariantsRecur();
//		int flag = pTree->_root->_leftChild->verifyNodeInvariants();
		//unweighted case
//		flag += pTree->verifyInvariantsInUnweightedCase();
		if (flag == 0)
		{
			cout << "============================Case " << cases++ << " passed!" << endl;
		}
		else
		{
			cout << endl << endl << endl << endl << "Not satify, please check! Error code: " << flag << endl;
			goto End;
		}

	}
End:
	return 0;
}

