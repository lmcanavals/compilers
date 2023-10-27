#include <iostream>
#include <memory>

class A {
	int x;

public:
	A(int x) : x(x) {}

	int &gitX1() { return x; }
	const int &gitX2() const { return x; }
};

int main() {
	std::unique_ptr<A> obj = std::unique_ptr<A>(new A(20));

	std::cout << obj->gitX1() << std::endl;
	int &y = obj->gitX1();
	y = 30;
	std::cout << obj->gitX1() << std::endl;
	const int &z = obj->gitX2();
	z = 40;

	return 0;
}
