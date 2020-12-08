#pragma once

#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <math.h>

#define M_PI 3.14159265359
#define getch() _getch()

#define MIN_UHEL 0
#define MAX_UHEL 180

#define MIN_SILA 1
#define MAX_SILA 50

#define PALIVO 20
#define MAX_ZIVOTY 100

#define GRAVITACE 10

#define SNIZENI_ZIVOTU 50
#define OKRUH_VYBUCHU 2
#define NASOBITEL_VYBUCHU 0.5

#define NICENI_POVRCHU true
#define POSKOZENI_V_OKOLI_DOPADU true

#define ID_VZDUCH 0
#define ID_ZEM 1

#define ID_H1 2
#define ID_H2 3

#define ID_H1A 10

#define ID_TRAJ 4
#define ID_TRAJ_DOPAD 5

#define ODPOCET 2

struct Hrac {
	char jmeno[50];
	int id;
	int x, y;
	int zbylePosuny;
	int uhel;
	int sila;
	int zivoty;
	bool nazivu = true;
};

int** vytvorMatici(int m, int n);
void inicializujMatici(int** matrix, int m, int n);
void nahrajMapuDoMatice(int** matrix, int m, int n, FILE** plan);
void nastavPoziciKurzoru(int x, int y);
void vykresliMatici(int** matrix, int m, int n);
void vypisHerniInfo(int m, int n, struct Hrac* aktualniHrac, struct Hrac* hrac1, struct Hrac* hrac2);
void dealokujMatici(int** matrix, int m);
void vykresliBod(int** matrix, int y, int x, int id);
int posunHrace(int** matrix, int m, int n, char d, struct Hrac* aktualniHrac);
int jednoduchyCasovac(int n_seconds);
void zmenZobrazeniKurzoru(bool showFlag);
void vyhra(struct Hrac* vyherce, int** matrix, int m, int n);
double stupneNaRadiany(int uhelVeStupnich);
void vystrel(int** matrix, int m, int n, struct Hrac* aktualniHrac, struct Hrac* druhyHrac);
void skryjTrajektorii(int** matrix, int m, int n);
