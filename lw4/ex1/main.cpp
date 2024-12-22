#include <iostream>
#include <fstream>
#include <cctype>
#include <unistd.h>
#include <vector>
#include <string>
#include <thread>

using namespace std;

char FlipCharCase(unsigned char ch)
{
	if (std::islower(ch))
	{
		return static_cast<char>(std::toupper(ch));
	}
	else
	{
		return static_cast<char>(std::tolower(ch));
	}
}


void FlipCase(const string& inputFile)
{
	ifstream infile(inputFile);
	if (!infile)
	{
		cerr << "Ошибка: не удалось открыть файл " << inputFile << endl;
		_exit(1); //TODO: Как обычно
	}

	string outputFileName = inputFile + ".out";
	ofstream outfile(outputFileName);
	if (!outfile)
	{
		cerr << "Ошибка: не удалось создать файл " << outputFileName << endl;
		_exit(1);
	}

	char ch;
	while (infile.get(ch))
	{
		ch = FlipCharCase(ch);
		outfile.put(ch);
	}

	if (!outfile.flush())
	{
		//TODO: Throw
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " <files...>" << endl;
		return 1;
	}

	vector<jthread> threads;
//TODO: try catch
	for (int i = 1; i < argc; ++i)
	{
		threads.emplace_back([inputFile = argv[i]] { return FlipCase(inputFile); });
	}

	return 0;
}
