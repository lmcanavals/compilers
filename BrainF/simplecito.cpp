#include <iostream>
#include <vector>

using namespace std;

int main() {
	vector<int> v;
	for (int i = 0; i < 10; ++i) {
		v.push_back(i * 100);
	}

	for (int i = 0; i < 10; ++i) {
		cout << v[i] << ' ';
	}
	cout << '\n';

	for (vector<int>::iterator it = v.begin(); it != v.end(); it++) {
		cout << *it << ' ';
	}
	cout << '\n';

	for (auto e : v) {
		cout << e << ' ';
	}
	cout << '\n';

	return 0;
}
