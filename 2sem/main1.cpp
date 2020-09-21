#include <iostream>

int main()
{
	int count(0);
	
	for (int x = 0; x < 7; x++)
	 	for (int y = 0; y < 8; y++)
			for (int z = 0; z < 9; z++)
				for (int t = 0; t < 10; t++)
					{if (x+y+z+t>=8)
						count++;}
	std::cout << count;
	
	return 0;
}
