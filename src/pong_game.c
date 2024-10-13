/*
* This pong game originated from code written by @vicentebolea on github.
* To see the original project, visit https://github.com/vicentebolea/Pong-curses/blob/master/pong.c
*/
#include <unistd.h>
#include <ncurses.h>

// Declares the Ball object used by the game
typedef struct{
  short int x, y, c;
  bool movhor, movver; // If false/true correspond to left/right or up/down.
  } object;

int start_pong(struct packet*) {
  // Initialize screen, colors, and register keypad.
  object scr; int i = 0,cont=0; bool end=false;
  initscr();
  start_color();
  init_pair(1,COLOR_BLUE,COLOR_BLACK);
  keypad(stdscr,true);
  noecho();
  curs_set(0);
  getmaxyx(stdscr,scr.y,scr.x);
  // Create game objects (paddles, ball)
  // b is ball, b2 is p1 paddle, b1 is p2 paddle
  // object counters all start at 0, keep track of score
  object b1={scr.x-2,scr.y/2,0,false,false},b2={1,scr.y/2,0,false,false},b={scr.x/2,scr.y/2,0,false,false};
  mvprintw(4,0,"\t           oooooooooo                                        \n"
               "\t           888    888  ooooooo    ooooooo    oooooooo8       \n"
               "\t           888oooo88 888     888 888   888  888    88o       \n"
               "\t           888       888     888 888   888   888oo888o       \n"
               "\t          o888o        88ooo88  o888o o888o 888     888      \n"
               "\t                                             888ooo888     \n\n"
               "\tA game by vicente.bolea@gmail.com, modified for network play \n"
               "\t \t\t\tYour controls are the arrows of the keyboard \n"
               "\t \t\t\tPush ANY key to start, 'p' for pause and ESC to quit" ); 

  /*
   * Need to add more logic here to initialize game by sending start request to router.
   * - Send game req to router with desired destination (Player invite). Waiting screen...
   * - Router sends invite packet to next destination
   * - Once both sides have accepted, router sends start message to both games.
   * - 
  */

  
  getch();
  //send_message();
  // Main Game loop. Runs until end is declared.
  for (nodelay(stdscr,1); !end; usleep(4000)) {

    //Checks the ball location

    // Add one to counter, and every 16 iterations check the border cases
    if (++cont%16==0){
      if ((b.y==scr.y-1)||(b.y==1)) // Check vertical borders, stop vert movement.
        b.movver=!b.movver;
      if ((b.x>=scr.x-2)||(b.x<=2)){ // Check horizontal borders, stop horizontal movement
        b.movhor=!b.movhor;
        // Change ball trajectory depending on position relative to paddle.
        if ((b.y==b1.y-1)||(b.y==b2.y-1)) {
          b.movver=false;
        } else if ((b.y==b1.y+1)||(b.y==b2.y+1)) {
          b.movver=true;
        } else if ((b.y != b1.y) && (b.y != b2.y)) { //Update score and reset ball.
          (b.x>=scr.x-2) ? b1.c++: b2.c++;
          b.x=scr.x/2;
          b.y=scr.y/2;
        }
      }
      b.x=b.movhor ? b.x+1 : b.x-1;
      b.y=b.movver ? b.y+1 : b.y-1;

      // Prevent paddles from moving offscreen.
      if (b1.y<=1)
        b1.y=scr.y-2;
      if (b1.y>=scr.y-1)
        b1.y=2;
      if (b2.y<=1)
        b2.y=scr.y-2;
      if (b2.y>=scr.y-1)
        b2.y=2;
    }
    /* Handles user input
     * getch takes in keystrokes and can provide appropriate funcitonality.
     * This will include setting packet data byte to the appropriate movement key
     */
    switch (getch()) {
      case KEY_DOWN:
        send_update(0x01);
        b1.y++;
        
        break;
      case KEY_UP: 
        send_update(0x02);  
        b1.y--;
        
        break;
      case 0x1B:
        send_update(0x03);  
        endwin();
        end++;
        // send end message...
        break; // This is the escape button
    }

    // Check the cache for user input
    /* 
    * Unlock thread so that send can send the message now. Then relock the thread. This won't be called until after
    the sending is done, because you can't lock a unlocked thread.
    */
    pthread_mutex_unlock(&send_mutex);

    pthread_mutex_lock(&send_mutex);
    
    // Erases and then redraws the screen.
    erase();
    mvprintw(2,scr.x/2-2,"%i | %i",b1.c,b2.c);
    mvvline(0,scr.x/2,ACS_VLINE,scr.y);
    attron(COLOR_PAIR(1));
    mvprintw(b.y,b.x,"o");
    for(i=-1;i<2;i++){
      mvprintw(b1.y+i,b1.x,"|");
      mvprintw(b2.y+i,b2.x,"|");}
    attroff(COLOR_PAIR(1));
  }
  return 0;
}

uint8_t* send_update(uint8_t data)
{
  uint8_t device_addr = 0x01;
  char* receiver_name;
  uint8_t receiver_addr = select_address(&receiver_name);
  //print_byte_binary(receiver_addr);
  size_t payload_length;
  uint8_t* payload = text_to_bytes(&payload_length, *receiver_name);

  size_t encoded_length;

  //printf("Size of encoded packet %ld\n", encoded_length);

  uint8_t* packet = (uint8_t*)malloc(50 * sizeof(uint8_t));
  *data_size = build_packet(device_addr, receiver_addr, hamload, encoded_length, packet);
  
  printf("Message sent successfully \n");
  return packet;
}

void read_message(uint8_t* packet, size_t packet_len, size_t* decoded_len){
//    uint8_t* hamload = ham_decode(packet, packet_len, decoded_len);
    char* message = bytes_to_text(packet, packet_len);
    printf("Message received: %s\n", message);
}