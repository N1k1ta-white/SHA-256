#include <iostream>
#include <fstream>

using namespace std;

void add_text() {
	char text[10000];
	cout << "Write your text: ";
	cin.ignore();
	cin.getline(text, 10000);
	fstream write("files/file.txt", ios::out);
	write << text;
	write.close();
	cout << endl;
}

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
		if (answer == 1) {
			add_text();
		}
		else if (answer == 2) {

		}
		else if (answer == 3) {

		}
		else if (answer == 4) {

		}
		else if (answer == 5);
		else
			cout << "Incorrect input" << endl << endl;
	}
}
