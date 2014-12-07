#include<iostream>
#include<fstream>
#include"Basic.h"
#include"Tree.h"

#pragma warning (disable:4018)

using namespace std;

void generator(char* fileName = "input.txt", int MaxY = 30, int UpdateRange = 150, int WeightRange = 100);

int main()
{
	int cases = 1;
	while (true)
	{
		generator(); //generator need to be fit the format
		ifstream in("input.txt");
		ofstream out("output.txt");

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
				// if x.s or x.e is not in _Y, insert it.	// TBD			
				if (x._id == 10)
				{
					int a = 0;
				}
				pTree->insertXinTree(x);
				/*if (pTree->verifyTreeInvariants() != 0)
				{
					int a = 0;
				}*/



			}break;


			case '2':
			{

			}break;
			case '3':
			{

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
		//output to file
		//cout << "end" << endl;
		
		/*vector<X> Z = pTree->_root->_Z;
		sort(Z.begin(), Z.end(), cmpXID);
		for (int i = 0; i < Z.size(); i++)
		{
		out << Z[i]._id << endl;
		}
		system("Glover.exe");*/

		in.close();
		out.close();
		//verify
		int flag = pTree->verifyInvariantsRecur();
//		int flag = pTree->verifyTreeInvariants();
		if (flag == 0
			//pTree->_root->verifyNodeInvariants()
			//&& pTree->_root->_leftChild->verifyNodeInvariants()
			//&& pTree->_root->_leftChild->_leftChild->verifyNodeInvariants() 
			//&& pTree->_root->_leftChild->_leftChild->_leftChild->_leftChild->veifiyNodeInvariants()
			//&& pTree->_root->_leftChild->_leftChild->_leftChild->_leftChild->_leftChild->veifiyNodeInvariants()
			)
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

