#include "funkceHry.h"

/*
Vytvorit hrace
Vytvorit mapu
Dokud nekdo nevyhraje:
	Key press
	Provest danou akci
	pokud vystrelil, ukoncit tah
*/

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
	fgets(hrac1.jmeno, 50, stdin);
	if ((strlen(hrac1.jmeno) > 0) && (hrac1.jmeno[strlen(hrac1.jmeno) - 1] == '\n'))
		hrac1.jmeno[strlen(hrac1.jmeno) - 1] = '\0';

	struct Hrac hrac2;
	hrac2.id = ID_H2;
	hrac2.zivoty = MAX_ZIVOTY;
	hrac2.uhel = 90;
	hrac2.sila = 30;
	hrac2.zbylePosuny = PALIVO;
	printf("Jmeno hrace 2? ");
	fgets(hrac2.jmeno, 50, stdin);
	if ((strlen(hrac2.jmeno) > 0) && (hrac2.jmeno[strlen(hrac2.jmeno) - 1] == '\n'))
		hrac2.jmeno[strlen(hrac2.jmeno) - 1] = '\0';

	int** matrix;

	struct Hrac* aktualniHrac;
	struct Hrac* druhyHrac;

	FILE* game_plan_in; 
	errno_t err = fopen_s(&game_plan_in, "mapa.txt", "r");
	if (err != 0) { //nenalezení herního plánku
		printf("Neobjeven herni plan!\n");
		return 1;
	}

	fscanf_s(game_plan_in, "%d %d\n", &n, &m); //načítání mapky a pozic hráčů
	fscanf_s(game_plan_in, "%d %d %d %d\n", &hrac1.x, &hrac1.y, &hrac2.x, &hrac2.y);

	matrix = vytvorMatici(m, n);

	inicializujMatici(matrix, m, n);
	nahrajMapuDoMatice(matrix, m, n, &game_plan_in);

	fclose(game_plan_in);

	//Přiřazení pozic hráčům
	matrix[hrac1.y][hrac1.x] = hrac1.id;
	matrix[hrac2.y][hrac2.x] = hrac2.id;

	//skryti kursoru
	zmenZobrazeniKurzoru(false);

	int counter = ODPOCET;
	while (counter > 1) {
		system("cls");
		printf("Do startu zbyva: %d s", counter);
		counter -= jednoduchyCasovac(1);
	}

	aktualniHrac = &hrac1;
	druhyHrac = &hrac2;
	vykresliMatici(matrix, m, n); //vykreslení mapy

	unsigned int key;
	bool konecTahu;
	while (1)
	{
		konecTahu = false;
		vypisHerniInfo(m, n, aktualniHrac, &hrac1, &hrac2);
		key = getch();
		//Odstraneni prebytecnych znaku odesilanych pri stisku smerovych sipek
		if (key == 0 || key == 0xE0) {
			key = getch();
		}

		skryjTrajektorii(matrix, m, n);

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
		else if (key == 50) {
			//klavesa 2
			aktualniHrac->sila += 1;
		}
		else if (key == 49) {
			//klavesa 1
			aktualniHrac->sila -= 1;
		}
		else if (key == 32) {
			//klavesa SPACE
			vystrel(matrix, m, n, aktualniHrac, druhyHrac);
			konecTahu = 1; //výstřel a ukončení tahu

			if (hrac1.zivoty <= 0) {
				hrac1.nazivu = false;
			}
			if (hrac2.zivoty <= 0) {
				hrac2.nazivu = false;
			}
		}
		else if (key == 75) {
			//klavesa LEFT
			if (aktualniHrac->zbylePosuny > 0)
				aktualniHrac->zbylePosuny -= posunHrace(matrix, m, n, 'l', aktualniHrac);
		}
		else if (key == 77) {
			//klavesa RIGHT
			if (aktualniHrac->zbylePosuny > 0)
				aktualniHrac->zbylePosuny -= posunHrace(matrix, m, n, 'r', aktualniHrac);
		}

		if (hrac1.nazivu == false && hrac2.nazivu == false) {
			//oba umreli
			vyhra(NULL, matrix, m, m);
			break;
		}
		if (hrac1.nazivu == false) {
			//hráč 2 vyhrál
			vyhra(&hrac2, matrix, m, n);
			break;
		}
		if (hrac2.nazivu == false) {
			//hráč 1 vyhrál
			vyhra(&hrac1, matrix, m, n);
			break;
		}

		//ošetření rozsahu síly a úhlu
		if (aktualniHrac->uhel < MIN_UHEL) aktualniHrac->uhel = MIN_UHEL;
		if (aktualniHrac->uhel > MAX_UHEL) aktualniHrac->uhel = MAX_UHEL;
		if (aktualniHrac->sila < MIN_SILA) aktualniHrac->sila = MIN_SILA;
		if (aktualniHrac->sila > MAX_SILA) aktualniHrac->sila = MAX_SILA;

		if (konecTahu) { //po stisku mezerníku
			if (aktualniHrac->id == hrac1.id) { //prohození rolí
				aktualniHrac = &hrac2; //hrát bude hráč2
				druhyHrac = &hrac1; //2. čeká
			}
			else { //jinak hraje hráč 1
				aktualniHrac = &hrac1;
				druhyHrac = &hrac2;
			}
			aktualniHrac->zbylePosuny = PALIVO; //nakonci tahu doplň palivo
		}


	}
	dealokujMatici(matrix, m);
	return 0;
}