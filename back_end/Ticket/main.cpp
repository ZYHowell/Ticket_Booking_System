#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "parser.hpp"
#include "bookingSystem.h"

ticketBookingSystem S;

int main() {
	std::ios_base::sync_with_stdio(0);
	std::string str,s;
	while (true) {
		std::getline(std::cin,str);
		std::stringstream ss(str);
		ss >> s;
		if (s == "add_train" || s == "modify_train") {
			ss >> s >> s >> s;
			int n;
			ss >> n;
			while (n--) {
				std::getline(std::cin, s);
				str = str + '\n' + s;
			}
		}
		EXECUTOR(S,str);
	}
	return 0;
}