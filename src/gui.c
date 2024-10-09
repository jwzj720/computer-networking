#include "gui.h"

int init_screen()
{
    object scr;
    initscr();
    noecho();          
    curs_set(FALSE);
    getmaxyx(stdscr,scr.y,scr.x);

    // Position for the dots
    int dot_col = (scr.x-strlen("Loading ..."))/2;
    int dot_row = scr.y/2;
    const char *dots[] = {"Loading .  ", "Loading .. ", "Loading ..."};
    int current_dot = 0;

    // Loop to animate dots. Loading loop breaks when connect is found.
    // TODO: Break infinite loop
    int i=0;
    while(i<5)
    {
        clear();
        current_dot = (current_dot + 1) % 3; // Cycle through the dots
        mvprintw(4, 0, "\t           OOOOOOo     OOOOOOO    OOOOOOO    OOOOOOO    OOOOOOO    OOOOOOO     OOOOOO \n"
                       "\t           O       O      O          O          O          O       O          O       \n"
                       "\t           OOOOOOOO       O          O          O          O       O          O       \n"
                       "\t           O      O       O          O          O          O       OOOOOO      OOOOO  \n"
                       "\t           O       O      O          O          O          O       O                O \n"
                       "\t           O      O       O          O          O          O       O                O \n"
                       "\t           OOOOOOO     OOOOOOO       O          O       OOOOOOO    OOOOOOO    OOOOOO  \n\n\n"
                       "\t	     Waiting to connect to network\n");
        mvprintw(dot_row, dot_col, "%s", dots[current_dot]);
        refresh(); //May not need to refresh the screen here...
        usleep(500000); // Sleep for 500 milliseconds
	i++;
        
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
    char* apps[APP_COUNT] = {"CHATROOM","PONG"};
    while(!selection)
    {
        clear();
        mvprintw(y_start-1, x_start, "%s", text);
        mvprintw(y_start, x_start, "%s", text2);

        for (int i=1; i<APP_COUNT+1;i++)
        {
            attron((hover==i)?A_STANDOUT:A_NORMAL);
            mvprintw(y_start+i,x_start,"%s",apps[i-1]);
            attroff((hover==i)?A_STANDOUT:A_NORMAL);
        }
        refresh();
        //usleep(500000);

        switch (getch()) {
            case KEY_DOWN: 
                hover = (hover % APP_COUNT)+1;
                break;
            case KEY_UP:
                hover = (--hover % APP_COUNT) ? hover : APP_COUNT;
                break;
            case '\n': 
		clear();
		printw("Enter key pressed! Selection: %d",hover);
		refresh();
		usleep(1000000);
		selection = hover;
	       	endwin();
		break; 
		

        }
        
    }    
    return selection;
}
