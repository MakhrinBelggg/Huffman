#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
using namespace std;

string compressed = "compressedText.txt",
uncompressed = "uncompressedText.txt";
ifstream treeFile("tree.txt", ios::in | ios::binary); // ������� ��� ������ � �������� ����

class Node
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

	friend ostream & operator<< (ostream &out, const Node &that)
	{
		if (that.ch == 10) return out << "[new_line] = " << that.key << endl;
		else return out << '[' << that.ch << "] = " << that.key << endl;
	}

	bool unzip()
	{
		ifstream compressedFile(compressed, ios::in | ios::binary);
		if (!compressedFile.is_open()) return 0;

		ofstream decompressedFile(uncompressed, ios::out | ios::binary); // ������� ����� ���� ��� ������ ��������� ������
		 
		Node *tmp = this;
		int counter = 0;
		char buff;
		compressedFile.read(&buff, sizeof(buff)); // ������ ������ ������
		bool flag;
		while (!compressedFile.eof()) // ����������� ��������� � ������� � ���������� � ����� ����
		{
			flag = (buff & (1 << (7 - counter))); // ���� �� 0
			if (flag) tmp = tmp->right;
			else tmp = tmp->left;

			if (tmp->left == nullptr && tmp->right == nullptr)
			{
				if (tmp->ch != 10)
				{
					//cout << noskipws << tmp->ch;
					decompressedFile << noskipws << tmp->ch; // ����� ������
				}
				else
				{
					//cout << noskipws << "\r\n";
					decompressedFile << noskipws << "\r\n"; // ��������� ������� �� ����� ������ � ���������� ������ �������� ������
				}
				tmp = this;
			}
			++counter;
			if (counter == 8)
			{
				counter ^= counter; // �������� �������
				compressedFile.read(&buff, sizeof(buff));
			}
		}
		compressedFile.close();
		decompressedFile.close();
		return 1;
	}
};

struct check
{
	bool operator() (const Node *l, const Node *r) const
	{
		return l->key < r->key;
	}
};

long int fileSize(string fileName)
{
	ifstream file(fileName, ios_base::binary);  // ������ ���� � �������� ����
	file.seekg(0, ios_base::end);
	int size = (int)file.tellg();
	file.close();
	return size;
}

void deleteLastByte(string fileName)
{
	ifstream file(fileName);
	stringstream buffer;
	buffer << file.rdbuf(); // ������

	string content = buffer.str();
	file.close();
	if (!content.empty()) content.pop_back(); // ������� ��������� 0

	ofstream _file(fileName, ios::trunc); // �������������� ��� ���� � �����
	_file << content;
	_file.close();
}

map<char, int> treeCreater() // ������� ���� �� �����-�����
{
	map<char, int> tab;
	int number, _key;
	treeFile >> number; //��������� ������ ����� �� tree.txt � number (���-�� ���, ������ ��������)
	cout << "pairs number = " << number << endl;

	char _ch, space;
	while (number != 0)
	{
		treeFile.read(&_ch, sizeof(_ch)); //������ ����������� � ch

		treeFile.read(&space, sizeof(space));

		treeFile >> _key;
		if (_ch == 13 || _ch == 10) _ch = 10;  // /r � /n
		
		tab[_ch] = _key; // ������� ���� ������ - ����������

		treeFile.read(&space, sizeof(space));

		cout << "tab[" << _ch << "] = " << _key << endl;
		number--;
	}
	return tab;
}

bool testYourLuck(string originalFile, string otherFile)
{
	if (fileSize(originalFile) != fileSize(otherFile)) return 0;

	ifstream file1(originalFile, ios::in | ios::binary);
	if (!file1.is_open()) return 0;

	ifstream file2(otherFile, ios::in | ios::binary);
	if (!file2.is_open()) return 0;
	
	char ch1, ch2;
	while (!file1.eof())
	{
		file1 >> ch1;
		file2 >> ch2;
		if (ch1 != ch2) return 0;
	}
	return 1;
}
int main()
{
	map<char, int> tab = treeCreater(); // �������� �������, ������� ���������� ����
	list<Node*> tree;
	for (auto ix = tab.begin(); ix != tab.end(); ix++) // ��������� ����
	{
		char _ch = ix->first; // ��������� ������
		if (_ch > 31 && _ch < 127) 
		{
			Node *tmp = new Node;
			tmp->ch = ix->first;
			tmp->key = ix->second;
			tree.push_back(tmp); // ��������� ���� � ����� ������
		}
		else if (_ch == 10)
		{
			Node *tmp = new Node;
			tmp->ch = '\n';
			tmp->key = ix->second;
			tree.push_back(tmp);
		}
	}

	while (tree.size() != 1) 
	{
		tree.sort(check()); // ���������: ����� ������ �������

		Node *l = tree.front();
		tree.pop_front(); // �������
		Node *r = tree.front();
		tree.pop_front(); // �������

		Node *root = new Node(l, r); // ������
		tree.push_back(root); // � ����� �����
	}
	// �������� �� ���������������� ������
	Node *root = tree.front();  // ������ � �����

	long int sizeOfFile = 0;
	for (int i = 10; i--;) char ch__ = treeFile.get(); // ��������� ����� fileSizeIs
	treeFile >> sizeOfFile; // ������� ������ �����
	treeFile.close();

	bool test = root->unzip();

	cout << endl;
	if (!test)
	{
		cout << "Couldn't open file " << compressed << endl;
		return 12;
	}
	cout << "size of file must be " << sizeOfFile << " bytes" << endl; 
	long fS = fileSize(uncompressed); // ������ ������ �����
	while (fS > sizeOfFile)  // ������� ��������� ���� ���� ������ ������ ����� �� �������� � ����������
	{
		deleteLastByte(uncompressed);
		fS = fileSize(uncompressed);
		cout << "current file size = " << fS << endl;
	} 
	cout << "File " << compressed << " was decompressed and rewritten to file uncomressedText.txt" << endl << endl;
	if (testYourLuck("originalText.txt", uncompressed)) cout << "JOB HAS DONE" << endl;
	else cout << "Comparing of texts failed" << endl;

	return 0;
}
