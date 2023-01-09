#include <iostream>
#include <fstream>
#include <bitset>

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

int size_of_text() {
	char reader;
	fstream read("files/file.txt", ios::in);
	int res = 0;
	while (read >> noskipws >> reader)
		res++;
	read.close();
	return res;
}

void message_to_bin(int chunks) {
	char reader;
	int time = 0, strings = 1;
	fstream read("files/file.txt", ios::in);
	fstream write("files/message_block.txt", ios::out);
	while (read >> noskipws >> reader) {
		bitset<8> bin(reader);
		write << bin;
		if (time == 3) {
			write << endl;
			time = -1;
			strings++;
		}
		time++;
	}
	bitset<8> bin(128);
	write << bin;
	time++;
	for (int i = time; i < 4; i++) {
		bitset<8> bin(0);
		write << bin;
	}
	write << endl;
	strings++;
	while (strings++ != chunks * 16 - 1) {
		write << "00000000000000000000000000000000" << endl;
	}
	bitset<64> size(size_of_text() * 8);
	for (int i = 63; i >= 0; i--) {
		write << size[i];
		if (i == 32)
			write << endl;
	}
	read.close();
	write.close();
}

void hash_func() {
	int count_chunk = (size_of_text() * 8 + 1 + 64) / 512 + 1;
	message_to_bin(count_chunk);
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
			hash_func();
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
