#include<iostream>
#include<fstream>
#include"Basic.h"
#include"Tree.h"

#pragma warning (disable:4018)

using namespace std;

void generator(char* fileName = "input.txt", int MaxY = 80, int UpdateRange = 300, int WeightRange = 1000);

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
				pTree->adjustXToProper(x);
				// if x.s or x.e is not in _Y, insert it.	// TBD			
				
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
				Y y;
				in >> y._id;	
				vector<Y>::iterator it = find(pTree->_root->_Y.begin(), pTree->_root->_Y.end(), y);
				if (it == pTree->_root->_Y.end())
				{
					pTree->insertYinTree(y);
				}
				else
				{
					cout <<"Y: "<<y._id << "already exists." << endl;
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
//		int flag = pTree->verifyTreeInvariantsSimple();
		int flag = pTree->verifyInvariantsRecur();
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

