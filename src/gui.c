#include <ncurses.h>
#include <string.h>
#include <unistd.h> // For sleep function

#define APP_COUNT 2

typedef struct{
  short int x, y;
  bool selected; // If true, it is currently selected.
} object;

int init_screen()
{
    initscr();
    noecho();          
    curs_set(FALSE);
    clear();
    // Display the text in a formatted way
    mvprintw(4, 0,  "\t           OOOOOOO     OOOOOOO    OOOOOOO    OOOOOOO    OOOOOOO    OOOOOOO     OOOOOO \n"
                    "\t           O      O       O          O          O          O       O          O       \n"
                    "\t           OOOOOOO        O          O          O          O       O          O       \n"
                    "\t           O      O       O          O          O          O       OOOOOO      OOOOO  \n"
                    "\t           O       O      O          O          O          O       O                O \n"
                    "\t           O      O       O          O          O          O       O                O \n"
                    "\t           OOOOOOO     OOOOOOO       O          O       OOOOOOO    OOOOOOO    OOOOOO  \n"
                    "\t \t\t\tWaiting to connect to network\n");

    // Position for the dots
    int dot_row = 10;
    int dot_col = 0;
    const char *dots[] = {"Loading .  ", "Loading .. ", "Loading ..."};
    int current_dot = 0;

    // Loop to animate dots. Loading loop breaks when connect is found.
    // TODO: Break infinite loop
    while(1)
    {
        mvprintw(dot_row, dot_col, "%s", dots[current_dot]);
        refresh(); //May not need to refresh the screen here...
        usleep(500000); // Sleep for 500 milliseconds

        current_dot = (current_dot + 1) % 3; // Cycle through the dots
        clear(); // Clear the screen for the next update

        // Redraw the "BITTIES" text
        mvprintw(4, 0, "\t           OOOOOOo     OOOOOOO    OOOOOOO    OOOOOOO    OOOOOOO    OOOOOOO     OOOOOO \n"
                       "\t           O      OO      O          O          O          O       O          O       \n"
                       "\t           OOOOOOOO       O          O          O          O       O          O       \n"
                       "\t           O      Oo      O          O          O          O       OOOOOO      OOOOO  \n"
                       "\t           O      Oo      O          O          O          O       O                O \n"
                       "\t           O      OO      O          O          O          O       O                O \n"
                       "\t           OOOOOOO     OOOOOOO       O          O       OOOOOOO    OOOOOOO    OOOOOO  \n"
                       "\t \t\t\tWaiting to connect to network\n");
    }

    return 0;
}

int app_select()
{
    object scr;
    initscr();
    keypad(stdscr,true);
    noecho();          
    curs_set(FALSE);
    getmaxyx(stdscr,scr.y,scr.x);
    clear();

    char* text = "Use up/down keys to navigate options";
    char* text2 = "Press enter to make selection";
    int x_start = (scr.x-strlen(text))/2;
    int y_start = scr.y/2;

    int selection = 0;
    int hover = 1;

    while(!selection)
    {
        clear();
        mvprintw(y_start-1, x_start, "%s", text);
        mvprintw(y_start, x_start, "%s", text2);

        attron((hover==1)? A_STANDOUT:A_NORMAL);
        mvprintw(y_start+1,x_start,"%s","Chatroom");
        attroff((hover==1)?A_STANDOUT:A_NORMAL);

        attron((hover==2)?A_STANDOUT:A_NORMAL);
        mvprintw(y_start+2,x_start,"%s","Pong");
        attroff((hover==2)?A_STANDOUT:A_NORMAL);
        refresh();

        switch (getch()) {
            case KEY_DOWN: 
                hover = (hover % APP_COUNT)+1;
                break;
            case KEY_UP:
                hover = (hover-- % APP_COUNT) ? hover : APP_COUNT;
                break;
            case KEY_ENTER: selection = hover; break; 
        }
        
    }
    
    return selection;
}