/**
*
* Solution to course project # 6
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2022/2023
*
* @author Nikita Biliy
* @idnumber 9MI8000022
* @compiler VC
*
* main code
*
*/


#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

const int STAND_SIZE = 10000;

void add_text() {
	char text[STAND_SIZE];
	cout << "Write your text: ";
	cin.getline(text, STAND_SIZE - 1);
	fstream write("files/message.txt", ios::out);
	write << text;
	write.close();
}


//searching number of symbols in message
int size_of_text(int func) {
	char reader;
	fstream read;
	if (func == 0)
		read.open("files/message.txt", ios::in);
	else
		read.open("files/user_value.txt", ios::in);
	int res = 0;
	while (read >> noskipws >> reader)
		res++;
	read.close();
	return res;
}


//converting text to binary
void message_to_bin(int **mes_block, int chunks, int func) {
	char reader, column = 0, row = 0;
	fstream read;
	if (func == 0)
		read.open("files/message.txt", ios::in);
	else
		read.open("files/user_value.txt", ios::in);
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
	bitset<64> bin_size(size_of_text(func) * 8);
	for (int i = 0; i < 32; i++)
		mes_block[row][i] = bin_size[63 - i];
	row++;
	for (int i = 0; i < 32; i++)
		mes_block[row][i] = bin_size[31 - i];
	read.close();
}

