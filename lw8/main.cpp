#include <stack>
#include <iostream>
#include <string>

int main() {
	int count{};
	std::cin >> count;

	std::string passwordLine;
	std::getline(std::cin, passwordLine);

	std::stack<char> password{};

	for (int i = 0; i < count; i++)
	{
		if (passwordLine[i] == '<') 
		{
			password.pop();
		}
		else
		{
			password.push(passwordLine[i]);
		}
	}

	for (auto passChar : password)
	{
		std::cout << passChar;
	}
}
