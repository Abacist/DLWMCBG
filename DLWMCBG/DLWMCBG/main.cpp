#include<iostream>
#include<fstream>
#include"Basic.h"
#include"Tree.h"

using namespace std;

int main()
{
	//generator("input.txt"); generator need to be fit the format
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
	cout << rangeOfY << endl;

	for (int i = 0; i < rangeOfY; i++)
	{
		Y temp;
		in >> temp._y;
	}


	Tree* pTree = new Tree();

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
			in >> x._id >> x._begin._y >> x._end._y >> x._w;
			pTree->insertXinTree(x);
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
			in >> x._id >> x._begin._y >> x._end._y;
		}break;
		case '7':
		{

		}break;
		case '8':
		{
			Y y;
			in >> y._y;

		}break;
		}

	}

	cout << "end" << endl;
	//system("LWCBGNew.exe");

	in.close();
	return 0;
}