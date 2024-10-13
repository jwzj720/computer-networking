#include <ncurses.h>
#include <string.h>
#include <unistd.h> // For sleep function

#define APP_COUNT 2

typedef struct{
  short int x, y;
  bool selected; // If true, it is currently selected.
} object;

int init_screen();

int app_select();
