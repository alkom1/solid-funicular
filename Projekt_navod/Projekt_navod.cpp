// Projekt_navod.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <math.h>

#define M_PI 3.14159265359

#define getch() _getch()

struct Player {
	char name[50];
	int id;
	int x, y;
	int movesRemaining;
	int angle;
	int power;
	int lifes;
};

//Vytvoreni dynamicke 2D matice
int **createMatrix(int m, int n) {
	int** matrix = new int*[m];
	for (int i = 0; i < m; i++)
	{
		matrix[i] = new int[n];
	}
	return matrix;
}

//Naplneni matice nulami
void initMatrix(int **matrix, int m, int n) {
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			matrix[i][j] = 0;
		}
	}
}

//Naplneni matice daty ze soubour
void fillMatrix(int **matrix, int m, int n, FILE** plan) {
	int p;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			fscanf_s(*plan, "%d ", &p);
			matrix[i][j] = p;
		}
		fscanf_s(*plan, "\n");
	}
}

//Nastaveni kurzoru v konzoli na pozici x = sloupce, y = radky
void setCursorPosition(int x, int y) {
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	fflush(stdin);
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

//Tisk matice + nastaveni barev v konzoli
void printMatrix(int **matrix, int m, int n) {
	system("cls");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	int stringSize = ((n * 2) + 2) * m;
	char *matrixString = new char[stringSize];

	printf("\n");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	for (int i = 0; i < m; i++)
	{
		printf(" |");
		for (int j = 0; j < n; j++)
		{
			if (matrix[i][j] == 0) {
				//prazdno
				//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				printf(" ");
			}
			else if (matrix[i][j] == 1) {
				//zem
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("Z");
			}
			else if (matrix[i][j] == 2) {
				//dopad
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("X");
			}
			else if (matrix[i][j] == 3) {
				//hrac 1
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("A");
			}
			else if (matrix[i][j] == 4) {
				//hrac 2
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("B");
			}

			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
			printf("|");

		}
		printf("\n");
	}
	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Tisk zahlavi a zapati
void updateHeadline(int m, int n, struct Player* currentPlayer, struct Player* player1, struct Player* player2) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	setCursorPosition(1, 0);
	if (currentPlayer->id == player1->id) {
		printf("Na tahu - ");
	}
	printf("%s: %d, palivo: %d, uhel: %d, sila: %d            ", player1->name, player1->lifes, player1->movesRemaining, player1->angle, player1->power);

	setCursorPosition(1, m + 1);
	if (currentPlayer->id == player2->id) {
		printf("Na tahu - ");
	}
	printf("%s: %d, palivo: %d, uhel: %d, sila: %d            ", player2->name, player2->lifes, player2->movesRemaining, player2->angle, player2->power);
	fflush(stdin);

	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Vyprazdneni pameti od alokovane matice
void deleteMatrix(int **matrix, int m) {
	for (int i = 0; i < m; i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
}

//Tisk bodu do koonzoloveho okna
void printPoint(int **matrix, int mActual, int nActual, int mOld, int nOld, int player) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	matrix[mOld][nOld] = 0;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	setCursorPosition(nOld * 2 + 2, mOld + 1);
	printf(" ");
	matrix[mActual][nActual] = player;
	setCursorPosition(nActual * 2 + 2, mActual + 1);
	if (player == 3) {
		//hrac 1
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		printf("A");
	}
	if (player == 4) {
		//hrac 2
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		printf("B");
	}
	fflush(stdin);

	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Tisk bodu do koonzoloveho okna bez prepisovani matrixu
void printPoint2(int m, int n, char* point) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	setCursorPosition(n * 2 + 2, m + 1);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	printf(point);
	fflush(stdin);

	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Posun bodu dle stisknute klavesy, s ochranou proti vstupu mimo oblast hraciho pole
int moveNumber(int **matrix, int m, int n, char d, struct Player* currentPlayer) {
	int mOld = currentPlayer->y, nOld = currentPlayer->x;
	int mNew = currentPlayer->y, nNew = currentPlayer->x;

	int offset = 0;
	if (d == 'l') offset = -1;
	if (d == 'r') offset = 1;

	nNew += offset;
	if (nNew < 0 || nNew >= n) return 0;

	if (matrix[mNew][nNew] == 3 || matrix[mNew][nNew] == 4) return 0;
	if (matrix[mNew][nNew] == 1) {
		//fall
		for (int newY = 0; newY <= mNew; newY++) {
			if (matrix[newY][nNew] == 1) {
				currentPlayer->y = newY - 1;
				currentPlayer->x = nNew;
				printPoint(matrix, currentPlayer->y, currentPlayer->x, mOld, nOld, currentPlayer->id);
				return 1;
			}
		}
	}
	else {
		for (int newY = mNew; newY < m; newY++) {
			if (matrix[newY][nNew] == 1) {
				currentPlayer->y = newY - 1;
				currentPlayer->x = nNew;
				printPoint(matrix, currentPlayer->y, currentPlayer->x, mOld, nOld, currentPlayer->id);
				return 1;
			}
		}
		currentPlayer->x = nNew;
		currentPlayer->y = 0;
		printPoint(matrix, currentPlayer->y, currentPlayer->x, mOld, nOld, currentPlayer->id);
		return 1;
	}
	return 0;
}

//Jednoduchy casovac zajistujici pocatecni zpozdeni
int simpleTimer(int n_seconds) {
	time_t start, now;

	start = time(NULL);
	now = start;
	//rozdil aktualniho a predchoziho casu CPU
	while (difftime(now, start) < (double)n_seconds) {
		now = time(NULL);
	}

	return 1;
}

//funkce pro skryti kursoru v konzoli
void showConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

// Prevadeni stupnu na radiany
inline double degToRad(int angleInDegrees) {
	return (((double)angleInDegrees) * M_PI / 180.0);
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

#define gravity 10
void shoot(int ** matrix, int m, int n, struct Player *currentPlayer, struct Player *otherPlayer) {
	const double step = 1.0 / 1000;
	int stepCounter = 0;
	double x0 = (double)(currentPlayer->x) + 0.5;
	double y0 = (double)(currentPlayer->y) + 0.5;
	double vx = cos(degToRad(currentPlayer->angle)) * currentPlayer->power;
	double vy = sin(degToRad(currentPlayer->angle)) * -currentPlayer->power;

	char buf[1024] = {};
	sprintf_s(buf, "%4.2f %4.2f %4.2f %4.2f %d %d\n", x0, y0, vx, vy, currentPlayer->angle, currentPlayer->power);
	OutputDebugString(convertCharArrayToLPCWSTR(buf));

	double time;
	double x;
	double y;
	int lastSeenX;
	int lastSeenY;
	while (1) {
		stepCounter++;
		time = step * stepCounter;
		x = x0 + vx * time;
		y = y0 + vy * time + (pow(time, 2) * gravity) / 2;

		
		/*
		char buf[1024] = {};
		sprintf_s(buf, "%d, %4.2f, %4.2f, %d, %d\n", stepCounter, x, y, tileX, tileY);
		OutputDebugString(convertCharArrayToLPCWSTR(buf));
		*/

		if (y > m || x < 0 || x > n) {
			break;
		}

		if (y < 0) {
			continue;
		}

		int tileX = int(x);
		int tileY = int(y);

		lastSeenX = tileX;
		lastSeenY = tileY;

		int tile = matrix[tileY][tileX];

		if (tile == otherPlayer->id) {
			//hit
			otherPlayer->lifes -= 50;
			return;
		}
		else if (tile == 1) {
			//zem
			//matrix[tileY][tileX] = 2;
			break;
		}		
	}
	printPoint2(lastSeenY, lastSeenX, "X");
}

int main()
{
	//m = radky, n = sloupce
	int m, n;
	
	struct Player player1;
	player1.id = 3;
	player1.lifes = 100;
	player1.angle = 90;
	player1.power = 30;
	player1.movesRemaining = 4;
	strcpy_s(player1.name, 50, "Pepa");
	struct Player player2;
	player2.id = 4;
	player2.lifes = 100;
	player2.angle = 90;
	player2.power = 30;
	player2.movesRemaining = 4;
	strcpy_s(player2.name, 50, "Ales");

	int** matrix;
	
	struct Player* currentPlayer;
	struct Player* otherPlayer;

	FILE *game_plan_in;
	errno_t err = fopen_s(&game_plan_in, ".\\..\\mapa.txt", "r");
	if (err != 0) {
		printf("Neobjeven herni plan!\n");
		return 1;
	}

	fscanf_s(game_plan_in, "%d %d\n", &n, &m);
	fscanf_s(game_plan_in, "%d %d %d %d\n", &player1.x, &player1.y, &player2.x, &player2.y);

	matrix = createMatrix(m, n);

	initMatrix(matrix, m, n);
	fillMatrix(matrix, m, n, &game_plan_in);

	matrix[player1.y][player1.x] = player1.id;
	matrix[player2.y][player2.x] = player2.id;


	//skryti kursoru
	showConsoleCursor(false);

	int counter = 0;

	unsigned int key;


	while (counter > 1) {
		system("cls");
		printf("Do startu zbyva: %d s", counter);
		counter -= simpleTimer(1);
	}

	currentPlayer = &player1;
	otherPlayer = &player2;
	printMatrix(matrix, m, n);

	bool turnDone;
	while (1)
	{
		turnDone = false;
		updateHeadline(m, n ,currentPlayer, &player1, &player2);
		key = getch();
		//Odstraneni prebytecnych znaku odesilanych pri stisku smerovych sipek
		if (key == 0 || key == 0xE0) {
			key = getch();
		}

		//ESC
		if (key == 27) {
			break;
		}
		else if (key == 72) {
			//klavesa UP
			currentPlayer->angle += 1;
		}
		else if (key == 80) {
			//klavesa DOWN
			currentPlayer->angle -= 1;
		}
		else if (key == 52) {
			//klavesa 4
			currentPlayer->power += 1;
		}
		else if (key == 49) {
			//klavesa 1
			currentPlayer->power -= 1;
		}
		else if (key == 32) {
			//klavesa SPACE
			shoot(matrix, m, n, currentPlayer, otherPlayer);
			turnDone = 1;
		}
		else if (key == 75) {
			//klavesa LEFT
			if (currentPlayer->movesRemaining > 0)
				currentPlayer->movesRemaining -= moveNumber(matrix, m, n, 'l', currentPlayer);
		}
		else if (key == 77) {
			//klavesa RIGHT
			if (currentPlayer->movesRemaining > 0)
				currentPlayer->movesRemaining -= moveNumber(matrix, m, n, 'r', currentPlayer);
		}

		if (player1.lifes <= 0) {
			//player2 won
			break;
		}
		if (player2.lifes <= 0) {
			//player1 won
			break;
		}

		if (currentPlayer->angle < 0) currentPlayer->angle = 0;
		if (currentPlayer->angle > 180) currentPlayer->angle = 180;
		if (currentPlayer->power < 1) currentPlayer->power = 1;
		if (currentPlayer->power > 50) currentPlayer->power = 50;

		if (turnDone == 1) {
			if (currentPlayer->id == player1.id) {
				currentPlayer = &player2;
				otherPlayer = &player1;
			}
			else {
				currentPlayer = &player1;
				otherPlayer = &player2;
			}
			currentPlayer->movesRemaining = 4;
		}


	}
	deleteMatrix(matrix, m);
	return 0;
}

