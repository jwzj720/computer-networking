#include "message_app.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "selection.h"
#include "gui.h"
#include "pong_game.h"
#include <ncurses.h>

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t read_cond = PTHREAD_COND_INITIALIZER;


void* read_thread(void* pinit)
{
    // Allocate memory for the packet object... Still need to allcate the packet data memory.
    struct AppData *app_data = (struct AppData*) pinit;
    app_data->received_packet = malloc(sizeof(struct Packet));
    // Create Data reading object, which will store a message's data.
    struct ReadData *rd = create_reader(1);

    struct Packet* packet = app_data->received_packet;
    packet->data = calloc(50,sizeof(uint8_t));
    
    // Check data was allocated
    if (rd->data == NULL)
    {
        fprintf(stderr, "Couldn't allocate memory for data\n");
        return NULL;
    }
    // Register the callback with user data
    int id = callback_ex(app_data->pinit, GPIO_RECEIVE, EITHER_EDGE, get_bit, rd);
    if (id < 0)
    {
        fprintf(stderr, "Failed to set callback\n");
        pigpio_stop(*(int*)pinit);
        
        return NULL;
    }

    // THIS is the per message read loop
    while(1)
    {
        // read a message
        read_bits(rd);
        
        // Data received, lock threading to hold reading until packet is interpreted.
        // We don't want rd->data to be overwritten during this time.
        pthread_mutex_lock(&read_mutex);
        printf("read entered, getting message...\n");
        data_to_packet(packet, rd->data);
        printf("Read Packet; ");
        for (size_t i=0; i<packet->dlength; i++)
        {
            printf("%02X ",packet->data[i]);
        }
        printf("\n");
	
	//reset readrate and run variables each iteration.
        reset_reader(rd);
        pthread_mutex_unlock(&read_mutex);
    }

    //When done with the reading thread
    callback_cancel(id);

    // Free Data
    free(rd->data); //Do we need to free the data? pretty sure this is done in the read_to_file.
    free(rd);
    free(packet->data);
    free(packet);

    return NULL;
}

void* send_thread(void* pinit) // passing app_data in instead of pinit
{
    struct AppData *app_data = (struct AppData*) pinit;
    // Allocate memory for the packet object... Still need to allcate the packet data memory.
    app_data->sent_packet = malloc(sizeof(struct Packet));
    struct Packet* packet_data = app_data->sent_packet;
    packet_data->sending_addy = 0x01;
    //char* receiver_name;
    //app_data->sent_packet->receiving_addy = select_address(&receiver_name);
    packet_data->receiving_addy = 0x02;
    packet_data->data = (uint8_t*)calloc(50,sizeof(uint8_t));
    packet_data->dlength=0;

    // Allocate payload memory
    uint8_t* payload = (uint8_t*)calloc(50, sizeof(uint8_t));

    // Wont start sending until thread first locked.

    while(1)
    {
        // Lock the send data while being sent...
        pthread_mutex_lock(&send_mutex);
        printf("write entered, sending message...\n");

        size_t pack_size;
        
        // put sendable data into payload variable and define packet size
        pack_size = build_packet(packet_data, payload);

        // Send the message
        int eval = send_bytes(payload, pack_size, GPIO_SEND, app_data->pinit);
        if (eval != 0)
        {
            printf("Failed to send message\n");
            return NULL;
        }
        printf("Sent Packet; ");
	for (size_t i=0; i<pack_size; i++)
	{
		printf("%02X ",payload[i]);
	}
	printf("\n");
        packet_data->dlength=0;
        // unlock so it can be repopulated.
        pthread_mutex_unlock(&send_mutex);
	usleep(500);
    }
    return NULL;
}

