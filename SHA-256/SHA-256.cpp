#include <iostream>
#include <fstream>

using namespace std;

int main() {
	int answer = 0;
	cout << "Hi" << endl << endl;
	while (answer != 5) {
		cout << "What do you want do?" << endl;
		cout << "1. Add text to file" << endl;
		cout << "2. Hash your file" << endl;
		cout << "3. Read your last hash again" << endl;
		cout << "4. Do a check of your hash" << endl;
		cout << "5. Exit" << endl;
		cout << "Write your answer: ";
		cin >> answer; 
		cout << endl;
		if (answer > 5 || answer < 1)
			cout << "Incorrect input" << endl << endl;
	}
}
