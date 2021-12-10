#include <iostream>
#include "caculator.hpp"
int main()
{
	//try {
	//	std::string input = "(1+2)*(3/2)+6*5-2*4/(2+5)*6-10";
	//	auto r = evalute_expression(input.data(), input.data() + input.size());
	//	std::cout << r << std::endl;
	//}
	//catch (std::exception const& e) {
	//	std::cout << e.what() << std::endl;
	//}


	// 
	// 
	while (true) {
		std::string input;
		std::cin >> input;
		if (input == "exit") {
			break;
		}
		try {
			auto r = caculator::evalute_expression(input.data(), input.data() + input.size());
			std::cout<< "result: " << r << std::endl;
		}
		catch (std::exception const& e) {
			std::cout << e.what() << std::endl;
		}
	}
}