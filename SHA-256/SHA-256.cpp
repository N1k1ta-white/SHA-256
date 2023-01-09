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

void message_to_bin(int **mes_block, int chunks) {
	char reader, column = 0, row = 0;
	fstream read("files/file.txt", ios::in);
	while (read >> noskipws >> reader) {
		bitset<8> bin(reader);
		for (int i = 7; i >= 0; i--, column++)
			mes_block[row][column] = bin[i];
		if (column == 32) {
			row++;
			column = 0;
		}
	}
	bitset<8> bin(128);
	for (int i = 7; i >= 0; i--, column++)
		mes_block[row][column] = bin[i];
	if (column == 32) {
		row++;
		column = 0;
	}
	while (row != chunks - 2) {
		for (int i = column; i < 32; i++)
			mes_block[row][i] = 0;
		column = 0;
		row++;
	}
	bitset<64> bin_size(size_of_text() * 8);
	for (int i = 0; i < 32; i++)
		mes_block[row][i] = bin_size[63 - i];
	row++;
	for (int i = 0; i < 32; i++)
		mes_block[row][i] = bin_size[31 - i];
	read.close();
}


void hash_func() {
	int count_chunk = ((size_of_text() * 8 + 1 + 64) / 512 + 1) * 16;
	int** mes_block = new int* [count_chunk];
	for (int i = 0; i < count_chunk; i++)
		mes_block[i] = new int[32];
	message_to_bin(mes_block, count_chunk);

	for (int i = 0; i < count_chunk; i++)
		delete mes_block[i];
	delete[] mes_block;
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
