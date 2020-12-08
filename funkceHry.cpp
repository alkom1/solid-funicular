#include "funkceHry.h"

//Vytvoreni dynamicke 2D matice
int** vytvorMatici(int m, int n) {
	int** matrix = new int* [m];
	for (int i = 0; i < m; i++)
	{
		matrix[i] = new int[n];
	}
	return matrix;
}

//Naplneni matice nulami / Vynulov�n�
void inicializujMatici(int** matrix, int m, int n) {
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			matrix[i][j] = ID_VZDUCH;
		}
	}
}

//Naplneni matice daty ze soubour
void nahrajMapuDoMatice(int** matrix, int m, int n, FILE** plan) {
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
void nastavPoziciKurzoru(int x, int y) {
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	fflush(stdin);
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

//Tisk matice + nastaveni barev v konzoli
void vykresliMatici(int** matrix, int m, int n) {
	system("cls");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	printf("\n");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	for (int i = 0; i < m; i++)
	{
		printf(" |");
		for (int j = 0; j < n; j++)
		{
			
			if (matrix[i][j] == ID_ZEM) {
				//zem
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("Z");
			}
			else if (matrix[i][j] == ID_H1) {
				//hrac 1
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
				printf("A");
			}
			else if (matrix[i][j] == ID_H2) {
				//hrac 2
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
				printf("B");
			}
			else {
				//prazdno
				//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				printf(" ");
			}
			//zbyle ID (TRAJ, TRAJ_DOPAD) by se nikdy nemeli vykreslovat touto funkci

			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
			printf("|");

		}
		printf("\n");
	}
	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Tisk zahlavi a zapati
void vypisHerniInfo(int m, int n, struct Hrac* aktualniHrac, struct Hrac* hrac1, struct Hrac* hrac2) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	//Mezery �e�� probl�m s v�pisem - pokud se z dvoucifern�ho ��sla stane jednocifern� - t�m se posune ��dek (zkr�t�) - nakonci by se vypsala "0", d�ky n�kolika mezer�m se nakonci vyp� mezera
	nastavPoziciKurzoru(1, 0);
	if (aktualniHrac->id == hrac1->id) {
		printf("Na tahu - ");
	}
	printf("%s: %d, palivo: %d, uhel: %d, sila: %d                ", hrac1->jmeno, hrac1->zivoty, hrac1->zbylePosuny, hrac1->uhel, hrac1->sila);

	nastavPoziciKurzoru(1, m + 2);
	if (aktualniHrac->id == hrac2->id) {
		printf("Na tahu - ");
	}
	printf("%s: %d, palivo: %d, uhel: %d, sila: %d                ", hrac2->jmeno, hrac2->zivoty, hrac2->zbylePosuny, 180-hrac2->uhel, hrac2->sila);
	fflush(stdin);

	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Vyprazdneni pameti od alokovane matice
void dealokujMatici(int** matrix, int m) {
	for (int i = 0; i < m; i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
}

//Tisk bodu do koonzoloveho okna
void vykresliBod(int** matrix, int y, int x, int id) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	matrix[y][x] = id;
	//"magicke" konstanty pro vypoce spravne pozice bodu
	nastavPoziciKurzoru(x * 2 + 2, y + 1);
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
		printf("o");
	}
	else if (id == ID_TRAJ_DOPAD) {
		//dopad
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		printf("X");
	}
	else if (id == ID_H1) {
		//hrac 1
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		//indexAKtualnihoHrace
		printf("A"); // 065 - b
	}
	else if (id == ID_H2) {
		//hrac 2
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		printf("B");
	}
	fflush(stdin);

	SetConsoleTextAttribute(hConsole, saved_attributes);
}

//Posun bodu dle stisknute klavesy, s ochranou proti vstupu mimo oblast hraciho pole
int posunHrace(int** matrix, int m, int n, char d, struct Hrac* aktualniHrac) {
	int yOld = aktualniHrac->y, xOld = aktualniHrac->x;
	int yNew = aktualniHrac->y, xNew = aktualniHrac->x;

	int offset = 0;
	if (d == 'l') offset = -1;
	if (d == 'r') offset = 1;

	if (offset == 0) return 0;

	xNew += offset;
	if (xNew < 0 || xNew >= n) return 0;

	if (matrix[yNew][xNew] == ID_H1 || matrix[yNew][xNew] == ID_H2) return 0; //prekazi nam tank

	if (matrix[yNew][xNew] == ID_ZEM) {
		//lezem do kopce
		for (int YKteryPraveSkenuju = yNew; YKteryPraveSkenuju >= 0; YKteryPraveSkenuju--) {
			if (matrix[YKteryPraveSkenuju][xNew] == ID_H1 || matrix[YKteryPraveSkenuju][xNew] == ID_H2) return 0; //prekazi nam tank

			if (matrix[YKteryPraveSkenuju][xNew] == ID_VZDUCH) {

				aktualniHrac->y = YKteryPraveSkenuju;
				aktualniHrac->x = xNew;
				vykresliBod(matrix, yOld, xOld, ID_VZDUCH);
				vykresliBod(matrix, aktualniHrac->y, aktualniHrac->x, aktualniHrac->id);
				return 1;
			}
		}
	}
	else {
		//rovina nebo dolu
		for (int skenovaneY = yNew; skenovaneY < m; skenovaneY++) {
			if (matrix[skenovaneY][xNew] == ID_H1 || matrix[skenovaneY][xNew] == ID_H2) return 0; //prekazi nam tank
			if (matrix[skenovaneY][xNew] == ID_ZEM) {
				aktualniHrac->y = skenovaneY - 1;
				aktualniHrac->x = xNew;
				vykresliBod(matrix, yOld, xOld, ID_VZDUCH);
				vykresliBod(matrix, aktualniHrac->y, aktualniHrac->x, aktualniHrac->id);
				return 1;
			}
		}
	}
	return 0;
}

//Jednoduchy casovac zajistujici pocatecni zpozdeni
int jednoduchyCasovac(int n_seconds) {
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
void zmenZobrazeniKurzoru(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

//zobrazi game over screen
void vyhra(struct Hrac* vyherce, int** matrix, int m, int n) {

	//skryjTrajektorii();
	vykresliMatici(matrix, m, n);
	if (vyherce != NULL) {
		printf("\n %s VYHR�V�!\n", vyherce->jmeno);
	}
	else {
		printf("\n Je to plichta! :( \n");
	}
}

// Prevadeni stupnu na radiany
double stupneNaRadiany(int uhelVeStupnich) {
	return (((double)uhelVeStupnich) * M_PI / 180.0);
}

//Vystreli projektil, umi pocitat trajektorii, ubrat zivoty, nicit povrch
void vystrel(int** matrix, int m, int n, struct Hrac* aktualniHrac, struct Hrac* druhyHrac) {
	//V�po�et trajektorie - �ikm� vrh:
	const double krok = 1.0 / 1000;
	int pocetKroku = 0;

	//int.......... 10
	//double....... 10.5

	double x0 = aktualniHrac->x + 0.5;
	double y0 = aktualniHrac->y + 0.5;

	double vx = cos(stupneNaRadiany(aktualniHrac->uhel)) * aktualniHrac->sila;
	double vy = sin(stupneNaRadiany(aktualniHrac->uhel)) * - aktualniHrac->sila;

	double cas;
	double x;
	double y;
	bool raketaOpustilaTank = false; //raketa neopustila tank //Kontrola 
	while (1) {
		pocetKroku++;
		cas = krok * pocetKroku;
		x = x0 + vx * cas;
		y = y0 + vy * cas + (pow(cas, 2) * GRAVITACE) / 2;

		if (y > m || x < 0 || x > n) { //raketa se nevr�t� pokud je pod nebo mimo stran mapy
			break;
		}

		if (y < 0) {
			continue; //pokud je naho�e nad mapu, raketa se vr�t� 
		}

		//double 10.5; int(10.9) ->  10

		int poleX = int(x); //zakrouhleni dolu
		int poleY = int(y);
		int pole = matrix[poleY][poleX];

		//if (pole >= ID_H1);
		//pole 11 -> hrac
		// 11 - ID_H1 -> 1 -> poleHracu[1].zivoty--;

		if (pole == aktualniHrac->id) { //Sn�en� �ivot� po z�sahu s�m sebe
			if (raketaOpustilaTank) {
				//Z�sah
				aktualniHrac->zivoty -= SNIZENI_ZIVOTU;
				break;
			}
		}
		else if (pole == druhyHrac->id) { //Z�sah druh�ho hr��e
			//Z�sah
			druhyHrac->zivoty -= SNIZENI_ZIVOTU;
			break;
		}
		else if (pole == ID_ZEM) { //Z�sah do zem� + ni�en� povrchu
			if (NICENI_POVRCHU) { //Pokud je zapnuto
				vykresliBod(matrix, poleY, poleX, ID_TRAJ_DOPAD); //vykresleni trajektorie

				//TODO: del
				for (int skenovaneY = poleY - OKRUH_VYBUCHU; skenovaneY <= poleY + OKRUH_VYBUCHU; skenovaneY++) { //Od horn� hranice okruhu v�buchu po spodn� s kontrolou rozsahu mimo mapu a okruh v�buchu
					for (int skenovaneX = poleX - OKRUH_VYBUCHU; skenovaneX <= poleX + OKRUH_VYBUCHU; skenovaneX++) { //Od leva doprava okruhu v�buchu 
						//(Y-y)^2 + (X-x)^2 < R^2
						//A^2 + B^2 = C^2
						if (pow(skenovaneY - poleY, 2) + pow(skenovaneX - poleX, 2) < pow(OKRUH_VYBUCHU, 2) && matrix[skenovaneY][skenovaneX] == ID_ZEM) { //v okoli vybuchu + v map�
							vykresliBod(matrix, skenovaneY, skenovaneX, ID_VZDUCH); //vykresli vzduch m�sto zem�
						}
					}
				}
				
				//////////////////
				//max(0, poleY - OKRUH_VYBUCHU)
				//max(0, -2) -> 0

				//0..m-1
				for (int skenovaneY = max(0, poleY - OKRUH_VYBUCHU); skenovaneY <= min(poleY + OKRUH_VYBUCHU, m - 1); skenovaneY++) { //Od horn� hranice okruhu v�buchu po spodn� s kontrolou rozsahu mimo mapu a okruh v�buchu
					for (int skenovaneX = max(0, poleX - OKRUH_VYBUCHU); skenovaneX <= min(poleX + OKRUH_VYBUCHU, n - 1); skenovaneX++) { //Od leva doprava okruhu v�buchu s kontrolou rozsahu mimo mapu a okruh v�buchu
						if (matrix[skenovaneY][skenovaneX] == ID_ZEM && pow(skenovaneY - poleY, 2) + pow(skenovaneX - poleX, 2) < pow(OKRUH_VYBUCHU, 2)) { //v okoli vybuchu
							vykresliBod(matrix, skenovaneY, skenovaneX, ID_VZDUCH); //vykresli vzduch m�sto zem�
						}
					}
				}
			}
			//Po�kozen� v okol� v�buchu
			if (POSKOZENI_V_OKOLI_DOPADU) {

				if (pow(poleY - aktualniHrac->y, 2) + pow(poleX - aktualniHrac->x, 2) < pow(OKRUH_VYBUCHU, 2)) { //pozice hr��e je uvnit� okruhu v�buchu
					aktualniHrac->zivoty -= SNIZENI_ZIVOTU * NASOBITEL_VYBUCHU; //��ste�n� demage //dmg_mult
				}


				if (pow(poleY - druhyHrac->y, 2) + pow(poleX - druhyHrac->x, 2) < pow(OKRUH_VYBUCHU, 2)) {
					druhyHrac->zivoty -= SNIZENI_ZIVOTU * NASOBITEL_VYBUCHU;
				}

				//1) projit vsechny -> vzdalenost
				//2) for -> for
			}
			//Gravitace
			if (NICENI_POVRCHU) {
				for (int skenoveX = max(0, poleX - OKRUH_VYBUCHU); skenoveX <= min(poleX + OKRUH_VYBUCHU, n - 1); skenoveX++) {
					int povrch = m; //Mapa za��n� na sou�adnic�ch 0,0; "m" je 1. ��dek pod mapou (m jsou ��dky)
					for (int skenovaneY = m - 1; skenovaneY >= 0; skenovaneY--) { //za�ni od posledn�ho ��dku mapy; je� dokud se nedostane� na 0 ��dek (za��tek/vr�ek mapy)
						if (povrch == skenovaneY + 1) {
							if (matrix[skenovaneY][skenoveX] == ID_ZEM) {
								povrch = skenovaneY; //pokud se tank st�le nach�z� "v map�" -> dej do "povrch" jeho novou pozici ��dku
							}
						}
						// pevna zem neni pod nama
						else {
							if (povrch == m) { //Povrch -> mimo mapu
								//void:
								if (aktualniHrac->id == matrix[skenovaneY][skenoveX]) {
									vykresliBod(matrix, skenovaneY, skenoveX, ID_VZDUCH);
									aktualniHrac->nazivu = false; //"zabij" hra�e
								}
								else if (druhyHrac->id == matrix[skenovaneY][skenoveX]) {
									vykresliBod(matrix, skenovaneY, skenoveX, ID_VZDUCH);
									druhyHrac->nazivu = false;
								}
								else { //vyma� v�e ostatn�
									vykresliBod(matrix, skenovaneY, skenoveX, ID_VZDUCH);
								}
							}
							else { //pevna zem je v ramci sveta
								//povrch--;
								//p�esun tanku a povrchu po zni�en�/zm�n� povrchu
								if (aktualniHrac->id == matrix[skenovaneY][skenoveX]) { //hr�� je v map�
									povrch--; //povrch hr��e se t�mto nastav� nad pozici zem�
									aktualniHrac->y = povrch; //tank nastav na tuto pozici
									vykresliBod(matrix, skenovaneY, skenoveX, ID_VZDUCH); //vykresli vzdych tam kde hr�� byl
									vykresliBod(matrix, povrch, skenoveX, aktualniHrac->id); //vykresli hr��e nad pol��kem zem�
								}
								else if (druhyHrac->id == matrix[skenovaneY][skenoveX]) {
									povrch--;
									druhyHrac->y = povrch;
									vykresliBod(matrix, skenovaneY, skenoveX, ID_VZDUCH);
									vykresliBod(matrix, povrch, skenoveX, druhyHrac->id);
								}
								else if (matrix[skenovaneY][skenoveX] == ID_ZEM) {
									povrch--;
									vykresliBod(matrix, skenovaneY, skenoveX, ID_VZDUCH);
									vykresliBod(matrix, povrch, skenoveX, ID_ZEM);
								}
							}
						}
					}
				}
			}
			break;
		}
		else if (pole == ID_VZDUCH) { //pokud raketa "zas�hne" vzduch
			raketaOpustilaTank = true; //opustila tank
			vykresliBod(matrix, poleY, poleX, ID_TRAJ); //za�ne vykreslovat trajektorii
		}
	}

}

//Vycisti trajektorii z obrazovky
void skryjTrajektorii(int** matrix, int m, int n) {
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (matrix[i][j] == ID_TRAJ || matrix[i][j] == ID_TRAJ_DOPAD) {
				vykresliBod(matrix, i, j, ID_VZDUCH);
			}
		}
	}
}