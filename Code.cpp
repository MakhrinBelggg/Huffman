#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <string>
using namespace std;

vector<bool> hufCode; //int
map<char, vector<bool> > trueTab;
string compressed = "compressedText.txt",
uncompressed = "uncompressedText.txt";
ofstream treeFile("tree.txt", ios::out | ios::binary);  // ��������� ���� ��� ������ 
long int sizeOfFile = 0;

class Node //������� ������
{
public:
	int key;
	char ch;
	Node *left;
	Node *right;

	Node()
	{
		key = 0;
		ch = 0;
		left = right = nullptr;
	}

	Node(Node *l, Node *r)
	{
		left = l;
		right = r;
		key = l->key + r->key;
	}

	void tabCreater()
	{
		if (left != nullptr)
		{
			hufCode.push_back(0); // ��������� � ����� ������� 0
			left->tabCreater();
		}
		if (right != nullptr)
		{
			hufCode.push_back(1); // ��������� � ����� ������� 1
			right->tabCreater(); // ��������
		}
		if (left == nullptr && right == nullptr) trueTab[ch] = hufCode; // ������� ��� ������� � ���� trueTab
		if (!hufCode.empty()) hufCode.pop_back(); //������� ��������� ������
	}

	bool zip(string fileName)
	{
		ifstream input(fileName, ios::out | ios::binary); // ��������� ���� �� ��������� �������� ������
		if (!input.is_open()) return 0; // ������ ���� �� ��������

		ofstream output(compressed, ios::out | ios::binary); // ��������� ���� �� ������
		int counter = 0;
		char buff = 0;
		while (!input.eof())
		{
			char _ch;
			input.read(&_ch, sizeof(_ch)); // ������ �����������
			if (_ch > 31 && _ch < 127 || _ch == 10) // ��������� �������  ��� endl
			{
				vector<bool> res = trueTab[_ch]; // ����� ��������� ������� ch, ������ � ������ ���

				for (int j = 0; j < (int) res.size(); ++j) //���� 4 �����������  ��������� � �������
				{
					buff |= (res[j] << (7 - counter)); 
					++counter;
					if (counter == 8)
					{
						counter ^= counter; // ��������
						output << buff; // ������� ����� � ����������������� ����
						buff ^= buff;   // ��������
						
					}
				}
			}
		}
		if (counter < 8) output << buff; // ������� 
		input.close();
		output.close();
		return 1;
	}

	friend ostream & operator << (ostream &out, const Node &that)
	{
		if (that.ch == 10) return out << "[new_line] = " << that.key << endl;
		else return out << '[' << that.ch << "] = " << that.key << endl;
	}
};

int fileSize(string fileName)
{
	ifstream inputFile;
	inputFile.open(fileName, ios_base::binary); 
	inputFile.seekg(0, ios_base::end); // �������� �� ����� 
	int size = (int)inputFile.tellg(); // ������� ������� ������, ������ ����� � ������
	inputFile.close();
	return size;
}

struct check
{
	bool operator() (const Node *l, const Node *r) const
	{
		return l->key < r->key;
	}
};

int main()
{
	string fileName = "originalText.txt";
	ifstream inputFile(fileName, ios::out | ios::binary); // ��������� ���� �� ��������� �������� ������
	map<char, int> tab;
	while (!inputFile.eof()) // ������� ������� ����������� � ����
	{
		char _ch;
		inputFile.read(&_ch, sizeof(_ch)); // ��������� ������ ����������� � ch
		if (_ch > 31 && _ch < 127)
		{
			if (tab.find(_ch) == tab.end()) 
				tab[_ch] = 0; // !! ���� ����� ���� ��� ���, ���������

			tab[_ch]++;
		}
		if (_ch = 13) 
		{
			if (tab.find(_ch) == tab.end())
				tab[_ch] = 0; // !! ���� ����� ���� ��� ���, ���������

			tab[10]++; // \n 
		}
	}
	treeFile << tab.size(); // ������� � tree.txt ���-�� ��������� �������� (��� � ����)

	list<Node*> tree; // ������� ���������� ������ 

	for (auto ix = tab.begin(); ix != tab.end(); ix++)
	{
		Node *tmp = new Node; // ������� �������/����
		char _ch = ix->first;  // ����� ����� �� ���� � ����
		if (_ch != 10) // ���� �� /n
		{
			tmp->ch = ix->first; // ������� �������� � ���� 
			tmp->key = ix->second; // ����
			treeFile << tmp->ch << tmp->key; // ������� � tree.txt ������ � ����
			tree.push_back(tmp); // ��������� � ����� ������ ������� ����
		}
		else
		{
			tmp->ch = '\n';
			tmp->key = ix->second;
			treeFile << '\r' << tmp->key;
			tree.push_back(tmp);
		}
		cout << noskipws << *tmp << endl;
	}
	// ������ �� ������� ��� ���������� �� Node
	while (tree.size() != 1)
	{
		tree.sort(check());
		Node *l = tree.front(); // ������ �� 1 �������
		tree.pop_front(); // ������� 1 �������
		Node *r = tree.front();
		tree.pop_front();
		Node *batya = new Node(l, r);
		tree.push_back(batya); // �������� � �����
	}
	Node *root = tree.front(); // ������ �� 1 �������
	root->tabCreater(); // �������� �� ������ ���������� ������� ���������
	inputFile.close(); // ��������� ����

	int test = root->zip(fileName); // ���� compressedText
	treeFile << "fileSizeIs" << fileSize(fileName); // ������� � tree.txt ������ ��������� �����
	treeFile.close();
	if (!test)
	{
		cout << "Couldn't open file " << fileName << endl;
		return 666;
	}
	cout << "File " << fileName << " was compressed and rewritten to file " << compressed << endl;
	return 777;
}
