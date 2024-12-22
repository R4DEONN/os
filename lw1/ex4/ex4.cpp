#include <iostream>
#include "cmath"

bool IsPrime(int n)
{
	if (n <= 1)
	{
		return false;
	}
	if (n == 2)
	{
		return true;
	}

	if (n % 2 == 0)
	{
		return false;
	}

	for (int i = 2; i <= std::sqrt(n); i++)
	{
		if (n % i == 0)
		{
			return false;
		}
	}
	return true;
}

int main()
{
	int num;
	std::cout << "Enter a number: ";
	std::cin >> num;
	if (IsPrime(num))
	{
		std::cout << "This number is prime" << std::endl;
	}
	else
	{
		std::cout << "This number isn't prime" << std::endl;
	}
	return 0;
}