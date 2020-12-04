// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

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
#define GRAVITACE 10
#define MAX_ZIVOTY 100
#define SNIZENI_ZIVOTU 50

#define OKRUH_VYBUCHU 2
#define NASOBITEL_VYBUCHU 0.5

#define NICENI_POVRCHU true
#define POSKOZENI_V_OKOLI_DOPADU true

#define ID_VZDUCH 0
#define ID_ZEM 1

#define ID_H1 2
#define ID_H2 3

#define ID_TRAJ 4
#define ID_TRAJ_DOPAD 5

#define ODPOCET 2