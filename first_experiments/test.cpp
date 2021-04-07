#include <iostream>
#include <string>
#include <tuple>

std::tuple<std::string, int> CreatePerson() {
	return { "Peter", 22 };
}

int main() {

	auto[name, age] = CreatePerson();

	std::cout << name << " " << age << std::endl;
}
