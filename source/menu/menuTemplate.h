#pragma once

#include <3ds.h>
#include <citro2d.h>
#include "main.h"
#include "menuButton.h"
#include "menuDialog.h"

typedef struct {
    //
} MenuTemplate__Struct;

void    menuTemplate__Init(void);
void    menuTemplate__Exit(void);
int     menuTemplate__Act(void);
void    menuTemplate__Render(gfxScreen_t screen);
bool    menuTemplate__AnimIn(void);
bool    menuTemplate__AnimOut(void);
