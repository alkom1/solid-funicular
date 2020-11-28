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

//Tisk bodu do koonzoloveho okna
void updateHeadline(int player, int movesRemaining, int angle, int power) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	setCursorPosition(1, 0);
	player -= 2;
	printf("Hrac: %d, pohyby: %d, uhel: %d, sila: %d            ", player, movesRemaining, angle, power);
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
void printPoint(int **matrix, int *mActual, int *nActual, int mOld, int nOld, int player) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	matrix[mOld][nOld] = 0;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	setCursorPosition(nOld * 2 + 2, mOld + 1);
	printf(" ");
	matrix[*mActual][*nActual] = player;
	setCursorPosition(*nActual * 2 + 2, *mActual + 1);
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

//Tisk bodu do koonzoloveho okna
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
int moveNumber(int **matrix, int m, int n, char d, int *mActual, int *nActual, int player) {
	int mOld = *mActual, nOld = *nActual;
	int mNew = *mActual, nNew = *nActual;

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
				(*mActual) = newY - 1;
				(*nActual) = nNew;
				printPoint(matrix, mActual, nActual, mOld, nOld, player);
				return 1;
			}
		}
	}
	else {
		for (int newY = mNew; newY < m; newY++) {
			if (matrix[newY][nNew] == 1) {
				(*mActual) = newY - 1;
				(*nActual) = nNew;
				printPoint(matrix, mActual, nActual, mOld, nOld, player);
				return 1;
			}
		}
		(*nActual) = nNew;
		(*mActual) = 0;
		printPoint(matrix, mActual, nActual, mOld, nOld, player);
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
void shoot(int ** matrix, int m, int n, int *cpn, int *cpm, int otherPlayer, int *opn, int *opm, int power, int angle) {
	const double step = 1.0 / 1000;
	int stepCounter = 0;
	power = 30;
	double x0 = (double)(*cpn) + 0.5;
	double y0 = (double)(*cpm) + 0.5;
	double vx = cos(degToRad(angle)) * power;
	double vy = sin(degToRad(angle)) * -power;

	char buf[1024] = {};
	sprintf_s(buf, "%4.2f %4.2f %4.2f %4.2f %d %d\n", x0, y0, vx, vy, angle, power);
	OutputDebugString(convertCharArrayToLPCWSTR(buf));

	double time;
	double x;
	double y;
	while (1) {
		stepCounter++;
		time = step * stepCounter;
		x = x0 + vx * time;
		y = y0 + vy * time + (pow(time, 2) * gravity) / 2;

		int tileX = int(x);
		int tileY = int(y);

		char buf[1024] = {};
		sprintf_s(buf, "%d, %4.2f, %4.2f, %d, %d\n", stepCounter, x, y, tileX, tileY);
		OutputDebugString(convertCharArrayToLPCWSTR(buf));

		if (y > m || x < 0 || x > n) {
			break;
		}

		if (y < 0) {
			continue;
		}

		int tile = matrix[tileY][tileX];
		printPoint2(tileY, tileX, "X");

		//char buf[1024] = {};
		//sprintf_s(buf, "%d, %4.2f, %4.2f, %d, %d %d\n", stepCounter, x, y, tileX, tileY, tile);
		//OutputDebugString(convertCharArrayToLPCWSTR(buf));

		if (tile == otherPlayer) {
			//hit
			break;
		}
		else if (tile == 1) {
			//zem
			//matrix[tileY][tileX] = 2;
			printPoint2(tileY, tileX, "X");
			break;
		}		
	}
}

int main()
{
	//m = radky, n = sloupce
	int m, n;
	int mP1 = 0, nP1 = 0, mP2 = 0, nP2 = 0;
	int p1Health = 100, p2Health = 100;
	int** matrix;
	int currentPlayer, moveRemaining, angle, power;

	FILE *game_plan_in;
	errno_t err = fopen_s(&game_plan_in, ".\\..\\mapa.txt", "r");
	if (err != 0) {
		printf("Neobjeven herni plan!\n");
		return 1;
	}

	fscanf_s(game_plan_in, "%d %d\n", &n, &m);
	fscanf_s(game_plan_in, "%d %d %d %d\n", &nP1, &mP1, &nP2, &mP2);

	matrix = createMatrix(m, n);

	initMatrix(matrix, m, n);
	fillMatrix(matrix, m, n, &game_plan_in);

	matrix[mP1][nP1] = 3;
	matrix[mP2][nP2] = 4;


	//skryti kursoru
	showConsoleCursor(false);

	int counter = 0;

	unsigned int key;
	char direction;


	while (counter > 1) {
		system("cls");
		printf("Do startu zbyva: %d s", counter);
		counter -= simpleTimer(1);
	}

	currentPlayer = 3;
	int otherPlayer = 4;
	moveRemaining = 99;
	power = 30;
	angle = 90;
	int *cpn = &nP1;
	int *cpm = &mP1;
	int *opn = &nP2;
	int *opm = &mP2;
	printMatrix(matrix, m, n);

	bool turnDone;
	while (1)
	{
		turnDone = false;
		updateHeadline(currentPlayer, moveRemaining, angle, power);
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
			angle += 1;
		}
		else if (key == 80) {
			//klavesa DOWN
			angle -= 1;
		}
		else if (key == 52) {
			//klavesa 4
			power += 1;
		}
		else if (key == 49) {
			//klavesa 1
			power -= 1;
		}
		else if (key == 32) {
			//klavesa SPACE
			OutputDebugString(L"Shoot");
			shoot(matrix, m, n, cpn, cpm, otherPlayer, opn, opm, power, angle);
			turnDone = 1;
		}
		else if (key == 75) {
			//klavesa LEFT
			if (moveRemaining > 0)
				moveRemaining -= moveNumber(matrix, m, n, 'l', cpm, cpn, currentPlayer);
		}
		else if (key == 77) {
			//klavesa RIGHT
			if (moveRemaining > 0)

				moveRemaining -= moveNumber(matrix, m, n, 'r', cpm, cpn, currentPlayer);
		}

		if (angle < 0) angle = 0;
		if (angle > 180) angle = 180;
		if (power < 1) power = 1;
		if (power > 50) power = 50;

		if (turnDone == 1) {
			if (currentPlayer == 3) {
				currentPlayer = 4;
				otherPlayer = 3;
				cpn = &nP2;
				cpm = &mP2;
				opn = &nP1;
				opm = &mP1;
			}
			else {
				currentPlayer = 3;
				otherPlayer = 4;
				cpn = &nP1;
				cpm = &mP1;
				opn = &nP2;
				opm = &mP2;
			}
			moveRemaining = 99;
		}


	}
	deleteMatrix(matrix, m);
	return 0;
}

