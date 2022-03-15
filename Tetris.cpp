#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>
#include <random>

using namespace std;

#define UP		72
#define DOWN	80
#define LEFT	75
#define RIGHT	77

unsigned short game_board[21];
unsigned short show_board[21];

// 테트리미노스의 7가지 모양과 4가지 방향
unsigned short tetriminos[7][4] = {

	{0x6600, 0x6600, 0x6600, 0x6600},       // O
	{0xF000, 0x4444, 0xF000, 0x4444},        // I
	{0x6C00, 0x8C40, 0x6C00, 0x8C40},    // S
	{0xC600, 0x4C80, 0xC600, 0x4C80},    // Z
	{0xE400, 0x8C80, 0x4E00, 0x4C40},    // T
	{0x2E00, 0xC440, 0xE800, 0x88C0},    // L
	{0x8E00, 0x44C0, 0xE200, 0xC880}    // J
};

unsigned char shape;                  // 테트리미노스의 7가지 모양
unsigned char pattern;                 // 테트리미노스의 4가지 패턴
unsigned char cur_line;               // 테트리니노스의 현재 라인
unsigned char cur_col;                // 테트리니노스의 현재 칸 
unsigned short temp_line[4];          // 테트리미노스 라인 임시 저장소

void gotoXY(int x, int y)
{
	HANDLE hOut;
	COORD Cur;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(hOut, Cur);
}


void clrscr()
{
	system("cls");
}

void setBoard()
{
	for (int i = 0; i < 20; i++) {
		game_board[i] = 0xE007;
	}

	game_board[20] = 0xFFFF;

	memcpy(show_board, game_board, sizeof(game_board));
}

string to_binary(int num) {
	string s = "";
	while (num > 0) {
		if (num % 2 == 1) s = "■" + s;
		else s = "□" + s;
		num >>= 1;
	}
	return s;
}

void newTetrimnos()
{
	srand(time(NULL));

	shape = rand() % 7;
	pattern = 0;

	temp_line[0] = (tetriminos[shape][pattern] & 0xF000) >> 6;
	temp_line[1] = (tetriminos[shape][pattern] & 0x0F00) >> 2;
	temp_line[2] = (tetriminos[shape][pattern] & 0x00F0) << 2;
	temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << 6;

	cur_line = 0;
	cur_col = 9;
}

void displayBoard()
{
	for (int i = 0; i < 21; i++)
	{
		cout << to_binary(show_board[i]) << endl;
	}
}

signed char collision(void)
{
	if (((game_board[cur_line] & temp_line[0]) != 0) || ((game_board[cur_line + 1] & temp_line[1]) != 0) ||
		((game_board[cur_line + 2] & temp_line[2]) != 0) || ((game_board[cur_line + 3] & temp_line[3]) != 0))
		return -1;         // 충돌
	else
		return 0;  // 충돌 없음
}

void moveAndRotate()
{
	if (GetAsyncKeyState(VK_SPACE))
	{
		while (!collision())
		{
			cur_line++;
		}

		memcpy(game_board, show_board, sizeof(show_board));

		newTetrimnos();
	}

	if (GetAsyncKeyState(VK_UP))
	{
		pattern++;
		if (pattern == 4) pattern = 0;

		temp_line[0] = ((tetriminos[shape][pattern] & 0xF000) >> 12) << (cur_col - 3);
		temp_line[1] = ((tetriminos[shape][pattern] & 0x0F00) >> 8) << (cur_col - 3);
		temp_line[2] = ((tetriminos[shape][pattern] & 0x00F0) >> 4) << (cur_col - 3);
		temp_line[3] = ((tetriminos[shape][pattern] & 0x000F) << (cur_col - 3));
	}


	if (GetAsyncKeyState(VK_RIGHT))
	{
		cur_col--;
		for (int i = 0; i < 4; i++)
		{
			temp_line[i] >>= 1;
		}
		if (collision())
		{
			for (int i = 0; i < 4; i++)
			{
				temp_line[i] <<= 1;
			}
			cur_col++;
		}

		show_board[cur_line] = game_board[cur_line] | temp_line[0];
		show_board[cur_line + 1] = game_board[cur_line + 1] | temp_line[1];
		show_board[cur_line + 2] = game_board[cur_line + 2] | temp_line[2];
		show_board[cur_line + 3] = game_board[cur_line + 3] | temp_line[3];
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		cur_col++;
		for (int i = 0; i < 4; i++)
		{
			temp_line[i] <<= 1;
		}
		if (collision())
		{
			for (int i = 0; i < 4; i++)
			{
				temp_line[i] >>= 1;
			}
			cur_col--;
		}

		show_board[cur_line] = game_board[cur_line] | temp_line[0];
		show_board[cur_line + 1] = game_board[cur_line + 1] | temp_line[1];
		show_board[cur_line + 2] = game_board[cur_line + 2] | temp_line[2];
		show_board[cur_line + 3] = game_board[cur_line + 3] | temp_line[3];
	}

	cur_line++;


	if (collision())
	{
		memcpy(game_board, show_board, sizeof(show_board));

		newTetrimnos();
	}

	memcpy(show_board, game_board, sizeof(game_board));

	show_board[cur_line] = game_board[cur_line] | temp_line[0];
	show_board[cur_line + 1] = game_board[cur_line + 1] | temp_line[1];
	show_board[cur_line + 2] = game_board[cur_line + 2] | temp_line[2];
	show_board[cur_line + 3] = game_board[cur_line + 3] | temp_line[3];
}

void CheckEnd()
{
	int cnt = 0;
	for (int i = 0; i < 20; i++)
	{
		if (game_board[i] == 0xFFFF)
		{
			game_board[i] = 0xE007;
			for (int i = 19 - cnt; i >= 0; i--)
			{
				game_board[i] = game_board[i - 1];
			}
		}
	}
}

int main()
{
	setBoard();
	newTetrimnos();

	while (true)
	{
		clrscr();
		CheckEnd();
		moveAndRotate();
		displayBoard();
		Sleep(500);
	}
}
