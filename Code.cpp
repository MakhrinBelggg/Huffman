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
ofstream treeFile("tree.txt", ios::out | ios::binary);  // открываем файл дл€ записи 
long int sizeOfFile = 0;

class Node //вершина дерева
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
			hufCode.push_back(0); // добавл€ем в конец вектора 0
			left->tabCreater();
		}
		if (right != nullptr)
		{
			hufCode.push_back(1); // добавл€ем в конец вектора 1
			right->tabCreater(); // рекурси€
		}
		if (left == nullptr && right == nullptr) trueTab[ch] = hufCode; // заносим код символа в мапу trueTab
		if (!hufCode.empty()) hufCode.pop_back(); //удал€ем последнюю €чейку
	}

	bool zip(string fileName)
	{
		ifstream input(fileName, ios::out | ios::binary); // открываем файл на потоковое двоичное чтение
		if (!input.is_open()) return 0; // выходи если не открылс€

		ofstream output(compressed, ios::out | ios::binary); // открываем файл на запись
		int counter = 0;
		char buff = 0;
		while (!input.eof())
		{
			char _ch;
			input.read(&_ch, sizeof(_ch)); // читаем посимвольно
			if (_ch > 31 && _ch < 127 || _ch == 10) // текстовые символы  или endl
			{
				vector<bool> res = trueTab[_ch]; // берем кодировку символа ch, кладем в вектор рес

				for (int j = 0; j < (int) res.size(); ++j) //этап 4 преобразуем  кодировки в символы
				{
					buff |= (res[j] << (7 - counter)); 
					++counter;
					if (counter == 8)
					{
						counter ^= counter; // обнул€ем
						output << buff; // заносим букву в компрессированный файл
						buff ^= buff;   // обнул€ем
						
					}
				}
			}
		}
		if (counter < 8) output << buff; // заносим 
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
	inputFile.seekg(0, ios_base::end); // пройтись до конца 
	int size = (int)inputFile.tellg(); // указать сколько прошли, размер файла в байтах
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
	ifstream inputFile(fileName, ios::out | ios::binary); // открываем файл на потоковое двоичное чтение
	map<char, int> tab;
	while (!inputFile.eof()) // создаем таблицу частотности в мапе
	{
		char _ch;
		inputFile.read(&_ch, sizeof(_ch)); // считываем строку посимвольно в ch
		if (_ch > 31 && _ch < 127)
		{
			if (tab.find(_ch) == tab.end()) 
				tab[_ch] = 0; // !! если такой пары еще нет, добавл€ем

			tab[_ch]++;
		}
		if (_ch = 13) 
		{
			if (tab.find(_ch) == tab.end())
				tab[_ch] = 0; // !! если такой пары еще нет, добавл€ем

			tab[10]++; // \n 
		}
	}
	treeFile << tab.size(); // заносим в tree.txt кол-во различных символов (пар в мапе)

	list<Node*> tree; // создаем двусв€зный список 

	for (auto ix = tab.begin(); ix != tab.end(); ix++)
	{
		Node *tmp = new Node; // создаем вершину/узел
		char _ch = ix->first;  // берем букву из пары в мапе
		if (_ch != 10) // если не /n
		{
			tmp->ch = ix->first; // заносим значение в узел 
			tmp->key = ix->second; // ключ
			treeFile << tmp->ch << tmp->key; // заносим в tree.txt символ и ключ
			tree.push_back(tmp); // добавл€ем в конец дерева текущий узел
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
	// каждый эл массива €вл указателем на Node
	while (tree.size() != 1)
	{
		tree.sort(check());
		Node *l = tree.front(); // ссылка на 1 элемент
		tree.pop_front(); // удал€ем 1 элемент
		Node *r = tree.front();
		tree.pop_front();
		Node *batya = new Node(l, r);
		tree.push_back(batya); // добавить в конец
	}
	Node *root = tree.front(); // ссылка на 1 элемент
	root->tabCreater(); // проходим по дереву составл€ем таблицу кодировки
	inputFile.close(); // закрываем файл

	int test = root->zip(fileName); // файл compressedText
	treeFile << "fileSizeIs" << fileSize(fileName); // заносим в tree.txt размер исходного файла
	treeFile.close();
	if (!test)
	{
		cout << "Couldn't open file " << fileName << endl;
		return 666;
	}
	cout << "File " << fileName << " was compressed and rewritten to file " << compressed << endl;
	return 777;
}