int start_pong(struct AppData* parent_data) {
  uint8_t p2_ready = 0x00;

  struct AppData* app_data = parent_data;


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
  // send mutex is default set to unlock on start, so it wont write until this runs.
  endwin();
  printf("Sending first update\n");
  send_update(app_data,(uint8_t)0x02);
  pthread_mutex_unlock(&read_mutex);

  while(!p2_ready)
  {
    pthread_mutex_unlock(&send_mutex);
    usleep(500);
    pthread_mutex_lock(&send_mutex);
    printf("Sent Message\n");
    send_update(app_data, (uint8_t)0x02); // queue another message...

    pthread_mutex_lock(&read_mutex);
    p2_ready = check_data(app_data);
    pthread_mutex_unlock(&read_mutex);
    printf("P2_ready: %"PRIu8"\n",p2_ready);
    //refresh();
  }
  // Lock thread so nothing sends until unlocked.
  while(1)
  {
  	printf("GAME READY!!!\n");
	fflush(stdout);
  }
  //usleep(5000000);
  // Main Game loop. Runs until end is declared.
  for (nodelay(stdscr,1); !end; usleep(500000)) {

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
        send_update(app_data,(uint8_t)0x01);
        b1.y++;
        
        break;
      case KEY_UP: 
        send_update(app_data,(uint8_t)0x02);  
        b1.y--;
        
        break;
      case 0x1B:
        send_update(app_data,(uint8_t)0x03);  
        endwin();
        end=true;
        // send end message...
        break; // This is the escape button
    }

    /* 
    * Unlock thread so that send can send the message now. Then relock the thread. This won't be called until after
    the sending is done
    */
    pthread_mutex_unlock(&send_mutex);
    usleep(500);
    pthread_mutex_lock(&send_mutex);

    // see if opponent has sent any new moves.
    pthread_mutex_lock(&read_mutex);
    uint8_t input = check_data(app_data);
    pthread_mutex_unlock(&read_mutex);

    // Update variables according to input
    switch (input) {
      // case 0:
      //   endwin();
      //   printf("An error occurred");
      //   end++;
      //   break;
      case 0x01:
        b2.y++;
        break;
      case 0x02: 
        b2.y--;
        break;
      case 0x03:  
        endwin();
        end=true;
        break; // This is the escape button
    }
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

  //unlock thread mutex when finished.
  return 0;
}

int main()
{

    // Initialize router connection
    // Pass in pthread address so that 
    init_screen();

    // Initialize app data object
    struct AppData app_data;

    // Initialize pigpio
    app_data.pinit = pigpio_start(NULL, NULL);

    if (app_data.pinit < 0)
    {
        fprintf(stderr, "Didn't initialize pigpio library\n");
        return 1;
    }

    // Set GPIO modes
    if (set_mode(app_data.pinit, GPIO_SEND, PI_OUTPUT) != 0 || 
        set_mode(app_data.pinit, GPIO_RECEIVE, PI_INPUT) != 0)
    {
        pigpio_stop(app_data.pinit);
        return 1;
    }

    // Start send thread locked...
    pthread_mutex_lock(&send_mutex);
    pthread_mutex_lock(&read_mutex);

    // Create reading/writing threads
    if(pthread_create(&reading_thread, NULL, read_thread, &app_data) != 0) {
        perror("Could not create reading thread");
        return 1;
    }
    time_sleep(.5);

    if(pthread_create(&write_thread, NULL, send_thread, &app_data) != 0) {
        perror("Could not create writing thread");
        return 1;
    }
    // Main Running loop
    while (1)
    {
        app_data.selected_application = app_select()-1;
        if (app_data.selected_application == 0) // Chat application
            {
                //start_message(&app_data); //Run the message app
                printf("Text application\n");
		        sleep(5000000);
                fflush(stdin);
                // payload = send_message(&data_size);
            }
            else if (app_data.selected_application == 1) // Pong application
            {
                start_pong(&app_data);
            }
            else {
                return 0; // Invalid application
            }
        // When app is exited and returns, go back to the app select screen.

    }
    // Wait for writing to complete before continuing
    pthread_join(write_thread, NULL);
    pthread_join(reading_thread, NULL);

    pigpio_stop(app_data.pinit);

    return 0;
}
