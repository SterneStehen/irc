#include <iostream>
#include <array>
#include <vector>


int main(){
	std::vector<std::string> food ={ "aple", "milk", "banan"};
	int i = 0;
	food.push_back("bred");
	while (i < food.size())
	{

		std::cout << food[i] << std::endl;
		i++;
	}
	return 0;
}