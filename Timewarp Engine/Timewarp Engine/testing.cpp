#include <iostream>
#include <vector>
using namespace std;

int msain() {
	vector<int> myVector;

	for (int i = 0; i < 10; i++) {
		myVector.push_back(i);
	}

	const int size = myVector.size();
	return 0;
}