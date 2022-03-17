#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
using namespace std;

vector<bool> hufCode;
map<char, vector<bool> > trueTab;
string compressed = "compressedText.txt";
string uncompressed = "uncompressedText.txt";
ofstream treeFile("tree.txt", ios::out | ios::binary);
long int sizeOfFile = 0;

class Node
{

	int key;
	char ch;
	Node *left;
	Node *right;

public:

	Node();
	Node(Node *l, Node *r);

	void tabCreater();

	int zip(string fileName);
	int unzip();
};

Node::Node()
{
	key = 0;
	ch = 0;
	left = right = nullptr;
}

Node::Node(Node *l, Node *r)
{
	left = l;
	right = r;
	key = l->key + r->key;
}

void Node::tabCreater()
{
	if (left != nullptr)
	{
		hufCode.push_back(0);
		left->tabCreater();
	}
	if (right != nullptr)
	{
		hufCode.push_back(1);
		right->tabCreater();
	}
	if (left == nullptr && right == nullptr) trueTab[ch] = hufCode;
	hufCode.pop_back();
}