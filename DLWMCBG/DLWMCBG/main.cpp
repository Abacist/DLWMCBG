#include<iostream>
#include<fstream>
#include"Basic.h"
#include"Tree.h"

#pragma warning (disable:4018)
#pragma warning (disable:4012)

using namespace std;

const int mY =			100;
const int ur =			500;
int verifyEachUpdate =	0;		//0 not verify, !0 verify
int gen =				1;

void generator(char* fileName = "input.txt", int MaxY = mY, int UpdateRange = ur, int WeightRange = 1000);

int main()
{
	int cases = 1;
	
	while (true)
	{
		if (gen)
		{
			generator();
		}
		ifstream in("input.txt");
		if (!in)
		{
			cout << "input file open error" << endl;
			return 0;
		}
		//get the range of Y
		int rangeOfY;
		in >> rangeOfY;
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
				in >> x._id >> x._s._id >> x._e._id >> x._w;	

				
			}break;


			case '2':
			{

			}break;
			case '3':
			{
				Y y;
				in >> y._id >>y._w;	
				

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
				in >> y._id>>y._w;

			}break;
			}


		}
		in.close();
		//verify

	}
End:
	return 0;
}

