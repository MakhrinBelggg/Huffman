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
ifstream treeFile("tree.txt", ios::in | ios::binary); // открыть для чтения в двоичном виде

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

		ofstream decompressedFile(uncompressed, ios::out | ios::binary); // заводим новый файл для записи расжатого текста
		 
		Node *tmp = this;
		int counter = 0;
		char buff;
		compressedFile.read(&buff, sizeof(buff)); // читаем первый символ
		bool flag;
		while (!compressedFile.eof()) // преобразуем кодировки в символы и записываем в новый файл
		{
			flag = (buff & (1 << (7 - counter))); // пока не 0
			if (flag) tmp = tmp->right;
			else tmp = tmp->left;

			if (tmp->left == nullptr && tmp->right == nullptr)
			{
				if (tmp->ch != 10)
				{
					cout << noskipws << tmp->ch;
					decompressedFile << noskipws << tmp->ch; // пишем символ
				}
				else
				{
					cout << noskipws << "\r\n";
					decompressedFile << noskipws << "\r\n"; // переносим бегунок на новую строку и записываем символ переноса строки
				}
				tmp = this;
			}
			++counter;
			if (counter == 8)
			{
				counter ^= counter; // обнулили счетчик
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

int fileSize(string fileName)
{
	ifstream file(fileName, ios_base::binary);  // читаем файл в двоичном виде
	file.seekg(0, ios_base::end);
	int size = file.tellg();
	file.close();
	return size;
}

void deleteLastByte(string fileName)
{
	ifstream file(fileName);
	stringstream buffer;
	buffer << file.rdbuf(); // читаем

	string content = buffer.str();
	file.close();
	content.pop_back(); // удаляем последний 0

	ofstream _file(fileName, ios::trunc); // перезаписываем без нуля в конце
	_file << content;
	_file.close();
}

map<char, int> treeCreater() // созадем мапу из файла-шапки
{
	map<char, int> tab;
	int number, _key;
	treeFile >> number; //считываем первое число из tree.txt в number (кол-во пар)
	cout << "pairs number = " << number << endl;

	char _ch;
	while (number != 0)
	{
		treeFile.read(&_ch, sizeof(_ch)); //читаем посимвольно в ch
		treeFile >> _key;
		if (_ch == 13) _ch = 10;  // перенос строки равно пробел
		tab[_ch] = _key; // заносим пару символ - количество
		cout << "tab[" << _ch << "] = " << _key << endl;
		number--;
	}
	return tab;
}

int main()
{
	map<char, int> tab = treeCreater(); // вызываем функцию, которая возвращает мапу
	list<Node*> tree;
	for (auto ix = tab.begin(); ix != tab.end(); ix++) // заполняем узлы
	{
		char _ch = ix->first; // считываем символ
		if (_ch > 31 && _ch < 127) 
		{
			Node *tmp = new Node;
			tmp->ch = ix->first;
			tmp->key = ix->second;
			tree.push_back(tmp); // добавляем узел в конец списка
		}
		if (_ch == 10)
		{
			Node *tmp = new Node;
			tmp->ch = '\n';
			tmp->key = ix->second;
			tree.push_back(tmp);
		}
	}

	while (tree.size() != 1) 
	{
		tree.sort(check()); // сортируем: левый меньше правого

		Node *l = tree.front();
		tree.pop_front(); // удаляем
		Node *r = tree.front();
		tree.pop_front(); // удаляем

		Node *root = new Node(l, r); // дерево
		tree.push_back(root); // в конец листа
	}
	// проходим по отсортированному дереву
	Node *root = tree.front();  // начало с корня

	long int sizeOfFile = 0;
	for (int i = 10; i--;) char ch__ = treeFile.get(); // пропустив слово fileSizeIs
	treeFile >> sizeOfFile; // смотрим размер файла
	treeFile.close();

	bool test = root->unzip();

	if (!test)
	{
		cout << "Couldn't open file " << compressed << endl;
		return 0;
	}
	cout << "size of file must be " << sizeOfFile - 1 << " bytes" << endl; 
	long fS = fileSize(uncompressed); // размер нового файла
	while (fS > sizeOfFile - 2) // удаляем фиктивные нули пока размер нового файла не совпадет с оригиналом
	{
		cout << "current file size = " << fS << endl;
		deleteLastByte(uncompressed);
		fS = fileSize(uncompressed);
	}
	cout << "File " << compressed << " was decompressed and rewritten to file uncomressedText.txt" << endl;
	return 0;
}
