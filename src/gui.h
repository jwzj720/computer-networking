#ifndef GUI_H
#define GUI_H

#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include "objects.h"

#define APP_COUNT 2



int init_screen();
int app_select();

#endif
