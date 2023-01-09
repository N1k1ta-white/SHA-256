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

void right_rotate(int bin[], int num) {
	int clone[32];
	for (int i = 0; i < 32; i++)
		clone[i] = bin[i];
	for (int i = 0; i < num; i++)
		bin[i] = clone[32 - num + i];
	for (int i = num; i < 32; i++)
		bin[i] = clone[i - num];
}

void right_shift(int bin[], int num) {
	right_rotate(bin, num);
	for (int i = 0; i < num; i++)
		bin[i] = 0;
}

void xor_bin(int res[], int bin1[], int bin2[], int bin3[]) {
	for (int i = 0; i < 32; i++) {
		res[i] = bin1[i] + bin2[i] + bin3[i];
		if (res[i] > 1)
			res[i] -= 2;
	}
}

void read_bin(int bin[], int mes_sched[64][32], int pos) {
	for (int i = 0; i < 32; i++)
		bin[i] = mes_sched[pos][i];
}

void create_sigma0(int s0[], int mes_sched[64][32], int pos) {
	int bin1[32], bin2[32], bin3[32];
	read_bin(bin1, mes_sched, pos - 15);
	read_bin(bin2, mes_sched, pos - 15);
	read_bin(bin3, mes_sched, pos - 15);
	right_rotate(bin1, 7);
	right_rotate(bin2, 18);
	right_shift(bin3, 3);
	xor_bin(s0, bin1, bin2, bin3);
}

void create_sigma1(int s1[], int mes_sched[64][32], int pos) {
	int bin1[32], bin2[32], bin3[32];
	read_bin(bin1, mes_sched, pos - 2);
	read_bin(bin2, mes_sched, pos - 2);
	read_bin(bin3, mes_sched, pos - 2);
	right_rotate(bin1, 17);
	right_rotate(bin2, 19);
	right_shift(bin3, 10);
	xor_bin(s1, bin1, bin2, bin3);
}

void addition_bin(int res[], int bin1[], int bin2[]) {
	for (int i = 0; i < 32; i++)
		res[i] = 0;
	for (int i = 31; i >= 0; i--) {
		res[i] += bin1[i] + bin2[i];
		if (res[i] > 1) {
			res[i] -= 2;
			if (i - 1 != -1)
				res[i - 1]++;
		}
	}
}

void create_new_bin(int new_row[32], int mes_sched[64][32], int pos) {
	int sigma0[32], sigma1[32], bin_num1[32], bin_num2[32], plus_sigms[32], plus_bins[32];
	create_sigma0(sigma0, mes_sched, pos);
	create_sigma1(sigma1, mes_sched, pos);
	read_bin(bin_num1, mes_sched, pos - 16);
	read_bin(bin_num2, mes_sched, pos - 7);
	addition_bin(plus_sigms, sigma0, sigma1);
	addition_bin(plus_bins, bin_num1, bin_num2);
	addition_bin(new_row, plus_bins, plus_sigms);
}

void creating_message_schedule(int mes_sched[64][32], int** mes_block, int rows) {
	int new_row[32];
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 32; j++) {
			mes_sched[i][j] = mes_block[rows - 16 + i][j];
		}
	}
	for (int i = 16; i < 64; i++) {
		create_new_bin(new_row, mes_sched, i);
		for (int j = 0; j < 32; j++) {
			mes_sched[i][j] = new_row[j];
		}
	}
}

void add_round_constants(int k_const[64][32]) {
	fstream read("resources/round_constants.txt", ios::in);
	char reader;
	int row = 0, pos = 0, num_of_hex_symb;
	while (read >> reader) {
		if ((int)reader >= 97 && (int)reader <= 102)
			num_of_hex_symb = 15 + (int)reader - 102;
		else
			num_of_hex_symb = (int)reader - 48;
		bitset<4> bin(num_of_hex_symb);
		for (int i = 3; i >= 0; i--, pos++) {
			k_const[row][pos] = bin[i];
		}
		if (pos == 32) {
			pos = 0;
			row++;
		}
	}
	read.close();
}

void hash_func() {
	int count_rows = ((size_of_text() * 8 + 1 + 64) / 512 + 1) * 16;
	int** mes_block = new int* [count_rows];
	int mes_schedule[64][32], round_const[64][32];
	for (int i = 0; i < count_rows; i++)
		mes_block[i] = new int[32];
	message_to_bin(mes_block, count_rows);
	creating_message_schedule(mes_schedule, mes_block, count_rows);
	add_round_constants(round_const);
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			cout << round_const[i][j];
		}
		cout << endl;
	}
	for (int i = 0; i < count_rows; i++)
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
