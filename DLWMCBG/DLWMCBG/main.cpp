#include<iostream>
#include<fstream>
#include"Basic.h"
#include"Tree.h"

#pragma warning (disable:4018)

using namespace std;

void generator(char* fileName = "input.txt", int MaxY=100, int UpdateRange = 300);

int main()
{
	// generator(); //generator need to be fit the format
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
				// in >> x._id >> x._s._id >> x._e._id >> x._w;	// divide by Space
				// if x.s or x.e is not in _Y, insert it.	// TBD				
				// pTree->insertXinTree(x);

				// for test
				int flag;
				in >> x._id >> x._s._id >> x._e._id >> x._w >> flag;	// divide by Space
				pTree->_root->testInsertXintoNode(x, flag);	// for test verification
				
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
	//output to file and verify
	cout << "The result of invariant verification is " << pTree->veifiyTreeInvariants() << endl;

	cout << "end" << endl;
	vector<X> Z = pTree->_root->_Z;
	sort(Z.begin(), Z.end(), cmpXID);
	for (int i = 0; i < Z.size(); i++)
	{
		out << Z[i]._id << endl;
	}
	system("Glover.exe");

	in.close();
	out.close();
	return 0;
}

