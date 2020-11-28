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

//Struktura pro informace o hráčích
struct Hrac {
	char jmeno[50];
	int id;
	int x, y;
	int zbylePosuny;
	int uhel;
	int sila;
	int zivoty;
};

#define MIN_UHEL 0
#define MAX_UHEL 180
#define MIN_SILA 1
#define MAX_SILA 50
#define PALIVO 20
#define GRAVITACE 10
#define MAX_ZIVOTY 100
#define SNIZENI_ZIVOTU 50

#define OKRUH_VYBUCHU 2
#define NASOBITEL_VYBUCHU 0.5

#define NICENI_POVRCHU true
#define POSKOZENI_OKOLI_DOPADU true

#define ID_VZDUCH 0
#define ID_ZEM 1
#define ID_TRAJ 2
#define ID_H1 3
#define ID_H2 4

#define ODPOCET 2

//Vytvoreni dynamicke 2D matice
int **createMatrix(int m, int n) {
	int** matrix = new int*[m];
	for (int i = 0; i < m; i++)
	{
		matrix[i] = new int[n];
	}
	return matrix;
}

//Naplneni matice nulami / Vynulování
void initMatrix(int **matrix, int m, int n) {
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			matrix[i][j] = ID_VZDUCH;
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
			if (matrix[i][j] == ID_VZDUCH) {
				//prazdno
				//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				printf(" ");
			}
			else if (matrix[i][j] == ID_ZEM) {
				//zem
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("Z");
			}
			else if (matrix[i][j] == ID_TRAJ) {
				//dopad
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
				printf("X");
			}
			else if (matrix[i][j] == ID_H1) {
				//hrac 1
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("A");
			}
			else if (matrix[i][j] == ID_H2) {
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
void updateHeadline(int m, int n, struct Hrac* aktualniHrac, struct Hrac* hrac1, struct Hrac* hrac2) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	//Mezery řeší problém s výpisem - pokud se z dvouciferného čísla stane jednociferné - tím se posune řádek (zkrátí) - nakonci by se vypsala "0", díky několika mezerám se nakonci vypíš mezera
	setCursorPosition(1, 0);
	if (aktualniHrac->id == hrac1->id) {
		printf("Na tahu - ");
	}
	printf("%s: %d, palivo: %d, uhel: %d, sila: %d                ", hrac1->jmeno, hrac1->zivoty, hrac1->zbylePosuny, hrac1->uhel, hrac1->sila);

	setCursorPosition(1, m + 2);
	if (aktualniHrac->id == hrac2->id) {
		printf("Na tahu - ");
	}
	printf("%s: %d, palivo: %d, uhel: %d, sila: %d                ", hrac2->jmeno, hrac2->zivoty, hrac2->zbylePosuny, hrac2->uhel, hrac2->sila);
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
void printPoint(int **matrix, int y, int x, int id) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	matrix[y][x] = id;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	setCursorPosition(x * 2 + 2, y + 1);
	if (id == ID_VZDUCH) {
		//prazdno
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
		printf(" ");
	}
	else if (id == ID_ZEM) {
		//zem
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		printf("Z");
	}
	else if (id == ID_TRAJ) {
		//dopad
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		printf("X");
	}
	else if (id == ID_H1) {
		//hrac 1
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		printf("A");
	}
	else if (id == ID_H2) {
		//hrac 2
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		printf("B");
	}
	fflush(stdin);

	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Posun bodu dle stisknute klavesy, s ochranou proti vstupu mimo oblast hraciho pole
int movePlayer(int **matrix, int m, int n, char d, struct Hrac* aktualniHrac) {
	int yOld = aktualniHrac->y, xOld = aktualniHrac->x;
	int yNew = aktualniHrac->y, xNew = aktualniHrac->x;

	int offset = 0;
	if (d == 'l') offset = -1;
	if (d == 'r') offset = 1;

	if (offset == 0) return 0;

	xNew += offset;
	if (xNew < 0 || xNew >= n) return 0;

	if (matrix[yNew][xNew] == ID_H1 || matrix[yNew][xNew] == ID_H2) return 0;
	if (matrix[yNew][xNew] == ID_ZEM) {
		//Dopad projektilu
		for (int newY = 0; newY <= yNew; newY++) {
			if (matrix[newY][xNew] == ID_ZEM) {
				aktualniHrac->y = newY - 1;
				aktualniHrac->x = xNew;
				printPoint(matrix, yOld, xOld, ID_VZDUCH);
				printPoint(matrix, aktualniHrac->y, aktualniHrac->x, aktualniHrac->id);
				return 1;
			}
		}
	}
	else {
		for (int newY = yNew; newY < m; newY++) {
			if (matrix[newY][xNew] == ID_ZEM) {
				aktualniHrac->y = newY - 1;
				aktualniHrac->x = xNew;
				printPoint(matrix, yOld, xOld, ID_VZDUCH);
				printPoint(matrix, aktualniHrac->y, aktualniHrac->x, aktualniHrac->id);
				return 1;
			}
		}
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

	return n_seconds;
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

//zobrazi game over screen
void vyhra(struct Hrac* vyherce, int **matrix, int m, int n) {
	//TODO: make look nicer
	printMatrix(matrix, m, n);
	printf("\n %s has won!\n", vyherce->jmeno);
}

// Prevadeni stupnu na radiany
double stupneNaRadiany(int uhelVeStupnich) {
	return (((double)uhelVeStupnich) * M_PI / 180.0);
}

//Vystreli projektil, umi pocitat trajektorii, ubrat zivoty, nicit povrch
void shoot(int ** matrix, int m, int n, struct Hrac* aktualniHrac, struct Hrac*druhyHrac) {
	const double krok = 1.0 / 1000;
	int pocetKroku = 0;
	double x0 = (double)(aktualniHrac->x) + 0.5;
	double y0 = (double)(aktualniHrac->y) + 0.5;
	double vx = cos(stupneNaRadiany(aktualniHrac->uhel)) * aktualniHrac->sila;
	double vy = sin(stupneNaRadiany(aktualniHrac->uhel)) * -aktualniHrac->sila;

	double cas;
	double x;
	double y;
	bool opusteniTanku = false;
	while (1) {
		pocetKroku++;
		cas = krok * pocetKroku;
		x = x0 + vx * cas;
		y = y0 + vy * cas + (pow(cas, 2) * GRAVITACE) / 2;

		if (y > m || x < 0 || x > n) {
			break;
		}

		if (y < 0) {
			continue;
		}

		int poleX = int(x);
		int poleY = int(y);

		int pole = matrix[poleY][poleX];
		if (pole == aktualniHrac->id) {
			if (opusteniTanku) {
				//Zásah
				aktualniHrac->zivoty -= SNIZENI_ZIVOTU;
				break;
			}
		}
		else if (pole == druhyHrac->id) {
			//Zásah
			druhyHrac->zivoty -= SNIZENI_ZIVOTU;
			break;
		}
		else if (pole == ID_ZEM) {
			//Zem
			//Ničení povrchu
			if (NICENI_POVRCHU) {
				printPoint(matrix, poleY, poleX, ID_TRAJ);

				for (int a = max(0, poleY - OKRUH_VYBUCHU); a <= min(poleY + OKRUH_VYBUCHU, m-1); a++) {
					for (int b = max(0, poleX - OKRUH_VYBUCHU); b <= min(poleX + OKRUH_VYBUCHU, n-1); b++) {
						if (pow(a - poleY, 2) + pow(b - poleX, 2) < pow(OKRUH_VYBUCHU, 2) && matrix[a][b] == ID_ZEM) {
							printPoint(matrix, a, b, ID_VZDUCH);
						}
					}
				}
			}
			//Poškoze v okolí výbuchu
			if (POSKOZENI_OKOLI_DOPADU) {
				if (pow(poleY - aktualniHrac->y, 2) + pow(poleX - aktualniHrac->x, 2) < pow(OKRUH_VYBUCHU, 2)) {
					aktualniHrac->zivoty -= SNIZENI_ZIVOTU * NASOBITEL_VYBUCHU;
				}
				if (pow(poleY - druhyHrac->y, 2) + pow(poleX - druhyHrac->x, 2) < pow(OKRUH_VYBUCHU, 2)) {
					druhyHrac->zivoty -= SNIZENI_ZIVOTU * NASOBITEL_VYBUCHU;
				}
			}
			//Dopad
			if (NICENI_POVRCHU) {
				for (int b = max(0, poleX - OKRUH_VYBUCHU); b <= min(poleX + OKRUH_VYBUCHU, n-1); b++) {
					int povrch = m;
					for (int a = m-1; a >= 0; a--) {
						if (povrch == a + 1) {
							//pod nama je zem
							if (matrix[a][b] == ID_ZEM) {
								povrch = a;
							}

						}
						else {
							if (povrch == m) {
								//void
								if (aktualniHrac->id == matrix[a][b]) {
									printPoint(matrix, a, b, ID_VZDUCH);
									vyhra(druhyHrac, matrix, m, n);
								}
								else if (druhyHrac->id == matrix[a][b]) {
									printPoint(matrix, a, b, ID_VZDUCH);
									vyhra(aktualniHrac, matrix, m, n);
								}
								else if (matrix[a][b] == ID_ZEM) {
									printPoint(matrix, a, b, ID_VZDUCH);
								}
							}
							else {
								//move
								
								if (aktualniHrac->id == matrix[a][b]) {
									povrch--;
									aktualniHrac->y = povrch;
									printPoint(matrix, a, b, ID_VZDUCH);
									printPoint(matrix, povrch, b, aktualniHrac->id);
								}
								else if (druhyHrac->id == matrix[a][b]) {
									povrch--;
									druhyHrac->y = povrch;
									printPoint(matrix, a, b, ID_VZDUCH);
									printPoint(matrix, povrch, b, druhyHrac->id);
								}
								else if (matrix[a][b] == ID_ZEM) {
									povrch--;
									printPoint(matrix, a, b, ID_VZDUCH);
									printPoint(matrix, povrch, b, ID_ZEM);
								}
							}
						}
					}
				}
			}
			break;
		}
		else if (pole == ID_VZDUCH) {
			opusteniTanku = true;
			printPoint(matrix, poleY, poleX, 2);
		}
	}

}

//Vycisti trajektorii z obrazovky
void cisteniTraj(int **matrix, int m, int n) {
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (matrix[i][j] == ID_TRAJ) {
				printPoint(matrix, i, j, ID_VZDUCH);
			}
		}
	}
}

//Main logic
int main()
{
	//m = radky, n = sloupce
	//m = y, n = x
	int m, n;

	struct Hrac hrac1;
	hrac1.id = ID_H1;
	hrac1.zivoty = MAX_ZIVOTY;
	hrac1.uhel = 90;
	hrac1.sila = 30;
	hrac1.zbylePosuny = PALIVO;
	printf("Jmeno hrace 1? ");
	scanf_s("%s", hrac1.jmeno, 50);

	struct Hrac hrac2;
	hrac2.id = ID_H2;
	hrac2.zivoty = MAX_ZIVOTY;
	hrac2.uhel = 90;
	hrac2.sila = 30;
	hrac2.zbylePosuny = PALIVO;
	printf("Jmeno hrace 2? ");
	scanf_s("%s", hrac2.jmeno, 50);

	int** matrix;

	struct Hrac* aktualniHrac;
	struct Hrac* druhyHrac;

	FILE *game_plan_in;
	errno_t err = fopen_s(&game_plan_in, ".\\..\\mapa.txt", "r");
	if (err != 0) {
		printf("Neobjeven herni plan!\n");
		return 1;
	}

	fscanf_s(game_plan_in, "%d %d\n", &n, &m);
	fscanf_s(game_plan_in, "%d %d %d %d\n", &hrac1.x, &hrac1.y, &hrac2.x, &hrac2.y);

	matrix = createMatrix(m, n);

	initMatrix(matrix, m, n);
	fillMatrix(matrix, m, n, &game_plan_in);

	fclose(game_plan_in);

	matrix[hrac1.y][hrac1.x] = hrac1.id;
	matrix[hrac2.y][hrac2.x] = hrac2.id;


	//skryti kursoru
	showConsoleCursor(false);

	int counter = ODPOCET;

	unsigned int key;


	while (counter > 1) {
		system("cls");
		printf("Do startu zbyva: %d s", counter);
		counter -= simpleTimer(1);
	}

	aktualniHrac = &hrac1;
	druhyHrac = &hrac2;
	printMatrix(matrix, m, n);

	bool konecTahu;
	while (1)
	{
		konecTahu = false;
		updateHeadline(m, n, aktualniHrac, &hrac1, &hrac2);
		key = getch();
		//Odstraneni prebytecnych znaku odesilanych pri stisku smerovych sipek
		if (key == 0 || key == 0xE0) {
			key = getch();
		}

		cisteniTraj(matrix, m, n);

		//ESC
		if (key == 27) {
			break;
		}
		else if (key == 72) {
			//klavesa UP
			aktualniHrac->uhel += 1;
		}
		else if (key == 80) {
			//klavesa DOWN
			aktualniHrac->uhel -= 1;
		}
		else if (key == 52) {
			//klavesa 4
			aktualniHrac->sila += 1;
		}
		else if (key == 49) {
			//klavesa 1
			aktualniHrac->sila -= 1;
		}
		else if (key == 32) {
			//klavesa SPACE
			shoot(matrix, m, n, aktualniHrac, druhyHrac);
			konecTahu = 1;
		}
		else if (key == 75) {
			//klavesa LEFT
			if (aktualniHrac->zbylePosuny > 0)
				aktualniHrac->zbylePosuny -= movePlayer(matrix, m, n, 'l', aktualniHrac);
		}
		else if (key == 77) {
			//klavesa RIGHT
			if (aktualniHrac->zbylePosuny > 0)
				aktualniHrac->zbylePosuny -= movePlayer(matrix, m, n, 'r', aktualniHrac);
		}

		if (hrac1.zivoty <= 0) {
			//hrac2 won
			vyhra(&hrac2, matrix, m, n);
			break;
		}
		if (hrac2.zivoty <= 0) {
			//hrac1 won
			vyhra(&hrac1, matrix, m, n);
			break;
		}

		if (aktualniHrac->uhel < MIN_UHEL) aktualniHrac->uhel = MIN_UHEL;
		if (aktualniHrac->uhel > MAX_UHEL) aktualniHrac->uhel = MAX_UHEL;
		if (aktualniHrac->sila < MIN_SILA) aktualniHrac->sila = MIN_SILA;
		if (aktualniHrac->sila > MAX_SILA) aktualniHrac->sila = MAX_SILA;

		if (konecTahu == 1) {
			if (aktualniHrac->id == hrac1.id) {
				aktualniHrac = &hrac2;
				druhyHrac = &hrac1;
			}
			else {
				aktualniHrac = &hrac1;
				druhyHrac = &hrac2;
			}
			aktualniHrac->zbylePosuny = PALIVO;
		}


	}
	deleteMatrix(matrix, m);
	return 0;
}