void right_rotate(int bin[], int num) {
	int clone_bin[32];
	for (int i = 0; i < 32; i++)
		clone_bin[i] = bin[i];
	for (int i = 0; i < num; i++)
		bin[i] = clone_bin[32 - num + i];
	for (int i = num; i < 32; i++)
		bin[i] = clone_bin[i - num];
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

void read_bin_from_arrays(int bin[], int mes_sched[64][32], int pos) {
	for (int i = 0; i < 32; i++)
		bin[i] = mes_sched[pos][i];
}

void create_sigma0(int s0[], int mes_sched[64][32], int pos) {
	int bin1[32], bin2[32], bin3[32];
	read_bin_from_arrays(bin1, mes_sched, pos - 15);
	read_bin_from_arrays(bin2, mes_sched, pos - 15);
	read_bin_from_arrays(bin3, mes_sched, pos - 15);
	right_rotate(bin1, 7);
	right_rotate(bin2, 18);
	right_shift(bin3, 3);
	xor_bin(s0, bin1, bin2, bin3);
}

void create_sigma1(int s1[], int mes_sched[64][32], int pos) {
	int bin1[32], bin2[32], bin3[32];
	read_bin_from_arrays(bin1, mes_sched, pos - 2);
	read_bin_from_arrays(bin2, mes_sched, pos - 2);
	read_bin_from_arrays(bin3, mes_sched, pos - 2);
	right_rotate(bin1, 17);
	right_rotate(bin2, 19);
	right_shift(bin3, 10);
	xor_bin(s1, bin1, bin2, bin3);
}

//binary plussing
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


//creating of new row of message schedule
void create_new_bin(int new_row[32], int mes_sched[64][32], int pos) {
	int sigma0[32], sigma1[32], bin_num1[32],
		bin_num2[32], plus_sigms[32], plus_bins[32];
	create_sigma0(sigma0, mes_sched, pos);
	create_sigma1(sigma1, mes_sched, pos);
	read_bin_from_arrays(bin_num1, mes_sched, pos - 16);
	read_bin_from_arrays(bin_num2, mes_sched, pos - 7);
	addition_bin(plus_sigms, sigma0, sigma1);
	addition_bin(plus_bins, bin_num1, bin_num2);
	addition_bin(new_row, plus_bins, plus_sigms);
}

//adding binarys from message block to message schedule
void create_message_schedule(int mes_sched[64][32], int** mes_block, int rows) {
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

//get number of symb in hex
int from_hex_char_to_int(char hex) {
	int num;
	if ((int)hex >= 97 && (int)hex <= 102)
		num = 15 + (int)hex - 102;
	else
		num = (int)hex - 48;
	return num;
}

void get_round_constants(int k_const[64][32]) {
	fstream read("resources/round_constants.txt", ios::in);
	char reader;
	int row = 0, pos = 0, num_of_hex_symb;
	while (read >> reader) {
		num_of_hex_symb = from_hex_char_to_int(reader);
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

void get_hash_values(int h_val[8][32]) {
	fstream read("resources/hash_values.txt", ios::in);
	char reader;
	int row = 0, pos = 0, num_of_hex_symb;
	while (read >> reader) {
		num_of_hex_symb = from_hex_char_to_int(reader);
		bitset<4> bin(num_of_hex_symb);
		for (int i = 3; i >= 0; i--, pos++) {
			h_val[row][pos] = bin[i];
		}
		if (pos == 32) {
			pos = 0;
			row++;
		}
	}
	read.close();
}

void create_epsilon0(int eps0[], int work_val[8][32]) {
	int bin1[32], bin2[32], bin3[32];
	read_bin_from_arrays(bin1, work_val, 0);
	read_bin_from_arrays(bin2, work_val, 0);
	read_bin_from_arrays(bin3, work_val, 0);
	right_rotate(bin1, 2);
	right_rotate(bin2, 13);
	right_rotate(bin3, 22);
	xor_bin(eps0, bin1, bin2, bin3);
}

void create_epsilon1(int eps1[], int work_val[8][32]) {
	int bin1[32], bin2[32], bin3[32];
	read_bin_from_arrays(bin1, work_val, 4);
	read_bin_from_arrays(bin2, work_val, 4);
	read_bin_from_arrays(bin3, work_val, 4);
	right_rotate(bin1, 6);
	right_rotate(bin2, 11);
	right_rotate(bin3, 25);
	xor_bin(eps1, bin1, bin2, bin3);
}

void and_bin(int res[], int bin1[], int bin2[]) {
	for (int i = 0; i < 32; i++) {
		if (bin1[i] + bin2[i] > 1)
			res[i] = 1;
		else
			res[i] = 0;
	}
}

void not_bin(int bin[]) {
	for (int i = 0; i < 32; i++) {
		if (bin[i] == 1)
			bin[i] = 0;
		else
			bin[i] = 1;
	}
}

void create_choice(int choice[], int work_val[8][32]) {
	int part1[32], part2[32], e[32], f[32], not_e[32], g[32];
	read_bin_from_arrays(e, work_val, 4);
	read_bin_from_arrays(not_e, work_val, 4);
	read_bin_from_arrays(f, work_val, 5);
	read_bin_from_arrays(g, work_val, 6);
	not_bin(not_e);
	and_bin(part1, e, f);
	and_bin(part2, not_e, g);
	for (int i = 0; i < 32; i++) {
		choice[i] = part1[i] + part2[i];
		if (choice[i] > 1)
			choice[i] -= 2;
	}
}

void create_majority(int majority[], int work_val[8][32]) {
	int part1[32], part2[32], part3[32], a[32], b[32], c[32];
	read_bin_from_arrays(a, work_val, 0);
	read_bin_from_arrays(b, work_val, 1);
	read_bin_from_arrays(c, work_val, 2);
	and_bin(part1, a, b);
	and_bin(part2, a, c);
	and_bin(part3, c, b);
	xor_bin(majority, part1, part2, part3);
}

void create_temp2(int temp2[], int work_val[8][32]) {
	int eps0[32], majority[32];
	create_epsilon0(eps0, work_val);
	create_majority(majority, work_val);
	addition_bin(temp2, eps0, majority);
}

void create_temp1(int temp1[], int work_val[8][32], int mes_schedule[64][32], int k_const[64][32], int pos) {
	int h[32], bin_mes_sched[32], bin_k_const[32], eps1[32],
		choice[32], part1[32], part2[32], part3[32];
	read_bin_from_arrays(h, work_val, 7);
	create_epsilon1(eps1, work_val);
	create_choice(choice, work_val);
	read_bin_from_arrays(bin_mes_sched, mes_schedule, pos);
	read_bin_from_arrays(bin_k_const, k_const, pos);
	addition_bin(part1, h, eps1);
	addition_bin(part2, choice, bin_k_const);
	addition_bin(part3, part1, part2);
	addition_bin(temp1, part3, bin_mes_sched);
}

void rewrite_work_val(int work_val[8][32], int pos, int bin[]) {
	for (int i = 0; i < 32; i++)
		work_val[pos][i] = bin[i];
}

//plussing of hash val to work val
void addition_hash_work_val(int work_val[8][32]) {
	int hash_val[8][32], bin_work[32], bin_hash[32], res[32];
	get_hash_values(hash_val);
	for (int i = 0; i < 8; i++) {
		read_bin_from_arrays(bin_hash, hash_val, i);
		read_bin_from_arrays(bin_work, work_val, i);
		addition_bin(res, bin_hash, bin_work);
		for (int j = 0; j < 32; j++)
			work_val[i][j] = res[j];
	}
}

//editing of work value
void transform_work_val(int work_val[8][32], int mes_schedule[64][32], int round_const[64][32]) {
	int old_a[32], old_e[32], temp1[32], temp2[32],
		b[32], c[32], f[32], g[32], new_a[32], new_e[32], d[32];
	for (int i = 0; i < 64; i++) {
		read_bin_from_arrays(old_a, work_val, 0);
		read_bin_from_arrays(old_e, work_val, 4);
		read_bin_from_arrays(d, work_val, 3);
		create_temp1(temp1, work_val, mes_schedule, round_const, i);
		create_temp2(temp2, work_val);
		addition_bin(new_a, temp1, temp2);
		addition_bin(new_e, d, temp1);
		rewrite_work_val(work_val, 0, new_a);
		rewrite_work_val(work_val, 4, new_e);
		read_bin_from_arrays(b, work_val, 1);
		rewrite_work_val(work_val, 1, old_a);
		read_bin_from_arrays(c, work_val, 2);
		rewrite_work_val(work_val, 2, b);
		rewrite_work_val(work_val, 3, c);
		read_bin_from_arrays(f, work_val, 5);
		rewrite_work_val(work_val, 5, old_e);
		read_bin_from_arrays(g, work_val, 6);
		rewrite_work_val(work_val, 6, f);
		rewrite_work_val(work_val, 7, g);
	}
	addition_hash_work_val(work_val);
}

char bin_to_char(int bin[4]) {
	int num = 0, bit_num;
	for (int i = 3; i >= 0; i--) {
		bit_num = bin[i];
		for (int j = 0; j < 3 - i; j++)
			bit_num *= 2;
		num += bit_num;
	}
	if (num < 10)
		return (char)(48 + num);
	else
		return (char)(97 - 10 + num);
}

void create_and_save_hash(int work_val[8][32], int func) {
	int bin[4];
	fstream save;
	if (func == 0)
		save.open("files/hash.txt", ios::out);
	else
		save.open("files/hash_comp.txt", ios::out);
	for (int i = 0, pos_w_val = 0; i < 8; i++, pos_w_val = 0) {
		for (int j = 0; j < 8; j++) {
			for (int l = 0; l < 4; l++)	{
				bin[l] = work_val[i][pos_w_val++];
			}
			save << bin_to_char(bin);
		}
	}
	save.close();
}

void read_hash() {
	char reader;
	fstream read("files/hash.txt", ios::in);
	while (read >> reader)
		cout << reader;
	read.close();
	cout << endl;
}

//main function for creating of hash
void hash_func(int func = 0) {
	int count_rows = ((size_of_text(func) * 8 + 1 + 64) / 512 + 1) * 16;
	int** mes_block = new int* [count_rows];
	int mes_schedule[64][32], round_const[64][32], work_val[8][32];
	for (int i = 0; i < count_rows; i++)
		mes_block[i] = new int[32];
	message_to_bin(mes_block, count_rows, func);
	create_message_schedule(mes_schedule, mes_block, count_rows);
	for (int i = 0; i < count_rows; i++)
		delete mes_block[i];
	delete[] mes_block;
	get_round_constants(round_const);
	get_hash_values(work_val);
	transform_work_val(work_val, mes_schedule, round_const);
	create_and_save_hash(work_val, func);
}

void get_hash(char hash[]) {
	fstream read("files/hash_comp.txt", ios::in);
	char reader;
	int pos = 0;
	while (read >> reader) {
		hash[pos++] = reader;
	}
	read.close();
}


//checking hash is valid or not
void valid_hash(int &crash) {
	char user_hash[66], user_val[STAND_SIZE], hash[64];
	cout << "Enter your hash: ";
	cin.getline(user_hash, 66);
	if (user_hash[64] != '\0') {
		crash++;
		cout << "Your hash is wrong\n\n";
		return;
	}
	cout << "Enter your message: ";
	cin.getline(user_val, STAND_SIZE);
	fstream write("files/user_value.txt", ios::out);
	write << user_val;
	write.close();
	hash_func(1);
	get_hash(hash);
	for (int i = 0; i < 64; i++) {
		if (hash[i] != user_hash[i]) {
			cout << "Your hash is wrong\n";
			return;
		}
	}
	cout << "Your hash is correct\n";
}

int main() {
	char answer[3];
	int crash = 0;
	cout << "Hi\n";
	while (true) {
		cout << "\nWhat do you want do?\n";
		cout << "1. Add text to file\n";
		cout << "2. Hash your file\n";
		cout << "3. Read your last hash again\n";
		cout << "4. Validating the hash\n";
		cout << "5. Exit\n";
		cout << "Write your answer: ";
		cin.getline(answer, 3);
		if (answer[1] != '\0' || (int)answer[0] < 49 || (int)answer[0] > 53) {
			cout << "Incorrect input\n";
			break;
		}
		cout << endl;
		if (answer[0] == '1') {
			add_text();
		}
		else if (answer[0] == '2') {
			hash_func();
			read_hash();
		}
		else if (answer[0] == '3') {
			read_hash();
		}
		else if (answer[0] == '4') {
			valid_hash(crash);
			if (crash != 0)
				break;
		}
		else if (answer[0] == '5')
			break;
	}
}
